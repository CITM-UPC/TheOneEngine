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

// SpatialAudioHelper.h
/// \file
/// Declares various helpers for the Spatial Audio Demos

#pragma once

#include "MovableChip.h"
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>
#include <memory>

class Page;

class Box : public Control
{
public:
	Box(Page& in_rParentPage) : Control(in_rParentPage), m_iWidth(0), m_iHeight(0) {}

	Box(Page& in_rParentPage, int in_iXPos, int in_iYPos, int in_iWidth, int in_iHeight)
		:Control(in_rParentPage)
		, m_iWidth(in_iWidth)
		, m_iHeight(in_iHeight)
	{
		SetPosition(in_iXPos, in_iYPos);
	}

	virtual ~Box() {}

	/// Updates the Control based on the input received.
	/// \sa UGBtnState
	virtual void Update(const UniversalGamepad& ) {}

	/// Draws the Control to the screen.
	/// \sa DrawStyle
	virtual void Draw(DrawStyle in_eDrawStyle = DrawStyle_Control)
	{
		if (m_iWidth > 0 && m_iHeight > 0)
		{
			DrawLineOnScreen(m_iXPos, m_iYPos, m_iXPos + m_iWidth, m_iYPos, in_eDrawStyle);
			DrawLineOnScreen(m_iXPos, m_iYPos + m_iHeight, m_iXPos + m_iWidth, m_iYPos + m_iHeight, in_eDrawStyle);
			DrawLineOnScreen(m_iXPos, m_iYPos, m_iXPos, m_iYPos + m_iHeight, in_eDrawStyle);
			DrawLineOnScreen(m_iXPos + m_iWidth, m_iYPos, m_iXPos + m_iWidth, m_iYPos + m_iHeight, in_eDrawStyle);
		}
	}

	virtual void Draw(float color[3])
	{
		if (m_iWidth > 0 && m_iHeight > 0)
		{
			DrawLineOnScreen(m_iXPos, m_iYPos, m_iXPos + m_iWidth, m_iYPos, color);
			DrawLineOnScreen(m_iXPos, m_iYPos + m_iHeight, m_iXPos + m_iWidth, m_iYPos + m_iHeight, color);
			DrawLineOnScreen(m_iXPos, m_iYPos, m_iXPos, m_iYPos + m_iHeight, color);
			DrawLineOnScreen(m_iXPos + m_iWidth, m_iYPos, m_iXPos + m_iWidth, m_iYPos + m_iHeight, color);
		}
	}
	
	void SetDimensions(
		int in_iWidth,				///< - Width
		int in_iHeight				///< - Height
		) {
		m_iWidth = in_iWidth;
		m_iHeight = in_iHeight;
	}

	void GetRect(float &out_x, float &out_y, float &out_w, float &out_h)
	{
		out_x = (float)m_iXPos;
		out_y = (float)m_iYPos;
		out_w = (float)m_iWidth;
		out_h = (float)m_iHeight;
	}
	
	void GetCenter(float &out_x, float &out_y)
	{
		out_x = (float)m_iXPos + m_iWidth / 2.f;
		out_y = (float)m_iYPos + m_iHeight / 2.f;
	}

	void GetBottomRightCorner(float& out_x, float& out_y)
	{
		out_x = (float)m_iXPos + m_iWidth;
		out_y = (float)m_iYPos + m_iHeight;
	}

	bool ContainsPoint(float x, float y)
	{
		return x >= (float)m_iXPos && x < (float)(m_iXPos + m_iWidth) &&
			y >= (float)m_iYPos && y < (float)(m_iYPos + m_iHeight);
	}

private:
	/// Box width.
	int m_iWidth;

	/// Box height.
	int m_iHeight;
};

class Line : public Control
{
public:
	Line(Page& in_rParentPage) : Control(in_rParentPage), m_iXPos2(-1), m_iYPos2(-1) {}
	virtual ~Line() {}

	/// Updates the Control based on the input received.
	/// \sa UGBtnState
	virtual void Update(const UniversalGamepad&) {}

	/// Draws the Control to the screen.
	/// \sa DrawStyle
	virtual void Draw(DrawStyle in_eDrawStyle = DrawStyle_Control)
	{
		if (m_iXPos2 >= 0)
		{
			DrawLineOnScreen(m_iXPos, m_iYPos, m_iXPos2, m_iYPos2, in_eDrawStyle);
		}
	}

	virtual void Draw(float* color)
	{
		if (m_iXPos2 >= 0)
		{
			DrawLineOnScreen(m_iXPos, m_iYPos, m_iXPos2, m_iYPos2, color);
		}
	}

	void SetPoints(
		int in_iX1,
		int in_iY1,
		int in_iX2,
		int in_iY2
		) {
		m_iXPos = in_iX1;
		m_iYPos = in_iY1;
		m_iXPos2 = in_iX2;
		m_iYPos2 = in_iY2;
	}

private:
	/// The x-coordinate of the end position of the line.
	int m_iXPos2;

