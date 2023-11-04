#ifndef __APP_H__
#define __APP_H__
#pragma once

#include "..\TheOneEngine\EngineCore.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <list>


// Forward declaration
class Module;
class Window;
class Input;
class Hardware;
class Gui;
class Renderer3D;

class App
{
public:

	App(int argc, char* args[]);
	virtual ~App();

	bool Awake();	// Called before render is available
	bool Start();	// Called before the first frame
	bool Update();	// Called each loop iteration
	bool CleanUp();	// Called before quitting

	// Add a new module to handle
	void AddModule(Module* module, bool activate);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;

	int GetFrameRate() const;
	void SetFrameRate(int refreshRate);

	double GetDT() const;

	std::vector<std::string> GetLogs();
	void LogConsole(const char* entry);
	void CleanLogs();


private:
	
	// Call order for each loop iteration
	void PrepareUpdate();
	bool PreUpdate();
	bool DoUpdate();
	bool PostUpdate();
	void FinishUpdate();


public:

	EngineCore* engine = nullptr;

	// Modules
	Window* window = nullptr;
	Input* input = nullptr;
	Hardware* hardware = nullptr;
	Gui* gui = nullptr;
	Renderer3D* renderer3D = nullptr;


private:

	int argc;
	char** args;
	std::string title;
	std::string organization;

	std::list<Module*> modules;

	//Logs
	std::string log;
	std::vector<std::string> logs;

	// Fps contro
	std::chrono::duration<double> targetFrameDuration;
	std::chrono::steady_clock::time_point frameStart, frameEnd;

	int frameRate = 240;
	double dt = 0;
	double dtCount = 0;
	int frameCount = 0;
	int fps = 0;
};

extern App* app;

#endif	// !__APP_H__