#pragma once

#include "Cube.h"
#include "Texture.h"

struct CubeImmediateMode : public Cube
{
	Texture texture;
	void draw();

	CubeImmediateMode();
};

