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

// DemoSpatialAudio.cpp
/// \file
/// Defines all methods declared in DemoSpatialAudio.h

#include "stdafx.h"

#include <math.h>
#include <float.h>
#include "Menu.h"
#include "Helpers.h"
#include "DemoSpatialAudio.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

#define _USE_MATH_DEFINES
#include <math.h>

//If you get a compiling error here, it means the file wasn't generated with the banks.  Did you generate the soundbanks before compiling?
#include "../WwiseProject/GeneratedSoundBanks/Wwise_IDs.h"		

// Our game object ID.  Completely arbitrary.
// IMPORTANT: As stated in AK::SpatialAudio::AddRoom and AddPortal, room and portal IDs exist in the same ID-space than game objects.
const AkGameObjectID EMITTER = 100;
const AkGameObjectID LISTENER = 103;
const AkGameObjectID DISTANCE_PROBE = 104;
const AkRoomID ROOM = 200;
const AkPortalID PORTAL0 = 300;
const AkPortalID PORTAL1 = 301;
const AkGeometrySetID GEOMETRY_OUTER_ROOM = 0;
const AkGeometrySetID GEOMETRY_INNER_ROOM = 1;
const AkGeometrySetID GEOMETRY_OBSTACLE = 2;

#define REPEAT_TIME 20

/////////////////////////////////////////////////////////////////////
// DemoSpatialAudio Public Methods
/////////////////////////////////////////////////////////////////////

DemoSpatialAudio::DemoSpatialAudio(Menu& in_ParentMenu, DemoSpatialAudio_Scenario in_scenario)
	: Page( in_ParentMenu, (in_scenario == Scenario_Portals) ? "Portal Demo" : "Portal and Geometry Demo")
	, m_pEmitterChip( NULL )
	, m_pListenerChip(NULL)
	, m_pRoom(NULL)
	, m_pPortal0(NULL)
	, m_pPortal1(NULL)
	, m_aLines(NULL)
	, m_numLines(0)
	, m_dryDiffraction( 0 )
	, m_wetDiffraction( 0 )
	, m_fWidth( 0.0f )
	, m_fHeight(0.0f)
	, m_portalsOpen(0x3)	// both portals open
	, m_uLastTick(0)
	, m_bTestNoise(false)
	, m_bMoved(true)
	, m_eScenario(in_scenario)
	, m_pGameSideObs(NULL)
	, m_pGeometryInRooms(NULL)
{
	switch (in_scenario)
	{
	case Scenario_Portals:
		m_szHelp =
			"In this demo we have 2 Rooms, ROOM (box in the upper left) and Outside, 2 Portals (PORTAL0 in the upper right, PORTAL1 in the lower-left), a listener/[L], and an emitter/[E].\n \n"
			"The sound propagation path(s) are displayed, with the resulting diffraction amounts in the lower-left corner. "
			"Spatial Audio is set up so that diffraction controls both project-wide obstruction and the Diffraction built-in parameter, although only the former is used in the Wwise project. "
			"Same-room obstruction (emitter-listener and portal-listener) is calculated through a combination of Portal-driven propagation and a native game-side obstruction algorithm. "
			"Finally, this demo shows how to use a Room to play multi-channel ambient sounds / room tones that contract and become point sources at Portals.\n"
			"Refer to Rooms and Portals in the SDK documentation for more details.\n \n"
			"Listener and emitter controls are specified under the Spatial Audio Demos menu help page.\n"
			"* <<UG_BUTTON1>> - Open/close Portals in sequence.";
		break;
	case Scenario_Portals_And_Geometry:
		m_szHelp =
			"In this demo we have 2 Rooms, ROOM (box in the upper left) and Outside, 2 Portals (PORTAL0 in the upper right, PORTAL1 in the lower-left), "
			"Geometry for the wall inside and outside of ROOM and an obstacle, a listener/[L], and an emitter/[E].\n \n"
			"The sound propagation path(s) are displayed, with the resulting diffraction amounts in the lower-left corner. "
			"Spatial Audio is set up so that diffraction controls both project-wide obstruction and the built-in Diffraction parameter, although only the former is used in the Wwise project. "
			"Spatial Audio handles diffraction and transmission using Portals and Geometry respectively. The demo does not compute additional obstruction and occlusion."
			"Finally, this demo shows how to use Geometry along with Rooms and Portals. "
			"Refer to Rooms and Portals and Geometry API in the SDK documentation for more details.\n \n"
			"Listener and emitter controls are specified under the Spatial Audio Demos menu help page.\n"
			"* <<UG_BUTTON1>> - Open/close Portals in sequence.";
		break;
	}
}

