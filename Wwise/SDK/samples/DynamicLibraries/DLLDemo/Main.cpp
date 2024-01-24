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

#include "stdafx.h"
#include "../AkSoundEngineDLL/AkSoundEngineDLL.h"

#include <AK/Tools/Common/AkPlatformFuncs.h>

#if defined(AK_WIN)
#include "./Windows/PlatformDefinitions.h"
#elif defined(AK_LINUX)
#include "./Linux/PlatformDefinitions.h"
#elif defined(AK_APPLE)
#include "./Mac/PlatformDefinitions.h"
#endif

static const AkGameObjectID LISTENER_ID = 10000;
static const AkGameObjectID GAME_OBJECT_HUMAN = 100;
#define LOOP_DURATION 100

// Settings kept for reinitialization in DemoOptions page.
AkMemSettings			m_memSettings;
AkStreamMgrSettings		m_stmSettings;
AkDeviceSettings		m_deviceSettings;
AkInitSettings			m_initSettings;
AkPlatformInitSettings	m_platformInitSettings;
AkMusicSettings			m_musicInit;
#ifndef AK_OPTIMIZED
AkCommSettings			m_commSettings;
#endif

void GetDefaultSettings()
{
	AK::MemoryMgr::GetDefaultSettings(m_memSettings);
	AK::StreamMgr::GetDefaultSettings(m_stmSettings);
	AK::StreamMgr::GetDefaultDeviceSettings(m_deviceSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(m_platformInitSettings);
	AK::MusicEngine::GetDefaultInitSettings(m_musicInit);
	AK::SoundEngine::GetDefaultInitSettings(m_initSettings);
	m_initSettings.szPluginDLLPath = (AkOSChar*)DLL_PATH;
}

AkCommSettings* GetCommSettings()
{
#ifndef AK_OPTIMIZED
	AK::Comm::GetDefaultInitSettings(m_commSettings);
	AKPLATFORM::SafeStrCpy(m_commSettings.szAppNetworkName, "DLLDemo", AK_COMM_SETTINGS_MAX_STRING_SIZE);
	return &m_commSettings;
#else
	return nullptr;
#endif
}

int main()
{
	//
	//Init
	//
	GetDefaultSettings();

	AKRESULT res = AK::SOUNDENGINE_DLL::Init(
		&m_memSettings,
		&m_stmSettings,
		&m_deviceSettings,
		&m_initSettings,
		&m_platformInitSettings,
		&m_musicInit,
		GetCommSettings()
	);

	if (res != AK_Success)
	{
		fprintf(stderr, "Failed to initialize Sound Engine\n");
		return 1;
	}

	AK::SOUNDENGINE_DLL::SetBasePath(SOUNDBANK_PATH);

	// Register Listener
	AK::SoundEngine::RegisterGameObj(LISTENER_ID, "Listener (Default)");
	AK::SoundEngine::SetDefaultListeners(&LISTENER_ID, 1);

	AkBankID bankID;
	res = AK::SoundEngine::LoadBank("Init.bnk", bankID);
	AK::StreamMgr::SetCurrentLanguage( AKTEXT( "English(US)" ) );
	res = AK::SoundEngine::LoadBank("DLLDemo.bnk", bankID);

	//
	// Main
	//

	AK::SoundEngine::RegisterGameObj(GAME_OBJECT_HUMAN, "Human");
	AkPlayingID m_uCurrentPlayingID = AK::SoundEngine::PostEvent("Play_Hello", GAME_OBJECT_HUMAN, 0);

	for (int i = 0; i < LOOP_DURATION; ++i)
	{
		AK::SOUNDENGINE_DLL::Tick();
		AKPLATFORM::AkSleep(16); // Simulate 60 fps
	}

	//
	// Term
	//

	AK::SoundEngine::StopPlayingID(m_uCurrentPlayingID, 0);

	AK::SoundEngine::UnloadBank("DLLDemo.bnk", NULL);
	AK::SoundEngine::UnloadBank("Init.bnk", NULL);

	AK::SoundEngine::UnregisterGameObj(GAME_OBJECT_HUMAN);
	AK::SoundEngine::UnregisterGameObj(LISTENER_ID);

	AK::SOUNDENGINE_DLL::Term();
	return 0;
}
