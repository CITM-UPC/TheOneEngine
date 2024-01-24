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

// DemoSpatialAudioPortalReflect.cpp
/// \file
/// Defines all methods declared in DemoSpatialAudioPortalReflect.h

#include "stdafx.h"

#include "Menu.h"
#include "Helpers.h"
#include "DemoSpatialAudioPortalReflect.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>

#define _USE_MATH_DEFINES
#include <math.h>

//If you get a compiling error here, it means the file wasn't generated with the banks.  Did you generate the soundbanks before compiling?
#include "../WwiseProject/GeneratedSoundBanks/Wwise_IDs.h"

const AkGameObjectID EMITTER = 100;
const AkGameObjectID LISTENER = 103;
const AkGameObjectID DISTANCE_PROBE = 104;

const AkRoomID ROOM_BASE_ID = 200;
const AkGeometryInstanceID OBSTACLE_BASE_ID = 300;
const AkPortalID PORTAL_BASE_ID = 400;

const AkGeometrySetID ROOM_GEOMETRY = 0;
const AkGeometrySetID OBSTACLE_GEOMETRY = 1;

#define REPEAT_TIME 20
#define REFLECT_ID 171

/////////////////////////////////////////////////////////////////////
// DemoSpatialAudioReflect Public Methods
/////////////////////////////////////////////////////////////////////

DemoSpatialAudioPortalReflect::DemoSpatialAudioPortalReflect(Menu& in_ParentMenu)
	:Page(in_ParentMenu, "Portal and Reflect Demo")
	, m_pListenerChip(NULL)
	, m_pEmitterChip(NULL)
	, m_fWidth(0.0f)
	, m_fHeight(0.0f)
	, m_dryDiffraction(0)
	, m_wetDiffraction(0)
	, m_transmissionLoss(0)
	, m_portalsOpen(0x3F)	// All portals open
	, m_bTestNoise(false)
	, m_bMoved(false)
{
	m_szHelp = 
		"*** Please connect Wwise and open the 3D Game Object Viewer (F12 layout). ***\n"
		"This demo showcases the use of the Wwise Reflect plug-in along with Rooms and Portals from Spatial Audio.\n \n"
		"In this demo there are 5 Rooms with Geometry, 6 Portals, and 3 Geometries as obstacles\n"
		"The reflection order is set to 2nd order. The diffraction path(s) and the reflection path(s) are displayed. "
		"The diffraction and transmission amounts are displayed in the lower-left corner.\n \n"
		"Listener and emitter controls are specified under the Spatial Audio Demos menu help page.\n"
		"* <<UG_BUTTON1>> - Open/close Portals 'Listener/[L]' is standing in.\n"
		"* <<UG_BUTTON5>> + <<UG_BUTTON1>> - Open/close Portals 'Emitter/[E]' is standing in.";
}

bool DemoSpatialAudioPortalReflect::Init()
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

void DemoSpatialAudioPortalReflect::InitControls()
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

	// Set up default wall surface for our Geometry
	AkAcousticSurface wallSurface;
	wallSurface.strName = "Room Wall";
	wallSurface.textureID = AK::SoundEngine::GetIDFromString("Brick");
	wallSurface.transmissionLoss = 0.8f;

	// Set up a default Geometry to make instances from
	SpatialAudioHelpers::BoxGeometryParams boxParams;
	boxParams.surfaces.push_back(wallSurface);
	SpatialAudioHelpers::AddBoxGeometry(ROOM_GEOMETRY, boxParams);

	wallSurface.strName = "Obstacle Wall";
	wallSurface.textureID = AK::SoundEngine::GetIDFromString("Concrete");
	wallSurface.transmissionLoss = 0.9f;
	boxParams.surfaces.clear();
	boxParams.surfaces.push_back(wallSurface);
	SpatialAudioHelpers::AddBoxGeometry(OBSTACLE_GEOMETRY, boxParams);

	Init_Rooms();
	Init_Portals();
	Init_Obstacles();

	AK::SoundEngine::PostEvent("Play_Reflect_Emitter", EMITTER);
	AK::SpatialAudio::SetReflectionsOrder(2, true);	// Demo presented with reflection order of 2 

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

