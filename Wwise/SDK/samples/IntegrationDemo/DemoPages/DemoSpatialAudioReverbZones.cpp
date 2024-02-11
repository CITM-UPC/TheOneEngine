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

// DemoSpatialAudioReverbZones.cpp
/// \file
/// Defines all methods declared in DemoSpatialAudioReverbZones.h

#include "stdafx.h"

#include "Menu.h"
#include "Helpers.h"
#include "DemoSpatialAudioReverbZones.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>

#define _USE_MATH_DEFINES
#include <math.h>

//If you get a compiling error here, it means the file wasn't generated with the banks.  Did you generate the soundbanks before compiling?
#include "../WwiseProject/GeneratedSoundBanks/Wwise_IDs.h"

const AkGameObjectID EMITTER = 100;
const AkGameObjectID LISTENER = 103;
const AkGameObjectID DISTANCE_PROBE = 104;

const AkRoomID ROOM = 200;
const AkRoomID PATIO = 201;
const AkGeometrySetID ROOM_GEOMETRY = 0;
const AkGeometrySetID PATIO_GEOMETRY = 1;
const AkGeometrySetID OBSTACLE_GEOMETRY = 2;

const AkPortalID PORTAL = 300;

const AkReal32 TRANSITION_ZONE_WIDTH = 5;

#define REPEAT_TIME 20

/////////////////////////////////////////////////////////////////////
// DemoSpatialAudioReverbZones Public Methods
/////////////////////////////////////////////////////////////////////

DemoSpatialAudioReverbZones::DemoSpatialAudioReverbZones(Menu& in_ParentMenu)
	:Page(in_ParentMenu, "Reverb Zones Demo")
	, m_pListenerChip(NULL)
	, m_pEmitterChip(NULL)
	, m_pRoom(NULL)
	, m_pPatio(NULL)
	, m_pPortal(NULL)
	, m_pObstacle(NULL)
	, m_fWidth(0.0f)
	, m_fHeight(0.0f)
	, m_dryDiffraction(0)
	, m_wetDiffraction(0)
	, m_transmissionLoss(0)
	, m_bTestNoise(false)
	, m_bMoved(false)
	, m_portalOpen(true)
{
	m_szHelp =
		"*** Please connect Wwise and open the 3D Game Object Viewer (F12 layout). ***\n"
		"This demo showcases the use of Reverb Zones from Spatial Audio.\n \n"
		"In this demo there are 2 Rooms with Geometry with one of the room being a Reverb Zone, 1 portal, and 1 Geometry as an obstacle\n"
		"The diffraction path(s) are displayed. The diffraction and transmission amounts are displayed in the lower-left corner.\n \n"
		"Listener and emitter controls are specified under the Spatial Audio Demos menu help page.\n"
		"* <<UG_BUTTON1>> - Open/close the Portal.\n";
}

bool DemoSpatialAudioReverbZones::Init()
{
	// Load the sound bank
	AkBankID bankID; // Not used
	if (AK::SoundEngine::LoadBank("Spatial_Audio_Demo.bnk", bankID) != AK_Success)
	{
		SetLoadFileErrorMessage("Spatial_Audio_Demo.bnk");
		return false;
	}

	return Page::Init();
}