bool DemoSpatialAudio::Init()
{
	// Load the sound bank
	AkBankID bankID; // Not used
	if ( AK::SoundEngine::LoadBank( "Spatial_Audio_Demo.bnk", bankID ) != AK_Success )
	{
		SetLoadFileErrorMessage( "Spatial_Audio_Demo.bnk" );
		return false;
	}

	m_uLastTick = m_pParentMenu->GetTickCount();

	return Page::Init();
}

void DemoSpatialAudio::InitControls()
{
	float fMargin = (float)m_pEmitterChip->GetRightBottomMargin();
	m_fWidth = (float) m_pParentMenu->GetWidth() - fMargin;
	m_fHeight = (float) m_pParentMenu->GetHeight() - fMargin;
	float centerX = m_fWidth / 2.0f;
	float centerY = m_fHeight / 2.0f;

	// Register listener in spatial audio and init chip controls
	m_pListenerChip = new SpatialAudioListener(*this, LISTENER, DISTANCE_PROBE, "Listener L");
	m_pListenerChip->SetLabel("<L>");
	m_pListenerChip->UseJoystick(UG_STICKRIGHT);
	m_pListenerChip->SetNonLinear();
	m_pListenerChip->SetPos(centerX + 30, centerY + 30);
	m_pListenerChip->SetMaxSpeed(3.f);

	// Register emitter in spatial audio and init chip controls
	m_pEmitterChip = new SpatialAudioGameObject(*this, EMITTER, LISTENER, "Emitter E");
	m_pEmitterChip->SetLabel("<E>");
	m_pEmitterChip->UseJoystick(UG_STICKRIGHT);
	m_pEmitterChip->SetNonLinear();
	m_pEmitterChip->SetPos(centerX - 100, centerY - 100);
	m_pEmitterChip->SetMaxSpeed(3.f);

	// In the Portals (only) demo, the "game" takes care of computing and setting obstruction between the emitter and listener when they are in the same room, and between portals and the listener.
	// This is implemented in GameSideObstructionComponent. No geometry in passed to Wwise Spatial Audio in that demo, and m_pGeometryInRooms is thus NULL.
	// In the Portals + Geometry demo, propagation of sound via diffraction effects is entirely handled by Wwise Spatial Audio, either via Portals or geometry, and thus m_pGameSideObs is not needed and left NULL.
	InitControls_Portals();
	switch (m_eScenario)
	{
	case Scenario_Portals:
		m_pGameSideObs = new GameSideObstructionComponent;
		AK::SoundEngine::PostEvent("Play_Room_Emitter", EMITTER);
		break;
	case Scenario_Portals_And_Geometry:
		m_pGeometryInRooms = new GeometryInRooms(this);
		AK::SoundEngine::PostEvent("Play_Room_Emitter", EMITTER);
		break;
	}

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
		m_pEmitterChip->UpdateRoom(m_pRoom, ROOM);
		m_pListenerChip->UpdateRoom(m_pRoom, ROOM);
		m_pListenerChip->UpdateDistanceProbeRoom(m_pRoom, ROOM);
	}
}

