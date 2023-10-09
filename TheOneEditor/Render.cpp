#include "App.h"
#include "Render.h"

#include "Defs.h"
#include "Log.h"

#define VSYNC true

Render::Render() : Module()
{
}

// Destructor
Render::~Render()
{}

// Called before render is available
bool Render::Awake()
{
	bool ret = true;



	return ret;
}

// Called before the first frame
bool Render::Start()
{
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	return true;
}

bool Render::Update(float dt)
{

	return true;
}

bool Render::PostUpdate()
{
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	return true;
}