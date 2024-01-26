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

#include "stdafx.h"

#include "DemoBankLoadingMenu.h"
#include "DemoPrepare.h"
#include "DemoExternalSources.h"
#include "DemoAutobanks.h"
#include "Menu.h"

DemoBankLoadingMenu::DemoBankLoadingMenu(Menu& in_ParentMenu)
	: Page(in_ParentMenu, "Bank & Event Loading Demos Menu")
{
}

void DemoBankLoadingMenu::InitControls()
{
	ButtonControl* newBtn;

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Prepare Event/Bank Demo");
	newBtn->SetDelegate((PageMFP)&DemoBankLoadingMenu::OpenPrepareDemoPage);
	m_Controls.push_back(newBtn);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("External Sources Demo");
	newBtn->SetDelegate((PageMFP)&DemoBankLoadingMenu::OpenExternalSourcesDemoPage);
	m_Controls.push_back(newBtn);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Autobanks Demo");
	newBtn->SetDelegate((PageMFP)&DemoBankLoadingMenu::OpenAutoBanksDemoPage);
	m_Controls.push_back(newBtn);
}

void DemoBankLoadingMenu::OpenPrepareDemoPage(void* in_pSender, ControlEvent* in_pEvent)
{
	DemoPrepare* pg = new DemoPrepare(*m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void DemoBankLoadingMenu::OpenExternalSourcesDemoPage(void* in_pSender, ControlEvent* in_pEvent)
{
	DemoExternalSources* pg = new DemoExternalSources(*m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void DemoBankLoadingMenu::OpenAutoBanksDemoPage(void* in_pSender, ControlEvent* in_pEvent)
{
	DemoAutobanks* pg = new DemoAutobanks(*m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}
