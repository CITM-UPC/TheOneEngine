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

#include <AK/SoundEngine/Common/AkSoundEngine.h>

#include "DemoPrepare.h"

static const char* kButtonLabels[] = {
	"Enter Area 1",
	"Enter Area 2"
};

class AreaButton : public ButtonControl
{
public:
	AreaButton(DemoPrepare* in_pParentPage, AkUInt32 uArea)
		: ButtonControl(*in_pParentPage)
		, m_pPage(in_pParentPage)
		, m_uArea(uArea)
	{}

	virtual void OnFocus() override
	{
		m_pPage->EnterArea(m_uArea);
	}

private:
	DemoPrepare* m_pPage;
	AkUInt32 m_uArea;
};

DemoPrepare::DemoPrepare(Menu& in_ParentMenu)
	: Page(in_ParentMenu, "Prepare Event & Bank Demo")
	, m_uCurrentArea(0)
	, m_uCurrentPlayingID(AK_INVALID_PLAYING_ID)
{
	m_szHelp =
		"This page shows how to use the PrepareBank and PrepareEvent API functions.\n\n"
		"When the page is loaded, a PrepareBank operation loads the lightweight structure and event data "
		"referenced by this demo, without loading any actual media into memory.\n\n"
		"When the user moves the cursor to an area button, a PrepareEvent operation loads "
		"the corresponding media file (a .wem file on disk) into memory in anticipation of a future PostEvent.\n\n" 
		"When the user finally enters the area, the event is posted and the media is ready to play. ";
}

bool DemoPrepare::Init()
{
	// First, load the structure and events covering both areas of the "game". No media will be loaded.
	AKRESULT eResult = AK::SoundEngine::PrepareBank(
		AK::SoundEngine::Preparation_Load,
		AKTEXT("PrepareDemo.bnk"),
		nullptr,
		nullptr,
		AK::SoundEngine::AkBankContent_StructureOnly);
	if (eResult != AK_Success)
		return false;

	// Register the "Human" game object
	AK::SoundEngine::RegisterGameObj(GAME_OBJECT_HUMAN, "Human");

	EnterArea(1);

	return Page::Init();
}

void DemoPrepare::Release()
{
	LeaveArea();

	AK::SoundEngine::PrepareBank(
		AK::SoundEngine::Preparation_Unload,
		AKTEXT("PrepareDemo.bnk"),
		nullptr,
		nullptr,
		AK::SoundEngine::AkBankContent_StructureOnly);

	AK::SoundEngine::UnregisterGameObj(GAME_OBJECT_HUMAN);

	Page::Release();
}

void DemoPrepare::InitControls()
{
	ButtonControl* newBtn;

	newBtn = new AreaButton(this, 1);
	newBtn->SetLabel(kButtonLabels[0]);
	newBtn->SetDelegate((PageMFP)&DemoPrepare::OnAreaPressed);
	m_Controls.push_back(newBtn);

	newBtn = new AreaButton(this, 2);
	newBtn->SetLabel(kButtonLabels[1]);
	newBtn->SetDelegate((PageMFP)&DemoPrepare::OnAreaPressed);
	m_Controls.push_back(newBtn);

}

static const char* EventForArea(AkUInt32 uArea)
{
	return uArea == 1 ? "Enter_Area_1" : "Enter_Area_2";
}

void DemoPrepare::EnterArea(AkUInt32 area)
{
	LeaveArea();

	// Entering new area, prepare the associated event, which will load the associated media
	const char* events[1] = { EventForArea(area) };
	AK::SoundEngine::PrepareEvent(AK::SoundEngine::Preparation_Load, events, 1);

	m_uCurrentArea = area;
}

void DemoPrepare::LeaveArea()
{
	if (m_uCurrentPlayingID != AK_INVALID_PLAYING_ID)
	{
		AK::SoundEngine::StopPlayingID(m_uCurrentPlayingID, 0);
		m_uCurrentPlayingID = AK_INVALID_PLAYING_ID;
	}
	if (m_uCurrentArea > 0)
	{
		// Leaving previous area, un-prepare the associated event, which will unload the associated media
		const char* events[1] = { EventForArea(m_uCurrentArea) };
		AK::SoundEngine::PrepareEvent(AK::SoundEngine::Preparation_Unload, events, 1);
	}
}

void DemoPrepare::OnAreaPressed(void* in_pSender, ControlEvent* in_pEvent)
{
	if (m_uCurrentPlayingID == AK_INVALID_PLAYING_ID)
		m_uCurrentPlayingID = AK::SoundEngine::PostEvent(EventForArea(m_uCurrentArea), GAME_OBJECT_HUMAN, 0);
}
