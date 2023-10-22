#include "App.h"


// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	window = new Window(this);
	input = new Input(this);
	hardware = new Hardware(this);
	renderer3D = new Renderer3D(this);

	// Ordered for awake / Start / Update
	// Reverse order for CleanUp
	
	AddModule(window, true);
	AddModule(input, true);
	AddModule(hardware, true);

	// Render last to swap buffer
	AddModule(renderer3D, true);

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
	bool ret = false;

	targetFrameDuration = (std::chrono::duration<double>)1 / targetFPS;

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
	bool ret = true;

	dt = 0.016; //hekbas

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
	frameStart = std::chrono::steady_clock::now();
}

// ---------------------------------------------
void App::FinishUpdate()
{
	frameEnd = std::chrono::steady_clock::now();
	auto frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(frameEnd - frameStart);

	//dt = frameDuration.count();

	if (frameDuration < targetFrameDuration)
	{
		std::chrono::duration<double> sleepTime = targetFrameDuration - frameDuration;
		std::this_thread::sleep_for(sleepTime);

		//dt = targetFrameDuration.count();
	}
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

		ret = item->PreUpdate();
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

		ret = item->Update(dt);
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

		ret = item->PostUpdate();
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
float App::GetDT()
{
	return dt;
}