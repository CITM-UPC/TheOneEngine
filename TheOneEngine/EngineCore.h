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
#include <memory>


class EngineCore
{
public:

	enum class RenderModes {RELEASE, DEBUG};

	EngineCore();

	void Awake();
	void Start();

	void Update(double dt);

	void Render(RenderModes renderMode, std::shared_ptr<Camera> camera);

	void OnWindowResize(int width, int height);

	bool GetVSync();
	bool SetVSync(bool vsync);

public:
	
	bool vsync = false;

private:

};

#endif // !__ENGINE_CORE_H__