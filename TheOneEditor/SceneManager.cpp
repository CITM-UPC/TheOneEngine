#include "App.h"
#include "SceneManager.h"
#include "..\TheOneEngine\MeshLoader.h"
#include "..\TheOneEngine\Mesh.h"


SceneManager::SceneManager(App* app) : Module(app), selectedGameObject(0)
{
    meshLoader = new MeshLoader();
}

SceneManager::~SceneManager()
{
    delete meshLoader;
}

bool SceneManager::Awake()
{
    return true;
}

bool SceneManager::Start()
{
    //hekbas testing creation of GO
    /*CreateEmptyGO();
    CreateCube();
    CreateSphere();*/
    CreateMeshGO("Assets/baker_house.fbx");

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
    for (const auto gameObject : gameObjects)
    {
        gameObject.get()->Draw();
    }

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

std::shared_ptr<GameObject> SceneManager::CreateMeshGO(std::string path)
{
    int size = gameObjects.size();

    std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>("Mesh GameObject");
    meshGO.get()->AddComponent(ComponentType::Transform);
    meshGO.get()->AddComponent(ComponentType::Mesh);
    //meshGO.get()->AddComponent(ComponentType::Texture);
    meshGO.get()->GetComponent<Mesh>().get()->meshes = meshLoader->loadFromFile(meshGO, path);

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        if (meshGO.get()->GetName() == gameObjects[i].get()->GetName())
        {
            size++;
            i = 0;
        }
    }

    if (size != 0)
    {
        meshGO.get()->SetName("Mesh GameObject " + std::to_string(size));
    }

    gameObjects.push_back(meshGO);

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

std::shared_ptr<GameObject> SceneManager::CreateMF()
{
    std::shared_ptr<GameObject> mfGO = std::make_shared<GameObject>("Parsecs!");
    mfGO.get()->AddComponent(ComponentType::Transform);
    mfGO.get()->AddComponent(ComponentType::Mesh);
    mfGO.get()->GetComponent<Mesh>().get()->meshes = meshLoader->loadFromFile(mfGO, "Assets/mf.fbx");

    gameObjects.push_back(mfGO);

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

//uint SceneManager::GetSelectedGO()
//{
//    return selectedGameObject;
//}

void SceneManager::SetSelectedGO(std::shared_ptr<GameObject> gameObj)
{
    selectedGameObject = gameObj;
}

std::shared_ptr<GameObject> SceneManager::GetSelectedGO()
{
    return selectedGameObject;
}

//void SceneManager::SetSelectedGO(uint index)
//{
//    selectedGameObject = index;
//}
