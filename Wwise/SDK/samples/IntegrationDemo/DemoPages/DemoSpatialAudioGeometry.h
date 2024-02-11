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
// Demo3dBus.h
/// \file
/// Contains the declaration for the Demo3dBus class.

#pragma once

#include "Page.h"
#include "SpatialAudioHelpers.h"

class DemoSpatialAudioGeometry : public Page
{
public:

	/// Demo3dBus class constructor.
	DemoSpatialAudioGeometry(
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
	void Init_Geometry();

	// Members
	
	// UI controls/widgets
	SpatialAudioListener *m_pListenerChip;
	SpatialAudioGameObject *m_pEmitterChip;
	Line ** m_aLines;
	int m_numLines;

	static const int k_numGeometryPlanes = 2;
	Box * m_geometry[k_numGeometryPlanes];

	float m_diffraction;
	float m_tramsissionLoss;
	float m_fWidth;
	float m_fHeight;
	
	bool m_bTestNoise;
	bool m_bMoved;
};
