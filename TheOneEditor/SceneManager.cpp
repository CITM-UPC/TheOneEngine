#include "App.h"
#include "SceneManager.h"


SceneManager::SceneManager(App* app) : Module(app), selectedGameObject(0) {}

SceneManager::~SceneManager() {}

bool SceneManager::Awake()
{
    return true;
}

bool SceneManager::Start()
{
    std::shared_ptr<GameObject> empty = CreateEmptyGO();

    return true;
}

bool SceneManager::PreUpdate()
{
    return true;
}

bool SceneManager::Update(double dt)
{


    return true;
}

bool SceneManager::PostUpdate()
{
    return true;
}

bool SceneManager::CleanUp()
{
    return true;
}

std::shared_ptr<GameObject> SceneManager::CreateEmptyGO()
{
    std::string name = "Empty GameObject";

    std::shared_ptr<GameObject> newObject = std::make_shared<GameObject>();

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateCube()
{


    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateSphere()
{
    return nullptr;
}

uint SceneManager::GetNumberGO()
{
    return static_cast<uint>(gameObjects.size());
}

std::vector<std::shared_ptr<GameObject>> SceneManager::GetGameObjects()
{
    return gameObjects;
}

uint SceneManager::GetSelectedGO()
{
    return selectedGameObject;
}