// Called at init but also every time we open or close (all) doors.
void DemoSpatialAudio::InitControls_Portals()
{
	m_pRoom = new Box(*this);
	m_pPortal0 = new Box(*this);
	m_pPortal1 = new Box(*this);

	// Register rooms to Spatial Audio.
	float roomWidth = m_fWidth / 2.f;
	float roomHeight = m_fHeight / 2.f;
	m_pRoom->SetPosition(0, 0);
	m_pRoom->SetDimensions((int)roomWidth, (int)roomHeight);

	// Set emitter radius. Game object radius allows games to define a radius (typically based off the geometry of the sound source) that is used by Spatial Audio for computing Spread. 
	// Attenuation curves supersede game-driven radii. This is why the emitter's attenuation ShareSet used in this demo does not have a Spread curve.
	AK::SpatialAudio::SetGameObjectRadius(EMITTER, IntegrationDemoHelpers::PixelsToAKLen_X(m_fWidth, roomWidth / 8.f), IntegrationDemoHelpers::PixelsToAKLen_X(m_fWidth, roomWidth / 12.f));

	AkRoomParams paramsRoom;
	// Let's orient our rooms towards the top of the screen. 
	paramsRoom.Front.X = 0.f;
	paramsRoom.Front.Y = 0.f;
	paramsRoom.Front.Z = 1.f;
	paramsRoom.Up.X = 0.f;
	paramsRoom.Up.Y = 1.f;
	paramsRoom.Up.Z = 0.f;
	paramsRoom.TransmissionLoss = 0.9f;	// Let's have a bit of sound transmitted through walls when all portals are closed.
	paramsRoom.RoomGameObj_KeepRegistered = true;	// We intend to use the room's game object to post events (see documentation of AkRoomParams::RoomGameObj_KeepRegistered).
	paramsRoom.RoomGameObj_AuxSendLevelToSelf = 0.25f;	// Since we will be playing an ambience ("Play_Ambience_Quad", below), on this room's game object, we here route some of it to the room's auxiliary bus to add some of its reverb.
	paramsRoom.ReverbAuxBus = AK::SoundEngine::GetIDFromString("Room");
	paramsRoom.GeometryInstanceID = GEOMETRY_OUTER_ROOM;	// We associate the geometry to the room in order to compute the room spread. 
													// If the geometry is not found (in "Portals Demo"), the room bounding box is calculated from the portals combined extent.
	
	AK::SpatialAudio::SetRoom(ROOM, paramsRoom, "Room Object");
	
	// Also register "outside". Outside is already created automatically, but we want to give it a name, and set its aux send (reverb)
	paramsRoom.Front.X = 0.f;
	paramsRoom.Front.Y = 0.f;
	paramsRoom.Front.Z = 1.f;
	paramsRoom.Up.X = 0.f;
	paramsRoom.Up.Y = 1.f;
	paramsRoom.Up.Z = 0.f;
	
	paramsRoom.TransmissionLoss = 0.f;
	paramsRoom.RoomGameObj_KeepRegistered = false;
	paramsRoom.ReverbAuxBus = AK::SoundEngine::GetIDFromString("Outside");
	paramsRoom.GeometryInstanceID = AkGeometryInstanceID(); // Invalid ID - no geometry for outside.
	AK::SpatialAudio::SetRoom(AK::SpatialAudio::kOutdoorRoomID, paramsRoom, "Outside Object");

	SetPortals();

	// Play a 4.0 ambient sound (room tone) on the ROOM game object. Leveraging the processing performed by Spatial Audio, the room tone, which is set to
	// 3D Spatialization in Wwise, will 
	// - rotate according to the listener orientation when the listener is in the room (this is not actually demonstrated since the listener never rotates);
	// - expand (using spread) as the listener penetrates the room;
	// - contract to portals as the listener walks away from the room.
	AK::SoundEngine::PostEvent("Play_Ambience_Quad", ROOM.AsGameObjectID());
}

void DemoSpatialAudio::Release()
{
	AK::SoundEngine::StopAll();
	AK::SpatialAudio::RemovePortal(PORTAL0);
	AK::SpatialAudio::RemovePortal(PORTAL1);
	AK::SpatialAudio::RemoveRoom(ROOM);
	AK::SpatialAudio::RemoveRoom(AK::SpatialAudio::kOutdoorRoomID);
	delete m_pEmitterChip;
	delete m_pListenerChip;
	AK::SoundEngine::UnloadBank("Spatial_Audio_Demo.bnk", NULL);

	for (int i = 0; i < m_numLines; i++)
	{
		if (m_aLines[i])
			delete m_aLines[i];
	}
	delete[] m_aLines;

	delete m_pGameSideObs;
	delete m_pGeometryInRooms;

	Page::Release();
}