void DemoSpatialAudioPortalReflect::Init_Rooms()
{
	AkRoomParams paramsRoom;
	// Register "outside". Outside is already created automatically, but we want to give it a name, and set its aux send (reverb)
	paramsRoom.Front = { 0.f, 0.f, 1.f };
	paramsRoom.Up = { 0.f, 1.f, 0.f };
	paramsRoom.TransmissionLoss = 0.f;
	paramsRoom.RoomGameObj_KeepRegistered = false;
	paramsRoom.ReverbAuxBus = AK::SoundEngine::GetIDFromString("Outside");
	paramsRoom.GeometryInstanceID = AkGeometryInstanceID(); // Invalid ID - no geometry for outside.
	AK::SpatialAudio::SetRoom(AK::SpatialAudio::kOutdoorRoomID, paramsRoom, "Outside Object");

	// parameterize ROOM dimention
	float widthFifth = m_fWidth / 5.f;
	float heightHalf = m_fHeight / 2.f;

	//
	// Define boxes for each ROOM
	//
	m_rooms.push_back(std::make_unique<Box>(*this, 0, 0, (int) (2.f * widthFifth), (int) heightHalf));
	m_rooms.push_back(std::make_unique<Box>(*this, (int) (3.f * widthFifth), 0, (int) (2.f * widthFifth), (int) heightHalf));
	m_rooms.push_back(std::make_unique<Box>(*this, 0, (int) heightHalf, (int) (2.f * widthFifth), (int) heightHalf));
	m_rooms.push_back(std::make_unique<Box>(*this, (int) (3.f * widthFifth), (int) heightHalf, (int) (2.f * widthFifth), (int) heightHalf));

	// Register each ROOM to SpatialAudio
	paramsRoom.ReverbAuxBus = AK::SoundEngine::GetIDFromString("Room");
	paramsRoom.TransmissionLoss = 0.9f;
	paramsRoom.RoomGameObj_KeepRegistered = true;
	int roomIdx = 0;
	char strRoomBuf[10];
	for (auto& room : m_rooms)
	{
		// Room
		snprintf(strRoomBuf, 10, "Room%d", roomIdx);
		paramsRoom.GeometryInstanceID = ROOM_BASE_ID + roomIdx;
		AK::SpatialAudio::SetRoom(ROOM_BASE_ID + roomIdx, paramsRoom, strRoomBuf);

		// Geometry Instance
		SpatialAudioHelpers::AddBoxGeometryInstance(ROOM_BASE_ID + roomIdx, ROOM_GEOMETRY, room.get(), m_fWidth, m_fHeight);
		roomIdx++;
	}
}

void DemoSpatialAudioPortalReflect::Init_Obstacles()
{
	//
	// Define boxes for each obstacle GEOMETRY
	//
	m_obstacles.push_back(std::make_unique<Box>(*this,
		(int) (15.f * m_fWidth / 31.f), (int) (m_fHeight / 6.f), (int) (m_fWidth / 31.f), (int) (4.f * m_fHeight / 6.f)));
	m_obstacles.push_back(std::make_unique<Box>(*this,
		(int) (m_fWidth / 10.f), (int) (m_fHeight / 3.f), (int) (2.f * m_fWidth / 10.f), (int) (m_fHeight / 30.f)));
	m_obstacles.push_back(std::make_unique<Box>(*this,
		(int) (m_fWidth - (3.f * m_fWidth / 10.f)), (int) (m_fHeight - (m_fHeight / 3.f)), (int) (2.f * m_fWidth / 10.f), (int) (m_fHeight / 30.f)));

	// Register each obstacle GEOMETRY to SpatialAudio 
	int obstacleIdx = 0;
	for (auto& obstacle : m_obstacles)
	{
		// Geometry Instance (make instance from the same geometry as rooms)
		SpatialAudioHelpers::AddBoxGeometryInstance(OBSTACLE_BASE_ID + obstacleIdx, OBSTACLE_GEOMETRY, obstacle.get(), m_fWidth, m_fHeight);
		obstacleIdx++;
	}
}

