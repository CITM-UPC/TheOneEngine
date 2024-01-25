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

#pragma once

#include "Page.h"

class DemoBankLoadingMenu : public Page
{
public:

	DemoBankLoadingMenu(Menu& in_ParentMenu);

private:

	virtual void InitControls();


	/// Delegate function for the "Prepare Events/Banks Demo" button.
	void OpenPrepareDemoPage(void* in_pSender, ControlEvent* in_pEvent);

	/// Delegate function for the "External Sources Demo" button.
	void OpenExternalSourcesDemoPage(void* in_pSender, ControlEvent* in_pEvent);

	/// Delegate function for the "Autobanks Demo" button.
	void OpenAutoBanksDemoPage(void* in_pSender, ControlEvent* in_pEvent);
};