void DemoSpatialAudio::SetPortals()
{
	const float PORTAL_MARGIN = 15.f;
	float px_roomX, px_roomY, px_roomWidth, px_roomHeight;
	m_pRoom->GetRect(px_roomX, px_roomY, px_roomWidth, px_roomHeight);
	//
	// Portal 0 (ROOM->Outside, like an horizontal tube, on the top right)
	//
	const float PORTAL0_WIDTH = 90.f;	// width in screen coordinates (left-right). This is actually used for the portal's depth.
	const float PORTAL0_HEIGHT = 60.f;
	float portal0TopLeftX = px_roomWidth - PORTAL0_WIDTH / 2.f;
	float portal0TopLeftZ = PORTAL_MARGIN;

	m_pPortal0->SetPosition((int)portal0TopLeftX, (int)portal0TopLeftZ);
	m_pPortal0->SetDimensions((int)PORTAL0_WIDTH, (int)PORTAL0_HEIGHT);
	
	// Register portals to SpatialAudio.
	AkPortalParams paramsPortal;

	float centerX_px, centerY_px;
	m_pPortal0->GetCenter(centerX_px, centerY_px);
	float x = IntegrationDemoHelpers::PixelsToAKPos_X(m_fWidth, centerX_px);
	float z = IntegrationDemoHelpers::PixelsToAKPos_Y(m_fHeight, centerY_px);
	paramsPortal.Transform.SetPosition(x, 0.f, z);

	// Points to the right.
	// Up vector: This is a 2D game with Y pointing towards the player, and so should the local Y.
	paramsPortal.Transform.SetOrientation(1.f, 0.f, 0.f, 0.f, 1.f, 0.f);

	// Portal extent. Defines the dimensions of the portal relative to its center; all components must be positive numbers. The local X and Y dimensions (side and top) are used in diffraction calculations, 
	// whereas the Z dimension (front) defines a depth value which is used to implement smooth transitions between rooms. It is recommended that users experiment with different portal depths to find a value 
	// that results in appropriately smooth transitions between rooms.
	// Important: divide height and width by 2, because Extent expresses dimensions relative to the center (like a radius).
	paramsPortal.Extent.halfWidth = IntegrationDemoHelpers::PixelsToAKLen_Y(m_fHeight, PORTAL0_HEIGHT / 2.f);	// Door width. Note: PORTAL0_HEIGHT is screen coordinates (up-down).
	paramsPortal.Extent.halfHeight = IntegrationDemoHelpers::PixelsToAKLen_Y(m_fHeight, PORTAL0_HEIGHT / 2.f);	// This app is 2D. Just pick a non-zero value so that the top and bottom edges are never chosen for diffraction.
	paramsPortal.Extent.halfDepth = IntegrationDemoHelpers::PixelsToAKLen_X(m_fWidth, PORTAL0_WIDTH / 2.f);	// Portal depth (transition region). Note: PORTAL0_WIDTH is screen coordinates (left-right).

	// Whether or not the portal is active/enabled. For example, this parameter may be used to simulate open/closed doors.
	paramsPortal.bEnabled = (m_portalsOpen & 0x1) > 0;	// Open if bit 0 of our portal open state m_portalsOpen is set.
	// ID of the room that the portal connects to, in the direction of the Front vector.
	paramsPortal.FrontRoom = AK::SpatialAudio::kOutdoorRoomID;
	// ID of room that that portal connects, in the direction opposite to the Front vector. 
	paramsPortal.BackRoom = ROOM;

	AK::SpatialAudio::SetPortal(PORTAL0, paramsPortal, "Portal ROOM->Outside, horizontal");

	//
	// Portal 1 (Outside->ROOM, like a wide vertical tube, bottom-left).
	//
	float portal1Width = px_roomWidth - 2.f * PORTAL_MARGIN;
	const float PORTAL1_HEIGHT = 90.f;
	float portal1TopLeftX = PORTAL_MARGIN;
	float portal1TopLeftZ = px_roomHeight - PORTAL1_HEIGHT / 2.f;

	m_pPortal1->SetPosition((int)portal1TopLeftX, (int)portal1TopLeftZ);
	m_pPortal1->SetDimensions((int)portal1Width, (int)PORTAL1_HEIGHT);

	m_pPortal1->GetCenter(centerX_px, centerY_px);
	x = IntegrationDemoHelpers::PixelsToAKPos_X(m_fWidth, centerX_px);
	z = IntegrationDemoHelpers::PixelsToAKPos_Y(m_fHeight, centerY_px);
	paramsPortal.Transform.SetPosition(x, 1e-4f, z); // Nudge slightly upwards. Workaround for WG-66362.

	// Points up towards ROOM.
	// Up vector: This is a 2D game with Y pointing towards the player, and so should the local Y.
	paramsPortal.Transform.SetOrientation(0.f, 0.f, 1.f, 0.f, 1.f, 0.f);

	// Portal extent. Defines the dimensions of the portal relative to its center; all components must be positive numbers. The local X and Y dimensions (side and top) are used in diffraction calculations, 
	// whereas the Z dimension (front) defines a depth value which is used to implement smooth transitions between rooms. It is recommended that users experiment with different portal depths to find a value 
	// that results in appropriately smooth transitions between rooms.
	// Important: divide width and height by 2, because Extent expresses dimensions relative to the center (like a radius).
	paramsPortal.Extent.halfWidth = IntegrationDemoHelpers::PixelsToAKLen_X(m_fWidth, portal1Width / 2.f);	// Door width. 
	paramsPortal.Extent.halfHeight = 30.f;	// This app is 2D. Let's pick a non-zero value so that the top and bottom edges are never chosen for diffraction, but large enough so that it does not skew the 
											// portal's aperture, which is based off the area defined by the extent's width and height.
	paramsPortal.Extent.halfDepth = IntegrationDemoHelpers::PixelsToAKLen_Y(m_fHeight, PORTAL1_HEIGHT / 2.f);	// Portal depth (transition region). 
	
	/// Whether or not the portal is active/enabled. For example, this parameter may be used to simulate open/closed doors.
	paramsPortal.bEnabled = (m_portalsOpen & 0x2) > 0;	// Open if bit 1 of our portal open state m_portalsOpen is set.
	// ID of the room that the portal connects to, in the direction of the Front vector.
	paramsPortal.FrontRoom = ROOM;
	// ID of room that that portal connects, in the direction opposite to the Front vector. 
	paramsPortal.BackRoom = AK::SpatialAudio::kOutdoorRoomID;

	AK::SpatialAudio::SetPortal(PORTAL1, paramsPortal, "Outside->ROOM, vertical");
}

