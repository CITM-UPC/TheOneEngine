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

#include "IntegrationDemo.h"
#include "Platform.h"

#if defined(INTDEMO_STATIC_LINK_PREMIUM_PLUGINS)

#if defined(__has_include)

// Enable Motion demo if AkMotion plugin is installed locally (and the platform supports it)
#if defined(AK_PLATFORM_SUPPORTS_MOTION) && __has_include(<AK/Plugin/AkMotionSinkFactory.h>) && __has_include(<AK/Plugin/AkMotionSourceSourceFactory.h>)
#define INTDEMO_LINK_MOTION
#endif

// Enable Reflect demo if AkReflect plugin is installed locally
#if __has_include(<AK/Plugin/AkReflectFXFactory.h>)
#define INTDEMO_LINK_REFLECT
#endif

#endif // __has_include


#if defined(INTDEMO_LINK_MOTION)

// Link statically with AkMotionSink
#include <AK/Plugin/AkMotionSinkFactory.h>
#pragma comment(lib, "AkMotionSink")

// Link statically with AkMotionSource
#include <AK/Plugin/AkMotionSourceSourceFactory.h>
#pragma comment(lib, "AkMotionSourceSource")

#endif // INTDEMO_LINK_MOTION


#if defined(INTDEMO_LINK_REFLECT)

// Link statically with AkReflectFX
#include <AK/Plugin/AkReflectFXFactory.h>
#pragma comment(lib, "AkReflectFX")

#endif // INTDEMO_LINK_REFLECT

#endif
