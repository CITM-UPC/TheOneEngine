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

// Drawing.h
/// \file 
/// Contains abstracted prototypes of cross-platform drawing functions

#pragma once

#include <string>
#include "Platform.h"
#include <AK/SoundEngine/Common/AkTypes.h>

#define INTEGRATIONDEMO_MAX_MESSAGE 1024

#if !defined(OLD_INTDEMO_RENDER) && !defined(INTEGRATIONDEMO_SCREEN_CHAR_WIDTH)
	#define INTEGRATIONDEMO_SCREEN_CHAR_WIDTH	115
#endif

/// Style options to modify how the text drawn to the screen looks.
/// \sa DrawTextOnScreen()
enum DrawStyle
{
	DrawStyle_Control = 0,  ///< Control text, this is the default style
	DrawStyle_Selected, ///< Highlighted Control text
	DrawStyle_Title,	///< Page title text
	DrawStyle_Error,	///< Error Message Text
	DrawStyle_Text,		///< Plain Text, smaller font
	DrawStyle_SelectedText,	///< Plain Text, smaller font, selected
	
	DrawStyle_Num		///< Number of styles
};

#if defined (OLD_INTDEMO_RENDER)

/// Initializes the system's drawing engine for usage.
/// \return True if the system has been initialized, false otherwise.
/// \sa TermDrawing()
bool InitDrawing(
	void* in_pParam,						///< - Pointer to any platform specific parameter (eg. the window handle in Windows)
	AkOSChar* in_szErrorBuffer,				///< - Buffer where error details will be written (if the function returns false)
	unsigned int in_unErrorBufferCharCount,	///< - Number of characters available in in_szErrorBuffer, including terminating NULL character
	AkUInt32 in_windowWidth = 640,			///< - window widht in pixels
	AkUInt32 in_windowHeight = 480			///< - window height in pixels
);

AkUInt32 GetWindowWidth();
AkUInt32 GetWindowHeight();

#else

/// Initializes the system's drawing engine for usage.
/// \return True if the system has been initialized, false otherwise.
/// \sa TermDrawing()

bool InitDrawing(
	AkOSChar* in_szErrorBuffer,					///< - Buffer where error details will be written (if the function returns false)
	unsigned int in_unErrorBufferCharCount,		///< - Number of characters available in in_szErrorBuffer, including terminating NULL character
	AkUInt32 in_canvasWidth,					///< - width of canvas that is drawn to
	AkUInt32 in_canvasHeight,					///< - height of canvas that is drawn to
	AkUInt32 in_layoutWidth,					///< - width of drawing space for layout (DrawTextOnScreen/DrawLineOnSCreen use layout coordinates)
	AkUInt32 in_layoutHeight);					///< - height of drawing space for layout (DrawTextOnScreen/DrawLineOnSCreen use layout coordinates)

AkUInt32 GetLayoutWidth();
AkUInt32 GetLayoutHeight();

#endif

// call to prepare drawing systems for a new frame
void BeginDrawing();

/// Draws a string of text on the screen at a given point.
/// \note The coordinates (0, 0) refer to the top-left corner of the screen.
/// \warning This function must be called between calls to BeginDrawing and DoneDrawing.
/// \sa BeginDrawing(), DoneDrawing()
void DrawTextOnScreen( 
	const char* in_szText,	///< - The string to draw on the screen
	int in_iXPos,			    ///< - The X value of the drawing position
	int in_iYPos,			    ///< - The Y value of the drawing position
	DrawStyle in_eDrawStyle = DrawStyle_Control		///< - The style with which to draw the text
);

/// Draws a line on the screen between given points.
/// \note The coordinates (0, 0) refer to the top-left corner of the screen.
/// \warning This function must be called between calls to BeginDrawing and DoneDrawing.
/// \sa BeginDrawing(), DoneDrawing()
void DrawLineOnScreen(
	int in_iXPosStart,		    ///< - The X value of the drawing position
	int in_iYPosStart,		    ///< - The Y value of the drawing position
	int in_iXPosEnd,		    ///< - The X value of the drawing position
	int in_iYPosEnd,		    ///< - The Y value of the drawing position
	DrawStyle in_eDrawStyle = DrawStyle_Control		///< - The style with which to draw the text
	);

/// Draws a line on the screen between given points with the given color (r, g, b).
/// \note The coordinates (0, 0) refer to the top-left corner of the screen.
/// \warning This function must be called between calls to BeginDrawing and DoneDrawing.
/// \sa BeginDrawing(), DoneDrawing()
void DrawLineOnScreen(int in_iXPosStart, int in_iYPosStart, int in_iXPosEnd, int in_iYPosEnd,
	float color[3]	///< - The color value in (r, g, b)
	);

/// Ends the drawing sequence and outputs the drawing.
/// \sa BeginDrawing()
void DoneDrawing();


/// Closes the drawing engine and releases any used resources.
/// \sa InitDrawing()
void TermDrawing();


/// Gets the height of a line of text drawn using the given style.
/// \return The height of a line of text, in pixels.
int GetLineHeight( 
	DrawStyle in_eDrawStyle		///< - The style of the text being queried
);
