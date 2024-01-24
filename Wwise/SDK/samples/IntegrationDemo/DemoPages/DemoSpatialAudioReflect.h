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

// DemoSpatialAudioReflect.h
/// \file
/// Contains the declaration for the DemoSpatialDemoReflect class.

#pragma once

#include "Page.h"
#include "SpatialAudioHelpers.h"

enum AcousticTextures
{
	Brick = 0,
	Anechoic,
	Carpet,
	Concrete,
	Drywall,
	Tile,
	Wood
};

class DemoSpatialAudioReflect : public Page
{
public:

	/// DemoSpatialAudioReflect class constructor.
	DemoSpatialAudioReflect(
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

	// Helpers

	void ShrinkRoom();			/// Decreases size of room box by 2px in width towards its center
	void EnlargeRoom();			/// Increases size of room box by 2px in width away from its center
	void setAcousticTexture();	/// Updates geometry acoustic texture

	// Members
	
	// UI controls/widgets
	SpatialAudioListener *m_pListenerChip;
	SpatialAudioGameObject *m_pEmitterChip;
	Box * m_pRoom;
	Box * m_pWall;

	Line ** m_aLines;
	int * m_lineReflectOrder;
	int m_numLines;

	float m_fWidth;
	float m_fHeight;
	float m_room_y;				// running room y position to avoid errors through type casting
	float m_room_height;		// running room height to avoid errors through type casting
	AkUInt32 m_reflect_order;

	AkUInt8 m_surface_texture;	// Index keeping track of current surface texture
	const char *str_texture;	// TextureID
	float texture_color[3];		// rgb color representation of texture

	bool m_bTestNoise;
	bool m_bMoved;
};