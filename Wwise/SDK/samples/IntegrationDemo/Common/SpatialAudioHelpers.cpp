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

// SpatialAudioHelper.cpp
/// \file
/// Defines all methods declared in SpatialAudioHelper.h

#include "stdafx.h"

#include <math.h>
#include <float.h>
#include "Menu.h"
#include "Page.h"
#include "Helpers.h"
#include "SpatialAudioHelpers.h"
#include <AK/Tools/Common/AkVectors.h>

/////////////////////////////////////////////////////////////////////
// SpatialAudioHelpers Methods
/////////////////////////////////////////////////////////////////////

//AddBoxGeometry(AkGeometrySetID in_geometryId, BoxGeometryParams in_boxParam, bool in_enableTriangles, bool in_enableDiffraction, bool in_enableBoudaryEdges)
void SpatialAudioHelpers::AddBoxGeometry(AkGeometrySetID in_geometryId, BoxGeometryParams in_boxParam, bool in_enableDiffraction, bool in_enableBoudaryEdges)
{
	// define unit size box geometry (position and size set with GeometryInstance)
	// 67|------|54
	//	 |		|
	// 01|------|32
	AkVertex vertices[] = {
		{ 0, 0, 0 },
		{ 0, 1.f, 0 },
		{ 1.f, 0, 0 },
		{ 1.f, 1.f, 0 },
		{ 1.f, 0, 1.f },
		{ 1.f, 1.f, 1.f },
		{ 0, 0, 1.f },
		{ 0, 1.f, 1.f }
	};

	// define a transparent surface
	AkAcousticSurface transparentSurface;
	transparentSurface.strName = "Transparent";
	transparentSurface.transmissionLoss = 0.0f;
	in_boxParam.surfaces.push_back(transparentSurface);
	AkSurfIdx transparentId = static_cast<AkSurfIdx>(in_boxParam.surfaces.size() - 1);

	AkGeometryParams geometryParams;
	geometryParams.NumVertices = 8;
	geometryParams.Vertices = vertices;

	std::vector<AkTriangle> tris;
	if (in_boxParam.bottomWall != AK_INVALID_SURFACE)
	{
		AKASSERT(in_boxParam.bottomWall < in_boxParam.surfaces.size()); // Surface index not defined in surface table in BoxGeometryParams
		tris.push_back(AkTriangle(0, 1, 2, in_boxParam.bottomWall));
		tris.push_back(AkTriangle(3, 2, 1, in_boxParam.bottomWall));
	}
	else
	{
		tris.push_back(AkTriangle(0, 1, 2, transparentId));
		tris.push_back(AkTriangle(1, 2, 3, transparentId));
	}

	if (in_boxParam.rightWall != AK_INVALID_SURFACE)
	{
		AKASSERT(in_boxParam.rightWall < in_boxParam.surfaces.size());
		tris.push_back(AkTriangle(2, 3, 4, in_boxParam.rightWall));
		tris.push_back(AkTriangle(5, 4, 3, in_boxParam.rightWall));
	}
	else
	{
		tris.push_back(AkTriangle(2, 3, 4, transparentId));
		tris.push_back(AkTriangle(3, 4, 5, transparentId));
	}

	if (in_boxParam.topWall != AK_INVALID_SURFACE)
	{
		AKASSERT(in_boxParam.topWall < in_boxParam.surfaces.size());
		tris.push_back(AkTriangle(4, 5, 6, in_boxParam.topWall));
		tris.push_back(AkTriangle(7, 6, 5, in_boxParam.topWall));
	}
	else
	{
		tris.push_back(AkTriangle(4, 5, 6, transparentId));
		tris.push_back(AkTriangle(5, 6, 7, transparentId));
	}

	if (in_boxParam.leftWall != AK_INVALID_SURFACE)
	{
		AKASSERT(in_boxParam.leftWall < in_boxParam.surfaces.size());
		tris.push_back(AkTriangle(6, 7, 0, in_boxParam.leftWall));
		tris.push_back(AkTriangle(1, 0, 7, in_boxParam.leftWall));
	}
	else
	{
		tris.push_back(AkTriangle(6, 7, 0, transparentId));
		tris.push_back(AkTriangle(7, 0, 1, transparentId));
	}

	if (in_boxParam.floor != AK_INVALID_SURFACE)
	{
		AKASSERT(in_boxParam.floor < in_boxParam.surfaces.size());
		tris.push_back(AkTriangle(0, 2, 4, in_boxParam.floor));
		tris.push_back(AkTriangle(6, 4, 0, in_boxParam.floor));
	}
	else
	{
		tris.push_back(AkTriangle(0, 2, 4, transparentId));
		tris.push_back(AkTriangle(0, 4, 6, transparentId));
	}

	if (in_boxParam.ceiling != AK_INVALID_SURFACE)
	{
		AKASSERT(in_boxParam.ceiling < in_boxParam.surfaces.size());
		tris.push_back(AkTriangle(1, 3, 5, in_boxParam.ceiling));
		tris.push_back(AkTriangle(7, 5, 1, in_boxParam.ceiling));
	}
	else
	{
		tris.push_back(AkTriangle(1, 3, 5, transparentId));
		tris.push_back(AkTriangle(1, 5, 7, transparentId));
	}

	geometryParams.NumTriangles = (AkTriIdx) tris.size();
	geometryParams.Triangles = tris.data();

	geometryParams.NumSurfaces = (AkTriIdx) in_boxParam.surfaces.size();
	geometryParams.Surfaces = in_boxParam.surfaces.data();

	geometryParams.EnableDiffraction = in_enableDiffraction;
	geometryParams.EnableDiffractionOnBoundaryEdges = in_enableBoudaryEdges;

	AK::SpatialAudio::SetGeometry(in_geometryId, geometryParams);
}

