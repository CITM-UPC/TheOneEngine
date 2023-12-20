#ifndef __RENDERER_3D_H__
#define __RENDERER_3D_H__
#pragma once

#include "Module.h"

#include <memory>


class EngineCore;
class GameObject;

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

    void CameraInput(double dt);

    void CreateRay();

public:

    //hekbas test
    double dtSum = 0;

    std::shared_ptr<GameObject> sceneCamera;
};

#endif // !__RENDERER_3D_H__