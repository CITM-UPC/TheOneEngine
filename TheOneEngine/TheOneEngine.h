#ifndef __THE_ONE_ENGINE_H__
#define __THE_ONE_ENGINE_H__
#pragma once

#include <chrono>

#include "types.h"
#include "Camera.h"


class TheOneEngine
{
public:

	enum class RenderModes {RELEASE, DEBUG};

	Camera camera;

	void step(double dt);
	void render(RenderModes renderMode);
};

#endif