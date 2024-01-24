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

// DemoSpatialAudioMenu.h
/// \file
/// Contains the declaration for the DemoSpatialAudioMenu class.

#pragma once

#include "Page.h"

/// This class represents the Spatial Audio Demos Menu page. This is simply a submenu to
/// link to various demonstrations involving spatial audio.
class DemoSpatialAudioMenu : public Page
{
public:
	/// DemoSpatialAudioMenu class constructor
	DemoSpatialAudioMenu(Menu& in_ParentMenu);

private:

	/// Initializes the controls on the page.
	virtual void InitControls();

	/// Delegate functions for spatial audio demo buttons.
	void Scenario_Portals_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void Scenario_PortalsGeometry_Pressed(void* in_pSender, ControlEvent* in_pEvent);

	/// Delegate functions for spatial audio geometry demo buttons.
	void Scenario_Geometry_Pressed(void* in_pSender, ControlEvent* in_pEvent);

	/// Delegate functions for spatial audio reflect demo buttons.
	void Scenario_Reflect_Pressed(void* in_pSender, ControlEvent* in_pEvent);

	/// Delegate functions for spatial audio portal and reflect demo buttons.
	void Scenario_PortalsReflect_Pressed(void* in_pSender, ControlEvent* in_pEvent);

	/// Delegate functions for spatial audio reverb zones.
	void Scenario_ReverbZones_Pressed(void* in_pSender, ControlEvent* in_pEvent);
};
