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
//////////////////////////////////////////////////////////////////////
//
// AkFilePackageLowLevelIODeferred.h
//
// Extends the CAkDefaultIOHookDeferred low level I/O hook with File 
// Package handling functionality. 
//
// See AkDefaultIOHookBlocking.h for details on using the deferred 
// low level I/O hook. 
// 
// See AkFilePackageLowLevelIO.h for details on using file packages.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../Common/AkFilePackageLowLevelIO.h"
#include "AkDefaultIOHookDeferred.h"

typedef CAkFilePackageLowLevelIO<CAkDefaultIOHookDeferred> CAkFilePackageLowLevelIODeferred;
