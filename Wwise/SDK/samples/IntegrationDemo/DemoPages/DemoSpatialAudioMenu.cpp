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

// DemoSpatialAudioMenu.cpp
/// \file 
/// Defines the methods declared in DemoSpatialAudioMenu.h.

#include "stdafx.h"

#include "Menu.h"
#include "DemoSpatialAudioMenu.h"
#include "DemoSpatialAudio.h"
#include "DemoSpatialAudioGeometry.h"
#include "DemoSpatialAudioReflect.h"
#include "DemoSpatialAudioPortalReflect.h"
#include "DemoSpatialAudioReverbZones.h"

/////////////////////////////////////////////////////////////////////
// DemoSpatialAudioMenu Public Methods
/////////////////////////////////////////////////////////////////////

DemoSpatialAudioMenu::DemoSpatialAudioMenu(Menu& in_ParentMenu) :Page(in_ParentMenu, "Spatial Audio Demos Menu")
{
	m_szHelp  = "This is the Spatial Audio Demo Menu.\n"
				"These demos show various ways to use Spatial Audio to model sound propagation across Rooms, Portals, and Geometry.\n"
				"Refer to Spatial Audio in the SDK documentation for more details.\n \n"
				"All Spatial Audio demos contains:\n"
				"* 'Emitter/[E]' - the position of an emitter,\n"
				"* 'Listener/[L]' - the position of the listener (namely the game character or camera).\n \n"
				"Their controls are:\n"
				"* <<UG_RIGHT_STICK>> or <<DIRECTIONAL_TYPE>> - Move the listener game object.\n"
				"* <<UG_BUTTON5>> + <<UG_RIGHT_STICK>> or <<DIRECTIONAL_TYPE>> - Move the emitter game object.\n"
				"* <<UG_BUTTON6>> - Offsets the listener from a 'distance probe/[DP]'.\n"
				"* <<UG_BUTTON3>> or <<UG_BUTTON4>> - Rotates the listener along the distance probe's azimuth.\n"
				"* <<UG_BUTTON5>> + <<UG_BUTTON3>> or <<UG_BUTTON4>> - Rotates the listener in elevation relative to the distance probe.\n"
				"* <<UG_BUTTON9>> - Changes Emitter output between a voice sample and noise.";
}

/////////////////////////////////////////////////////////////////////
// DemoPositioningMenu Private Methods
/////////////////////////////////////////////////////////////////////

void DemoSpatialAudioMenu::InitControls()
{
	ButtonControl* newBtn;

	// Add button to open the portals demo page
	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Portal Demo");
	newBtn->SetDelegate((PageMFP)&DemoSpatialAudioMenu::Scenario_Portals_Pressed);
	m_Controls.push_back(newBtn);

	// Add button to open the portals and geometry demo page
	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Portal and Geometry Demo");
	newBtn->SetDelegate((PageMFP)&DemoSpatialAudioMenu::Scenario_PortalsGeometry_Pressed);
	m_Controls.push_back(newBtn);

	// Add button to open the geometry demo page
	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Geometry Demo");
	newBtn->SetDelegate((PageMFP)&DemoSpatialAudioMenu::Scenario_Geometry_Pressed);
	m_Controls.push_back(newBtn);

	// Add button to open the reflect demo page
	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Reflect Demo");
	newBtn->SetDelegate((PageMFP)&DemoSpatialAudioMenu::Scenario_Reflect_Pressed);
	m_Controls.push_back(newBtn);
	
	// Add button to open the portal and reflect demo page
	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Portal and Reflect Demo");
	newBtn->SetDelegate((PageMFP)&DemoSpatialAudioMenu::Scenario_PortalsReflect_Pressed);
	m_Controls.push_back(newBtn);

	// Add button to open the portal and reflect demo page
	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Reverb Zone Demo");
	newBtn->SetDelegate((PageMFP)&DemoSpatialAudioMenu::Scenario_ReverbZones_Pressed);
	m_Controls.push_back(newBtn);
}

void DemoSpatialAudioMenu::Scenario_Portals_Pressed(void*, ControlEvent*)
{
	DemoSpatialAudio* pg = new DemoSpatialAudio(*m_pParentMenu, Scenario_Portals);
	m_pParentMenu->StackPage(pg);
}

void DemoSpatialAudioMenu::Scenario_PortalsGeometry_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	DemoSpatialAudio* pg = new DemoSpatialAudio(*m_pParentMenu, Scenario_Portals_And_Geometry);
	m_pParentMenu->StackPage(pg);
}

void DemoSpatialAudioMenu::Scenario_Geometry_Pressed(void*, ControlEvent*)
{
	DemoSpatialAudioGeometry* pg = new DemoSpatialAudioGeometry(*m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void DemoSpatialAudioMenu::Scenario_Reflect_Pressed(void*, ControlEvent*)
{
	DemoSpatialAudioReflect* pg = new DemoSpatialAudioReflect(*m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void DemoSpatialAudioMenu::Scenario_PortalsReflect_Pressed(void*, ControlEvent*)
{
	DemoSpatialAudioPortalReflect* pg = new DemoSpatialAudioPortalReflect(*m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void DemoSpatialAudioMenu::Scenario_ReverbZones_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	DemoSpatialAudioReverbZones* pg = new DemoSpatialAudioReverbZones(*m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}
