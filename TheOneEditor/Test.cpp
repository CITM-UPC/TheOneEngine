#include "App.h"
#include "Test.h"

#include "Defs.h"
#include "Log.h"

#define VSYNC true

Test::Test() : Module()
{
}

// Destructor
Test::~Test()
{}

// Called before render is available
bool Test::Awake()
{
	bool ret = true;



	return ret;
}

// Called before the first frame
bool Test::Start()
{
	return true;
}

// Called each loop iteration
bool Test::PreUpdate()
{
	return true;
}

bool Test::Update(float dt)
{

	return true;
}

bool Test::PostUpdate()
{
	return true;
}

// Called before quitting
bool Test::CleanUp()
{
	return true;
}