#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__
#pragma once

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

private:
    void RecurseDrawChildren(std::shared_ptr<GameObject> parentGO);

public:
    Scene* currentScene = nullptr;

private:
    std::shared_ptr<GameObject> selectedGameObject;
    MeshLoader* meshLoader;
};

#endif // !__SCENE_MANAGER_H__