#include "App.h"
#include "SceneManager.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

SceneManager::SceneManager(App* app) : Module(app), selectedGameObject(0)
{
    meshLoader = new MeshLoader();
    rootSceneGO = std::make_shared<GameObject>("Scene");;
}

SceneManager::~SceneManager()
{
    delete meshLoader;
}

bool SceneManager::Awake()
{
    std::filesystem::create_directories("Library/");
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
        rootSceneGO.get()->children.begin(), rootSceneGO.get()->children.end(),
        [&uniqueName](const std::shared_ptr<GameObject>& obj)
        { return obj.get()->GetName() == uniqueName; }))
    {
        uniqueName = baseName + "(" + std::to_string(counter) + ")";
        ++counter;
    }

    return uniqueName;
}

std::shared_ptr<GameObject> SceneManager::CreateEmptyGO(std::string name)
{
    std::shared_ptr<GameObject> emptyGO = std::make_shared<GameObject>(name);
    emptyGO.get()->AddComponent<Transform>();

    emptyGO.get()->parent = rootSceneGO.get()->weak_from_this();

    rootSceneGO.get()->children.emplace_back(emptyGO);

    return emptyGO;
}

std::shared_ptr<GameObject> SceneManager::CreateMeshGO(std::string path)
{
    std::vector<MeshBufferedData> meshes = meshLoader->LoadMesh(path);
    std::vector<std::shared_ptr<Texture>> textures = meshLoader->LoadTexture(path);

    std::string name = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);
    name = GenerateUniqueName(name);

    // Create emptyGO parent if meshes >1
    bool isSingleMesh = meshes.size() > 1 ? false : true;
    std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
    if (!isSingleMesh) emptyParent.get()->SetName(name);

    for (auto& mesh : meshes)
    {
        std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mesh.meshName);
        meshGO.get()->AddComponent<Transform>();
        meshGO.get()->AddComponent<Mesh>();
        //meshGO.get()->AddComponent<Texture>(); // hekbas: must implement

        meshGO.get()->GetComponent<Mesh>()->mesh = mesh;
        meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex];
        // hekbas: need to set Transform?

        if (isSingleMesh)
        {
            meshGO.get()->parent = rootSceneGO;
            rootSceneGO.get()->children.push_back(meshGO);
        }
        else
        {
            meshGO.get()->parent = emptyParent;
            emptyParent.get()->children.push_back(meshGO);
        }
    }

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateExistingMeshGO(std::string fbxName)
{
    std::string folderName = "Library/Models/" + fbxName + "/";

    std::string name = fbxName;
    name = GenerateUniqueName(name);

    std::vector<std::string> fileNames;

    uint fileCount = 0;

    for (const auto& entry : fs::directory_iterator(folderName)) {
        if (fs::is_regular_file(entry)) {
            std::string path = entry.path().filename().string();
            LOG(LogType::LOG_WARNING, "- %s is in", path.data());
            fileNames.push_back(entry.path().string());
            fileCount++;
        }
    }

    // Create emptyGO parent if meshes >1
    bool isSingleMesh = fileCount > 1 ? false : true;
    std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
    if (!isSingleMesh) emptyParent.get()->SetName(name);

    for (const auto& file : fileNames)
    {
        MeshData mData = meshLoader->deserializeMeshData(file);

        meshLoader->BufferData(mData);

        std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mData.meshName);
        meshGO.get()->AddComponent<Transform>();
        meshGO.get()->AddComponent<Mesh>();
        //meshGO.get()->AddComponent<Texture>(); // hekbas: must implement

        meshGO.get()->GetComponent<Mesh>()->meshData = mData;
        //meshGO.get()->GetComponent<Mesh>()->mesh = meshLoader->GetBufferData();
        //meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex];
        // hekbas: need to set Transform?

        if (isSingleMesh)
        {
            meshGO.get()->parent = rootSceneGO;
            rootSceneGO.get()->children.push_back(meshGO);
        }
        else
        {
            meshGO.get()->parent = emptyParent;
            emptyParent.get()->children.push_back(meshGO);
        }

    }

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateCube()
{
    std::shared_ptr<GameObject> cubeGO = std::make_shared<GameObject>("Cube");
    cubeGO.get()->AddComponent<Transform>();
    cubeGO.get()->AddComponent<Mesh>();

    cubeGO.get()->parent = rootSceneGO.get()->weak_from_this();

    rootSceneGO.get()->children.emplace_back(cubeGO);

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateSphere()
{
    std::shared_ptr<GameObject> sphereGO = std::make_shared<GameObject>("Sphere");
    sphereGO.get()->AddComponent<Transform>();
    sphereGO.get()->AddComponent<Mesh>();

    sphereGO.get()->parent = rootSceneGO.get()->weak_from_this();

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
    return static_cast<uint>(rootSceneGO.get()->children.size());
}

std::vector<std::shared_ptr<GameObject>> SceneManager::GetGameObjects()
{
    return rootSceneGO.get()->children;
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
