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

// DemoSpatialAudioReverbZones.h
/// \file
/// Contains the declaration for the DemoSpatialAudioReverbZones class.

#pragma once

#include "Page.h"
#include "SpatialAudioHelpers.h"

#include <memory>

class DemoSpatialAudioReverbZones: public Page
{
public:
	/// DemoSpatialAudioReverbZones class constructor.
	DemoSpatialAudioReverbZones(
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

	virtual bool OnPointerEvent(PointerEventType in_eType, int in_x, int in_y);

private:
	/// Initializes the controls on the page.
	virtual void InitControls();
	void Init_Rooms();
	void Init_Portals();

	void SetPortals();

	// Members

	// UI controls/widgets
	SpatialAudioListener* m_pListenerChip;
	SpatialAudioGameObject* m_pEmitterChip;
	Box* m_pRoom;
	Box* m_pPatio;
	Box* m_pPortal;
	Box* m_pObstacle;
	
	std::vector<std::unique_ptr<Line>> m_diffractionLines;

	float m_fWidth;
	float m_fHeight;
	float m_dryDiffraction;
	float m_wetDiffraction;
	float m_transmissionLoss;

	AkUInt32 m_uLastTick;
	bool m_bTestNoise;
	bool m_bMoved;

	bool m_portalOpen;
};
