/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

// Demo3dAudio.cpp
/// \file
/// Defines all methods declared in Demo3dAudio.h

#include "stdafx.h"

#include <math.h>
#include "Menu.h"
#include "MovableChip.h"
#include "Demo3dAudio.h"
#include "Helpers.h"
#include "IntegrationDemo.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

//If you get a compiling error here, it means the file wasn't generated with the banks.  Did you generate the soundbanks before compiling?
#include "../WwiseProject/GeneratedSoundBanks/Wwise_IDs.h"		

//Our game object ID.  Completely arbitrary.
#define GAME_OBJECT_AMBIANCE  100
#define GAME_OBJECT_FOOTSTEPS 101

void DeviceMeteringCallback(AkOutputDeviceMeteringCallbackInfo* in_pCallbackInfo)
{
	Demo3dAudio* pDemo = (Demo3dAudio*)in_pCallbackInfo->pCookie;
	pDemo->OnDeviceMetering(in_pCallbackInfo);
}

static const AkUInt8 kMaxNumSharesets = 4;
static const AkUInt8 kSharesetMode3DMask = 0b01;
static const AkUInt8 kSharesetModeObjectsMask = 0b10;
static const AkUInt8 kDefaultSharesetMode = kSharesetMode3DMask | kSharesetModeObjectsMask; // Activate 3D audio and objects by default

// Indices correspond to shareset mode
static AkUInt32 s_uDeviceSharesets[kMaxNumSharesets] = {
	AK::SoundEngine::GetIDFromString("System_No3D"),
	AK::SoundEngine::GetIDFromString("System_NoObjects"),
	AK::SoundEngine::GetIDFromString("System_No3D"),
	AK::SoundEngine::GetIDFromString("System")
};
static const char* s_uDeviceSharesetLabels[kMaxNumSharesets] = {
	"No 3D audio",
	"3D audio without objects",
	"No 3D audio",
	"3D audio with objects"
};

/////////////////////////////////////////////////////////////////////
// Demo3dAudio Public Methods
/////////////////////////////////////////////////////////////////////

Demo3dAudio::Demo3dAudio(Menu& in_ParentMenu)
	: Page( in_ParentMenu, "3dAudio Demo")
	, m_pChip( NULL )
	, m_fGameObjectX( 0 )
	, m_fGameObjectZ( 0 )
	, m_fWidth( 0.0f )
	, m_fHeight( 0.0f )
	, m_LastX(0.0f)
	, m_LastY(0.0f)
	, m_iLastFootstepTick(-1)
	, m_uMMPeaks(0)
	, m_uPTPeaks(0)
	, m_uObjPeaks(0)
	, m_uCurrentSharesetMode(kDefaultSharesetMode)
	, m_bButtonPressed(true) // Start true so initial press for entering the menu is not registered.
{
	AkZeroMemSmall(&m_MainMixPeaks, sizeof(m_MainMixPeaks));
	AkZeroMemSmall(&m_PassthroughPeaks, sizeof(m_PassthroughPeaks));
	AkZeroMemSmall(&m_ObjectPeaks, sizeof(m_ObjectPeaks));
	m_szHelp =  "This demo shows how to leverage platform-specific 3D audio systems in Wwise. "
				"Ambient sounds start as you enter the page. The background noise of waves and seagulls are "
				"going through a bus optimized for ambient sounds in a sphere around the listener. "
				"The music, on the other hand, goes through a bus configured to bypass any 3D Audio HRTF filters "
				"in order to conserve the musical harmonics. "
                "Move the 'o' around the listener in X and Z, i.e., the plane of the screen, using the "
				"following keys: <<UG_RIGHT_STICK>> <<DIRECTIONAL_TYPE>>, "
				"and hear the sound move along with it. Depending on system capabilities, the footsteps will be "
				"either passed directly as a System Audio Object (point source) to the system's endpoint, or mixed with the other sounds "
				"using the traditional 3D spatialization algorithms of Wwise. "
				"Perform A/B comparisons using <<UG_BUTTON1>>, <<UG_BUTTON3>>, and <<UG_BUTTON4>>."
				"Consult the notes attached to each bus in the hierarchy to learn more.";
}

