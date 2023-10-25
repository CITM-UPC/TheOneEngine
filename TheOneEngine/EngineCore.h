#ifndef __ENGINE_CORE_H__
#define __ENGINE_CORE_H__
#pragma once


//hekbas: Include here all headers needed in Editor
//Then Include on Editor: #include "../TheOneEngine/EngineCore.h"
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

	Camera camera;

	void step(double dt);
	void render(RenderModes renderMode);
};

#endif // !__ENGINE_CORE_H__