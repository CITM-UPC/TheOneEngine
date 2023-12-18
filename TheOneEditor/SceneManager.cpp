#include "App.h"
#include "SceneManager.h"


SceneManager::SceneManager(App* app) : Module(app), selectedGameObject(0)
{
    meshLoader = new MeshLoader();
    rootSceneGO = std::make_shared<GameObject>("RootScene GameObject");;
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
    CreateMeshGO("Assets\\baker_house.fbx");

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
    DrawChildren(rootSceneGO);

    return true;
}

bool SceneManager::CleanUp()
{
    return true;
}

std::string SceneManager::GenerateUniqueName(const std::string& baseName)
{
    std::string uniqueName = baseName;
    int counter = 1;

    while (std::any_of(
        gameObjects.begin(), gameObjects.end(),
        [&uniqueName](const std::shared_ptr<GameObject>& obj)
        { return obj.get()->GetName() == uniqueName; }))
    {
        uniqueName = baseName + "(" + std::to_string(counter) + ")";
        ++counter;
    }

    return uniqueName;
}

std::shared_ptr<GameObject> SceneManager::CreateEmptyGO()
{
    std::shared_ptr<GameObject> emptyGO = std::make_shared<GameObject>("Empty GameObject");
    emptyGO.get()->AddComponent<Transform>();

    rootSceneGO.get()->children.emplace_back(emptyGO);

    return emptyGO;
}

std::shared_ptr<GameObject> SceneManager::CreateMeshGO(std::string path)
{
    std::vector<MeshBufferedData> meshes = meshLoader->LoadMesh(path);
    std::vector<std::shared_ptr<Texture>> textures = meshLoader->LoadTexture(path);

    // Create emptyGO parent if meshes >1
    std::shared_ptr<GameObject> root = meshes.size() > 1 ? CreateEmptyGO() : nullptr;
    std::string name = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);
    name = GenerateUniqueName(name);
    if (root != nullptr) root.get()->SetName(name);

    for (auto& mesh : meshes)
    {
        std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mesh.meshName);
        meshGO.get()->AddComponent<Transform>();
        meshGO.get()->AddComponent<Mesh>();
        //meshGO.get()->AddComponent<Texture>(); // hekbas: must implement

        mesh.parent = root;
        meshGO.get()->parent = root.get()->weak_from_this();
        root.get()->children.push_back(meshGO);
        meshGO.get()->GetComponent<Mesh>()->mesh = mesh;
        meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex];

        // hekbas: need to set Transform?
    }

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateCube()
{
    std::shared_ptr<GameObject> cubeGO = std::make_shared<GameObject>("Cube");
    cubeGO.get()->AddComponent<Transform>();
    cubeGO.get()->AddComponent<Mesh>();

    rootSceneGO.get()->children.emplace_back(cubeGO);

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateSphere()
{
    std::shared_ptr<GameObject> sphereGO = std::make_shared<GameObject>("Sphere");
    sphereGO.get()->AddComponent<Transform>();
    sphereGO.get()->AddComponent<Mesh>();

    rootSceneGO.get()->children.emplace_back(sphereGO);

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateMF()
{
    CreateMeshGO("Assets/mf.fbx");
    /*std::shared_ptr<GameObject> mfGO = std::make_shared<GameObject>("Parsecs!");
    mfGO.get()->AddComponent<Transform>();
    mfGO.get()->AddComponent<Mesh>();

    Mesh* mesh = mfGO.get()->GetComponent<Mesh>();
    mesh->meshes = meshLoader->LoadMesh(mfGO, "Assets/mf.fbx");

    gameObjects.push_back(mfGO);*/

    return nullptr;
}

uint SceneManager::GetNumberGO() const
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

std::shared_ptr<GameObject> SceneManager::GetSelectedGO() const
{
    return selectedGameObject;
}

std::shared_ptr<GameObject> SceneManager::GetRootSceneGO() const
{
    return rootSceneGO;
}

void SceneManager::DrawChildren(std::shared_ptr<GameObject> parentGO)
{
    for (const auto gameObject : parentGO.get()->children)
    {
        gameObject.get()->Draw();
        DrawChildren(gameObject);
    }
}
