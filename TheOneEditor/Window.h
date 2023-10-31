#ifndef __WINDOW_H__
#define __WINDOW_H__
#pragma once

#include "Module.h"

#include <SDL2/SDL.h>


#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


class Window : public Module
{
public:
    Window(App* app);
    virtual ~Window();

    bool Awake();
    bool Start();
    bool CleanUp();

    static SDL_Window* initSDLWindowWithOpenGL();
    static SDL_GLContext createSdlGlContext(SDL_Window* window);
    static void initOpenGL();

public:
    SDL_Window* window;
    SDL_GLContext glContext;
};

#endif // !__WINDOW_H__