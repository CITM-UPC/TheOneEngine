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

// DemoSpatialAudioReflect.cpp
/// \file
/// Defines all methods declared in DemoSpatialAudioReflect.h

#include "stdafx.h"

#include <float.h>
#include "Menu.h"
#include "Helpers.h"
#include "DemoSpatialAudioReflect.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

#define _USE_MATH_DEFINES
#include <math.h>

//If you get a compiling error here, it means the file wasn't generated with the banks.  Did you generate the soundbanks before compiling?
#include "../WwiseProject/GeneratedSoundBanks/Wwise_IDs.h"

// Our game object ID.  Completely arbitrary.
const AkGameObjectID EMITTER = 100;
const AkGameObjectID LISTENER = 103;
const AkGameObjectID DISTANCE_PROBE = 104;
const AkGeometrySetID GEOMETRY_BOX = 0;
const AkGeometryInstanceID GEOMETRY_INST_ROOM = 1;
const AkGeometryInstanceID GEOMETRY_INST_WALL = 2;

#define REPEAT_TIME 20
#define REFLECT_ID 171

/////////////////////////////////////////////////////////////////////
// DemoSpatialAudioReflect Public Methods
/////////////////////////////////////////////////////////////////////

DemoSpatialAudioReflect::DemoSpatialAudioReflect(Menu& in_ParentMenu)
	:Page(in_ParentMenu, "Reflect Demo")
	, m_pListenerChip(NULL)
	, m_pEmitterChip(NULL)
	, m_aLines(NULL)
	, m_numLines(0)
	, m_fWidth(0.0f)
	, m_fHeight(0.0f)
	, m_room_y(0.0f)
	, m_room_height(0.0f)
	, m_reflect_order( 0 )
	, m_bTestNoise(false)
	, m_bMoved(false)
{
	m_szHelp =
		"*** Please connect Wwise and open the 3D Game Object Viewer (F12 layout). ***\n"
		"This demo showcases the Wwise Reflect plug-in using the Geometry API to simulate early reflection.\n \n"
		"This demo consists of a room and a wall, defined by Spatial Audio Geometry, a listener/[L], and an emitter/[E].\n"
		"The reflection path(s) are displayed. Spatial Audio is set up with the reflection order in the lower-left corner. "
		"Additionally, Spatial Audio allows reflection path(s) to diffract.\n"
		"Refer to the Wwise Reflect in the Plug-ins documentation for more details.\n \n"
		"Listener and emitter controls are specified under the Spatial Audio Demos menu help page.\n"
		"* <<UG_BUTTON1>> - Change reflection order in sequence.\n"
		"* <<UG_BUTTON10>> - Decrease room size.\n"
		"* <<UG_BUTTON5>> + <<UG_BUTTON10>> - Increase room size.\n"
		"* <<UG_BUTTON8>> - Change Geometry accoustic texture in sequence.\n";
}

bool DemoSpatialAudioReflect::Init()
{
	// Load the sound bank
	AkBankID bankID; // Not used
	if (AK::SoundEngine::LoadBank("Reflect.bnk", bankID) != AK_Success)
	{
		SetLoadFileErrorMessage("Reflect.bnk");
		return false;
	}
	
	// Verify for Reflect Plug-in
	if (!AK::SoundEngine::IsPluginRegistered(AkPluginType::AkPluginTypeEffect, 0, REFLECT_ID))
	{
		SetPluginErrorMessage("Could not find Reflect Plug-in.\nPlease make sure to install the Reflect Plug-in from the Wwise Launcher and\n"
			"that the soundbank is generated with the Reflect effect in use.");
		return false;
	}

	return Page::Init();
}

