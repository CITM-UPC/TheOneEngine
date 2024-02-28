#ifndef __SCENE_H__
#define __SCENE_H__
#pragma once

#include <string>

typedef unsigned int uint;

class GameObject;

class Scene
{
public:
	Scene();
	~Scene();


private:
	uint index;
	std::string sceneName;
	std::shared_ptr<GameObject> rootSceneGO;
};


#endif // !__SCENE_H__