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
    //hekbas testing creation of GO
    CreateEmptyGO();
    CreateEmptyGO();
    CreateEmptyGO();
    CreateEmptyGO();

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
    std::shared_ptr<GameObject> emptyGO = std::make_shared<GameObject>("Empty GameObject");
    emptyGO.get()->AddComponent(ComponentType::Transform);

    gameObjects.push_back(emptyGO);

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateCube()
{
    std::shared_ptr<GameObject> cubeGO = std::make_shared<GameObject>("Cube");
    cubeGO.get()->AddComponent(ComponentType::Transform);
    cubeGO.get()->AddComponent(ComponentType::Mesh);

    gameObjects.push_back(cubeGO);

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateSphere()
{
    std::shared_ptr<GameObject> sphereGO = std::make_shared<GameObject>("Sphere");
    sphereGO.get()->AddComponent(ComponentType::Transform);
    sphereGO.get()->AddComponent(ComponentType::Mesh);

    gameObjects.push_back(sphereGO);

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