void SpatialAudioHelpers::AddBoxGeometryInstance(AkGeometryInstanceID in_instanceId, AkGeometrySetID in_geometryId, Box* in_pBox, float in_fWidth, float in_fHeight, AkRoomID in_roomId, bool in_usedForReflectionAndDiffraction)
{
	float px_x, px_y, px_width, px_height;
	in_pBox->GetRect(px_x, px_y, px_width, px_height);

	float width = IntegrationDemoHelpers::PixelsToAKLen_X(in_fWidth, px_width);
	float height = IntegrationDemoHelpers::PixelsToAKLen_Y(in_fHeight, px_height);

	float x = IntegrationDemoHelpers::PixelsToAKPos_X(in_fWidth, px_x);
	float y = -30.f;
	float z = IntegrationDemoHelpers::PixelsToAKPos_Y(in_fHeight, px_y) - height;

	AkGeometryInstanceParams instanceParams;
	instanceParams.GeometrySetID = in_geometryId;

	AkVector scale;
	scale.X = width;
	scale.Z = height;
	scale.Y = 60.f;
	instanceParams.Scale = scale;

	AkWorldTransform position;
	position.SetOrientation(0.f, 0.f, 1.f, 0.f, 1.f, 0.f);
	position.SetPosition(x, y, z);
	instanceParams.PositionAndOrientation = position;

	instanceParams.UseForReflectionAndDiffraction = in_usedForReflectionAndDiffraction;

	AK::SpatialAudio::SetGeometryInstance(in_instanceId, instanceParams);
}

/////////////////////////////////////////////////////////////////////
// SpatialAudioListener Public Methods
/////////////////////////////////////////////////////////////////////

SpatialAudioListener::SpatialAudioListener(Page& in_rParentPage, AkGameObjectID in_listenerId, AkGameObjectID in_distanceProbeId, const char* in_pszObjName)
	: SpatialAudioGameObject(in_rParentPage, in_listenerId, in_listenerId, in_pszObjName)
	, m_distanceProbeId(in_distanceProbeId)
	, m_pDistanceProbeChip(NULL)
	, m_fAzimuth(0.f)
	, m_fElevation(0.f)
	, m_fPlayerOffset (0.f)
{
	// And register our listener as the one and only spatial audio listener.
	AK::SpatialAudio::RegisterListener(m_gameObjId);
}

SpatialAudioListener::~SpatialAudioListener()
{
	if (m_pDistanceProbeChip)
	{
		AK::SoundEngine::UnregisterGameObj(m_distanceProbeId);
		m_pDistanceProbeChip = NULL;
	}
}

void SpatialAudioListener::Update(const UniversalGamepad& in_Gamepad)
{
	MovableChip::Update(in_Gamepad);
	if(m_pDistanceProbeChip)
		m_pDistanceProbeChip->Update(in_Gamepad);
}

