#ifndef __ENGINE_CORE_H__
#define __ENGINE_CORE_H__
#pragma once


// hekbas: Include here all headers needed in Editor
// Include in Editor when needed: #include "../TheOneEngine/EngineCore.h"
#include "Defs.h"
#include "Camera.h"
#include "Asset.h"
#include "AssetMesh.h"
#include "Texture.h"

#include <chrono>


class EngineCore
{
public:

	enum class RenderModes {RELEASE, DEBUG};

	EngineCore();

	void Awake();
	void Start();

	void Update(double dt);

	void Render(RenderModes renderMode);

	void OnWindowResize(int width, int height);

public:
	Camera camera;

};

#endif // !__ENGINE_CORE_H__