void DemoSpatialAudioReverbZones::InitControls()
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

	Init_Rooms();
	Init_Portals();

	// Set up the room
	{
		AkAcousticSurface wallSurface;

		// Set up a default Geometry to make instances from
		SpatialAudioHelpers::BoxGeometryParams boxParams;

		wallSurface.strName = "Room Wall";
		wallSurface.textureID = AK::SoundEngine::GetIDFromString("Brick");
		wallSurface.transmissionLoss = 0.95f;
		boxParams.surfaces.push_back(wallSurface);

		// The left wall is not wall-less, neither the floor nor the ceiling
		boxParams.floor = 0;
		boxParams.ceiling = 0;
		SpatialAudioHelpers::AddBoxGeometry(ROOM_GEOMETRY, boxParams, true, true);
		SpatialAudioHelpers::AddBoxGeometryInstance(ROOM_GEOMETRY, ROOM_GEOMETRY, m_pRoom, m_fWidth, m_fHeight, AK::SpatialAudio::kOutdoorRoomID, true);
	}

	// Set up the patio room
	{
		AkAcousticSurface wallSurface;
		wallSurface.strName = "Wall-less Wall";
		wallSurface.textureID = AK::SoundEngine::GetIDFromString("Brick");
		wallSurface.transmissionLoss = 0.0f;

		// Set up a default Geometry to make instances from
		SpatialAudioHelpers::BoxGeometryParams boxParams;
		boxParams.surfaces.push_back(wallSurface);

		wallSurface.strName = "Room Wall";
		wallSurface.textureID = AK::SoundEngine::GetIDFromString("Brick");
		wallSurface.transmissionLoss = 1.0f;
		boxParams.surfaces.push_back(wallSurface);

		// The left wall is not wall-less, neither the floor nor the ceiling
		boxParams.leftWall = 1;
		boxParams.floor = 1;
		boxParams.ceiling = 1;
		SpatialAudioHelpers::AddBoxGeometry(PATIO_GEOMETRY, boxParams, true, true);
		SpatialAudioHelpers::AddBoxGeometryInstance(PATIO_GEOMETRY, PATIO_GEOMETRY, m_pPatio, m_fWidth, m_fHeight, AK::SpatialAudio::kOutdoorRoomID, true);
	}

	// Setup the obstacle
	{
		m_pObstacle = new Box(*this);

		m_pObstacle->SetPosition((int)(m_fWidth * 3.f / 4.f), (int)(m_fHeight / 3.f));
		m_pObstacle->SetDimensions(100, 100);
		
		AkAcousticSurface wallSurface;

		// Set up a default Geometry to make instances from
		SpatialAudioHelpers::BoxGeometryParams boxParams;

		wallSurface.strName = "Room Wall";
		wallSurface.textureID = AK::SoundEngine::GetIDFromString("Brick");
		wallSurface.transmissionLoss = 1.0f;
		boxParams.surfaces.push_back(wallSurface);

		boxParams.floor = 0;
		boxParams.ceiling = 0;
		SpatialAudioHelpers::AddBoxGeometry(OBSTACLE_GEOMETRY, boxParams, true, true);
		SpatialAudioHelpers::AddBoxGeometryInstance(OBSTACLE_GEOMETRY, OBSTACLE_GEOMETRY, m_pObstacle, m_fWidth, m_fHeight, AK::SpatialAudio::kOutdoorRoomID, true);
	}

	AK::SoundEngine::PostEvent("Play_Room_Emitter", EMITTER);

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

