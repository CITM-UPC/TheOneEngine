#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__
#pragma once

#include "Module.h"
#include "..\TheOneEngine\Defs.h"
#include "..\TheOneEngine\GameObject.h"
#include "..\TheOneEngine\Component.h"
#include "..\TheOneEngine\MeshLoader.h"
#include "..\TheOneEngine\Transform.h"
#include "..\TheOneEngine\Camera.h"
#include "..\TheOneEngine\Mesh.h"
#include "..\TheOneEngine\Texture.h"
#include "..\TheOneEngine\Listener.h"
#include "..\TheOneEngine\Source.h"
#include "..\TheOneEngine\N_SceneManager.h"

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#define NOT_SPECIFIED ""

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

    std::string GenerateUniqueName(const std::string& baseName);

    // Basic 3D
    std::shared_ptr<GameObject> CreateEmptyGO(std::string name = "Empty GameObject");
    std::shared_ptr<GameObject> CreateCameraGO(std::string name);
    std::shared_ptr<GameObject> CreateMeshGO(std::string path);
    std::shared_ptr<GameObject> CreateExistingMeshGO(std::string fbxName);
    std::shared_ptr<GameObject> CreateCube();
    std::shared_ptr<GameObject> CreateSphere();
    std::shared_ptr<GameObject> CreateMF();
    std::shared_ptr<GameObject> CreateTeapot(std::string path);

    //static
    std::shared_ptr<GameObject> spatialObject1 = nullptr;
    //movable
    std::shared_ptr<GameObject> spatialObject2 = nullptr;

    // Get/Set
    uint GetNumberGO() const;
    std::vector<std::shared_ptr<GameObject>>GetGameObjects();
    
    std::shared_ptr<GameObject> GetSelectedGO() const;
    void SetSelectedGO(std::shared_ptr<GameObject> gameObj);

    std::shared_ptr<GameObject> GetRootSceneGO() const;

    std::shared_ptr<GameObject> FindGOByUID(uint32_t _UID) const;

    static SceneManager& GetInstance() {
        static SceneManager instance; // Static instance of the class
        return instance;
    }

    /*SCENE SERIALIZATION*/
    void SaveScene();
    void LoadScene(const std::string& filename);

    //MeshLoader GetMeshLoader();

private:
	// Private constructor to enforce singleton pattern
	SceneManager() {}

public:
	N_SceneManager* N_sceneManager = nullptr;
};

#endif // !__SCENE_MANAGER_H__