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

// Drawing.cpp
/// \file 
// Implements the Render.h functions


/////////////////////////
//  INCLUDES
/////////////////////////

#include "stdafx.h"
#include "Render.h"

#include <Platform.h>

#if defined(INTDEMO_RENDER_D3D12)

/////////////////////////
//    BACKEND CORE     //
/////////////////////////

#include "D3D12/RenderDevice.h"
#include "D3D12/LineGraphicsPipeline.h"
#include "D3D12/TextureGraphicsPipeline.h"
#include "D3D12/DynamicTexture2D.h"

///////////////////////////////
//  BACKEND TEXTURE HANDLING //
///////////////////////////////

using namespace d3d12;

LineGraphicsPipeline g_lineGraphicsPipeline;
TextureGraphicsPipeline g_textureGraphicsPipeline;

IDynamicTexture2D* RenderCreateDynamicTexture(AkUInt32 width, AkUInt32 height)
{
	return new DynamicTexture2D(width, height);
}

//////////////////////////////////////////////////////////////////////////
// texture 2d drawing

void RenderDrawTexture2D(IDynamicTexture2D& texture, DrawRect src, DrawRect dest)
{
	g_textureGraphicsPipeline.AddTexture(src, dest, &static_cast<DynamicTexture2D&>(texture));
}

//////////////////////////////////////////////////////////////////////////
// line 2D drawing

void RenderDrawLine2D(float x1, float y1, float x2, float y2, float color[3])
{
	g_lineGraphicsPipeline.AddLine(x1, y1, x2, y2, color);
}

//////////////////////////////////////////////////////////////////////////
// Render implementation
//////////////////////////////////////////////////////////////////////////

bool RenderInit(void* in_pParam, AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount)
{
	bool res = RenderDevice::Instance().Init(reinterpret_cast<HWND>(in_pParam));
	g_textureGraphicsPipeline.Init();
	g_lineGraphicsPipeline.Init();
	return res;
}

void RenderTerm()
{
	auto& device = RenderDevice::Instance();

	device.WaitForFrame();

	g_textureGraphicsPipeline.Term();
	g_lineGraphicsPipeline.Term();
	device.Term();
}

void RenderBeginFrame()
{
	RenderDevice::Instance().BeginFrame();

	// RenderDrawTexture2D is called for each texture every frame 
	g_textureGraphicsPipeline.ClearTextures();

	// RenderDrawLine2D is called for each line every frame
	g_lineGraphicsPipeline.ClearLines();
}

void RenderEndFrame()
{
	auto& device = RenderDevice::Instance();

	device.RenderBegin();
	{
		g_textureGraphicsPipeline.SendDrawCommands();
		g_lineGraphicsPipeline.SendDrawCommands();
	}
	device.RenderEnd();
}

#endif
