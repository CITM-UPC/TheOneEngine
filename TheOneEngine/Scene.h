#ifndef __SCENE_H__
#define __SCENE_H__
#pragma once

#include <string>

typedef unsigned int uint;

class Scene
{
public:
	Scene();
	~Scene();


private:
	uint index;
	std::string sceneName;

};


#endif // !__SCENE_H__