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

#include <AK/Tools/Common/AkAssert.h>

// In order to define a function called CreateWindow, the Windows macro needs to // be undefined.
#if defined(CreateWindow)
#undef CreateWindow
#endif

#if defined (max)
#undef max
#endif

// Windows Runtime Library (for ComPtr)
#include <wrl.h>
using namespace Microsoft::WRL; // todo: move this to ccp

// DirectX 12 headers
#ifdef AK_WIN
	#include "d3d12.h"
	#include <dxgi1_6.h> 
	#define IGraphicsUnknown IUnknown
#else
	#define IGraphicsUnknown IGraphicsUnknown
#endif

// D3D12 extension library.
#if defined(AK_WIN)
#include "d3dx12.h"
#endif

#include <cassert>
#include <cstdint>

#ifdef AK_WIN
#define PLATFORM_IDD_PPV_ARGS IID_PPV_ARGS
#else
#define PLATFORM_IDD_PPV_ARGS IID_GRAPHICS_PPV_ARGS
#endif

struct vec2 { float x; float y; };
struct vec3 { float x; float y; float z; };

namespace RenderingSettings
{
	static const uint32_t kConcurrentFrameCount = 2;
}

#define ThrowIfFailed(Result) { HRESULT throwIfFailedResult = Result; AKASSERT(SUCCEEDED(throwIfFailedResult)); }