void DemoSpatialAudioPortalReflect::Init_Portals()
{
	const int NUM_PORTAL = 6;	// Number of portals in demo
	// Parameterize PORTAL dimention
	const float PORTAL_DEPTH = 60.f;
	const float HALF_PORTAL_DEPTH = PORTAL_DEPTH / 2.f;
	const float PORTAL_WIDTH = 100.f;
	const float HALF_PORTAL_WIDTH = PORTAL_WIDTH / 2.f;

	// Register each PORTAL to SpatialAudio
	for (int iPortal = 0; iPortal < NUM_PORTAL; iPortal++)
	{
		m_portals.push_back(std::make_unique<Box>(*this));
		auto& portal = m_portals.back();

		// Sets portal box position and dimension
		switch (iPortal)
		{
		case 0:
			// Portals who's axis points up/down
			portal->SetDimensions((int)PORTAL_WIDTH, (int)PORTAL_DEPTH);
			portal->SetPosition((int)(m_fWidth / 5.f - HALF_PORTAL_WIDTH), (int)(m_fHeight / 2.f - HALF_PORTAL_DEPTH));
			break;
		case 1:
			// Portals who's axis points up/down
			portal->SetDimensions((int)PORTAL_WIDTH, (int)PORTAL_DEPTH);
			portal->SetPosition((int)(4.f * m_fWidth / 5.f - HALF_PORTAL_WIDTH), (int)(m_fHeight / 2.f - HALF_PORTAL_DEPTH));
			break;
		case 2:
			// Portals who's axis points left/right
			portal->SetDimensions((int)PORTAL_DEPTH, (int)PORTAL_WIDTH);
			portal->SetPosition((int)(2.f * m_fWidth / 5.f - HALF_PORTAL_DEPTH), (int)(m_fHeight / 4.f - HALF_PORTAL_WIDTH));
			break;
		case 3:
			// Portals who's axis points left/right
			portal->SetDimensions((int)PORTAL_DEPTH, (int)PORTAL_WIDTH);
			portal->SetPosition((int)(3.f * m_fWidth / 5.f - HALF_PORTAL_DEPTH), (int)(m_fHeight / 4.f - HALF_PORTAL_WIDTH));
			break;
		case 4:
			// Portals who's axis points left/right
			portal->SetDimensions((int)PORTAL_DEPTH, (int)PORTAL_WIDTH);
			portal->SetPosition((int)(2.f * m_fWidth / 5.f - HALF_PORTAL_DEPTH), (int)(3.f * m_fHeight / 4.f - HALF_PORTAL_WIDTH));
			break;
		case 5:
			// Portals who's axis points left/right
			portal->SetDimensions((int)PORTAL_DEPTH, (int)PORTAL_WIDTH);
			portal->SetPosition((int)(3.f * m_fWidth / 5.f - HALF_PORTAL_DEPTH), (int)(3.f * m_fHeight / 4.f - HALF_PORTAL_WIDTH));
			break;
		default:
			break;
		}

		Set_Portal(iPortal);
	}
}

void DemoSpatialAudioPortalReflect::Set_Portal(int in_portalNum)
{
	float px_x, px_y, px_width, px_height;
	m_portals[in_portalNum]->GetRect(px_x, px_y, px_width, px_height);

	// Register PORTAL to SpatialAudio
	AkPortalParams paramsPortal;

	// Sets size and position/orienttion of PORTAL
	paramsPortal.Transform.SetPosition(	IntegrationDemoHelpers::PixelsToAKPos_X(m_fWidth, px_width/2.f + px_x), 1e-4f, // Nudge slightly upwards. Workaround for WG-66362.
		IntegrationDemoHelpers::PixelsToAKPos_Y(m_fHeight, px_height/2.f + px_y));
	if (in_portalNum < 2)
	{
		// Portals who's axis points up/down (Portal width = Box Width)
		paramsPortal.Transform.SetOrientation(0.f, 0.f, 1.f, 0.f, 1.f, 0.f); // facing +'ve Z
		paramsPortal.Extent.halfWidth = IntegrationDemoHelpers::PixelsToAKLen_X(m_fWidth, px_width / 2.f);
		paramsPortal.Extent.halfHeight = IntegrationDemoHelpers::PixelsToAKLen_Y(m_fHeight, px_width / 2.f);
		paramsPortal.Extent.halfDepth = IntegrationDemoHelpers::PixelsToAKLen_Y(m_fHeight, px_height / 2.f);
	}
	else
	{
		// Portals who's axis points left/right (Portal width = Box Height)
		paramsPortal.Transform.SetOrientation(1.f, 0.f, 0.f, 0.f, 1.f, 0.f);// facing +'ve X
		paramsPortal.Extent.halfWidth = IntegrationDemoHelpers::PixelsToAKLen_Y(m_fHeight, px_height / 2.f);
		paramsPortal.Extent.halfHeight = IntegrationDemoHelpers::PixelsToAKLen_Y(m_fHeight, px_height / 2.f);
		paramsPortal.Extent.halfDepth = IntegrationDemoHelpers::PixelsToAKLen_X(m_fWidth, px_width / 2.f);
	}

	// Enable (Open/Close) PORTAL
	paramsPortal.bEnabled = (m_portalsOpen & (0x1<<in_portalNum)) > 0;

	// Set front & back ROOM of PORTAL
	switch (in_portalNum)
	{
	case 0:
		paramsPortal.FrontRoom = ROOM_BASE_ID;
		paramsPortal.BackRoom = ROOM_BASE_ID + 2;
		AK::SpatialAudio::SetPortal(PORTAL_BASE_ID + in_portalNum, paramsPortal, "Portal0-2");
		break;
	case 1:
		paramsPortal.FrontRoom = ROOM_BASE_ID + 1;
		paramsPortal.BackRoom = ROOM_BASE_ID + 3;
		AK::SpatialAudio::SetPortal(PORTAL_BASE_ID + in_portalNum, paramsPortal, "Portal1-3");
		break;
	case 2:
		paramsPortal.FrontRoom = AkRoomID();
		paramsPortal.BackRoom = ROOM_BASE_ID;
		AK::SpatialAudio::SetPortal(PORTAL_BASE_ID + in_portalNum, paramsPortal, "Portal0-Outside");
		break;
	case 3:
		paramsPortal.FrontRoom = ROOM_BASE_ID + 1;
		paramsPortal.BackRoom = AkRoomID();
		AK::SpatialAudio::SetPortal(PORTAL_BASE_ID + in_portalNum, paramsPortal, "Portal1-Outside");
		break;
	case 4:
		paramsPortal.FrontRoom = AkRoomID();
		paramsPortal.BackRoom = ROOM_BASE_ID + 2;
		AK::SpatialAudio::SetPortal(PORTAL_BASE_ID + in_portalNum, paramsPortal, "Portal2-Outside");
		break;
	case 5:
		paramsPortal.FrontRoom = ROOM_BASE_ID + 3;
		paramsPortal.BackRoom = AkRoomID();
		AK::SpatialAudio::SetPortal(PORTAL_BASE_ID + in_portalNum, paramsPortal, "Portal3-Outside");
		break;
	default:
		break;
	}
}

