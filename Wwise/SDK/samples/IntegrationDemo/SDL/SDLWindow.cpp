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

#if defined(AK_USE_SYSTEM_SDL)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include <stdio.h>

void * CreateSDLWindow(int width, int height)
{
	// Initialize the various components of the application and show the window
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("Integration Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	if (win == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return nullptr;
	}
    return (void*)win;
}
