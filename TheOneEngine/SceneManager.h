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

public:

	void CreateNewScene(uint _index, std::string name);

	// Change between scenes
	void LoadScene(uint index);
	void LoadScene(std::string sceneName);

	std::string GenerateUniqueName(const std::string& baseName);

	// Create GameObjects functions
	std::shared_ptr<GameObject> CreateEmptyGO(std::string name = "Empty GameObject");
	std::shared_ptr<GameObject> CreateCameraGO(std::string name);

	// Try to mix this two (CreateExistingMeshGO should be erased and CreateMeshGO has to do)
	std::shared_ptr<GameObject> CreateMeshGO(std::string path);
	std::shared_ptr<GameObject> CreateExistingMeshGO(std::string fbxName);

	std::shared_ptr<GameObject> CreateCube();
	std::shared_ptr<GameObject> CreateSphere();
	std::shared_ptr<GameObject> CreateMF();
	std::shared_ptr<GameObject> CreateTeapot(std::string path);

	// Get/Set
	uint GetNumberGO() const;
	std::vector<std::shared_ptr<GameObject>>GetGameObjects();

	// SelectedGo (Editor Only???)
	std::shared_ptr<GameObject> GetSelectedGO() const;
	void SetSelectedGO(std::shared_ptr<GameObject> gameObj);

	/*SCENE SERIALIZATION*/
	void SaveScene();
	void LoadSceneFromJSON(const std::string& filename);

private:
	void RecurseDrawChildren(std::shared_ptr<GameObject> parentGO);

public:
	Scene* currentScene = nullptr; //Convert to smart ptr

private:
	std::shared_ptr<GameObject> selectedGameObject;
	MeshLoader* meshLoader;
};

class Scene
{
public:
	Scene() : sceneName("Scene"), index(0), isDirty(true) {}

	inline Scene(uint _index, std::string name) : sceneName(name), index(_index), isDirty(true) {}

	~Scene() {}

	inline std::string GetSceneName() const { return sceneName; }
	inline void SetSceneName(std::string name) { sceneName = name; }

	inline uint GetIndex() const { return index; }
	inline void SetIndex(uint _index) { index = _index; }
	
	inline bool IsDirty() const { return isDirty; }
	inline void SetIsDirty(bool state) { isDirty = state; }

	inline std::shared_ptr<GameObject> GetRootSceneGO() const { return rootSceneGO; }

private:
	uint index;
	std::string sceneName;
	std::shared_ptr<GameObject> rootSceneGO;

	//Historn: This is to remember to save the scene if any change is made
	bool isDirty;

	std::string path;
};

#endif // !__SCENE_MANAGER_H__