void DemoSpatialAudioPortalReflect::Release()
{
	AK::SoundEngine::StopAll();
	delete m_pEmitterChip;
	delete m_pListenerChip;

	for (unsigned int i = 0; i < m_rooms.size(); i++)
	{
		AK::SpatialAudio::RemoveRoom(ROOM_BASE_ID + i);
		AK::SpatialAudio::RemoveGeometryInstance(ROOM_BASE_ID + i);
	}
	for (unsigned int i = 0; i < m_obstacles.size(); i++)
	{
		AK::SpatialAudio::RemoveGeometryInstance(OBSTACLE_BASE_ID + i);
	}
	for (unsigned int i = 0; i < m_portals.size(); i++)
	{
		AK::SpatialAudio::RemovePortal(PORTAL_BASE_ID + i);
	}
	AK::SpatialAudio::RemoveGeometry(ROOM_GEOMETRY);
	AK::SpatialAudio::RemoveGeometry(OBSTACLE_GEOMETRY);
	AK::SoundEngine::UnloadBank( "Reflect.bnk", NULL );

	m_diffractionLines.clear();
	m_reflectionLines.clear();
	m_reflectionOrders.clear();

	Page::Release();
}

bool DemoSpatialAudioPortalReflect::Update()
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

		if ((*it).IsButtonTriggered(UG_BUTTON1))
		{
			// toggle portals open state with listener containment check.
			float x, y;
			if ((*it).IsButtonDown(UG_BUTTON5))
				m_pEmitterChip->GetPos(x, y);
			else
				m_pListenerChip->GetPos(x, y);

			int iPortal= 0;
			for (auto& portal : m_portals)
			{
				if (portal->ContainsPoint(x, y))
				{
					m_portalsOpen = m_portalsOpen ^ (0x1<<iPortal);
					Set_Portal(iPortal);
					m_bMoved = true;
					break;
				}
				iPortal++;
			}
		}

		if ((*it).IsButtonTriggered(UG_BUTTON6))
		{
			m_pListenerChip->AddToPlayerOffset(5.f);
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
		// Update emitters, set room for spatial audio.
		m_pEmitterChip->SetPosition();
		m_pEmitterChip->UpdateRoom(m_rooms, ROOM_BASE_ID);

		// Update listener, set room for spatial audio.
		m_pListenerChip->SetPosition();
		m_pListenerChip->UpdateRoom(m_rooms, ROOM_BASE_ID);
		m_pListenerChip->UpdateDistanceProbeRoom(m_rooms, ROOM_BASE_ID);

		//
		// Query Diffraction Path for UI display.
		//
		m_diffractionLines.clear();

		AkVector64 emitterPos, listenerPos;
		AkDiffractionPathInfo diffractionPaths[8];
		AkUInt32 uNumPaths = 8;	// 8 arbitrary
		AK::SpatialAudio::QueryDiffractionPaths(
			EMITTER,	///< The ID of the game object that the client wishes to query.
			0,				///< Position index 0, we only have one.
			listenerPos,	///< Returns the position of the listener game object that is associated with the game object \c in_gameObjectID.
			emitterPos,		///< Returns the position of the emitter game object \c in_gameObjectID.
			diffractionPaths,			///< Pointer to an array of \c AkPropagationPathInfo's which will be filled after returning.
			uNumPaths		///< The number of slots in \c out_aPaths, after returning the number of valid elements written.
			);

		m_dryDiffraction = 0.f;
		m_wetDiffraction = 0.f;
		m_transmissionLoss = 0.f;

		if (uNumPaths > 0)
		{
			float dryDiffraction = 1.f;
			float wetDiffraction = 1.f;
			float transmissionLoss = 0.f;
			bool wetDiffractionSet = false;

			for (AkUInt32 path = 0; path < uNumPaths; path++)
			{
				// Create line segments between each node.
				// Meanwhile, find out whether one of these nodes is a portal and store its ID.
				if (diffractionPaths[path].nodeCount > 0)
				{
					// Listener to node 0.
					m_diffractionLines.push_back(std::make_unique<Line>(*this));
					m_diffractionLines.back()->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, listenerPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, listenerPos.Z), 
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, diffractionPaths[path].nodes[0].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, diffractionPaths[path].nodes[0].Z));

					AkPortalID portalID = diffractionPaths[path].portals[0];
					AkUInt32 node = 1;
					while (node < diffractionPaths[path].nodeCount)
					{
						m_diffractionLines.push_back(std::make_unique<Line>(*this));
						m_diffractionLines.back()->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, diffractionPaths[path].nodes[node - 1].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, diffractionPaths[path].nodes[node - 1].Z), 
							IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, diffractionPaths[path].nodes[node].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, diffractionPaths[path].nodes[node].Z));
						if (!portalID.IsValid())
							portalID = diffractionPaths[path].portals[node];
						++node;
					}

					// Last node to emitter.
					m_diffractionLines.push_back(std::make_unique<Line>(*this));
					m_diffractionLines.back()->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, diffractionPaths[path].nodes[node - 1].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, diffractionPaths[path].nodes[node - 1].Z),
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, emitterPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, emitterPos.Z));

					// Query portal's wet diffraction.
					AkReal32 portalWetDiffraction;
					if (portalID.IsValid() 
						&& AK::SpatialAudio::QueryWetDiffraction(portalID, portalWetDiffraction) == AK_Success
						&& portalWetDiffraction < wetDiffraction)
					{
						// Pick wetDiffraction as the smallest wet diffraction value across parallel paths.
						wetDiffraction = portalWetDiffraction;
						wetDiffractionSet = true;
					}
				}
				else
				{
					// No node: Listener to emitter.
					m_diffractionLines.push_back(std::make_unique<Line>(*this));
					m_diffractionLines.back()->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, listenerPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, listenerPos.Z),
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, emitterPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, emitterPos.Z));
				}

				// Display the minimum diffraction amount of the paths. Since we only have Room (i.e. game object) scoped RTPCs in Wwise at the moment, this is the actual value that will be pushed
				// to our built-in game parameter "Diffraction".
				if (diffractionPaths[path].transmissionLoss == 0.f)
					dryDiffraction = AkMin(dryDiffraction, diffractionPaths[path].diffraction);
				else
					transmissionLoss = diffractionPaths[path].transmissionLoss;
			}

			m_dryDiffraction = dryDiffraction * 100.f;
			if (wetDiffractionSet)
				m_wetDiffraction = wetDiffraction * 100.f;
			else
				m_wetDiffraction = 0.f;
			m_transmissionLoss = transmissionLoss * 100.f;
		}

		//
		// Query Reflection Path for UI display.
		//
		m_reflectionLines.clear();
		m_reflectionOrders.clear();

		AkReflectionPathInfo reflectionPaths[16];
		uNumPaths = 16;	// 16 arbitrary
		AK::SpatialAudio::QueryReflectionPaths(
			EMITTER,		///< The ID of the game object that the client wishes to query.
			0,				///< Position index 0, we only have one.
			listenerPos,	///< Returns the position of the listener game object that is associated with the game object \c in_gameObjectID.
			emitterPos,		///< Returns the position of the emitter game object \c in_gameObjectID.
			reflectionPaths,			///< Pointer to an array of \c AkPropagationPathInfo's which will be filled after returning.
			uNumPaths		///< The number of slots in \c out_aPaths, after returning the number of valid elements written.
			);

		for (AkUInt32 path = 0; path < uNumPaths; path++)
		{
			int numPathPoints = reflectionPaths[path].numPathPoints;
			int numReflections = reflectionPaths[path].numReflections;

			if (numPathPoints > 0)
			{
				// Emitter to path point 0.
				m_reflectionLines.push_back(std::make_unique<Line>(*this));
				m_reflectionLines.back()->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, emitterPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, emitterPos.Z),
					IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, reflectionPaths[path].pathPoint[0].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, reflectionPaths[path].pathPoint[0].Z));
				m_reflectionOrders.push_back(numReflections);

				// path point i-1 to path point i
				for (int i = 1; i < numPathPoints; i++)
				{
					m_reflectionLines.push_back(std::make_unique<Line>(*this));
					m_reflectionLines.back()->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, reflectionPaths[path].pathPoint[i - 1].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, reflectionPaths[path].pathPoint[i - 1].Z),
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, reflectionPaths[path].pathPoint[i].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, reflectionPaths[path].pathPoint[i].Z));
					m_reflectionOrders.push_back(numReflections);
				}
					
				// Last path point to listener.
				m_reflectionLines.push_back(std::make_unique<Line>(*this));
				m_reflectionLines.back()->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, reflectionPaths[path].pathPoint[numPathPoints - 1].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, reflectionPaths[path].pathPoint[numPathPoints - 1].Z),
					IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, listenerPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, listenerPos.Z));
				m_reflectionOrders.push_back(numReflections);
			}
		}
	}

	// Reset m_bMoved.
	m_bMoved = false;

	return Page::Update();
}

