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

#if defined(AK_USE_SYSTEM_SDL)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "UniversalInput.h"
#include "IAkInputMgr.h"

class UniversalInput;

class SDLInputMgr : public IAkInputMgr
{
public:

	SDLInputMgr();

	/// SDLInputMgr class destructor
	~SDLInputMgr();

	virtual bool Init(AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount) override;

	virtual UniversalInput* UniversalInputAdapter() const override;

	virtual void Update() override;

	virtual void Release() override;

	virtual bool ShouldQuit() const override { return m_bShouldQuit; }
	
	AkUInt32 GetControllerOutputId(int in_iPlayerIndex) const override { return 0; }

private:
	UGBtnState ScanCodeToBtnState( SDL_Scancode in_scanCode ) const;

	/// Pointer to the Universal Input object to interface with the Menu System
	/// \sa UniversalInput
	UniversalInput* m_pUInput = nullptr;

	// Keyboard input history; entry 0 is the most recent
	UGBtnState m_btnKeyboard = 0;

	bool m_bDidMotion = false;
	bool m_bShouldQuit = false;
};