// Naive containment test.
// Returns the room index.
AkRoomID DemoSpatialAudio::IsInRoom(float in_x, float in_y)
{
	if (m_pRoom->ContainsPoint(in_x, in_y))
		return ROOM;
	return AK::SpatialAudio::kOutdoorRoomID;
}

bool DemoSpatialAudio::Update()
{
	const float kAngleIncrement = M_PI / 180.f;

	UniversalInput::Iterator it;
	for ( it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++ )
	{
		// Skip this input device if it's not connected
		if ( ! it->IsConnected() )
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

		if ((*it).IsButtonDown(UG_BUTTON5))
		{
			m_pEmitterChip->Update(*it);
			m_bMoved = true;
		}
		else
		{
			float xprev, yprev;
			m_pListenerChip->MovableChip::GetPos(xprev, yprev);
			m_pListenerChip->Update(*it);
			float x, y; 
			m_pListenerChip->MovableChip::GetPos(x, y);
			if ((m_uLastTick + REPEAT_TIME) <= (AkUInt32)m_pParentMenu->GetTickCount()
				&& (((x - xprev)*(x - xprev) + (y - yprev)*(y - yprev)) > 0.1f))
			{
				AK::SoundEngine::PostEvent("Play_Footstep", LISTENER);
				m_uLastTick = m_pParentMenu->GetTickCount();
			}

			m_bMoved = true;
		}

		if ((*it).IsButtonTriggered(UG_BUTTON1))
		{
			// toggle portals open state.
			m_portalsOpen = (m_portalsOpen + 1) & 0x3;
			SetPortals();
			m_bMoved = true;
		}

		if ((*it).IsButtonTriggered(UG_BUTTON6))
		{
			m_pListenerChip->AddToPlayerOffset(5.f);
			m_bMoved = true;
		}

		if ((*it).IsButtonTriggered(UG_BUTTON9))
		{
			AK::SoundEngine::StopAll(EMITTER);
			if (m_bTestNoise)
			{
				AK::SoundEngine::PostEvent("Play_Room_Emitter", EMITTER);
				m_bTestNoise = false;
			}
			else
			{
				AK::SoundEngine::PostEvent("Play_Test_Noise", EMITTER);
				m_bTestNoise = true;
			}
		}
	}

	if ( m_bMoved )
	{
		// Update emitters, set room for spatial audio.
		m_pEmitterChip->SetPosition();
		m_pEmitterChip->UpdateRoom(m_pRoom, ROOM);

		// Update listener, set room for spatial audio.
		m_pListenerChip->SetPosition();
		m_pListenerChip->UpdateRoom(m_pRoom, ROOM);
		m_pListenerChip->UpdateDistanceProbeRoom(m_pRoom, ROOM);

		if (m_pGameSideObs)
			m_pGameSideObs->Update(this);

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
		AkDiffractionPathInfo paths[8];
		AkUInt32 uNumPaths = 8;	// 8 arbitrary
		AK::SpatialAudio::QueryDiffractionPaths(
			EMITTER,	///< The ID of the game object that the client wishes to query.
			0,				///< Position index 0, we only have one.
			listenerPos,	///< Returns the position of the listener game object that is associated with the game object \c in_gameObjectID.
			emitterPos,		///< Returns the position of the emitter game object \c in_gameObjectID.
			paths,			///< Pointer to an array of \c AkPropagationPathInfo's which will be filled after returning.
			uNumPaths		///< The number of slots in \c out_aPaths, after returning the number of valid elements written.
			);

		// Count lines to draw.
		for (AkUInt32 path = 0; path < uNumPaths; path++)
		{
			int nodeCount = paths[path].nodeCount;
			int numSegments = nodeCount + 1;
			m_numLines += numSegments;
		}

		m_dryDiffraction = 0.f;
		m_wetDiffraction = 0.f;
		m_transmissionLoss = 0.f;

		if (m_numLines > 0)
		{
			float dryDiffraction = 1.f;
			float wetDiffraction = 1.f;
			float transmissionLoss = 0.f;
			bool wetDiffractionSet = false;

			m_aLines = new Line*[m_numLines];

			int line = 0;
			for (AkUInt32 path = 0; path < uNumPaths; path++)
			{
				// Create line segments between each node. 
				// Meanwhile, find out whether one of these nodes is a portal and store its ID.
				if (paths[path].nodeCount > 0)
				{
					// Listener to node 0.
					m_aLines[line] = new Line(*this);
					m_aLines[line++]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, listenerPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, listenerPos.Z), 
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].nodes[0].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].nodes[0].Z));

					AkPortalID portalID = paths[path].portals[0];

					AkUInt32 node = 1;
					while (node < paths[path].nodeCount)
					{
						m_aLines[line] = new Line(*this);
						m_aLines[line++]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].nodes[node - 1].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].nodes[node - 1].Z), 
							IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].nodes[node].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].nodes[node].Z));
						if (!portalID.IsValid())
							portalID = paths[path].portals[node];
						++node;
					}
					// Last node to emitter.
					m_aLines[line] = new Line(*this);
					m_aLines[line++]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, paths[path].nodes[node - 1].X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, paths[path].nodes[node - 1].Z),
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
					m_aLines[line] = new Line(*this);
					m_aLines[line++]->SetPoints(IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, listenerPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, listenerPos.Z),
						IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, emitterPos.X), IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, emitterPos.Z));
				}

				// Display the minimum diffraction amount of the paths. Since we only have Room (i.e. game object) scoped RTPCs in Wwise at the moment, this is the actual value that will be pushed
				// to our built-in game parameter "Diffraction".
				if (paths[path].transmissionLoss == 0.f)
					dryDiffraction = AkMin(dryDiffraction, paths[path].diffraction);
				else
					transmissionLoss = paths[path].transmissionLoss;

			}
			m_dryDiffraction = dryDiffraction * 100.f;
			if (wetDiffractionSet)
				m_wetDiffraction = wetDiffraction * 100.f;
			else
				m_wetDiffraction = 0.f;
			m_transmissionLoss = transmissionLoss * 100.f;
		}
		else
		{
			m_dryDiffraction = 0.f;
			m_wetDiffraction = 0.f;
			m_transmissionLoss = 0.f;
		}
	}

	// Reset m_bMoved.
	m_bMoved = false;

	return Page::Update();
}

