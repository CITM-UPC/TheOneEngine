#ifndef __ENGINE_CORE_H__
#define __ENGINE_CORE_H__
#pragma once


// hekbas: Include here all headers needed in Editor
// Include in Editor when needed: #include "../TheOneEngine/EngineCore.h"
#include "Defs.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "Ray.h"

#include "..\TheOneAudio\AudioCore.h"

#include <chrono>
#include <memory>


class EngineCore
{
public:

	EngineCore();

	void Awake();
	void Start();

	void Update(double dt);

	void Render(Camera* camera);

	void CleanUp();

	void DrawAxis();
	void DrawGrid(int grid_size, int grid_step);
	void DrawFrustum(const Frustum& frustum);
	void DrawRay(const Ray& ray);

	// (x, y) Indicate the bottom left corner1
	void OnWindowResize(int x, int y, int width, int height);

	bool GetVSync();
	bool SetVSync(bool vsync);

public:
	
	bool vsync = false;
	AudioCore* audio = nullptr;

private:

};

#endif // !__ENGINE_CORE_H__