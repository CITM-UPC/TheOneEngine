#ifndef __BUILDER_WINDOW_H__
#define __BUILDER_WINDOW_H__
#pragma once

#include "BuilderModule.h"
#include "BuilderDefs.h"
#include "../TheOneEngine/Defs.h"

#include <SDL2/SDL.h>

enum class DisplayMode
{
    WINDOWED,
    FULLSCREEN,
    FULLSCREEN_DESKTOP,
    BORDERLESS,
};

enum class Resolution
{
    R_3840x2160,
    R_2560x1440,
    R_1920x1080,
    R_1280x720,
    R_854x480,
    R_640x360,
    R_426x240,
    R_NATIVE
};

class BuilderWindow : public BuilderModule
{
public:
    BuilderWindow(BuilderApp* app);
    virtual ~BuilderWindow();

    bool Awake();
    bool Start();
    bool CleanUp();

    bool initSDLWindowWithOpenGL();
    bool createSdlGlContext();
    bool initOpenGL();

    uint GetDisplayRefreshRate();

    DisplayMode GetDisplayMode();
    void SetDisplayMode(DisplayMode mode);

    void GetSDLWindowSize(int* w, int* h);

    Resolution GetResolution();
    void SetResolution(Resolution res);

    void OnResizeWindow(int width, int height);

public:
    SDL_Window* window;
    SDL_GLContext glContext;

private:
    uint refreshRate;

    DisplayMode displayMode;
    bool borderless;

    Resolution resolution;
    uint width;
    uint height;
};

#endif // !__BUILDER_WINDOW_H__