void DemoSpatialAudio::Draw()
{
	Page::Draw();

	if (m_pEmitterChip)
		m_pEmitterChip->Draw();
	if (m_pListenerChip)
		m_pListenerChip->Draw();

	if (m_pRoom)
		m_pRoom->Draw(DrawStyle_Selected);
	if (m_pPortal0)
	{
		if ((m_portalsOpen & 0x1) > 0)
			m_pPortal0->Draw(DrawStyle_Selected);
		else
			m_pPortal0->Draw(DrawStyle_Error);
	}
	if (m_pPortal1)
	{
		if ((m_portalsOpen & 0x2) > 0)
			m_pPortal1->Draw(DrawStyle_Selected);
		else
			m_pPortal1->Draw(DrawStyle_Error);
	}

	for (int i = 0; i < m_numLines; i++)
	{
		if (m_aLines[i])
			m_aLines[i]->Draw();
	}

	if (m_pGeometryInRooms)
		m_pGeometryInRooms->Draw();

	if (m_pGameSideObs)
		m_pGameSideObs->Draw();

	float fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ;
	m_pListenerChip->GetAkPos(fAkPosListenerX, fAkPosListenerZ);
	m_pEmitterChip->GetAkPos(fAkPosEmitterX, fAkPosEmitterZ);

	char strBuf[128];
	if (m_numLines)
		snprintf(strBuf, 128, "[L](X,Z): %.2f  %.2f\n[E](X,Z): %.2f  %.2f\nDiffraction dry: %.2f\nDiffraction wet: %.2f\nTransmission Loss: %.2f",
			fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ, m_dryDiffraction, m_wetDiffraction, m_transmissionLoss);
	else
		snprintf(strBuf, 128, "[L](X,Z): %.2f  %.2f\n[E](X,Z): %.2f  %.2f", fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ);

	// strBuf does contain 5 lines but not making room for an empty line do crash the display on Android device (Pixel 6), setting max lines to 6
	static const int MAX_LINES_FOR_TEXT = 6;
	static int s_nOffset = MAX_LINES_FOR_TEXT * GetLineHeight( DrawStyle_Text );

	DrawTextOnScreen( strBuf, 5, m_pParentMenu->GetHeight() - s_nOffset, DrawStyle_Text );
	
	// Display instructions at the bottom of the page
	int iInstructionsY = m_pParentMenu->GetHeight() - 3 * GetLineHeight( DrawStyle_Text );
	DrawTextOnScreen( "(Press <<UG_BUTTON2>> To Go Back...)", (int)(m_pParentMenu->GetWidth() * 0.4), iInstructionsY, DrawStyle_Text );
}

bool DemoSpatialAudio::OnPointerEvent(PointerEventType in_eType, int in_x, int in_y)
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


/////////////////////////////////////////////////////////////////////
// GameSideObstructionComponent:
// Implements game-side obstruction when emitters and listeners are in the same room.
/////////////////////////////////////////////////////////////////////
GameSideObstructionComponent::GameSideObstructionComponent()
	:m_aRaycastEtoL(NULL)
{
}

GameSideObstructionComponent::~GameSideObstructionComponent()
{
	delete m_aRaycastEtoL;
}

