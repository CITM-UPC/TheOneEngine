#include "App.h"
#include "SceneManager.h"
#include "Log.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

SceneManager::SceneManager(App* app) : Module(app)
{
}

SceneManager::~SceneManager()
{
}

bool SceneManager::Awake()
{
	N_sceneManager = new N_SceneManager();

	return true;
}

bool SceneManager::Start()
{
	N_sceneManager->Start();

	return true;
}

bool SceneManager::PreUpdate()
{
	N_sceneManager->PreUpdate();

	return true;
}

bool SceneManager::Update(double dt)
{
	N_sceneManager->Update(dt);

	return true;
}

bool SceneManager::PostUpdate()
{
	N_sceneManager->PostUpdate();

	return true;
}

bool SceneManager::CleanUp()
{
	N_sceneManager->CleanUp();

	return true;
}