void DemoSpatialAudioPortalReflect::Draw()
{
	Page::Draw();
	float lineColor[3];

	if (m_pEmitterChip)
		m_pEmitterChip->Draw();
	if (m_pListenerChip)
		m_pListenerChip->Draw();

	for (auto& room : m_rooms)
		room->Draw();

	// Concrete texture color
	lineColor[0] = 0.5f;
	lineColor[1] = 0.5f;
	lineColor[2] = 0.5f;
	for (auto& obstacle : m_obstacles)
		obstacle->Draw(lineColor);

	int iPortal = 0;
	for (auto& portal : m_portals)
	{
		if ((m_portalsOpen & (0x1<<iPortal)) > 0)
			portal->Draw(DrawStyle_Selected);
		else
			portal->Draw(DrawStyle_Error);
		iPortal++;
	}

	for (auto& diffractionLine : m_diffractionLines)
		diffractionLine->Draw();

	int i = 0;
	for (auto& reflectionLine : m_reflectionLines)
	{
		switch (m_reflectionOrders[i++])
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
		reflectionLine->Draw(lineColor);
	}

	float fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ;
	m_pListenerChip->GetAkPos(fAkPosListenerX, fAkPosListenerZ);
	m_pEmitterChip->GetAkPos(fAkPosEmitterX, fAkPosEmitterZ);

	char strBuf[128];
	snprintf(strBuf, 128, "[L](X,Z): %.2f  %.2f\n[E](X,Z): %.2f  %.2f\nDiffraction dry: %.2f\nDiffraction wet: %.2f\nTransmission Loss: %.2f",
		fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ, m_dryDiffraction, m_wetDiffraction, m_transmissionLoss);

	static int s_nOffset = 6 * GetLineHeight( DrawStyle_Text );
	DrawTextOnScreen( strBuf, 5, m_pParentMenu->GetHeight() - s_nOffset, DrawStyle_Text );

	// Display instructions at the bottom of the page
	int iInstructionsY = m_pParentMenu->GetHeight() - 3 * GetLineHeight( DrawStyle_Text );
	DrawTextOnScreen( "(Press <<UG_BUTTON2>> To Go Back...)", (int)(m_pParentMenu->GetWidth() * 0.4), iInstructionsY, DrawStyle_Text );
}

bool DemoSpatialAudioPortalReflect::OnPointerEvent(PointerEventType in_eType, int in_x, int in_y)
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