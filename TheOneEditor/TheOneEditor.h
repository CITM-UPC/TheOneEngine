#pragma once

#ifndef __TEST_H__
#define __TEST_H__

#include "Module.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>

#include "GL/glew.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "../TheOneEngine/TheOneEngine.h"


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define FPS 60


class TheOneEditor : public Module
{
public:

	TheOneEditor();

	// Destructor
	virtual ~TheOneEditor();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	int infiniteLoopOfDoom();


public:
	std::chrono::duration<double> FDT;

};

#endif // __TEST_H__