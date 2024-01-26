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

// main.cpp
/// \file 
/// Contains the entry point for the application.

/////////////////////////
//  INCLUDES
/////////////////////////

#include "stdafx.h"

#include <malloc.h>
#include "IntegrationDemo.h"
#include "Helpers.h"
#include "Platform.h"

#if defined(AK_RENDER_SDL)
#include "SDLWindow.h"
#include "SDL.h"
#include "SDL_syswm.h"
#endif

#if defined(AK_INPUT_SDL)
#include "SDLInputMgr.h"
SDLInputMgr g_sdlInputMgr;
IAkInputMgr* g_pInputMgr = &g_sdlInputMgr;
#else
#include "InputMgr.h"
WindowsInputMgr g_InputMgr;
WindowsInputMgr* g_pInputMgr = &g_InputMgr;
#endif

/////////////////////////
//  PROTOTYPES
/////////////////////////

LRESULT CALLBACK WindowProc( HWND, UINT, WPARAM, LPARAM );

/////////////////////////
//  MAIN
/////////////////////////

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow )
{
	MSG  msg;
	void* hRenderHandle;
	HWND hWnd;
	AkUInt32 width = 853;
	AkUInt32 height = 480;

#if defined(AK_RENDER_SDL)
	SDL_Window * sdlWindow = (SDL_Window*)CreateSDLWindow(width, height);
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(sdlWindow, &wmInfo);
	hWnd = wmInfo.info.win.window;
	hRenderHandle = (void*)sdlWindow;


#else
	WNDCLASSEX wc;

	// Setup the application's window
	RECT windowsRect = { 0, 0, (long)width, (long)height };
	AdjustWindowRect(&windowsRect, WS_OVERLAPPEDWINDOW, false);

	ZeroMemory( &wc, sizeof( WNDCLASSEX ) );
	wc.cbSize = sizeof( WNDCLASSEX );
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WindowProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"DemoWindowClass";
	RegisterClassEx( &wc );
	hWnd = CreateWindowEx( NULL,
                           L"DemoWindowClass",    
                           L"Wwise Integration Demo",
                           ( WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ),
                           100,    // x-position of window
                           100,    // y-position of window
                           windowsRect.right - windowsRect.left,
                           windowsRect.bottom - windowsRect.top,
                           NULL,
                           NULL,
                           hInstance,
                           NULL );
	hRenderHandle = (void*)hWnd;
	g_pInputMgr->SetHwnd(hWnd, false);
#endif
	
	//Change default values of AkPlatformInitSettings before the call to Init.
	//All other settings can also be changed in similar way
	AkPlatformInitSettings& platformInitSettings = IntegrationDemo::Instance().GetPlatformInitSettings();
	platformInitSettings.hWnd = hWnd;

	// To showcase the Microsoft Spatial Sound dynamic objects in Positioning -> 3D Audio Demo
	// The number is not high, because we don't need a whole lot. Best to keep the number low
	// to accomodate other processes who might need to reserve objects.
	platformInitSettings.uMaxSystemAudioObjects = 32;

	// Set the DLL path for plugin
	AkInitSettings& initSettings = IntegrationDemo::Instance().GetInitSettings();
	initSettings.szPluginDLLPath = GetPluginDllPath();
	
	// Initialize the various components of the application and show the window
	AkOSChar szError[500];
	if ( !IntegrationDemo::Instance().Init( hRenderHandle, g_pInputMgr, szError, (unsigned int) IntegrationDemoHelpers::AK_ARRAYSIZE(szError), width, height ) )
	{
		AkOSChar szMsg[550];
		__AK_OSCHAR_SNPRINTF( szMsg, IntegrationDemoHelpers::AK_ARRAYSIZE(szMsg), AKTEXT("Failed to initialize the Integration Demo.\r\n%s"), szError );
		MessageBox( NULL, szMsg, L"Error", MB_ICONERROR | MB_OK );
		return 1;
	}
	ShowWindow( hWnd, nCmdShow );


	// The application's main loop
	// Each iteration represents 1 frame
	for(;;) 
	{
		// Records the starting time of the current frame
		IntegrationDemo::Instance().StartFrame();

		// Empty the Windows message queue
		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			if ( msg.message == WM_QUIT )
			{
				break;
			}
		}
		if ( msg.message == WM_QUIT )
		{
			break;
		}

		if (IntegrationDemo::Instance().GetInputMgr()->ShouldQuit())
		{
			break;
		}

		// Process the current frame, quit if Update() is false.
		if ( ! IntegrationDemo::Instance().Update() )
		{
			break;
		}
		IntegrationDemo::Instance().Render();
		
		// Ends the frame and regulates the application's framerate
		IntegrationDemo::Instance().EndFrame();
	}

	// Terminate the various components of the application
	IntegrationDemo::Instance().Term();

	return 0;
}



LRESULT CALLBACK WindowProc( HWND in_hWnd, UINT in_unMsg, WPARAM in_wParam, LPARAM in_lParam )
{
	switch ( in_unMsg )
	{
	case WM_SIZE:
		if(in_wParam == SIZE_MINIMIZED)
			IntegrationDemo::Instance().Pause();
		if(in_wParam == SIZE_RESTORED || in_wParam == SIZE_MAXIMIZED)
			IntegrationDemo::Instance().Resume();
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return TRUE;
	}
	return ( DefWindowProc( in_hWnd, in_unMsg, in_wParam, in_lParam ) );
}