void DemoSpatialAudioReflect::InitControls()
{
	float fMargin = (float)m_pEmitterChip->GetRightBottomMargin();
	m_fWidth = (float)m_pParentMenu->GetWidth() - fMargin;
	m_fHeight = (float)m_pParentMenu->GetHeight() - fMargin;
	float centerX = m_fWidth / 2.0f;
	float centerY = m_fHeight / 2.0f;

	// Register listener in spatial audio and init chip controls
	m_pListenerChip = new SpatialAudioListener(*this, LISTENER, DISTANCE_PROBE, "Listener L");
	m_pListenerChip->SetLabel("<L>");
	m_pListenerChip->UseJoystick(UG_STICKRIGHT);
	m_pListenerChip->SetNonLinear();
	m_pListenerChip->SetPos(centerX + 70, centerY + 30);
	m_pListenerChip->SetMaxSpeed(3.f);

	// Register emitter in spatial audio and init chip controls
	m_pEmitterChip = new SpatialAudioGameObject(*this, EMITTER, LISTENER, "Emitter E");
	m_pEmitterChip->SetLabel("<E>");
	m_pEmitterChip->UseJoystick(UG_STICKRIGHT);
	m_pEmitterChip->SetNonLinear();
	m_pEmitterChip->SetPos(centerX - 70, centerY - 30);
	m_pEmitterChip->SetMaxSpeed(3.f);

	Init_Geometry();
	AK::SoundEngine::PostEvent("Play_Reflect_Emitter", EMITTER);

	m_reflect_order = 1;
	AK::SpatialAudio::SetReflectionsOrder(m_reflect_order, true);

	// Update movable chips the first time.
	UniversalInput::Iterator it;
	for (it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++)
	{
		// Skip this input device if it's not connected
		if (!it->IsConnected())
			continue;

		m_pEmitterChip->Update(*it);
		m_pListenerChip->Update(*it);
		m_pEmitterChip->SetPosition();
		m_pListenerChip->SetPosition();
	}
}

void DemoSpatialAudioReflect::Init_Geometry()
{
	// Room
	m_surface_texture = Brick;
	str_texture = "Brick"; // initial accoustic texture
	texture_color[0] = 0.5f;
	texture_color[1] = 0.f;
	texture_color[2] = 0.f;

	m_pRoom = new Box(*this);

	float room_width_px = m_fWidth / 2.f;
	float room_height_px = m_room_height = m_fHeight / 2.f;
	m_room_y = m_fHeight / 4.f;

	m_pRoom->SetPosition((int)(m_fWidth / 4.f), (int)(m_fHeight / 4.f));
	m_pRoom->SetDimensions((int)room_width_px, (int)room_height_px);
	
	// Register to SpatialAudio
	AkAcousticSurface wallSurface;
	wallSurface.strName = "Wall";
	wallSurface.textureID = AK::SoundEngine::GetIDFromString(str_texture);

	SpatialAudioHelpers::BoxGeometryParams boxParams;
	boxParams.surfaces.push_back(wallSurface);

	SpatialAudioHelpers::AddBoxGeometry(GEOMETRY_BOX, boxParams);
	SpatialAudioHelpers::AddBoxGeometryInstance(GEOMETRY_INST_ROOM, GEOMETRY_BOX, m_pRoom, m_fWidth, m_fHeight);

	// Wall
	m_pWall = new Box(*this);

	float wall_width_px = m_fWidth / 4.f;
	float wall_height_px = 1;

	m_pWall->SetPosition((int)(3.f * m_fWidth / 8.f), (int)(5.f * m_fHeight / 16.f));
	m_pWall->SetDimensions((int)wall_width_px, (int)wall_height_px);

	// Register to SpatialAudio
	// The same geometry can be used with a new instance to define the wall
	SpatialAudioHelpers::AddBoxGeometryInstance(GEOMETRY_INST_WALL, GEOMETRY_BOX, m_pWall, m_fWidth, m_fHeight);
}

void DemoSpatialAudioReflect::Release()
{
	AK::SoundEngine::StopAll();
	delete m_pEmitterChip;
	delete m_pListenerChip;
	AK::SpatialAudio::RemoveGeometryInstance(GEOMETRY_INST_ROOM);
	AK::SpatialAudio::RemoveGeometryInstance(GEOMETRY_INST_WALL);
	AK::SpatialAudio::RemoveGeometry(GEOMETRY_BOX);
	AK::SoundEngine::UnloadBank( "Reflect.bnk", NULL );

	for (int i = 0; i < m_numLines; i++)
	{
		if (m_aLines[i])
			delete m_aLines[i];
	}
	delete[] m_aLines;

	Page::Release();
}

