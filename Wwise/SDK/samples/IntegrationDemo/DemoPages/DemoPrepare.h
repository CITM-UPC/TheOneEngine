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

class DemoPrepare : public Page
{
public:

	DemoPrepare(Menu& in_ParentMenu);

	virtual bool Init();

	virtual void Release();

	void EnterArea(AkUInt32 area);
	void LeaveArea();

private:

	virtual void InitControls();

	void OnAreaPressed(void* in_pSender, ControlEvent* in_pEvent);

	/// Game Object ID for the "Human".
	static const AkGameObjectID GAME_OBJECT_HUMAN = 100;
	AkUInt32 m_uCurrentArea;
	AkPlayingID m_uCurrentPlayingID;
};