void SpatialAudioListener::SetPosition()
{
	float x, y;
	if(m_pDistanceProbeChip)
		m_pDistanceProbeChip->GetPos(x, y);
	else
		MovableChip::GetPos(x, y);

	float m_fWidth, m_fHeight;
	m_fWidth = (float) (m_pParentPage->ParentMenu()->GetWidth() - GetRightBottomMargin());
	m_fHeight = (float) (m_pParentPage->ParentMenu()->GetHeight() - GetRightBottomMargin());

	// Converting X-Y UI into X-Z world plan.
	AkVector position;
	m_AkPosX = position.X = IntegrationDemoHelpers::PixelsToAKPos_X(m_fWidth, x);
	position.Y = 0;
	m_AkPosZ = position.Z = IntegrationDemoHelpers::PixelsToAKPos_Y(m_fHeight, y);

	AkVector orientationFront;
	orientationFront.X = sinf(m_fAzimuth) * cosf(m_fElevation);
	orientationFront.Y = sinf(m_fElevation);
	orientationFront.Z = cosf(m_fAzimuth) * cosf(m_fElevation);

	AkVector orientationTop;
	orientationTop.X = sinf(m_fElevation) * -sinf(m_fAzimuth);
	orientationTop.Y = cosf(m_fElevation);
	orientationTop.Z = -sinf(m_fElevation) * cosf(m_fAzimuth);

	if (m_fPlayerOffset > 0.f)
	{
		// Defines distance probe
		if (!m_pDistanceProbeChip)
		{
			AK::SoundEngine::RegisterGameObj(m_distanceProbeId, "Distance Probe");
			AK::SoundEngine::SetDistanceProbe(m_gameObjId, m_distanceProbeId);

			// New movable chip for the offset listener
			m_pDistanceProbeChip = new MovableChip(*m_pParentPage);
			m_pDistanceProbeChip->SetLabel("<DP>");
			m_pDistanceProbeChip->UseJoystick(UG_STICKRIGHT);
			m_pDistanceProbeChip->SetNonLinear();
			m_pDistanceProbeChip->SetPos((float) IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, position.X),
				(float) IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, position.Z));
			m_pDistanceProbeChip->SetMaxSpeed(3.f);
		}

		AkSoundPosition soundPos;
		soundPos.Set(position, orientationFront, orientationTop);
		AK::SoundEngine::SetPosition(m_distanceProbeId, soundPos);

		// offset listener position
		position.X -= orientationFront.X * m_fPlayerOffset;
		position.Y -= orientationFront.Y * m_fPlayerOffset;
		position.Z -= orientationFront.Z * m_fPlayerOffset;

		MovableChip::SetPos((float) IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, position.X),
				(float) IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, position.Z));
	}
	else
	{
		if (m_pDistanceProbeChip)
		{
			AK::SoundEngine::UnregisterGameObj(m_distanceProbeId);
			AK::SoundEngine::SetDistanceProbe(m_gameObjId, AK_INVALID_GAME_OBJECT);
			m_pDistanceProbeChip = NULL;

			MovableChip::SetPos((float) IntegrationDemoHelpers::AKPosToPixels_X(m_fWidth, position.X),
				(float) IntegrationDemoHelpers::AKPosToPixels_Y(m_fHeight, position.Z));
		}
	}

	AKASSERT(fabsf(Ak3DVector(orientationTop).Dot(Ak3DVector(orientationFront))) < 0.001);

	AkSoundPosition soundPos;
	soundPos.Set(position, orientationFront, orientationTop);
	AK::SoundEngine::SetPosition(m_gameObjId, soundPos);
}

bool SpatialAudioListener::UpdateDistanceProbeRoom(Box* in_pBox, AkRoomID in_roomId)
{
	if (m_pDistanceProbeChip)
	{
		float x, y;
		GetDistanceProbePos(x, y);
		if (in_pBox->ContainsPoint(x, y))
		{
			AK::SpatialAudio::SetGameObjectInRoom(m_distanceProbeId, in_roomId);
			return true;
		}

		AK::SpatialAudio::SetGameObjectInRoom(m_distanceProbeId, AK::SpatialAudio::kOutdoorRoomID);
		return false;
	}

	return true;
}

bool SpatialAudioListener::UpdateDistanceProbeRoom(std::vector<std::unique_ptr<Box>>& in_boxes, AkRoomID in_baseRoomId)
{
	if (m_pDistanceProbeChip)
	{
		float x, y;
		GetDistanceProbePos(x, y);

		int roomNumber = 0;
		for (auto& in_box : in_boxes)
		{
			if (in_box->ContainsPoint(x, y))
			{
				AK::SpatialAudio::SetGameObjectInRoom(m_distanceProbeId, in_baseRoomId + roomNumber);
				return true;
			}
			roomNumber++;
		}
	
		AK::SpatialAudio::SetGameObjectInRoom(m_distanceProbeId, AK::SpatialAudio::kOutdoorRoomID);
		return false;
	}

	return true;
}