bool Demo3dAudio::Init()
{
	AK::SoundEngine::RegisterGameObj(GAME_OBJECT_AMBIANCE, "Ambiance");
	AK::SoundEngine::RegisterGameObj(GAME_OBJECT_FOOTSTEPS, "Footsteps");

	// Load the sound bank
	AkBankID bankID; // Not used
	if ( AK::SoundEngine::LoadBank( "ThreeD_Audio_Demo.bnk", bankID ) != AK_Success )
	{
		SetLoadFileErrorMessage( "ThreeD_Audio_Demo.bnk" );
		return false;
	}
	
	m_RegisteredCallbackOutputID = AK_INVALID_OUTPUT_DEVICE_ID;
	
	AK::SoundEngine::PostEvent(AK::EVENTS::PLAY_3D_AUDIO_DEMO, GAME_OBJECT_AMBIANCE);

	return Page::Init();
}

void Demo3dAudio::Release()
{
	AK::SoundEngine::StopAll();
	AK::SoundEngine::UnregisterGameObj(GAME_OBJECT_AMBIANCE);
	AK::SoundEngine::UnregisterGameObj(GAME_OBJECT_FOOTSTEPS);
	AK::SoundEngine::UnloadBank( "ThreeD_Audio_Demo.bnk", NULL );
	
	UnregisterCallback();
	
	if (m_uCurrentSharesetMode != kDefaultSharesetMode)
	{
		// Restore default shareset when we leave the page
		ReplaceOutput(kDefaultSharesetMode);
	}

	Page::Release();
}

void Demo3dAudio::UpdateCallback()
{
	
	AkUniqueID audioDeviceShareset = s_uDeviceSharesets[m_uCurrentSharesetMode];
	AkOutputDeviceID uCurrentDeviceID = AK::SoundEngine::GetOutputID(audioDeviceShareset, 0);
	if (m_RegisteredCallbackOutputID != uCurrentDeviceID)
	{
		UnregisterCallback();
		if (AK_Success == AK::SoundEngine::RegisterOutputDeviceMeteringCallback(uCurrentDeviceID, DeviceMeteringCallback, AkMeteringFlags::AK_EnableBusMeter_Peak, this))
		{
			m_RegisteredCallbackOutputID = uCurrentDeviceID;
		}
	}
}

void Demo3dAudio::UnregisterCallback()
{
	if (m_RegisteredCallbackOutputID != AK_INVALID_OUTPUT_DEVICE_ID)
	{
		if (AK_Success == AK::SoundEngine::RegisterOutputDeviceMeteringCallback(m_RegisteredCallbackOutputID, nullptr, AkMeteringFlags::AK_NoMetering, this))
		{
			m_RegisteredCallbackOutputID = AK_INVALID_OUTPUT_DEVICE_ID;
		}
	}
}

void Demo3dAudio::ReplaceOutput(AkUInt8 uSharesetMode)
{
	AkOutputSettings newSettings = IntegrationDemo::Instance().GetInitSettings().settingsMainOutput;
	newSettings.audioDeviceShareset = s_uDeviceSharesets[uSharesetMode];
	AKVERIFY(AK_Success == AK::SoundEngine::ReplaceOutput(newSettings, 0/*Main Output*/));
}

void Demo3dAudio::UpdateGameObjPos()
{
	float x, y;
	m_pChip->GetPos(x, y);
	
	// Set 3D position
	// Converting X-Y UI into X-Z world plan.
	AkVector position;
	m_fGameObjectX = position.X = IntegrationDemoHelpers::PixelsToAKPos_X(m_fWidth, x);
	position.Y = 0;
	m_fGameObjectZ = position.Z = IntegrationDemoHelpers::PixelsToAKPos_Y(m_fHeight, y);
	AkVector orientationFront;
	orientationFront.Z = 1;
	orientationFront.Y = orientationFront.X = 0;
	AkVector orientationTop;
	orientationTop.X = orientationTop.Z = 0;
	orientationTop.Y = 1; 
		
	AkSoundPosition soundPos;
	soundPos.Set(position, orientationFront, orientationTop);
	AK::SoundEngine::SetPosition(GAME_OBJECT_FOOTSTEPS, soundPos);

	// Determine whether we should play a footstep sound or not
	float dx = x - m_LastX;
	float dy = y - m_LastY;
	float dist = (float)sqrt(dx * dx + dy * dy);

	//Post the Footstep event if appropriate (if we are moving!)
	if (dist < 0.1f && m_iLastFootstepTick != -1)
	{
		AK::SoundEngine::PostEvent(AK::EVENTS::PLAY_SANDSTEP, GAME_OBJECT_FOOTSTEPS);
		m_iLastFootstepTick = -1;
	}
	else if (dist > 0.1f && m_pParentMenu->GetTickCount() - m_iLastFootstepTick > 20)
	{
		AK::SoundEngine::PostEvent(AK::EVENTS::PLAY_SANDSTEP, GAME_OBJECT_FOOTSTEPS);
		m_iLastFootstepTick = m_pParentMenu->GetTickCount();
	}

	m_LastX = x;
	m_LastY = y;
}