void GameSideObstructionComponent::Draw()
{
	if (m_aRaycastEtoL)
		m_aRaycastEtoL->Draw(DrawStyle_Selected);
}

void GameSideObstructionComponent::Update(DemoSpatialAudio * in_pDemo)
{
	// Perform naive ray-casting for same-room obstruction.
	// We use game-side ray-casting to compute the obstruction between the listener and each portal.
	// We also use it to compute obstruction between the emitter and listener *only when they are in the same room*. When they aren't, obstruction flows from more diffraction/transmission modeling of SpatialAudio's rooms and portals.
	const float k_obstructed = 0.7f;

	float listx, listy;
	in_pDemo->m_pListenerChip->GetPos(listx, listy);
	float emitx, emity;
	in_pDemo->m_pEmitterChip->GetPos(emitx, emity);

	// Compute coordinates relative to the room's corner.
	// Quadrants:
	// 2 | 1
	// -----
	// 3 | 4
	float roomCornerX, roomCornerY;
	in_pDemo->m_pRoom->GetBottomRightCorner(roomCornerX, roomCornerY);
	float relListx = (listx - roomCornerX);
	float relListy = (listy - roomCornerY);

	// Compute obstruction between portals and the listener. Portal 0 is obstructed when the listener is in quadrant 3, and portal 1 is obstructed when the listener is in quadrant 1.
	float obsPortal0 = (relListx < 0 && relListy > 0) ? k_obstructed : 0.f;
	float obsPortal1 = (relListx > 0 && relListy < 0) ? k_obstructed : 0.f;
	
	// Pass to SpatialAudio.
	AK::SpatialAudio::SetPortalObstructionAndOcclusion(PORTAL0, obsPortal0, 0.f);
	AK::SpatialAudio::SetPortalObstructionAndOcclusion(PORTAL1, obsPortal1, 0.f);

	// Emitter: if it is in the same room as the listener, compute a ray-cast based obstruction value.
	// This is done by the game, with various levels of complexity, and is set directly to the sound engine via AK::SoundEngine::SetObjectObstructionAndOcclusion.
	// If the emitter is in a different room, Spatial Audio will pick the largest values between obstruction/occlusion provided by the user, and those driven by
	// diffraction/transmission. Since we want the latter to apply, let's pass 0.
	float obsEmitter = 0.f;
	if (in_pDemo->IsInRoom(listx, listy) == in_pDemo->IsInRoom(emitx, emity))
	{
		float relEmitx = (emitx - roomCornerX);
		float relEmity = (emity - roomCornerY);
		// Obstruction is non-zero only if emitter and listener are not in the same quadrant
		if (((relEmity >= 0) ^ (relListy >= 0)) && relEmitx * relListx < 0)
		{
			float crossprod = relListx * relEmity - relEmitx * relListy;
			obsEmitter = (relEmitx * crossprod > 0.f) ? k_obstructed : 0.f;
		}
	}
	AK::SoundEngine::SetObjectObstructionAndOcclusion(EMITTER, LISTENER, obsEmitter, 0.0f);

	// Ray-cast line between emitter and listener. Show it when they are in the same room and obstruction is 0.
	delete m_aRaycastEtoL;
	m_aRaycastEtoL = NULL;
	if (obsEmitter == 0.f && (in_pDemo->IsInRoom(listx, listy) == in_pDemo->IsInRoom(emitx, emity)))
	{
		Line * pLine = new Line(*in_pDemo);
		pLine->SetPoints((int)emitx, (int)emity, (int)listx, (int)listy);
		m_aRaycastEtoL = pLine;
	}
}


