#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__
#pragma once

#include "Module.h"
#include "..\TheOneEngine\Defs.h"
#include "..\TheOneEngine\GameObject.h"
#include "..\TheOneEngine\Component.h"
#include "..\TheOneEngine\MeshLoader.h"

#include <vector>
#include <string>
#include <memory>


class SceneManager : public Module
{
public:
    SceneManager(App* app);

    virtual ~SceneManager();

    bool Awake();
    bool Start();

    bool PreUpdate();
    bool Update(double dt);
    bool PostUpdate();

    bool CleanUp();

    // Basic 3D 
    std::shared_ptr<GameObject> CreateEmptyGO();
    std::shared_ptr<GameObject> CreateMeshGO(std::string path);
    std::shared_ptr<GameObject> CreateCube();
    std::shared_ptr<GameObject> CreateSphere();

    // Get/Set
    uint GetNumberGO();
    std::vector<std::shared_ptr<GameObject>>GetGameObjects();
    uint GetSelectedGO();


private:
    uint selectedGameObject;
    std::vector<std::shared_ptr<GameObject>> gameObjects;

    MeshLoader* meshLoader;
};

#endif // !__SCENE_MANAGER_H__