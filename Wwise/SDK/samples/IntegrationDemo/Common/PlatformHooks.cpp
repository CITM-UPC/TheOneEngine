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
#include "PlatformHooks.h"
#include "Platform.h"

namespace AK
{
	namespace IntegrationDemo
	{
		void PlatformHooks::PreInit(::IntegrationDemo *pDemo)
		{
			// no-op by default
		}

#if !defined(AK_PLATFORM_HAS_HOOKS)
		static PlatformHooks g_PlatformHooks;
		PlatformHooks* PlatformHooks::Get()
		{
			return &g_PlatformHooks;
		}
#endif

	}
}