/////////////////////////////////////////////////////////////////////
// GeometryInRooms:
// Defines and registers geometry to Spatial Audio.
/////////////////////////////////////////////////////////////////////
GeometryInRooms::GeometryInRooms(DemoSpatialAudio * in_pDemo)
{

	// Let's construct 3 disjoint meshes: one for the inside wall, assigned to room ROOM, one for the outside wall, assigned to room AK::SpatialAudio::kOutdoorRoomID, and another one for an obstacle, outside.

	// In general, for diffraction, it is important to join meshes to avoid sound leaking through cracks.
	// Here in our 2D geometry, we do not assign triangles to the top and bottom planes (through screen), and utilize
	// AkGeometryParams::EnableDiffractionOnBoundaryEdges = false so that edges on top and bottom will not allow sound to pass through. 

	float px_roomX, px_roomY, px_roomWidth, px_roomHeight;
	in_pDemo->m_pRoom->GetRect(px_roomX, px_roomY, px_roomWidth, px_roomHeight);

	{
		// Outer wall (assigned to Outside).
		
		//		  |
		//        |
		// -------|

		// Box in_pDemo->m_pRoom already defined in InitControls_Portals()
		// Register to SpatialAudio

		// Assign surfaces. Direct path diffraction is not affected by the acoustic properties of surfaces, because in this model, the edges don't absorb any energy.
		// In this demo, the only purpose of using acoustic surfaces with textures defined in Wwise is for showing colors in the 3D Game Object Viewer.
		AkAcousticSurface outerSurface;
		outerSurface.strName = "Outside";
		outerSurface.textureID = AK::SoundEngine::GetIDFromString("Brick");
		outerSurface.transmissionLoss = 0.8f;	// Let's set a transmission loss smaller than that of the room, so that the transmission loss of the room dominates.

		// Outer wall are the bottom wall and right wall of the box
		SpatialAudioHelpers::BoxGeometryParams outerWallParams ( 0, 0);
		outerWallParams.surfaces.push_back(outerSurface);

		SpatialAudioHelpers::AddBoxGeometry(GEOMETRY_OUTER_ROOM, outerWallParams);
		SpatialAudioHelpers::AddBoxGeometryInstance(GEOMETRY_OUTER_ROOM, GEOMETRY_OUTER_ROOM, in_pDemo->m_pRoom, in_pDemo->m_fWidth, in_pDemo->m_fHeight, AK::SpatialAudio::kOutdoorRoomID);
	}

	{
		// Inner wall (assigned to ROOM)
		// Note: the geometry assigned to ROOM is offset from the outside wall constructed above, by k_offsetInnerWall pixels, to represent the wall of a room and the thickness of the "building" as it would
		// typically occur in a video game, or real life. However this is not necessary; the walls may be coincident (k_offsetInnerWall = 0) or even reversed (k_offsetInnerWall < 0). An emitter in room ROOM
		// will always only see these walls 
		
		// Draw Box.
		const int k_offsetInnerWall = 5;
		int innerRoomWidth = (int) px_roomWidth - k_offsetInnerWall;
		int innerRoomHeight = (int) px_roomHeight - k_offsetInnerWall;

		m_pInnerRoom = new Box(*in_pDemo);
		m_pInnerRoom->SetPosition(0, 0);
		m_pInnerRoom->SetDimensions(innerRoomWidth, innerRoomHeight);

		// Register to SpatialAudio
		AkAcousticSurface innerSurface;
		innerSurface.strName = "Inside";
		innerSurface.textureID = AK::SoundEngine::GetIDFromString("Drywall");
		innerSurface.transmissionLoss = 0.8f;	// Let's set a transmission loss smaller than that of the room, so that the transmission loss of the room dominates.

		SpatialAudioHelpers::BoxGeometryParams innerWallParams;
		innerWallParams.surfaces.push_back(innerSurface);

		SpatialAudioHelpers::AddBoxGeometry(GEOMETRY_INNER_ROOM, innerWallParams);
		SpatialAudioHelpers::AddBoxGeometryInstance(GEOMETRY_INNER_ROOM, GEOMETRY_INNER_ROOM, m_pInnerRoom, in_pDemo->m_fWidth, in_pDemo->m_fHeight, ROOM);
	}

	{
		// Obstacle, outside.

		// Draw Box.
		const int k_thickness = 5;
		int topLeftX = (int) (px_roomWidth / 3.f);
		int topLeftY = (int) (px_roomHeight * 1.333f);
		int obstacleWidth = (int) px_roomWidth;
		int obstacleHeight = k_thickness;

		m_pObstacle = new Box(*in_pDemo);
		m_pObstacle->SetPosition(topLeftX, topLeftY);
		m_pObstacle->SetDimensions(obstacleWidth, obstacleHeight);

		// Register to SpatialAudio
		AkAcousticSurface obstacleSurface;
		obstacleSurface.strName = "Wall";
		obstacleSurface.textureID = AK::SoundEngine::GetIDFromString("Brick");
		obstacleSurface.transmissionLoss = 0.95f;	// Let's set a transmission loss almost opaque for this obstacle.

		SpatialAudioHelpers::BoxGeometryParams obstacleParams;
		obstacleParams.surfaces.push_back(obstacleSurface);

		SpatialAudioHelpers::AddBoxGeometry(GEOMETRY_OBSTACLE, obstacleParams);
		SpatialAudioHelpers::AddBoxGeometryInstance(GEOMETRY_OBSTACLE, GEOMETRY_OBSTACLE, m_pObstacle, in_pDemo->m_fWidth, in_pDemo->m_fHeight, AK::SpatialAudio::kOutdoorRoomID);
	}
}

GeometryInRooms::~GeometryInRooms()
{
	AK::SpatialAudio::RemoveGeometry(0);
	AK::SpatialAudio::RemoveGeometry(1);
	AK::SpatialAudio::RemoveGeometry(2);
}

void GeometryInRooms::Draw()
{
	m_pInnerRoom->Draw();
	m_pObstacle->Draw();
}
