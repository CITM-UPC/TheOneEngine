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

// DemoPositioningMenu.cpp
/// \file 
/// Defines the methods declared in DemoPositioningMenu.h.

#include "stdafx.h"

#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

#include "Menu.h"
#include "DemoPositioningMenu.h"
#include "DemoPositioning.h"
#include "Demo3dBus.h"
#include "Demo3dAudio.h"


/////////////////////////////////////////////////////////////////////
// DemoPositionMenu Public Methods
/////////////////////////////////////////////////////////////////////

DemoPositioningMenu::DemoPositioningMenu(Menu& in_ParentMenu) :Page(in_ParentMenu, "Positioning Demos Menu")
{

}


/////////////////////////////////////////////////////////////////////
// DemoPositioningMenu Private Methods
/////////////////////////////////////////////////////////////////////

void DemoPositioningMenu::InitControls()
{
	ButtonControl* newBtn;

	// Add button to open the localization demo page
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Position Demo" );
	newBtn->SetDelegate((PageMFP)&DemoPositioningMenu::SinglePosition_Pressed);
	m_Controls.push_back( newBtn );

	// Add button to open the dynamic dialogue demo page
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Multi-Position Demo" );
	newBtn->SetDelegate((PageMFP)&DemoPositioningMenu::Multiposition_Pressed);
	m_Controls.push_back(newBtn);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("3D Bus - Clustering/3D Submix");
	newBtn->SetDelegate((PageMFP)&DemoPositioningMenu::Scenario_3dSubmix_Pressed);
	m_Controls.push_back(newBtn);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("3D Bus - 3D Portal and Standard Room");
	newBtn->SetDelegate((PageMFP)&DemoPositioningMenu::Scenario_CoupledRoomsSimple_Pressed);
	m_Controls.push_back(newBtn);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("3D Bus - 2X 3D Portals");
	newBtn->SetDelegate((PageMFP)&DemoPositioningMenu::Scenario_CoupledRoomsWithFeedback_Pressed);
	m_Controls.push_back(newBtn);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("3D Audio Objects and Spatialized Bed");
	newBtn->SetDelegate((PageMFP)&DemoPositioningMenu::Scenario_3DAudio_Pressed);
	m_Controls.push_back(newBtn);
}

void DemoPositioningMenu::SinglePosition_Pressed(void*, ControlEvent*)
{
	DemoPositioning* pg = new DemoPositioning(*m_pParentMenu, false);
	m_pParentMenu->StackPage( pg );
}

void DemoPositioningMenu::Multiposition_Pressed(void*, ControlEvent*)
{
	DemoPositioning* pg = new DemoPositioning(*m_pParentMenu, true);
	m_pParentMenu->StackPage( pg );
}

void DemoPositioningMenu::Scenario_CoupledRoomsSimple_Pressed(void*, ControlEvent*)
{
	Demo3dBus* pg = new Demo3dBus(*m_pParentMenu, Scenario_CoupledRoomsSimple);
	m_pParentMenu->StackPage(pg);
}

void DemoPositioningMenu::Scenario_CoupledRoomsWithFeedback_Pressed(void*, ControlEvent*)
{
	Demo3dBus* pg = new Demo3dBus(*m_pParentMenu, Scenario_CoupledRoomsWithFeedback);
	m_pParentMenu->StackPage(pg);
}

void DemoPositioningMenu::Scenario_3dSubmix_Pressed(void*, ControlEvent*)
{
	Demo3dBus* pg = new Demo3dBus(*m_pParentMenu, Scenario_3dSubmix);
	m_pParentMenu->StackPage(pg);
}

void DemoPositioningMenu::Scenario_3DAudio_Pressed(void*, ControlEvent*)
{
	Demo3dAudio* pg = new Demo3dAudio(*m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}
