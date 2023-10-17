#ifndef __APP_H__
#define __APP_H__
#pragma once

#include "Module.h"
#include "Defs.h"

#include "Window.h"
#include "Input.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <list>


// Modules
class Window;
class Input;
class Render;

using namespace std;
using namespace chrono;

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module, bool activate);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;

	float GetDT();


private:

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

public:

	// Modules
	Window* window;
	Input* input;
	Render* render;

	int musicValue = 100;
	int sfxValue = 100;

private:

	int argc;
	char** args;
	string title;
	string organization;

	list<Module*> modules;

	// Fps control
	double targetFPS = 60;
	std::chrono::duration<double> targetFrameDuration;
	std::chrono::steady_clock::time_point frameStart, frameEnd;
	float dt;

};

#endif	// __APP_H__