bool DemoSpatialAudioReflect::Update()
{
	const float kAngleIncrement = M_PI / 180.f;

	UniversalInput::Iterator it;
	for (it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++)
	{
		// Skip this input device if it's not connected
		if (!it->IsConnected())
			continue;

		if ((*it).IsButtonDown(UG_BUTTON3))
		{
			if ((*it).IsButtonDown(UG_BUTTON5))
				m_pListenerChip->IncrementElevation(-kAngleIncrement);
			else
				m_pListenerChip->IncrementAzimuth(-kAngleIncrement);

			m_bMoved = true;
		}

		if ((*it).IsButtonDown(UG_BUTTON4))
		{
			if ((*it).IsButtonDown(UG_BUTTON5))
				m_pListenerChip->IncrementElevation(kAngleIncrement);
			else
				m_pListenerChip->IncrementAzimuth(kAngleIncrement);

			m_bMoved = true;
		}

		if ((*it).IsButtonDown(UG_BUTTON10))
		{
			if ((*it).IsButtonDown(UG_BUTTON5))
				EnlargeRoom();
			else
				ShrinkRoom();
		}

		if ((*it).IsButtonTriggered(UG_BUTTON1))
		{
			// toggle reflect order state.
			m_reflect_order = m_reflect_order % 4 + 1;
			AK::SpatialAudio::SetReflectionsOrder(m_reflect_order, true);
			m_bMoved = true;
		}

		if ((*it).IsButtonTriggered(UG_BUTTON6))
		{
			m_pListenerChip->AddToPlayerOffset(5.f);
			m_bMoved = true;
		}

		if ((*it).IsButtonTriggered(UG_BUTTON8))
		{
			m_surface_texture = (m_surface_texture + 1) % 7;
			setAcousticTexture();

			m_bMoved = true;
		}

		if ((*it).IsButtonDown(UG_BUTTON5))
		{
			m_pEmitterChip->Update(*it);
			m_bMoved = true;
		}
		else
		{
			m_pListenerChip->Update(*it);
			m_bMoved = true;
		}

		if ((*it).IsButtonTriggered(UG_BUTTON9))
		{
			AK::SoundEngine::StopAll(EMITTER);
			if (m_bTestNoise)
			{
				AK::SoundEngine::PostEvent("Play_Reflect_Emitter", EMITTER);
				m_bTestNoise = false;
			}
			else
			{
				AK::SoundEngine::PostEvent("Play_Reflect_Noise", EMITTER);
				m_bTestNoise = true;
			}
		}
	}

	if (m_bMoved)
	{
		// Update emitters and listener.
		m_pEmitterChip->SetPosition();
		m_pListenerChip->SetPosition();

		//
		// Query propagation for UI display.
		//
		
		for (int i = 0; i < m_numLines; i++)
		{
			if (m_aLines[i])
				delete m_aLines[i];
		}
		delete[] m_aLines;
		m_aLines = NULL;
		m_numLines = 0;

		AkVector64 emitterPos, listenerPos;
		AkReflectionPathInfo paths[16];
		AkUInt32 uNumPaths = 16;	// 16 arbitrary
		AK::SpatialAudio::QueryReflectionPaths(
			EMITTER,		///< The ID of the game object that the client wishes to query.
			0,				///< Position index 0, we only have one.
			listenerPos,	///< Returns the position of the listener game object that is associated with the game object \c in_gameObjectID.
			emitterPos,		///< Returns the position of the emitter game object \c in_gameObjectID.
			paths,			///< Pointer to an array of \c AkPropagationPathInfo's which will be filled after returning.
			uNumPaths		///< The number of slots in \c out_aPaths, after returning the number of valid elements written.
			);

		// Count lines to draw.
		for (AkUInt32 path = 0; path < uNumPaths; path++)
		{
			int pathPoints = paths[path].numPathPoints;
			int numSegments = pathPoints + 1;
			m_numLines += numSegments;
		}

		if (m_numLines > 0)
		{
			m_aLines = new Line*[m_numLines];
			m_lineReflectOrder = new int[m_numLines];

			int line = 0;
			for (AkUInt32 path = 0; path < uNumPaths; path++)
			{
				int numPathPoints = paths[path].numPathPoints;
				int numReflections = paths[path].numReflections;

				if (numPathPoints > 0)
				{
					// Emitter to node 0.
					m_aLines[line] = new Line(*this);
					m_aLines[line]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, emitterPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, emitterPos.Z),
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].pathPoint[0].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].pathPoint[0].Z));
					m_lineReflectOrder[line] = numReflections;
					++line;

					// node i-1 to node i
					for (int i = 1; i < numPathPoints; i++)
					{
						m_aLines[line] = new Line(*this);
						m_aLines[line]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].pathPoint[i - 1].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].pathPoint[i - 1].Z),
							IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].pathPoint[i].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].pathPoint[i].Z));
						m_lineReflectOrder[line] = numReflections;
						++line;
					}
					
					// Last node to listener.
					m_aLines[line] = new Line(*this);
					m_aLines[line]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].pathPoint[numPathPoints - 1].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].pathPoint[numPathPoints - 1].Z),
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, listenerPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, listenerPos.Z));
					m_lineReflectOrder[line] = numReflections;
					++line;
				}
				else
				{
					// A path with no node: completely obstructed. Draw a line from emitter to listener, with style "Selected" (Draw() checks m_diffraction)
					m_aLines[line] = new Line(*this);
					m_aLines[line]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, emitterPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, emitterPos.Z),
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, listenerPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, listenerPos.Z));
					++line;
				}
			}
		}
	}

	// Reset m_bMoved.
	m_bMoved = false;

	return Page::Update();
}

