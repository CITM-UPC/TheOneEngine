#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__
#pragma once

#include <string>

typedef unsigned int uint;

class SceneManager 
{
public:
	SceneManager();
	~SceneManager();

	void LoadScene(uint index);
	void LoadScene(std::string sceneName);




private:

};

#endif // !__SCENE_MANAGER_H__