bool Demo3dAudio::Update()
{
	//Always update the MovableChip

	bool bMoved = false;
	UniversalInput::Iterator it;
	for ( it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++ )
	{
		// Skip this input device if it's not connected
		if ( ! it->IsConnected() )
			continue;

		m_pChip->Update(*it);

		// Only one button press is registered at a time for this demo
		bool btn1 = (*it).IsButtonDown(UG_BUTTON1);
		bool btn3 = (*it).IsButtonDown(UG_BUTTON3);
		bool btn4 = (*it).IsButtonDown(UG_BUTTON4);
		if (!m_bButtonPressed)
		{
			// Check for new button press
			if (btn1)
			{
				// Cycle modes; this is important for platforms with very limited number of input buttons (e.g. tvOS Remote)
				m_uCurrentSharesetMode = (m_uCurrentSharesetMode + 1) % kMaxNumSharesets;

				ReplaceOutput(m_uCurrentSharesetMode);
			}
			else if (btn3)
			{
				// Toggle 3D audio
				m_uCurrentSharesetMode ^= kSharesetMode3DMask;
				ReplaceOutput(m_uCurrentSharesetMode);
			}
			else if (btn4)
			{
				// Toggle objects
				m_uCurrentSharesetMode ^= kSharesetModeObjectsMask;
				ReplaceOutput(m_uCurrentSharesetMode);
			}
		}
		m_bButtonPressed = btn1 || btn3 || btn4;

		bMoved = true;
	}

	if ( bMoved )
	{
		UpdateGameObjPos();
	}

	UpdateCallback();
	
	return Page::Update();
}

AkForceInline AkReal32 LinTodB(AkReal32 in_fLinValue)
{
	return 20 * (AkReal32)log10(in_fLinValue);
}

void Demo3dAudio::Draw()
{
	Page::Draw();

	m_pChip->Draw();

	int iLineHeight = GetLineHeight(DrawStyle_Text);

	// Representing the listener position
	float origX = (m_pParentMenu->GetWidth() - m_pChip->GetRightBottomMargin()) / 2.0f;
	float origY = (m_pParentMenu->GetHeight() - m_pChip->GetRightBottomMargin()) / 2.0f;
	DrawTextOnScreen("X", (int)origX, (int)origY, DrawStyle_Text);

	const int kSidebarMargin = 50;

	int iModeHeight = iLineHeight * 9;
	char mode[256];
	AKPLATFORM::SafeStrFormat(mode, 256, "Mode: %s\n", s_uDeviceSharesetLabels[m_uCurrentSharesetMode]);
	DrawTextOnScreen(mode, kSidebarMargin, iModeHeight, DrawStyle_Text);

#ifndef AK_OPTIMIZED
	// Display metering
	int iMeteringY = iModeHeight + iLineHeight;
	DrawTextOnScreen("Main Mix:", 50, iMeteringY, DrawStyle_Text);
	iMeteringY += iLineHeight;
	for (int c = 0; c < (int)m_uMMPeaks; c++)
	{
		AkReal32 dB = LinTodB(m_MainMixPeaks[c]);
		AkUInt32 length = AkClamp((AkUInt32)(dB + 100), 10, 150);
		DrawLineOnScreen(kSidebarMargin, iMeteringY, kSidebarMargin + length, iMeteringY);
		iMeteringY += 5;
	}
	if (m_uPTPeaks > 0)
	{
		DrawTextOnScreen("Passthrough mix:", 50, iMeteringY, DrawStyle_Text);
		iMeteringY += iLineHeight;
		for (int c = 0; c < (int)m_uPTPeaks; c++)
		{
			AkReal32 dB = LinTodB(m_PassthroughPeaks[c]);
			AkUInt32 length = AkClamp((AkUInt32)(dB + 100), 10, 150);
			DrawLineOnScreen(kSidebarMargin, iMeteringY, kSidebarMargin + length, iMeteringY);
			iMeteringY += 5;
		}
	}
	if (m_uObjPeaks > 0)
	{
		DrawTextOnScreen("3D Audio Objects:", 50, iMeteringY, DrawStyle_Text);
		iMeteringY += iLineHeight;
		for (int c = 0; c < (int)m_uObjPeaks; c++)
		{
			AkReal32 dB = LinTodB(m_ObjectPeaks[c]);
			AkUInt32 length = AkClamp((AkUInt32)(dB + 100), 10, 150);
			DrawLineOnScreen(kSidebarMargin, iMeteringY, kSidebarMargin + length, iMeteringY);
			iMeteringY += 5;
		}
	}
#endif

	char strBuf[50];
	snprintf( strBuf, 50, "X: %.2f\nZ: %.2f", m_fGameObjectX, m_fGameObjectZ );
	static int s_nOffset = 3 * iLineHeight;
	DrawTextOnScreen( strBuf, 5, m_pParentMenu->GetHeight() - s_nOffset, DrawStyle_Text );

	// Display instructions at the bottom of the page
	int iInstructionsY = m_pParentMenu->GetHeight() - 6 * iLineHeight;
	DrawTextOnScreen("(Press <<UG_BUTTON1>> To Cycle Modes...)", (int)(m_pParentMenu->GetWidth() * 0.4), iInstructionsY, DrawStyle_Text);
	iInstructionsY += iLineHeight;
	DrawTextOnScreen("(Press <<UG_BUTTON2>> To Go Back...)", (int)(m_pParentMenu->GetWidth() * 0.4), iInstructionsY, DrawStyle_Text);
	iInstructionsY += iLineHeight;
	DrawTextOnScreen("(Press <<UG_BUTTON3>> To Toggle 3D Audio...)", (int)(m_pParentMenu->GetWidth() * 0.4), iInstructionsY, DrawStyle_Text);
	iInstructionsY += iLineHeight;
	DrawTextOnScreen("(Press <<UG_BUTTON4>> To Toggle System Audio Objects...)", (int)(m_pParentMenu->GetWidth() * 0.4), iInstructionsY, DrawStyle_Text);

}

