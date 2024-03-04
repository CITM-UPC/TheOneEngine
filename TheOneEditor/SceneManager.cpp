#include "App.h"
#include "SceneManager.h"
#include "Gui.h"
#include "PanelInspector.h"
#include "../TheOneEngine/Log.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

SceneManager::SceneManager(App* app) : Module(app) {}

SceneManager::~SceneManager() {}

bool SceneManager::Awake()
{
	N_sceneManager = new N_SceneManager();

	return true;
}

bool SceneManager::Start()
{
	N_sceneManager->currentScene = new Scene(0, "NewUntitledScene");
	//N_sceneManager->LoadScene("Scene_2");
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
	N_sceneManager->Update(dt, app->IsPlaying());

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