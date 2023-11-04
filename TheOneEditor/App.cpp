#include "App.h"

#include "Window.h"
#include "Input.h"
#include "Hardware.h"
#include "Gui.h"
#include "Renderer3D.h"

#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "PanelProject.h"
#include "PanelScene.h"
#include "PanelSettings.h"


App::App(int argc, char* args[]) : argc(argc), args(args)
{
	engine = new EngineCore();

	window = new Window(this);
	input = new Input(this);
	hardware = new Hardware(this);
	gui = new Gui(this);
	renderer3D = new Renderer3D(this);

	// Ordered for awake / Start / Update
	// Reverse order for CleanUp
	
	AddModule(window, true);
	AddModule(input, true);
	AddModule(hardware, true);
	AddModule(gui, true);

	// Render last to swap buffer
	AddModule(renderer3D, true);

}

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
	bool ret = false;

	targetFrameDuration = (std::chrono::duration<double>)1 / frameRate;

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
	dt = 0;

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->Start() == false)
			return false;
	}

	//LOG("----------------- Time Start(): %f", timer.ReadMSec());

	return true;
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


// -------------------- LOOP ITERATION --------------------
void App::PrepareUpdate()
{	
	frameStart = std::chrono::steady_clock::now();
}

bool App::PreUpdate()
{
	//OPTICK_CATEGORY("PreUpdate", Optick::Category::GameLogic);
	bool ret = true;

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->PreUpdate() == false)
			return false;
	}

	return true;
}

bool App::DoUpdate()
{
	//OPTICK_CATEGORY("DoUpdate", Optick::Category::GameLogic);

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->Update(dt) == false)
			return false;		
	}

	return true;
}

bool App::PostUpdate()
{
	//OPTICK_CATEGORY("PostUpdate", Optick::Category::GameLogic);

	for (const auto& module : modules)
	{
		if (module->active == false)
			continue;

		if (module->PostUpdate() == false)
			return false;
	}

	return true;
}

void App::FinishUpdate()
{
	// dt calculation
	frameEnd = std::chrono::steady_clock::now();
	auto frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(frameEnd - frameStart);

	dt = frameDuration.count();

	if (frameDuration < targetFrameDuration)
	{
		std::chrono::duration<double> sleepTime = targetFrameDuration - frameDuration;
		std::this_thread::sleep_for(sleepTime);

		dt = targetFrameDuration.count();
	}

	// fps calculation
	dtCount += dt;
	frameCount++;

	if (dtCount >= 1)
	{
		fps = frameCount;
		frameCount = 0;
		dtCount = 0;
	}

	app->gui->panelSettings->AddFpsValue(fps);
}


// -------------------- QUIT --------------------
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


// -------------------- Get / Set / Funtionalities --------------------
int App::GetArgc() const
{
	return argc;
}

const char* App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

int App::GetFrameRate() const
{
	return frameRate;
}

void App::SetFrameRate(int frameRate)
{
	this->frameRate = frameRate == 0 ? app->window->GetDisplayRefreshRate() : frameRate;
	targetFrameDuration = (std::chrono::duration<double>)1 / this->frameRate;
}

double App::GetDT() const
{
	return dt;
}

std::vector<std::string> App::GetLogs()
{
	return logs;
}

void App::LogConsole(const char* entry)
{
	if (logs.size() > MAX_LOGS_CONSOLE)
		logs.erase(logs.begin());

	log.append(entry);

	std::string toAdd = entry;
	logs.push_back(toAdd);
}

void App::CleanLogs()
{
	logs.clear();
}