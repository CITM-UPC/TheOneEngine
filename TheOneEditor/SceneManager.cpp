#include "App.h"
#include "SceneManager.h"


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
    if (selectedGameObject && app->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
    {
        auto child = CreateMF();
        selectedGameObject.get()->AddChild(child);
    }

    return true;
}

bool SceneManager::PostUpdate()
{
    for (const auto gameObject : gameObjects)
    {
        if (gameObject.get()->IsEnabled())
        {
            gameObject.get()->Draw();
        }
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
    emptyGO.get()->AddComponent<Transform>();

    int size = gameObjects.size();

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        if (emptyGO.get()->GetName() == gameObjects[i].get()->GetName())
        {
            size++;
            i = 0;
        }
    }

    if (size != 0)
    {
        emptyGO.get()->SetName("Empty GameObject " + std::to_string(size));
    }

    gameObjects.push_back(emptyGO);

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateMeshGO(std::string path)
{
    std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>("Mesh GameObject");
    meshGO.get()->AddComponent<Transform>();
    meshGO.get()->AddComponent<Mesh>();
    //meshGO.get()->AddComponent<Texture>();

    Mesh* mesh = meshGO.get()->GetComponent<Mesh>();
    mesh->meshes = meshLoader->loadFromFile(meshGO, path);

    int size = gameObjects.size();

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
    cubeGO.get()->AddComponent<Transform>();
    cubeGO.get()->AddComponent<Mesh>();

    int size = gameObjects.size();

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        if (cubeGO.get()->GetName() == gameObjects[i].get()->GetName())
        {
            size++;
            i = 0;
        }
    }

    if (size != 0)
    {
        cubeGO.get()->SetName("Cube " + std::to_string(size));
    }

    gameObjects.push_back(cubeGO);

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateSphere()
{
    std::shared_ptr<GameObject> sphereGO = std::make_shared<GameObject>("Sphere");
    sphereGO.get()->AddComponent<Transform>();
    sphereGO.get()->AddComponent<Mesh>();

    int size = gameObjects.size();

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        if (sphereGO.get()->GetName() == gameObjects[i].get()->GetName())
        {
            size++;
            i = 0;
        }
    }

    if (size != 0)
    {
        sphereGO.get()->SetName("Sphere " + std::to_string(size));
    }

    gameObjects.push_back(sphereGO);

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateMF()
{
    std::shared_ptr<GameObject> mfGO = std::make_shared<GameObject>("Parsecs!");
    mfGO.get()->AddComponent<Transform>();
    mfGO.get()->AddComponent<Mesh>();

    Mesh* mesh = mfGO.get()->GetComponent<Mesh>();
    mesh->meshes = meshLoader->loadFromFile(mfGO, "Assets/mf.fbx");

    int size = gameObjects.size();

    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        if (mfGO.get()->GetName() == gameObjects[i].get()->GetName())
        {
            size++;
            i = 0;
        }
    }

    if (size != 0)
    {
        mfGO.get()->SetName("Parsecs! " + std::to_string(size));
    }

    gameObjects.push_back(mfGO);

    return mfGO; //Should return the GO to be able to add as child?
}

uint SceneManager::GetNumberGO()
{
    return static_cast<uint>(gameObjects.size());
}

std::vector<std::shared_ptr<GameObject>> SceneManager::GetGameObjects()
{
    return gameObjects;
}

void SceneManager::SetSelectedGO(std::shared_ptr<GameObject> gameObj)
{
    selectedGameObject = gameObj;
}

std::shared_ptr<GameObject> SceneManager::GetSelectedGO()
{
    return selectedGameObject;
}