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
// Implements the Drawing.h functions for the new Integration Demo renderer

/////////////////////////
//  INCLUDES
/////////////////////////

#include "stdafx.h"

#include "Drawing.h"
#include "Render.h"
#include "FreetypeGraphicRenderer.h"

#if !defined (OLD_INTDEMO_RENDER)

/////////////////////////
//  GLOBAL OBJECTS
/////////////////////////

static FreetypeGraphicRenderer* g_pFreetypeRenderer = NULL;
static IDynamicTexture2D* g_pFreetypeTexture = NULL;

/////////////////////////
//  FUNCTIONS
/////////////////////////

static AkUInt32 g_iLayoutWidth;
static AkUInt32 g_iLayoutHeight;

AkUInt32 GetLayoutWidth()
{
	return g_iLayoutWidth;
}

AkUInt32 GetLayoutHeight()
{
	return g_iLayoutHeight;
}


bool InitDrawing(
	AkOSChar* in_szErrorBuffer,
	unsigned int in_unErrorBufferCharCount,
	AkUInt32 in_canvasWidth,
	AkUInt32 in_canvasHeight,
	AkUInt32 in_layoutWidth,
	AkUInt32 in_layoutHeight
)
{
	g_pFreetypeRenderer = new FreetypeGraphicRenderer;

	bool bRet = g_pFreetypeRenderer->InitDrawing(in_szErrorBuffer,
		in_unErrorBufferCharCount,
		in_canvasWidth,
		in_canvasHeight,
		in_layoutWidth,
		in_layoutHeight);

	if (!bRet)
	{
		return bRet;
	}

	g_pFreetypeTexture = RenderCreateDynamicTexture(in_canvasWidth, in_canvasHeight);
	g_pFreetypeTexture->AddRef();

	g_iLayoutWidth = in_layoutWidth;
	g_iLayoutHeight = in_layoutHeight;

	return true;
}

void TermDrawing()
{
	if (g_pFreetypeRenderer != NULL)
	{
		g_pFreetypeRenderer->TermDrawing();
		delete g_pFreetypeRenderer;
		g_pFreetypeRenderer = NULL;
	}

	if (g_pFreetypeTexture)
	{
		g_pFreetypeTexture->Release();
		delete g_pFreetypeTexture;
		g_pFreetypeTexture = NULL;
	}
}

void BeginDrawing()
{
	if (g_pFreetypeRenderer)
		g_pFreetypeRenderer->BeginDrawing();
}

#define RENDER_TEST 0

#if RENDER_TEST

#include <cmath>

//////////////////////////////////////////////////////////////////////////
// Helper functions for testing parts of the renderer

void DrawTextureTest(IDynamicTexture2D& texture, DrawRect src, DrawRect dest)
{
	static int frameCount = 300;

	++frameCount;

	float centerX = std::cos(frameCount / 300.0f) / 4.0f + 0.5f;
	float centerY = std::sin(frameCount / 300.0f) / 2.25f + 0.5f;

	float boxLeft = centerX - 0.042f;
	float boxTop = centerY - 0.075f;
	float boxRight = centerX + 0.042f;
	float boxBottom = centerY + 0.075f;

	// Top left
	RenderDrawTexture2D(texture,
		DrawRect( 0.0f, 0.0f, boxLeft, boxTop ),
		DrawRect( 0.0f, 0.0f, boxLeft, boxTop ));

	// Top mid
	RenderDrawTexture2D(texture,
		DrawRect( boxLeft, 0.0f, boxRight, boxTop ),
		DrawRect( boxLeft, 0.0f, boxRight, boxTop ));

	// Top right
	RenderDrawTexture2D(texture,
		DrawRect( boxRight, 0.0f, 1.0f, boxTop ),
		DrawRect( boxRight, 0.0f, 1.0f, boxTop ));

	// Mid left
	RenderDrawTexture2D(texture,
		DrawRect( 0.0f, boxTop, boxLeft, boxBottom ),
		DrawRect( 0.0f, boxTop, boxLeft, boxBottom ));

	// Mid mid
	RenderDrawTexture2D(texture,
		DrawRect( boxLeft, boxBottom, boxRight, boxTop ),
		DrawRect( boxLeft, boxTop, boxRight, boxBottom ));
	float color[3] = { 1.0f, 0.0f, 0.0f };
	RenderDrawLine2D(boxLeft, boxTop, boxRight, boxTop, color);
	RenderDrawLine2D(boxRight, boxTop, boxRight, boxBottom, color);
	RenderDrawLine2D(boxRight, boxBottom, boxLeft, boxBottom, color);
	RenderDrawLine2D(boxLeft, boxBottom, boxLeft, boxTop, color);

	// Mid right
	RenderDrawTexture2D(texture,
		DrawRect( boxRight, boxTop, 1.0f, boxBottom ),
		DrawRect( boxRight, boxTop, 1.0f, boxBottom ));

	// Bot left
	RenderDrawTexture2D(texture,
		DrawRect( 0.0f, boxBottom, boxLeft, 1.0f ),
		DrawRect( 0.0f, boxBottom, boxLeft, 1.0f ));

	// Bot mid
	RenderDrawTexture2D(texture,
		DrawRect( boxLeft, boxBottom, boxRight, 1.0f ),
		DrawRect( boxLeft, boxBottom, boxRight, 1.0f ));

	// Bot right
	RenderDrawTexture2D(texture,
		DrawRect( boxRight, boxBottom, 1.0f, 1.0f ),
		DrawRect( boxRight, boxBottom, 1.0f, 1.0f ));
}

