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

// DemoSpatialAudioPortalReflect.h
/// \file
/// Contains the declaration for the DemoSpatialAudioPortalReflect class.

#pragma once

#include "Page.h"
#include "SpatialAudioHelpers.h"

#include <memory>

class DemoSpatialAudioPortalReflect : public Page
{
public:
	/// DemoSpatialAudioPortalReflect class constructor.
	DemoSpatialAudioPortalReflect(
		Menu& in_pParentMenu	///< - Pointer to the Menu that the page belongs to
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
	void Init_Rooms();
	void Init_Portals();
	void Init_Obstacles();
	void Set_Portal(int in_portalNum);

	// Members
	
	// UI controls/widgets
	SpatialAudioListener *m_pListenerChip;
	SpatialAudioGameObject *m_pEmitterChip;
	std::vector<std::unique_ptr<Box>> m_rooms;
	std::vector<std::unique_ptr<Box>> m_obstacles;
	std::vector<std::unique_ptr<Box>> m_portals;

	std::vector<std::unique_ptr<Line>> m_diffractionLines;
	std::vector<std::unique_ptr<Line>> m_reflectionLines;
	std::vector<int> m_reflectionOrders;

	float m_fWidth;
	float m_fHeight;
	float m_dryDiffraction;
	float m_wetDiffraction;
	float m_transmissionLoss;
	AkUInt8 m_portalsOpen;	// bit field: bit0 -> portal0-2, bit1 -> portal1-3, bit2 -> portal0-Outside, bit3 -> portal1-Outside, bit4 -> portal2-Outside, bit5 -> porta3-Outside

	bool m_bTestNoise;
	bool m_bMoved;
};
