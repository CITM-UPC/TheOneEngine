#include "App.h"

#include "Defs.h"
#include "Log.h"


// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	frames = 0;

	input = new Input(this);
	window = new Window(this);

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	//AddModule(test, true);

	// Render last to swap buffer
 	AddModule(window, true);

}

// Destructor
App::~App()
{
	// Release modules
	modules.clear();
}

void App::AddModule(Module* module, bool activate)
{
	if(activate == true)
		module->Init();

	modules.push_back(module);
}

// Called before render is available
bool App::Awake()
{
	//timer = Timer();

	bool ret = false;

	//Load config from XML
	//ret = LoadConfig();
	ret = true;

	if (ret == true)
	{
		//title = configNode.child("app").child("title").child_value();
		
		//maxFrameDuration = configNode.child("app").child("frcap").attribute("value").as_int();
	
		for (const auto& item : modules)
		{
			if (item->active == false)
				continue;

			item->Awake();
		}
	}

	//LOG("---------------- Time Awake: %f/n", timer.ReadMSec());

	return ret;
}

// Called before the first frame
bool App::Start()
{
	/*timer.Start();
	startupTime.Start();
	lastSecFrameTime.Start();*/

	bool ret = true;

	for (const auto& item : modules)
	{
		if (item->active == false)
			continue;

		item->Start();
	}

	//LOG("----------------- Time Start(): %f", timer.ReadMSec());

	return ret;
}

// Called each loop iteration
bool App::Update()
{
	bool ret = true;
	PrepareUpdate();

	/*if (input->GetWindowEvent(WE_QUIT) == true)
		ret = false;*/

	if (ret == true)
		ret = PreUpdate();

	if (ret == true)
		ret = DoUpdate();

	if (ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}


// ---------------------------------------------
void App::PrepareUpdate()
{
	//frameTime.Start();
}

// ---------------------------------------------
void App::FinishUpdate()
{
	// Shows the time measurements in the window title
	static char title[256];
	//app->win->SetTitle(title);
}

// Call modules before each loop iteration
bool App::PreUpdate()
{
	//OPTICK_CATEGORY("PreUpdate", Optick::Category::GameLogic);
	bool ret = true;

	for (const auto& item : modules)
	{
		if (item->active == false)
			continue;

		item->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool App::DoUpdate()
{
	//OPTICK_CATEGORY("DoUpdate", Optick::Category::GameLogic);
	bool ret = true;

	for (const auto& item : modules)
	{
		if (item->active == false)
			continue;

		item->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool App::PostUpdate()
{
	//OPTICK_CATEGORY("PostUpdate", Optick::Category::GameLogic);
	bool ret = true;

	for (const auto& item : modules)
	{
		if (item->active == false)
			continue;

		item->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool App::CleanUp()
{
	bool ret = true;

	for (auto item = modules.rbegin(); item != modules.rend(); ++item)
	{
		Module* module = *item;
		module->CleanUp();
	}

	return ret;
}

// ---------------------------------------
int App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
uint App::GetFPS() 
{
	return framesPerSecond;
}

// ---------------------------------------
float App::GetAverageFPS()
{
	return averageFps;
}

// ---------------------------------------
float App::GetDT()
{
	return dt;
}

// ---------------------------------------
float App::GetTimesSinceStart()
{
	return secondsSinceStartup;
}

// ---------------------------------------
uint App::GetFrameCount()
{
	return frameCount;
}