bool Demo3dAudio::OnPointerEvent( PointerEventType in_eType, int in_x, int in_y )
{
	if ( in_eType == PointerEventType_Moved )
	{
		m_pChip->SetPos( (float) in_x, (float) in_y );
		UpdateGameObjPos();
	}

	return Page::OnPointerEvent( in_eType, in_x, in_y );
}

void Demo3dAudio::OnDeviceMetering(AkOutputDeviceMeteringCallbackInfo* in_pCallbackInfo)
{
	AkZeroMemSmall(m_MainMixPeaks, sizeof(m_MainMixPeaks));
	AkZeroMemSmall(m_PassthroughPeaks, sizeof(m_PassthroughPeaks));
	AkZeroMemSmall(m_ObjectPeaks, sizeof(m_ObjectPeaks));
	m_uMMPeaks = AkMin(36, in_pCallbackInfo->mainMixConfig.uNumChannels);
	m_uPTPeaks = AkMin(2, in_pCallbackInfo->passthroughMixConfig.uNumChannels);
	m_uObjPeaks = AkMin(20, in_pCallbackInfo->uNumSystemAudioObjects);

	auto peaks = in_pCallbackInfo->pMainMixMetering->peak;
	for (int c = 0; c < (int)m_uMMPeaks; c++)
	{
		m_MainMixPeaks[c] = peaks[c];
	}
	if (in_pCallbackInfo->pPassthroughMetering)
	{
		auto peaks = in_pCallbackInfo->pPassthroughMetering->peak;
		for (int c = 0; c < (int)m_uPTPeaks; c++)
		{
			m_PassthroughPeaks[c] = peaks[c];
		}
	}
	if (in_pCallbackInfo->uNumSystemAudioObjects > 0)
	{
		for (int c = 0; c < (int)m_uObjPeaks; c++)
		{
			m_ObjectPeaks[c] = in_pCallbackInfo->ppSystemAudioObjectMetering[c]->peak[0]; // Object meters are always mono
		}
	}
}

void Demo3dAudio::InitControls()
{
	m_pChip = new MovableChip(*this);
	m_pChip->SetLabel( "o" );
	m_pChip->UseJoystick(UG_STICKRIGHT);
	m_pChip->SetNonLinear();

	m_fWidth = (float)m_pParentMenu->GetWidth() - (float)m_pChip->GetRightBottomMargin();
	m_fHeight = (float)m_pParentMenu->GetHeight() - (float)m_pChip->GetRightBottomMargin();
}