void DemoSpatialAudioReflect::Draw()
{
	Page::Draw();

	if (m_pEmitterChip)
		m_pEmitterChip->Draw();
	if (m_pListenerChip)
		m_pListenerChip->Draw();

	if (m_pRoom)
		m_pRoom->Draw(texture_color);

	if (m_pWall)
		m_pWall->Draw(texture_color);

	for (int i = 0; i < m_numLines; i++)
	{
		if (m_aLines[i])
		{
			float lineColor[3];
			switch (m_lineReflectOrder[i])
			{
			case 1: // 1nd order reflection -> yellow
				lineColor[0] = 1.f;
				lineColor[1] = 1.f;
				lineColor[2] = 0.f;
				break;
			case 2: // 2nd order reflection -> orange
				lineColor[0] = 1.f;
				lineColor[1] = 0.5f;
				lineColor[2] = 0.f;
				break;
			case 3: // 3rd order reflection -> red
				lineColor[0] = 1.f;
				lineColor[1] = 0.f;
				lineColor[2] = 0.f;
				break;
			case 4: // 4th order reflection -> brown
				lineColor[0] = 0.5f;
				lineColor[1] = 0.f;
				lineColor[2] = 0.f;
				break;
			default: // default -> white
				lineColor[0] = 1.f;
				lineColor[1] = 1.f;
				lineColor[2] = 1.f;
				break;
			}
			m_aLines[i]->Draw(lineColor);
		}
	}

	float fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ;
	m_pListenerChip->GetAkPos(fAkPosListenerX, fAkPosListenerZ);
	m_pEmitterChip->GetAkPos(fAkPosEmitterX, fAkPosEmitterZ);

	char strBuf[128];
	snprintf(strBuf, 128, "[L](X,Z): %.2f  %.2f\n[E](X,Z): %.2f  %.2f\nReflection Order: %d\nAccoustic Texture: %s",
		fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ, m_reflect_order, str_texture);

	static const int MAX_LINES_FOR_TEXT = 5;
	static int s_nOffset = MAX_LINES_FOR_TEXT * GetLineHeight( DrawStyle_Text );
	DrawTextOnScreen( strBuf, 5, m_pParentMenu->GetHeight() - s_nOffset, DrawStyle_Text );

	// Display instructions at the bottom of the page
	int iInstructionsY = m_pParentMenu->GetHeight() - 3 * GetLineHeight( DrawStyle_Text );
	DrawTextOnScreen( "(Press <<UG_BUTTON2>> To Go Back...)", (int)(m_pParentMenu->GetWidth() * 0.4), iInstructionsY, DrawStyle_Text );
}