void RenderDrawLineTest()
{
	static int frameCount = 300;

	++frameCount;

	struct Matrix4x4
	{
		float v[4][4];
		Matrix4x4()
		{
			memset(v, 0, 4 * 4 * sizeof(float));
			v[0][0] = 1.0f;
			v[1][1] = 1.0f;
			v[2][2] = 1.0f;
			v[3][3] = 1.0f;
		}
	};

	struct Vector4
	{
		float v[4];

		static Vector4 mul(Matrix4x4 m, Vector4 v)
		{
			Vector4 ret =
			{
				m.v[0][0] * v.v[0] + m.v[0][1] * v.v[1] + m.v[0][2] * v.v[2] + m.v[0][3] * v.v[3],
				m.v[1][0] * v.v[0] + m.v[1][1] * v.v[1] + m.v[1][2] * v.v[2] + m.v[1][3] * v.v[3],
				m.v[2][0] * v.v[0] + m.v[2][1] * v.v[1] + m.v[2][2] * v.v[2] + m.v[2][3] * v.v[3],
				m.v[3][0] * v.v[0] + m.v[3][1] * v.v[1] + m.v[3][2] * v.v[2] + m.v[3][3] * v.v[3]
			};
			return ret;
		}

		static Vector4 project(Vector4 v)
		{
			Vector4 ret =
			{
				v.v[0] / v.v[3],
				v.v[1] / v.v[3],
				v.v[2] / v.v[3],
				v.v[3] / v.v[3]
			};
			return ret;
		}
	};

	Vector4 points[8] = {
		{0.0f, 0.0f, 0.0f, 1.0f}, // 0
		{0.0f, 0.0f, 1.0f, 1.0f}, // 1
		{0.0f, 1.0f, 0.0f, 1.0f}, // 2
		{0.0f, 1.0f, 1.0f, 1.0f}, // 3 
		{1.0f, 0.0f, 0.0f, 1.0f}, // 4
		{1.0f, 0.0f, 1.0f, 1.0f}, // 5
		{1.0f, 1.0f, 0.0f, 1.0f}, // 6
		{1.0f, 1.0f, 1.0f, 1.0f}  // 7
	};

	Matrix4x4 translate, view, proj;

	// set up translate
	{
		translate.v[0][3] = -0.5f;
		translate.v[1][3] = -0.5f;
		translate.v[2][3] = -0.5f;
	}

	// set up view
	{
		float xTheta = frameCount / 360.f;
		float yTheta = frameCount / -512.f;
		float zTheta = frameCount / 1200.f;

		float cosX = std::cos(xTheta);
		float sinX = std::sin(xTheta);
		float cosY = std::cos(yTheta);
		float sinY = std::sin(yTheta);
		float cosZ = std::cos(zTheta);
		float sinZ = std::sin(zTheta);

		view.v[0][0] = cosY * cosZ;
		view.v[0][1] = sinX * sinY * cosZ - cosX * sinZ;
		view.v[0][2] = sinX * sinZ + cosX * sinY * cosZ;
		view.v[1][0] = cosY * sinZ;
		view.v[1][1] = cosX * cosZ + sinX * sinY * sinZ;
		view.v[1][2] = cosX * sinY * sinZ - sinX * cosZ;
		view.v[2][0] = 0 - sinY;
		view.v[2][1] = sinX * cosY;
		view.v[2][2] = cosX * cosY;

	}

	// set up proj
	{
		proj.v[0][0] = 6 / (16.f);
		proj.v[1][1] = 6 / (9.f);
	}

	// transform points as needed
	for (int i = 0; i < 8; ++i)
	{
		points[i] = Vector4::mul(translate, points[i]);
		points[i] = Vector4::mul(view, points[i]);
		points[i] = Vector4::mul(proj, points[i]);
		points[i] = Vector4::project(points[i]);
	}

	// convert them to linepts
	int linePts[8][2];
	for (int i = 0; i < 8; ++i)
	{
		linePts[i][0] = (int)(points[i].v[0] * 160.f + 80.f);
		linePts[i][1] = (int)(points[i].v[1] * 90.f + 90.f);
	}

	// draw all of the lines using indices provided
	int idx[12][2] = {
		{0,1}, {0,2}, {0,4},
		{1,3}, {1,5},
		{2,3}, {2,6},
		{3,7},
		{4,5}, {4,6},
		{5,7},
		{6,7}
	};

	for (int i = 0; i < 12; ++i)
	{
		DrawLineOnScreen(
			linePts[idx[i][0]][0], linePts[idx[i][0]][1],
			linePts[idx[i][1]][0], linePts[idx[i][1]][1],
			i % 2 == 0 ? DrawStyle_Selected : DrawStyle_Control);
	}
}
#endif

