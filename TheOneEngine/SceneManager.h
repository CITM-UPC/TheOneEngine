#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Component.h"
#include "MeshLoader.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"

#include <string>
#include <memory>

typedef unsigned int uint;

class Scene;
class GameObject;
class MeshLoader;

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	bool Awake();
	bool Start();

	bool PreUpdate();
	bool Update(double dt);
	bool PostUpdate();

	bool CleanUp();

	void CreateNewScene();

	void LoadScene(uint index);
	void LoadScene(std::string sceneName);

	/*SCENE SERIALIZATION*/
	void SaveScene();
	void LoadSceneFromJSON(const std::string& filename);

	// Create GameObjects functions
	std::shared_ptr<GameObject> CreateEmptyGO(std::string name = "Empty GameObject");
	std::shared_ptr<GameObject> CreateCameraGO(std::string name);
	std::shared_ptr<GameObject> CreateMeshGO(std::string path);
	std::shared_ptr<GameObject> CreateExistingMeshGO(std::string fbxName);
	std::shared_ptr<GameObject> CreateCube();
	std::shared_ptr<GameObject> CreateSphere();
	std::shared_ptr<GameObject> CreateMF();
	std::shared_ptr<GameObject> CreateTeapot(std::string path);

private:
	void RecurseDrawChildren(std::shared_ptr<GameObject> parentGO);

public:
	Scene* currentScene = nullptr;

private:
	std::shared_ptr<GameObject> selectedGameObject;
	MeshLoader* meshLoader;
};

class Scene
{
public:
	Scene() {}
	~Scene() {}

private:
	uint index;
	std::string sceneName;
	std::shared_ptr<GameObject> rootSceneGO;
};

#endif // !__SCENE_MANAGER_H__