bool DemoSpatialAudioReflect::OnPointerEvent(PointerEventType in_eType, int in_x, int in_y)
{
	if ( in_eType == PointerEventType_Moved )
	{
		bool bMoveListener = true;
		for (UniversalInput::Iterator it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++)
		{
			if ((*it).IsButtonDown(UG_BUTTON5))
			{
				bMoveListener = false;
				break;
			}
		}

		if (bMoveListener)
			m_pListenerChip->SetPos((float)in_x, (float)in_y);
		else
			m_pEmitterChip->SetPos((float)in_x, (float)in_y);

		m_bMoved = true;	// Force update of game objects and GUI of sound propagation.
	}

	return Page::OnPointerEvent( in_eType, in_x, in_y );
}

#define MINIMUM_ROOM_SIZE (100.f)

void DemoSpatialAudioReflect::ShrinkRoom()
{
	float px_y_offset = m_fHeight / m_fWidth;
	
	if ((m_room_height - 2.f * px_y_offset) > MINIMUM_ROOM_SIZE)
	{
		float px_x, px_y, px_width, px_height;
		m_pRoom->GetRect(px_x, px_y, px_width, px_height);

		m_room_y += px_y_offset;
		m_room_height -= 2.f * px_y_offset;

		m_pRoom->SetPosition((int) (px_x + 1.f), (int) m_room_y);
		m_pRoom->SetDimensions((int) (px_width - 2.f), (int) m_room_height);

		SpatialAudioHelpers::AddBoxGeometryInstance(GEOMETRY_INST_ROOM, GEOMETRY_BOX, m_pRoom, m_fWidth, m_fHeight);
	}
}

void DemoSpatialAudioReflect::EnlargeRoom()
{
	float px_y_offset = m_fHeight / m_fWidth;

	if ((m_room_height + 2.f * px_y_offset) < m_fHeight)
	{
		float px_x, px_y, px_width, px_height;
		m_pRoom->GetRect(px_x, px_y, px_width, px_height);

		m_room_y -= px_y_offset;
		m_room_height += 2.f * px_y_offset;

		m_pRoom->SetPosition((int) (px_x - 1.f), (int) m_room_y);
		m_pRoom->SetDimensions((int) (px_width + 2.f), (int) m_room_height);

		SpatialAudioHelpers::AddBoxGeometryInstance(GEOMETRY_INST_ROOM, GEOMETRY_BOX, m_pRoom, m_fWidth, m_fHeight);
	}
}

void DemoSpatialAudioReflect::setAcousticTexture()
{
	switch (m_surface_texture)
	{
	case Anechoic:
		str_texture = "Anechoic";
		texture_color[0] = 0.5f;
		texture_color[1] = 0.f;
		texture_color[2] = 1.f;
		break;
	case Carpet:
		str_texture = "Carpet";
		texture_color[0] = 0.f;
		texture_color[1] = 0.75f;
		texture_color[2] = 0.f;
		break;
	case Concrete:
		str_texture = "Concrete";
		texture_color[0] = 0.5f;
		texture_color[1] = 0.5f;
		texture_color[2] = 0.5f;
		break;
	case Drywall:
		str_texture = "Drywall";
		texture_color[0] = 0.25f;
		texture_color[1] = 0.75f;
		texture_color[2] = 1.f;
		break;
	case Tile:
		str_texture = "Tile";
		texture_color[0] = 0.f;
		texture_color[1] = 0.75f;
		texture_color[2] = 0.75f;
		break;
	case Wood:
		str_texture = "Wood";
		texture_color[0] = 0.5f;
		texture_color[1] = 0.25f;
		texture_color[2] = 0.f;
		break;
	default:
		str_texture = "Brick";
		texture_color[0] = 0.5f;
		texture_color[1] = 0.f;
		texture_color[2] = 0.f;
		break;
	}

	// Register to SpatialAudio
	AkAcousticSurface wallSurface;
	wallSurface.strName = "Wall";
	wallSurface.textureID = AK::SoundEngine::GetIDFromString(str_texture);

	SpatialAudioHelpers::BoxGeometryParams boxParams;
	boxParams.surfaces.push_back(wallSurface);

	SpatialAudioHelpers::AddBoxGeometry(GEOMETRY_BOX, boxParams);
}
