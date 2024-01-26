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

// Render.h
/// \file 
/// Contains prototypes of per-platform rendering facilities

#pragma once


#include <string>
#include "Helpers.h"
#include <AK/SoundEngine/Common/AkTypes.h>

#if !defined (OLD_INTDEMO_RENDER)

enum TextureFormat {
	TextureFormat_R8G8B8A8,
	TextureFormat_Num
};

// Interface for a texture to be updated from the CPU frequently (e.g. once-per-frame)
// Currently, only RGBA (4B/tx) textures are supported
class IDynamicTexture2D : public IRefCountable
{ 
protected:
	IDynamicTexture2D() {}
public:
	virtual ~IDynamicTexture2D() {}
	// bufferPitch and bufferLength each define the length of one row of data and the total image in number of bytes
	virtual void UpdateResource(const void* buffer, size_t bufferPitch, size_t bufferLength) = 0;
};

/// Initializes the system's drawing engine for usage.
/// \return True if the system has been initialized, false otherwise.
/// \sa TermDrawing()
bool RenderInit(
	void* in_pParam,						///< - Pointer to any platform specific parameter (eg. the window handle in Windows)
	AkOSChar* in_szErrorBuffer,				///< - Buffer where error details will be written (if the function returns false)
	unsigned int in_unErrorBufferCharCount	///< - Number of characters available in in_szErrorBuffer, including terminating NULL character
);

/// Closes the drawing engine and releases any used resources.
/// \sa InitDrawing()
void RenderTerm();

/// Begins a drawing sequence
/// \sa DoneDrawing()
void RenderBeginFrame();

/// Ends the drawing sequence and outputs the drawing.
/// \sa BeginDrawing()
void RenderEndFrame();

IDynamicTexture2D* RenderCreateDynamicTexture(AkUInt32 width, AkUInt32 height);

struct DrawRect
{
	float x1, y1, x2, y2;

	DrawRect(float _x1, float _y1, float _x2, float _y2)
		:x1(_x1), y1(_y1), x2(_x2), y2(_y2)
	{}

	DrawRect()
		:x1(0.0f), y1(0.0f), x2(0.0f), y2(0.0f)
	{}
};

// Draws the portion of the provided texture described by the DrawRect "src", to the portion
// of the viewport described by the DrawRect "dest"
// Top-left of the screen and texture are (0,0), and bottom-right are (1,1)
void RenderDrawTexture2D(IDynamicTexture2D& texture, DrawRect src, DrawRect dest);

// Draws a line to the viewport with the given start (x1,y1) and end (x2, y2) coordinates
// with the provided color (r, g, b)
// top-left of screen being (0,0), bottom-right being (1,1)
// color of black being (0,0,0), white being (1,1,1)
void RenderDrawLine2D(float x1, float y1, float x2, float y2, float color[3]);

#endif