void DemoSpatialAudioReverbZones::Init_Rooms()
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

	// Let's define the room
	m_pRoom = new Box(*this);

	float roomWidth = m_fWidth / 3.f;
	float roomHeight = m_fHeight / 2.f;
	m_pRoom->SetPosition(100, 120);
	m_pRoom->SetDimensions((int)roomWidth, (int)roomHeight);

	paramsRoom.Front.X = 0.f;
	paramsRoom.Front.Y = 0.f;
	paramsRoom.Front.Z = 1.f;
	paramsRoom.Up.X = 0.f;
	paramsRoom.Up.Y = 1.f;
	paramsRoom.Up.Z = 0.f;
	paramsRoom.TransmissionLoss = 0.95f;	// Let's have a bit of sound transmitted through walls
	paramsRoom.RoomGameObj_KeepRegistered = false;
	paramsRoom.RoomGameObj_AuxSendLevelToSelf = 0.0f;
	paramsRoom.ReverbAuxBus = AK::SoundEngine::GetIDFromString("Room");
	paramsRoom.GeometryInstanceID = ROOM_GEOMETRY;	// We associate the geometry to the room in order to compute the room spread. 
													// If the geometry is not found (in "Portals Demo"), the room bounding box is calculated from the portals combined extent.

	AK::SpatialAudio::SetRoom(ROOM, paramsRoom, "Room Object");

	// Let's define tha patio
	m_pPatio = new Box(*this);

	// Register rooms to Spatial Audio.
	float patioWidth = m_fWidth / 5.f;
	float patioHeight = m_fHeight / 3.f;
	m_pPatio->SetPosition(100 + (int)(m_fWidth / 3.f), 120);
	m_pPatio->SetDimensions((int)patioWidth, (int)patioHeight);

	paramsRoom.Front.X = 0.f;
	paramsRoom.Front.Y = 0.f;
	paramsRoom.Front.Z = 1.f;
	paramsRoom.Up.X = 0.f;
	paramsRoom.Up.Y = 1.f;
	paramsRoom.Up.Z = 0.f;
	paramsRoom.TransmissionLoss = 0.0f;	// Let's have a bit of sound transmitted through walls when all portals are closed.
	paramsRoom.RoomGameObj_KeepRegistered = true;	// We intend to use the room's game object to post events (see documentation of AkRoomParams::RoomGameObj_KeepRegistered).
	paramsRoom.RoomGameObj_AuxSendLevelToSelf = 0.0f;	// Since we will be playing an ambience ("Play_Ambience_Quad", below), on this room's game object, we here route some of it to the room's auxiliary bus to add some of its reverb.
	paramsRoom.ReverbAuxBus = AK::SoundEngine::GetIDFromString("Patio");
	paramsRoom.GeometryInstanceID = PATIO_GEOMETRY;	// We associate the geometry to the room in order to compute the room spread. 
													// If the geometry is not found (in "Portals Demo"), the room bounding box is calculated from the portals combined extent.

	AK::SpatialAudio::SetRoom(PATIO, paramsRoom, "Patio Object");

	AK::SpatialAudio::SetReverbZone(PATIO, AK::SpatialAudio::kOutdoorRoomID, TRANSITION_ZONE_WIDTH);
}

void DemoSpatialAudioReverbZones::Init_Portals()
{
	float rx, ry, rw, rh;
	m_pRoom->GetRect(rx, ry, rw, rh);

	m_pPortal = new Box(*this);
	m_pPortal->SetPosition((int)(rx + rw - 20), (int)(ry + rh / 2.0f - rh / 10.f));
	m_pPortal->SetDimensions((int)(40), (int)(rh / 5.f));

	SetPortals();
}

void DemoSpatialAudioReverbZones::SetPortals()
{
	// Create a portal
	float px, py, pw, ph;
	m_pPortal->GetRect(px, py, pw, ph);

	AkPortalParams paramsPortal;

	paramsPortal.Transform.SetPosition(IntegrationDemoHelpers::PixelsToAKPos_X(m_fWidth, pw / 2.f + px), 0,
		IntegrationDemoHelpers::PixelsToAKPos_Y(m_fHeight, ph / 2.f + py));
	paramsPortal.Transform.SetOrientation(1.f, 0.f, 0.f, 0.f, 1.f, 0.f); // facing +'ve Z
	paramsPortal.Extent.halfWidth = IntegrationDemoHelpers::PixelsToAKLen_X(m_fHeight, ph / 2.f);
	paramsPortal.Extent.halfHeight = IntegrationDemoHelpers::PixelsToAKLen_X(m_fHeight, ph / 2.f);
	paramsPortal.Extent.halfDepth = IntegrationDemoHelpers::PixelsToAKLen_X(m_fWidth, pw / 2.f);
	paramsPortal.bEnabled = m_portalOpen;

	paramsPortal.FrontRoom = PATIO;
	paramsPortal.BackRoom = ROOM;
	AK::SpatialAudio::SetPortal(PORTAL, paramsPortal, "Portal");
}

