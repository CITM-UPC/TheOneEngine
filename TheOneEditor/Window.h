#ifndef WINDOW_H
#define WINDOW_H
#pragma once

#include "Module.h"
#include "App.h"

#include <SDL2/SDL.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720


using namespace std;
using namespace chrono;

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

    SDL_Window* GetWindow() const;
    SDL_GLContext GetGLContext() const;

private:
    SDL_Window* window;
    SDL_GLContext glContext;
};

#endif // WINDOW_H