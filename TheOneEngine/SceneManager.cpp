#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "MeshLoader.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{

}

bool SceneManager::Awake()
{

    return true;
}

bool SceneManager::Start()
{

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
    //RecurseDrawChildren(rootSceneGO);

    return true;
}

bool SceneManager::CleanUp()
{
    return true;
}

void SceneManager::CreateNewScene()
{
}


void SceneManager::RecurseDrawChildren(std::shared_ptr<GameObject> parentGO)
{
    for (const auto gameObject : parentGO.get()->children)
    {
        gameObject.get()->Draw();
        RecurseDrawChildren(gameObject);
    }
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

void SceneManager::LoadSceneFromJSON(const std::string& filename)
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

    currentScene.get()->children.clear();

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