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

// InputMgrScePadHelpers.h
/// \file 
/// Declares other methods used for adding ScePad support to IntegrationDemo

#include "stdafx.h"

/// Uncomment/define this to enable support of libScePad. Note that you will need the *static* libScePad for PC extension to use this
//#define INTDEMO_LIBSCEPAD_SUPPORT

#if defined(INTDEMO_LIBSCEPAD_SUPPORT)
// Initializes scepad, and populates the provided array with padHandles for each user
void InitScePad(int* out_pScePadHandles);
// closes all of the handles provided
void TermScePadHandle(int in_scePadHandle);
// Reads the current state of the scePad into the UG structures
void ReadScePadState(int in_iScePadHandle, bool& out_bConnected, UGBtnState& out_iState, UGStickState& out_joystickLeft, UGStickState& out_joystickRight);
// Fetches a deviceId for Wwise Motion device identification (AK_Success if connected, AK_Fail otherwise)
void GetScePadDeviceId(int in_padHandle, AkDeviceID& out_resolvedId);
#endif
