#ifndef __APP_H__
#define __APP_H__

#include "Module.h"
#include "Defs.h"

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

	uint GetFPS();
	float GetAverageFPS();
	float GetDT();
	float GetTimesSinceStart();
	uint GetFrameCount();


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

	bool saveGameRequested;
	bool loadGameRequested;

	int musicValue = 100;
	int sfxValue = 100;

private:

	int argc;
	char** args;
	string title;
	string organization;

	list<Module*> modules;

	uint frames;
	float dt;

	/*Timer timer;
	PerfTimer ptimer;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;*/

	uint64 frameCount = 0;
	uint32 framesPerSecond = 0;
	uint32 lastSecFrameCount = 0;

	float averageFps = 0.0f;
	float secondsSinceStartup = 0.0f;

	uint32 maxFrameDuration = 0;

};

extern App* app;

#endif	// __APP_H__