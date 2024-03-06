#ifndef __BUILDER_SCENE_MANAGER_H__
#define __BUILDER_SCENE_MANAGER_H__
#pragma once

#include "BuilderModule.h"
#include "..\TheOneEngine\N_SceneManager.h"

class Camera;

class BuilderSceneManager : public BuilderModule
{
public:

	BuilderSceneManager(BuilderApp* app);
	~BuilderSceneManager();

	bool Awake();
	bool Start();

	bool PreUpdate();
	bool Update(double dt);
	bool PostUpdate();

	bool CleanUp();

public:
	N_SceneManager* N_sceneManager = nullptr;
	Camera* cameraToRender = nullptr;

};

#endif // !__BUILDER_SCENE_MANAGER_H__