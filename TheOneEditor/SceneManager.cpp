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
    // hekbas testing creation of GO
    
    //CreateEmptyGO();
    //CreateCube();
    //CreateSphere();
    
    //CreateMeshGO("Assets\\Meshes\\baker_house.fbx");
    CreateMeshGO("Assets\\Meshes\\street.fbx");

    std::shared_ptr<GameObject> gameCam = CreateCameraGO("Game Camera");
    gameCam.get()->GetComponent<Camera>()->setPosition({ -10, 4, 0 });

    return true;
}

bool SceneManager::PreUpdate()
{
    return true;
}

bool SceneManager::Update(double dt)
{
    if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
    {
        SaveScene();
    }

    return true;
}

bool SceneManager::PostUpdate()
{
    RecurseDrawChildren(rootSceneGO);

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

std::shared_ptr<GameObject> SceneManager::CreateCameraGO(std::string name)
{
    std::shared_ptr<GameObject> cameraGO = std::make_shared<GameObject>(name);
    cameraGO.get()->AddComponent<Transform>();
    cameraGO.get()->AddComponent<Camera>();

    cameraGO.get()->parent = rootSceneGO.get()->weak_from_this();

    rootSceneGO.get()->children.emplace_back(cameraGO);

    return cameraGO;
}

std::shared_ptr<GameObject> SceneManager::CreateMeshGO(std::string path)
{
    std::vector<MeshBufferedData> meshes = meshLoader->LoadMesh(path);
    std::vector<std::shared_ptr<Texture>> textures = meshLoader->LoadTexture(path);

    if (!meshes.empty())
    {
        std::string name = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);

        //Take name before editing for meshData lookUp
        std::string folderName = "Library/Meshes/" + name + "/";

        name = GenerateUniqueName(name);

        // Create emptyGO parent if meshes >1
        bool isSingleMesh = meshes.size() > 1 ? false : true;
        std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
        if (!isSingleMesh) emptyParent.get()->SetName(name);

        std::vector<std::string> fileNames;

        uint fileCount = 0;

        for (const auto& entry : fs::directory_iterator(folderName)) {
            if (fs::is_regular_file(entry)) {
                std::string path = entry.path().filename().string();
                fileNames.push_back(entry.path().string());
                fileCount++;
            }
        }

        for (auto& mesh : meshes)
        {
            std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mesh.meshName);
            meshGO.get()->AddComponent<Transform>();
            meshGO.get()->AddComponent<Mesh>();
            //meshGO.get()->AddComponent<Texture>(); // hekbas: must implement

            meshGO.get()->GetComponent<Mesh>()->mesh = mesh;
            meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex];
            //meshGO.get()->GetComponent<Texture>() = &meshGO.get()->GetComponent<Mesh>()->mesh.texture;

            //Load MeshData from custom files
            for (const auto& file : fileNames)
            {
                std::string fileName = file.substr(file.find_last_of("\\/") + 1, file.find_last_of('.') - file.find_last_of("\\/") - 1);
                if (fileName == mesh.meshName)
                {
                    MeshData mData = meshLoader->deserializeMeshData(file);

                    meshGO.get()->GetComponent<Mesh>()->meshData = mData;
                }
                
            }

            // hekbas: need to set Transform?

            meshGO.get()->GetComponent<Mesh>()->GenerateAABB();

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

       /* if (!textures.empty()) {
            if (meshes.size() == 1) {

                ComponentTexture* texture = (ComponentTexture*)GetComponent(ComponentType::TEXTURE);
                textures->setTexture((*meshes.begin())->texture);
                defaultTexture = texture->getTexture()->path;
            }
            else {
                for (auto i = meshes.begin(); i != meshes.end(); ++i) {
 
                    ComponentTexture* texturePart = (ComponentTexture*)GOPart->GetComponent(ComponentType::TEXTURE);
                    texturePart->setTexture((*i)->texture);
                    defaultTexture = texturePart->getTexture()->path;

                }
            }
        }*/
    }

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateExistingMeshGO(std::string path)
{
    std::string fbxName = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);

    std::string folderName = "Library/Meshes/" + fbxName + "/";

    std::vector<std::string> fileNames;

    uint fileCount = 0;

    if (fs::is_directory(folderName))
    {
        for (const auto& entry : fs::directory_iterator(folderName)) {
            if (fs::is_regular_file(entry)) {
                std::string path = entry.path().filename().string();
                LOG(LogType::LOG_WARNING, "- %s is in", path.data());
                fileNames.push_back(entry.path().string());
                fileCount++;
            }
        }
    }

    if (fileCount < 1)
    {
        CreateMeshGO(path);
    }
    else
    {
        std::string name = fbxName;
        name = GenerateUniqueName(name);

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
            meshGO.get()->GetComponent<Mesh>()->mesh = meshLoader->GetBufferData();
            //meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex]; //Implement texture deserialization
            // hekbas: need to set Transform?

            meshGO.get()->GetComponent<Mesh>()->GenerateAABB();

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
    CreateMeshGO("Assets/Meshes/mf.fbx");

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

void SceneManager::SaveScene()
{
    fs::path filename = fs::path(ASSETS_PATH) / "Scenes" / "scene.toe";
    //string filename = "Assets/Scenes/";
    fs::path folderName = fs::path(ASSETS_PATH) / "Scenes";
    fs::create_directories(folderName);

    json sceneJSON;

    json gameObjectsJSON;
    /*Save all gameobjects*/
    for (const auto& go : GetGameObjects())
    {
        gameObjectsJSON.push_back(go.get()->SaveGameObject());
    }

    sceneJSON["GameObjects"] = gameObjectsJSON;

    std::ofstream(filename) << sceneJSON;
}

void SceneManager::LoadScene(const std::string& filename)
{

    // Load the JSON document from a file
    std::ifstream inFile(filename);
    nlohmann::json json;
    inFile >> json;
    inFile.close();

    //// Deserialize MeshData members from JSON
    //data.meshName = json["meshName"].get<std::string>();
    //data.format = json["format"].get<Formats>();

    //// Deserialize vertex_data from JSON array
    //for (const auto& vertexJson : json["vertex_data"]) {
    //    V3T2 vertex;
    //    vertex.v.x = vertexJson["v"]["x"].get<float>();
    //    vertex.v.y = vertexJson["v"]["y"].get<float>();
    //    vertex.v.z = vertexJson["v"]["z"].get<float>();
    //    vertex.t.x = vertexJson["t"]["x"].get<float>();
    //    vertex.t.y = vertexJson["t"]["y"].get<float>();
    //    data.vertex_data.push_back(vertex);
    //}

    //// Deserialize index_data from JSON array
    //for (const auto& index : json["index_data"]) {
    //    data.index_data.push_back(index.get<unsigned int>());
    //}

}

void SceneManager::RecurseDrawChildren(std::shared_ptr<GameObject> parentGO)
{
    for (const auto gameObject : parentGO.get()->children)
    {
        gameObject.get()->Draw();
        RecurseDrawChildren(gameObject);
    }
}
