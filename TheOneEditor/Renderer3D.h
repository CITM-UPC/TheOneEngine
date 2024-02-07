#ifndef __RENDERER_3D_H__
#define __RENDERER_3D_H__
#pragma once

#include "Module.h"

#include <memory>
#include "Shader.h"


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

    void DrawGameObject(std::shared_ptr<GameObject> object);

    void CameraInput(double dt);

    void CreateRay();

private:

    void FillDefaultShaders(Shader& shader) const;


public:

    //hekbas test
    double dtSum = 0;

    std::shared_ptr<GameObject> sceneCamera;

private:
    Shader default_shader_;
};

#endif // !__RENDERER_3D_H__