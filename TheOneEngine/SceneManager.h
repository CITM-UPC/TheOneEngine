#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__
#pragma once

#include <string>
#include <memory>

typedef unsigned int uint;

class GameObject;

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

	void LoadScene(uint index);
	void LoadScene(std::string sceneName);

    /*SCENE SERIALIZATION*/
    void SaveScene();
    void LoadScene(const std::string& filename);

private:
    void RecurseDrawChildren(std::shared_ptr<GameObject> parentGO);

private:

};

#endif // !__SCENE_MANAGER_H__