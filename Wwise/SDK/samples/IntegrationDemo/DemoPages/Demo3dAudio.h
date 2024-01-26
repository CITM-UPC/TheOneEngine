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

// Demo3dAudio.h
/// \file
/// Contains the declaration for the Demo3dAudio class.

#pragma once

#include "Page.h"
#include "MovableChip.h"

struct AkOutputDeviceMeteringCallbackInfo;

class Demo3dAudio : public Page
{
public:
	/// Demo3dAudio class constructor.
	Demo3dAudio(
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

	void OnDeviceMetering(AkOutputDeviceMeteringCallbackInfo* in_pCallbackInfo);
private:

	/// Initializes the controls on the page.
	virtual void InitControls();
	void UpdateGameObjPos();
	void UpdateCallback();
	void UnregisterCallback();
	void ReplaceOutput(AkUInt8 uSharesetMode);

	// Members

	MovableChip *m_pChip;

	float m_fGameObjectX;
	float m_fGameObjectZ;
	float m_fWidth;
	float m_fHeight;
	float m_LastX;
	float m_LastY;
	int m_iLastFootstepTick;

	// Metering info for the output device
	AkUInt32 m_uMMPeaks;
	AkUInt32 m_uPTPeaks;
	AkUInt32 m_uObjPeaks;
	AkReal32 m_MainMixPeaks[36];
	AkReal32 m_PassthroughPeaks[2];
	AkReal32 m_ObjectPeaks[20];

	AkUInt8 m_uCurrentSharesetMode; // Two-bit bitfield: 00 == No 3D, 01 == 3D but no objects, 11 3D with objects
	AkOutputDeviceID m_RegisteredCallbackOutputID;
	bool m_bButtonPressed;
};
