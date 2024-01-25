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

// DemoSpatialAudio.h
/// \file
/// Contains the declaration for the DemoSpatialAudio class.

#pragma once

#include "Page.h"
#include "SpatialAudioHelpers.h"

enum DemoSpatialAudio_Scenario
{
	Scenario_Portals,
	Scenario_Portals_And_Geometry
};

class DemoSpatialAudio : public Page
{
	friend class GameSideObstructionComponent;
	friend class GeometryInRooms;

public:

	/// Demo3dBus class constructor.
	DemoSpatialAudio(
		Menu& in_pParentMenu,	///< - Pointer to the Menu that the page belongs to
		DemoSpatialAudio_Scenario in_scenario
		);

	/// Override of the Parent's Init() method.
	virtual bool Init();

	/// Override of the Parent's Release() method.
	virtual void Release();

	/// Override of the Parent's Update() method.
	virtual bool Update();

	/// Override of the Parent's Draw() method.
	virtual void Draw();

	virtual bool OnPointerEvent( PointerEventType in_eType, int in_x, int in_y );

private:

	/// Initializes the controls on the page.
	virtual void InitControls();
	void InitControls_Portals();
	void SetPortals();

	struct AkRoomID IsInRoom(float in_x, float in_z);

	// Members
	
	// UI controls/widgets
	SpatialAudioGameObject *m_pEmitterChip;
	SpatialAudioListener *m_pListenerChip;
	Box * m_pRoom;
	Box * m_pPortal0;
	Box * m_pPortal1;
	Line ** m_aLines;
	int m_numLines;

	float m_dryDiffraction;
	float m_wetDiffraction;
	float m_transmissionLoss;
	float m_fWidth;
	float m_fHeight;
	AkUInt8 m_portalsOpen;	// bit field: bit0 -> portal0, bit1 -> portal1

	AkUInt32 m_uLastTick;
	bool m_bTestNoise;
	bool m_bMoved;
	
	DemoSpatialAudio_Scenario m_eScenario;

	// Optional components. 
	// In the Portals (only) demo, the "game" takes care of computing and setting obstruction between the emitter and listener when they are in the same room, and between portals and the listener.
	// This is implemented in GameSideObstructionComponent. No geometry in passed to Wwise Spatial Audio in that demo, and m_pGeometryInRooms is thus NULL.
	// In the Portals + Geometry demo, propagation of sound via diffraction effects is entirely handled by Wwise Spatial Audio, either via Portals or geometry, and thus m_pGameSideObs is not needed and left NULL.
	class GameSideObstructionComponent * m_pGameSideObs;
	class GeometryInRooms * m_pGeometryInRooms;
};

// Implements game-side obstruction when emitters and listeners are in the same room.
class GameSideObstructionComponent
{
public:
	GameSideObstructionComponent();
	~GameSideObstructionComponent();

	void Update(DemoSpatialAudio * in_pDemo);
	void Draw();

private:
	Line * m_aRaycastEtoL;
};

// Defines and registers geometry to Spatial Audio.
class GeometryInRooms
{
public:
	GeometryInRooms(DemoSpatialAudio * in_pDemo);
	~GeometryInRooms();

	void Draw();

private:
	Box * m_pInnerRoom;
	Box * m_pObstacle;
};