void SpatialAudioListener::Draw(DrawStyle in_eDrawStryle)
{
	MovableChip::Draw(in_eDrawStryle);
	if (m_pDistanceProbeChip)
		m_pDistanceProbeChip->Draw();
}

void SpatialAudioListener::GetDistanceProbePos(float& out_x, float& out_y)
{
	if(m_pDistanceProbeChip)
		m_pDistanceProbeChip->GetPos(out_x, out_y);
	else
		MovableChip::GetPos(out_x, out_y);
}

/////////////////////////////////////////////////////////////////////
// SpatialAudioGameObject Public Methods
/////////////////////////////////////////////////////////////////////

SpatialAudioGameObject::SpatialAudioGameObject(Page& in_rParentPage, AkGameObjectID in_gameObjId, AkGameObjectID in_listenerId, const char* in_pszObjName)
	: MovableChip(in_rParentPage)
	, m_gameObjId(in_gameObjId)
	, m_AkPosX(100.f)
	, m_AkPosZ(100.f)
{
	// Register the emitter game object
	AK::SoundEngine::RegisterGameObj(m_gameObjId, in_pszObjName);

	// Associate our emitters with our listener. This will set up direct path from EMITTER -> LISTENER
	static const int kNumLstnrsForEm = 1;
	static const AkGameObjectID aLstnrsForEmitter[kNumLstnrsForEm] = { in_listenerId };
	AK::SoundEngine::SetListeners(m_gameObjId, aLstnrsForEmitter, kNumLstnrsForEm);
}

SpatialAudioGameObject::~SpatialAudioGameObject()
{
	AK::SoundEngine::UnregisterGameObj(m_gameObjId);
}

void SpatialAudioGameObject::SetPosition()
{
	float x, y;
	MovableChip::GetPos(x, y);

	float m_fWidth, m_fHeight;
	m_fWidth = (float) (m_pParentPage->ParentMenu()->GetWidth() - GetRightBottomMargin());
	m_fHeight = (float) (m_pParentPage->ParentMenu()->GetHeight() - GetRightBottomMargin());

	// Converting X-Y UI into X-Z world plan.
	AkVector position;
	m_AkPosX = position.X = IntegrationDemoHelpers::PixelsToAKPos_X(m_fWidth, x);
	position.Y = 0;
	m_AkPosZ = position.Z = IntegrationDemoHelpers::PixelsToAKPos_Y(m_fHeight, y);

	AkVector orientationFront;
	orientationFront.X = orientationFront.Y = 0;
	orientationFront.Z = 1;

	AkVector orientationTop;
	orientationTop.X = orientationTop.Z = 0;
	orientationTop.Y = 1;

	AkSoundPosition soundPos;
	soundPos.Set(position, orientationFront, orientationTop);
	AK::SoundEngine::SetPosition(m_gameObjId, soundPos);
}

bool SpatialAudioGameObject::UpdateRoom(Box* in_pBox, AkRoomID in_roomId)
{
	float x, y;
	MovableChip::GetPos(x, y);

	if (in_pBox->ContainsPoint(x, y))
	{
		AK::SpatialAudio::SetGameObjectInRoom(m_gameObjId, in_roomId);
		return true;
	}
	
	AK::SpatialAudio::SetGameObjectInRoom(m_gameObjId, AK::SpatialAudio::kOutdoorRoomID);
	return false;
}

bool SpatialAudioGameObject::UpdateRoom(std::vector<std::unique_ptr<Box>>& in_boxes, AkRoomID in_baseRoomId)
{
	float x, y;
	MovableChip::GetPos(x, y);

	int roomNumber = 0;
	for (auto& in_box : in_boxes)
	{
		if (in_box->ContainsPoint(x, y))
		{
			AK::SpatialAudio::SetGameObjectInRoom(m_gameObjId, in_baseRoomId + roomNumber);
			return true;
		}
		roomNumber++;
	}
	
	AK::SpatialAudio::SetGameObjectInRoom(m_gameObjId, AK::SpatialAudio::kOutdoorRoomID);
	return false;
}
