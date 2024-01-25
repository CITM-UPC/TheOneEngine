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

// DemoDialogueMenu.h
/// \file
/// Contains the declaration for the DemoDialogueMenu class.

#pragma once

#include "Page.h"

/// This class represents the Dialogue Demos Menu page. This is simply a submenu to
/// link to various demonstrations involving dialogue.
class DemoDialogueMenu : public Page
{
public:

	/// DemoDialogueMenu class constructor
	DemoDialogueMenu( Menu& in_ParentMenu );

private:

	/// Initializes the controls on the page.
	virtual void InitControls();

	/// Delegate function for the "Localization Demo" button.
	void LocalizationButton_Pressed( void* in_pSender, ControlEvent* in_pEvent );

	/// Delegate function for the "Dynamic Dialogue Demo" button.
	void DynamicDialogueButton_Pressed( void* in_pSender, ControlEvent* in_pEvent );
};
