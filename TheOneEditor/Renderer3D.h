#ifndef __RENDERER_3D_H__
#define __RENDERER_3D_H__
#pragma once

#include "Module.h"

#include <memory>

class EngineCore;
class GameObject;
class SceneManager;

class Renderer3D : public Module
{
public:
	Renderer3D(App* app);

	virtual ~Renderer3D();

	bool Awake();
	bool Start();

	bool PreUpdate();
	bool Update(double dt);
	bool PostUpdate();

	bool CleanUp();

	void CameraInput(GameObject* cam);

	void CreateRay();

public:

	//hekbas test
	double dtSum = 0;
	double dtTemp = 0;
};

#endif // !__RENDERER_3D_H__