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


class DemoAutobanks : public Page
{
public:
	static const AkGameObjectID GAME_OBJECT_HUMAN = 100;

	DemoAutobanks(Menu& in_ParentMenu);

	virtual bool Init();

	virtual void Release();

private:

	virtual void InitControls();
	void TogglePrepared();
	void ToggleSetMedia();

	void OnPreparePressed(void* in_pSender, ControlEvent* in_pEvent);
	void OnSetMediaPressed(void* in_pSender, ControlEvent* in_pEvent);
	void OnPlayPressed(void* in_pSender, ControlEvent* in_pEvent);

	AkBankID m_bankID;
	ButtonControl* m_btnPrepare;
	ButtonControl* m_btnSetMedia;
	void* m_pMediaBuffer;
	AkUInt32 m_uMediaSize;

	bool m_bPrepared;
	bool m_bMediaSet;
};