void DoneDrawing()
{
	if (g_pFreetypeRenderer)
	{
		g_pFreetypeRenderer->DoneDrawing();
		if (g_pFreetypeTexture)
		{
			g_pFreetypeTexture->UpdateResource(g_pFreetypeRenderer->GetCanvasBuffer(),
				g_pFreetypeRenderer->GetCanvasBufferPitch(),
				g_pFreetypeRenderer->GetCanvasBufferLength());

			DrawRect src ( 0.0f, 0.0f, 1.0f, 1.0f );
			DrawRect dest( 0.0f, 0.0f, 1.0f, 1.0f );
#if RENDER_TEST
			RenderDrawLineTest();
			DrawTextureTest(*g_pFreetypeTexture, src, dest);
#else
			RenderDrawTexture2D(*g_pFreetypeTexture, src, dest);
#endif
		}
	}
}


void DrawTextOnScreen(const char* in_szText, int in_iXPos, int in_iYPos, DrawStyle in_eDrawStyle)
{
	if (g_pFreetypeRenderer)
	{
		g_pFreetypeRenderer->DrawTextOnScreen(in_szText, in_iXPos, in_iYPos, in_eDrawStyle);
	}
}

void DrawLineOnScreen(int in_iXPosStart, int in_iYPosStart, int in_iXPosEnd, int in_iYPosEnd, DrawStyle in_eDrawStyle)
{
	float color[3];
	switch (in_eDrawStyle)
	{
	case DrawStyle_SelectedText:
	case DrawStyle_Selected:
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 0.0f;
		break;
	case DrawStyle_Error:
		color[0] = 1.0f;
		color[1] = 0.0f;
		color[2] = 0.0f;
		break;
	case DrawStyle_Text:
	case DrawStyle_Control:
	case DrawStyle_Title:
	default:
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;
		break;
	}

	RenderDrawLine2D(
		in_iXPosStart / (float)g_iLayoutWidth,
		in_iYPosStart / (float)g_iLayoutHeight,
		in_iXPosEnd / (float)g_iLayoutWidth,
		in_iYPosEnd / (float)g_iLayoutHeight,
		color);
}

void DrawLineOnScreen(int in_iXPosStart, int in_iYPosStart, int in_iXPosEnd, int in_iYPosEnd, float color[3])
{
	RenderDrawLine2D(
		in_iXPosStart / (float)g_iLayoutWidth,
		in_iYPosStart / (float)g_iLayoutHeight,
		in_iXPosEnd / (float)g_iLayoutWidth,
		in_iYPosEnd / (float)g_iLayoutHeight,
		color);
}

int GetLineHeight(DrawStyle in_eDrawStyle)
{
	if (g_pFreetypeRenderer == NULL)
		return 0;

	return g_pFreetypeRenderer->GetLayoutLineHeight(in_eDrawStyle);
}

#endif