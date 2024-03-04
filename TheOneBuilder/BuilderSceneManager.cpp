#include "BuilderSceneManager.h"

BuilderSceneManager::BuilderSceneManager(BuilderApp* app) : BuilderModule(app) {}

BuilderSceneManager::~BuilderSceneManager() {}

bool BuilderSceneManager::Awake()
{
	N_sceneManager = new N_SceneManager();

	return true;
}

bool BuilderSceneManager::Start()
{
	/*N_sceneManager->currentScene = new Scene(0, "NewUntitledScene");
	N_sceneManager->LoadScene("SceneNameToLoad");*/ // add the name of the starting scene here
	N_sceneManager->Start();

	return true;
}

bool BuilderSceneManager::PreUpdate()
{
	N_sceneManager->PreUpdate();

	return true;
}

bool BuilderSceneManager::Update(double dt)
{
	N_sceneManager->Update(dt);

	return true;
}

bool BuilderSceneManager::PostUpdate()
{
	N_sceneManager->PostUpdate();

	return true;
}

bool BuilderSceneManager::CleanUp()
{
	N_sceneManager->CleanUp();

	return true;
}
