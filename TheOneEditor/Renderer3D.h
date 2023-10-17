#ifndef __RENDERER_3D__
#define __RENDERER_3D__
#pragma once

#include "Module.h"
#include "App.h"

#include "../TheOneEngine/TheOneEngine.h"

class Renderer3D : public Module
{
public:
    Renderer3D(App* app);

    virtual ~Renderer3D();

    bool Awake();
    bool Start();

    bool PreUpdate();
    bool Update(double dt);
    bool PostUpdate();

    bool CleanUp();

private:
    TheOneEngine engine;

};

#endif __RENDERER_3D__