	/// The y-coordinate of the end position of the line.
	int m_iYPos2;
};

/// Movable chip representing an emitter
class SpatialAudioGameObject : public MovableChip
{
public:
	SpatialAudioGameObject(Page& in_pParentPage, AkGameObjectID in_gameObjId, AkGameObjectID in_listenerId, const char* in_pszObjName);
	virtual ~SpatialAudioGameObject();

	void GetAkPos(float &out_AkX, float &out_AkZ) {out_AkX = m_AkPosX; out_AkZ = m_AkPosZ;}

	// Sound engine Game object update helper functions
	virtual void SetPosition();
	virtual bool UpdateRoom(Box* in_pBox, AkRoomID in_roomId);
	virtual bool UpdateRoom(std::vector<std::unique_ptr<Box>>& in_boxes, AkRoomID in_baseRoomId);

protected:
	AkGameObjectID m_gameObjId;
	float m_AkPosX = 0.f;
	float m_AkPosZ = 0.f;
};

/// Movable chip representing a listener that can have a distance probe chip
class SpatialAudioListener : public SpatialAudioGameObject
{
public:
	SpatialAudioListener(Page& in_pParentPage, AkGameObjectID in_listenerId, AkGameObjectID in_distanceProbeId, const char* in_pszObjName);
	virtual ~SpatialAudioListener();

	/// Override of the Parent's Update() method.
	virtual void Update(const UniversalGamepad& in_Gamepad) override;

	/// Override of the Parent's Draw() method.
	virtual void Draw(DrawStyle in_eDrawStyle = DrawStyle_Control) override;

	// orientation adjustment helper functions
	void AddToPlayerOffset(float offset)
	{
		m_fPlayerOffset += offset;
		if (m_fPlayerOffset > 20.f)
			m_fPlayerOffset = 0.f;
	}
	void IncrementAzimuth(float in_angleIncrement) { m_fAzimuth += in_angleIncrement; }
	void IncrementElevation(float in_angleIncrement) { m_fElevation += in_angleIncrement; }

	/// Returns the Distance Probe's position if it exists
	void GetDistanceProbePos(float &out_x, float &out_y);

	// Sound engine Game object update helper functions

	/// Override of the Parent's SetPosition() method.
	void SetPosition() override;
	/// Sets the distance probe in a room.
	bool UpdateDistanceProbeRoom(Box* in_pBox, AkRoomID in_roomId);
	bool UpdateDistanceProbeRoom(std::vector<std::unique_ptr<Box>>& in_boxes, AkRoomID in_baseRoomId);

private:
	AkGameObjectID m_distanceProbeId;
	MovableChip *m_pDistanceProbeChip = NULL;
	float m_fAzimuth = 0.f;		// Listener azimuth angle
	float m_fElevation = 0.f;		// Listener elevation angle
	float m_fPlayerOffset = 0.f;	// Listener offset value from the distance probe
};

namespace SpatialAudioHelpers
{
	/// Data structure defining what surfaces to use for each wall when making a geometry out of a box.
	/// If surface index for wall is defined as AK_INVALID_SURFACE, geometry for that wall is not defined.
	/// Do not forget to define the surfaces (surface table) passed into AkGeometryParams.
	struct BoxGeometryParams
	{
		/// Constructor
		BoxGeometryParams(AkSurfIdx in_bottomWall = 0, AkSurfIdx in_rightWall = 0,
			AkSurfIdx in_topWall = 0, AkSurfIdx in_leftWall = 0,
			AkSurfIdx in_floor = AK_INVALID_SURFACE, AkSurfIdx in_ceiling = AK_INVALID_SURFACE)
			: bottomWall(in_bottomWall)
			, rightWall(in_rightWall)
			, topWall(in_topWall)
			, leftWall(in_leftWall)
			, floor(in_floor)
			, ceiling(in_ceiling)
		{
		}

		/// Index into the surface table passed into \c AkGeometryParams that describes the surface properties of the wall.  
		/// If this field is left as \c AK_INVALID_SURFACE, then that wall will not be part of the geometry.
		AkSurfIdx bottomWall, rightWall, topWall, leftWall, floor, ceiling;

		/// Surface table passed into \c AkGeometryParams
		std::vector<AkAcousticSurface> surfaces;
	};

	/// Adds a AK unit sized box geometry to spatial audio
	void AddBoxGeometry(AkGeometrySetID in_geometryId, BoxGeometryParams in_boxParams, bool in_enableDiffraction = true, bool in_enableBoudaryEdges = false);

	/// Adds a geometry instance to spatial audio and positions and scales geometry set respective to the given box
	void AddBoxGeometryInstance(AkGeometryInstanceID in_instanceId, AkGeometrySetID in_geometryId, Box* in_pBox, float in_fWidth, float in_fHeight, AkRoomID in_roomId = AkRoomID(), bool in_usedForReflectionAndDiffraction = true);
}
