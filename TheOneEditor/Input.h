#ifndef INPUT_H
#define INPUT_H
#pragma once

#include "Module.h"
#include "App.h"
#include <SDL2/SDL.h>


class Input : public Module
{
public:
    Input(App* app);

    virtual ~Input();

    bool PreUpdate();

    bool processSDLEvents();
};

#endif // INPUT_H