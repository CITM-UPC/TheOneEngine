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
	// Create default mesh
	CreateMeshGO("");

	return true;
}

bool SceneManager::PreUpdate()
{
	// Do nothing
	return true;
}

bool SceneManager::Update(double dt)
{
	// Do nothing
	return true;
}

bool SceneManager::PostUpdate()
{
	// Draw
	RecurseDrawChildren(rootSceneGO);

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
	}

	return nullptr;
}