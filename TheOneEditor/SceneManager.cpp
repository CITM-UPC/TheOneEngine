#include "App.h"
#include "SceneManager.h"
#include "Log.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

SceneManager::SceneManager(App* app) : Module(app), selectedGameObject(0)
{
    meshLoader = new MeshLoader();
    rootSceneGO = std::make_shared<GameObject>("Scene");
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
    
    CreateMeshGO("Assets\\Meshes\\baker_house.fbx");
    /*CreateMeshGO("Assets\\Meshes\\street_new.fbx");
    CreateMeshGO("Assets\\Meshes\\Cadillac_CT4_V_2022.fbx");

    for (auto mesh : GetGameObjects()) {
        if (mesh->GetName() == "Cadillac_CT4_V_2022_LowPoly") {
            demo = mesh;
        }
    }

    rotationAngle = 0.0f;
    rotationSpeed = 30.0f;

    std::shared_ptr<GameObject> gameCam = CreateCameraGO("Game Camera");
    gameCam.get()->GetComponent<Camera>()->setPosition({ -10, 8, 0 });

    spatialObject2 = CreateTeapot("Assets\\Meshes\\teapot.fbx").get();
    spatialObject2->GetComponent<Transform>()->setPosition({ -50,15,0 });
    
    spatialObject1 = CreateTeapot("Assets\\Meshes\\teapot.fbx").get();
    spatialObject1->GetComponent<Transform>()->setPosition({ 50,15,0 });*/

    return true;
}

bool SceneManager::PreUpdate()
{
    return true;
}