void DemoSpatialAudioReverbZones::Release()
{
	AK::SoundEngine::StopAll();
	delete m_pEmitterChip;
	delete m_pListenerChip;

	AK::SpatialAudio::RemoveRoom(ROOM);
	AK::SpatialAudio::RemoveRoom(PATIO);
	AK::SpatialAudio::RemoveRoom(AK::SpatialAudio::kOutdoorRoomID);
	
	AK::SpatialAudio::RemovePortal(PORTAL);

	AK::SpatialAudio::RemoveGeometry(ROOM_GEOMETRY);
	AK::SpatialAudio::RemoveGeometry(PATIO_GEOMETRY);
	AK::SpatialAudio::RemoveGeometry(OBSTACLE_GEOMETRY);
	AK::SoundEngine::UnloadBank("Spatial_Audio_Demo.bnk", NULL);

	m_diffractionLines.clear();
	
	delete m_pRoom;
	m_pRoom = NULL;

	delete m_pPatio;
	m_pPatio = NULL;

	delete m_pPortal;
	m_pPortal = NULL;

	delete m_pObstacle;
	m_pObstacle = NULL;

	Page::Release();
}

bool DemoSpatialAudioReverbZones::Update()
{
	const float kAngleIncrement = M_PI / 180.f;

	UniversalInput::Iterator it;
	for (it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++)
	{
		// Skip this input device if it's not connected
		if (!it->IsConnected())
			continue;

		if ((*it).IsButtonTriggered(UG_BUTTON1))
		{
			m_portalOpen = !m_portalOpen;
			SetPortals();

			m_bMoved = true;
		}

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
				&& (((x - xprev) * (x - xprev) + (y - yprev) * (y - yprev)) > 0.1f))
			{
				AK::SoundEngine::PostEvent("Play_Footstep", LISTENER);
				m_uLastTick = m_pParentMenu->GetTickCount();
			}

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

	if (m_bMoved)
	{
		// Update emitters, set room for spatial audio.
		m_pEmitterChip->SetPosition();

		// Update listener, set room for spatial audio.
		m_pListenerChip->SetPosition();
	
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
	}

	// Reset m_bMoved.
	m_bMoved = false;

	return Page::Update();
}

void DemoSpatialAudioReverbZones::Draw()
{
	Page::Draw();
	float lineColor[3];

	if (m_pEmitterChip)
		m_pEmitterChip->Draw();
	if (m_pListenerChip)
		m_pListenerChip->Draw();

	m_pRoom->Draw();
	m_pPatio->Draw();
	m_pObstacle->Draw();

	if (m_portalOpen)
		m_pPortal->Draw(DrawStyle_Selected);
	else
		m_pPortal->Draw(DrawStyle_Error);

	// Concrete texture color
	lineColor[0] = 0.5f;
	lineColor[1] = 0.5f;
	lineColor[2] = 0.5f;

	for (auto& diffractionLine : m_diffractionLines)
		diffractionLine->Draw();

	int i = 0;

	float fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ;
	m_pListenerChip->GetAkPos(fAkPosListenerX, fAkPosListenerZ);
	m_pEmitterChip->GetAkPos(fAkPosEmitterX, fAkPosEmitterZ);

	char strBuf[128];
	snprintf(strBuf, 128, "[L](X,Z): %.2f  %.2f\n[E](X,Z): %.2f  %.2f\nDiffraction dry: %.2f\nDiffraction wet: %.2f\nTransmission Loss: %.2f",
		fAkPosListenerX, fAkPosListenerZ, fAkPosEmitterX, fAkPosEmitterZ, m_dryDiffraction, m_wetDiffraction, m_transmissionLoss);

	static int s_nOffset = 6 * GetLineHeight(DrawStyle_Text);
	DrawTextOnScreen(strBuf, 5, m_pParentMenu->GetHeight() - s_nOffset, DrawStyle_Text);

	// Display instructions at the bottom of the page
	int iInstructionsY = m_pParentMenu->GetHeight() - 3 * GetLineHeight(DrawStyle_Text);
	DrawTextOnScreen("(Press <<UG_BUTTON2>> To Go Back...)", (int)(m_pParentMenu->GetWidth() * 0.4), iInstructionsY, DrawStyle_Text);
}

bool DemoSpatialAudioReverbZones::OnPointerEvent(PointerEventType in_eType, int in_x, int in_y)
{
	if (in_eType == PointerEventType_Moved)
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

	return Page::OnPointerEvent(in_eType, in_x, in_y);
}