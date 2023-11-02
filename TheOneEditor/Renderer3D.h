#ifndef __RENDERER_3D__
#define __RENDERER_3D__
#pragma once

#include "Module.h"

class EngineCore;

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

    void CameraMovement();

    bool CleanUp();

private:

    //hekbas test
    double dtSum = 0;
};

#endif // !__RENDERER_3D__