bool SceneManager::Update(double dt)
{
    //Save Scene
    if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
    {
        SaveScene();
    }
    //Load Scene
    if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
    {
        std::string filename = "Assets/Scenes/scene.toe";
        LoadScene(filename);
    }
    //Audio moving
    /*if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && app->state == GameState::PLAY)
        spatialObject2->GetComponent<Transform>()->setPosition({spatialObject2->GetComponent<Transform>()->getPosition().x + 1 ,15,0 });
    if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && app->state == GameState::PLAY)
        spatialObject2->GetComponent<Transform>()->setPosition({ spatialObject2->GetComponent<Transform>()->getPosition().x -1 ,15,0 });*/

    /*if (app->IsPlaying()) {
        demo->GetComponent<Transform>()->rotate({ 0, 1, 0 }, rotationAngle);

        rotationAngle += rotationSpeed * dt;

        if (rotationAngle >= 360.0f)
            rotationAngle -= 360.0f;
    }

    if (app->state == GameState::NONE) {
        demo->GetComponent<Transform>()->rotate({ 1, 0, 0 }, 0.0);
        rotationAngle = 0.0;
        spatialObject2->GetComponent<Transform>()->setPosition({ -50,15,0 });
    }*/

    /*app->engine->audio->SetSpatial1Transform(
        spatialObject1->GetComponent<Transform>()->getPosition().x,
        spatialObject1->GetComponent<Transform>()->getPosition().y,
        spatialObject1->GetComponent<Transform>()->getPosition().z);
    app->engine->audio->SetSpatial2Transform(
        spatialObject2->GetComponent<Transform>()->getPosition().x,
        spatialObject2->GetComponent<Transform>()->getPosition().y,
        0);*/

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
    cameraGO.get()->GetComponent<Camera>()->UpdateCamera();

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

        // Take name before editing for meshData lookUp
        std::string folderName = "Library/Meshes/" + name + "/";

        name = GenerateUniqueName(name);

        // Create emptyGO parent if meshes > 1
        bool isSingleMesh = meshes.size() > 1 ? false : true;
        std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
        if (!isSingleMesh) emptyParent.get()->SetName(name);

        std::vector<std::string> fileNames;

        uint fileCount = 0;

        for (const auto& entry : fs::directory_iterator(folderName))
        {
            if (fs::is_regular_file(entry))
            {
                std::string path = entry.path().filename().string();
                fileNames.push_back(entry.path().string());
                fileCount++;
            }
        }

        for (auto& mesh : meshes)
        {
            std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mesh.meshName);

            // Transform ---------------------------------
            meshGO.get()->AddComponent<Transform>();

          

            // Mesh  --------------------------------------
            meshGO.get()->AddComponent<Mesh>();

            meshGO.get()->GetComponent<Mesh>()->mesh = mesh;
            meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex];
            
            //Load MeshData from custom files
            for (const auto& file : fileNames)
            {
                std::string fileName = file.substr(file.find_last_of("\\/") + 1, file.find_last_of('.') - file.find_last_of("\\/") - 1);
                if (fileName == mesh.meshName)
                {
                    MeshData mData = meshLoader->deserializeMeshData(file);

                    meshGO.get()->GetComponent<Mesh>()->meshData = mData;
                    meshGO.get()->GetComponent<Mesh>()->path = file;
                }               
            }
           
            // AABB
            meshGO.get()->GenerateAABBFromMesh();

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

        // hekbas: Implement Texture/Material Component
        //meshGO.get()->AddComponent<Texture>();
        //meshGO.get()->GetComponent<Texture>() = &meshGO.get()->GetComponent<Mesh>()->mesh.texture;
        /* if (!textures.empty())
        {
             if (meshes.size() == 1)
             {
                 ComponentTexture* texture = (ComponentTexture*)GetComponent(ComponentType::TEXTURE);
                 textures->setTexture((*meshes.begin())->texture);
                 defaultTexture = texture->getTexture()->path;
             }
             else
             {
                 for (auto i = meshes.begin(); i != meshes.end(); ++i)
                 {
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
            meshGO.get()->GetComponent<Mesh>()->path = file;
            //meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex]; //Implement texture deserialization
            // hekbas: need to set Transform?

            meshGO.get()->GenerateAABBFromMesh();

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

    return cubeGO;
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
    return CreateMeshGO("Assets/Meshes/mf.fbx");
}

std::shared_ptr<GameObject> SceneManager::CreateTeapot(std::string path)
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

        for (const auto& entry : fs::directory_iterator(folderName))
        {
            if (fs::is_regular_file(entry))
            {
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
                    meshGO.get()->GetComponent<Mesh>()->path = file;
                }

            }

            // hekbas: need to set Transform?

            meshGO.get()->GenerateAABBFromMesh();

            if (isSingleMesh)
            {
                meshGO.get()->parent = rootSceneGO;
                rootSceneGO.get()->children.push_back(meshGO);
                return meshGO;
            }
            else
            {
                meshGO.get()->parent = emptyParent;
                emptyParent.get()->children.push_back(meshGO);
                return emptyParent;

            }
        }
    }
    
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

std::shared_ptr<GameObject> SceneManager::FindGOByUID(uint32_t _UID) const
{
    for (const auto& go : rootSceneGO.get()->children)
    {
        if (go.get()->GetUID() == _UID) 
        {
            return go;
        }
        else
        {
            FindGOByUID(_UID);
        }
    }

    return nullptr;
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

    std::ofstream(filename) << sceneJSON.dump(2);
    LOG(LogType::LOG_OK, "SAVE SUCCESFUL");
}

void SceneManager::LoadScene(const std::string& filename)
{
    // Check if the scene file exists
    if (!fs::exists(filename))
    {
        LOG(LogType::LOG_ERROR, "Scene file does not exist: {}", filename.data());
        return;
    }

    // Read the scene JSON from the file
    std::ifstream file(filename);
    if (!file.is_open())
    {
        LOG(LogType::LOG_ERROR, "Failed to open scene file: {}", filename.data());
        return;
    }

    json sceneJSON;
    try
    {
        file >> sceneJSON;
    }
    catch (const json::parse_error& e)
    {
        LOG(LogType::LOG_ERROR, "Failed to parse scene JSON: {}", e.what());
        return;
    }

    // Close the file
    file.close();

    rootSceneGO.get()->children.clear();

    // Load game objects from the JSON data
    if (sceneJSON.contains("GameObjects"))
    {
        const json& gameObjectsJSON = sceneJSON["GameObjects"];

        for (const auto& gameObjectJSON : gameObjectsJSON)
        {
            // Create a new game object
            auto newGameObject = CreateEmptyGO();

            // Load the game object from JSON
            newGameObject->LoadGameObject(gameObjectJSON);

        }

        LOG(LogType::LOG_OK, "LOAD SUCCESSFUL");
    }
    else
    {
        LOG(LogType::LOG_ERROR, "Scene file does not contain GameObjects information");
    }

}

void SceneManager::RecurseDrawChildren(std::shared_ptr<GameObject> parentGO)
{
    for (const auto gameObject : parentGO.get()->children)
    {
        gameObject.get()->Draw();
        RecurseDrawChildren(gameObject);
    }
}
