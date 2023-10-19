#ifndef __INPUT_H__
#define __INPUT_H__
#pragma once

#include "Module.h"
#include "App.h"
#include "Defs.h"


class Input : public Module
{
public:
    Input(App* app);

    virtual ~Input();

    bool Awake();

    bool PreUpdate();

    bool processSDLEvents();

public:

    //SDL_Event event;
};

#endif // __INPUT_H__