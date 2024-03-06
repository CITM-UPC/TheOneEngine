#include "N_SceneManager.h"
#include "GameObject.h"
#include "MeshLoader.h"
#include "Component.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

N_SceneManager::N_SceneManager()
{
	meshLoader = new MeshLoader();
}

N_SceneManager::~N_SceneManager()
{
}

bool N_SceneManager::Awake()
{
	fs::create_directories("Library/");
	return true;
}

bool N_SceneManager::Start()
{
	return true;
}

bool N_SceneManager::PreUpdate()
{
	// Do nothing
	return true;
}

bool N_SceneManager::Update(double dt, bool isPlaying)
{
	// Do nothing

	// Save Scene by checking if isDirty and pressing CTRL+S
	//if (currentScene->IsDirty()) SaveScene();
	
	if (isPlaying)
	{
		currentScene->UpdateGOs(dt);
	}

	return true;
}

bool N_SceneManager::PostUpdate()
{
	// Draw
	currentScene->Draw();

	return true;
}

bool N_SceneManager::CleanUp()
{
	delete currentScene;

	delete meshLoader;

	return true;
}

void N_SceneManager::CreateNewScene(uint _index, std::string name)
{
	// Historn: Change currentScene when creating a new one? (First need save scene correctly for not loosing scenes)
	Scene* newScene = new Scene(_index, name);
	// Create with JSON Scene file
}

void N_SceneManager::LoadScene(uint index)
{

}

void N_SceneManager::LoadScene(std::string sceneName)
{
	std::string fileName = "Assets/Scenes/" + sceneName + ".toe";

	LoadSceneFromJSON(fileName);
}

void N_SceneManager::SaveScene()
{
	//Change to save the Scene Class

	std::string fileNameExt = currentScene->GetSceneName() + ".toe";

	fs::path filename = fs::path(ASSETS_PATH) / "Scenes" / fileNameExt;
	//string filename = "Assets/Scenes/";
	fs::path folderName = fs::path(ASSETS_PATH) / "Scenes";
	fs::create_directories(folderName);

	json sceneJSON;

	sceneJSON["sceneName"] = currentScene->GetSceneName();
	sceneJSON["index"] = currentScene->GetIndex();
	sceneJSON["path"] = filename;

	json gameObjectsJSON;
	/*Save all gameobjects*/
	for (const auto& go : currentScene->GetRootSceneGO().get()->children)
	{
		gameObjectsJSON.push_back(go.get()->SaveGameObject());
	}

	sceneJSON["GameObjects"] = gameObjectsJSON;

	std::ofstream(filename) << sceneJSON.dump(2);
	LOG(LogType::LOG_OK, "SAVE SUCCESFUL");

	currentScene->SetIsDirty(false);
}

void N_SceneManager::LoadSceneFromJSON(const std::string& filename)
{
	//Change to load the Scene Class

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

	if (sceneJSON.contains("sceneName"))
	{
		currentScene->SetSceneName(sceneJSON["sceneName"]);
		currentScene->GetRootSceneGO().get()->SetName(sceneJSON["sceneName"]);
	}
	if (sceneJSON.contains("index"))
	{
		currentScene->SetIndex(sceneJSON["index"]);
	}
	if (sceneJSON.contains("path"))
	{
		currentScene->SetPath(sceneJSON["path"]);
	}

	currentScene->GetRootSceneGO().get()->children.clear();

	// Load game objects from the JSON data
	if (sceneJSON.contains("GameObjects"))
	{
		const json& gameObjectsJSON = sceneJSON["GameObjects"];

		for (const auto& gameObjectJSON : gameObjectsJSON)
		{
			// Create a new game object
			auto newGameObject = CreateEmptyGO();
			newGameObject.get()->SetName(currentScene->GetSceneName());
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

std::string N_SceneManager::GenerateUniqueName(const std::string& baseName)
{
	std::string uniqueName = baseName;
	int counter = 1;

	while (std::any_of(
		currentScene->GetRootSceneGO().get()->children.begin(), currentScene->GetRootSceneGO().get()->children.end(),
		[&uniqueName](const std::shared_ptr<GameObject>& obj)
		{ return obj.get()->GetName() == uniqueName; }))
	{
		uniqueName = baseName + "(" + std::to_string(counter) + ")";
		++counter;
	}

	return uniqueName;
}

std::shared_ptr<GameObject> N_SceneManager::CreateEmptyGO(std::string name)
{
	std::shared_ptr<GameObject> emptyGO = std::make_shared<GameObject>(name);
	emptyGO.get()->AddComponent<Transform>();

	emptyGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();

	currentScene->GetRootSceneGO().get()->children.emplace_back(emptyGO);

	return emptyGO;
}

std::shared_ptr<GameObject> N_SceneManager::CreateCameraGO(std::string name)
{
	std::shared_ptr<GameObject> cameraGO = std::make_shared<GameObject>(name);
	cameraGO.get()->AddComponent<Transform>();
	cameraGO.get()->AddComponent<Camera>();
	cameraGO.get()->GetComponent<Camera>()->UpdateCamera();

	cameraGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();

	currentScene->GetRootSceneGO().get()->children.emplace_back(cameraGO);

	return cameraGO;
}

std::shared_ptr<GameObject> N_SceneManager::CreateMeshGO(std::string path)
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
				meshGO.get()->parent = currentScene->GetRootSceneGO();
				currentScene->GetRootSceneGO().get()->children.push_back(meshGO);
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

std::shared_ptr<GameObject> N_SceneManager::CreateExistingMeshGO(std::string path)
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
				meshGO.get()->parent = currentScene->GetRootSceneGO();
				currentScene->GetRootSceneGO().get()->children.push_back(meshGO);
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

std::shared_ptr<GameObject> N_SceneManager::CreateCube()
{
	std::shared_ptr<GameObject> cubeGO = std::make_shared<GameObject>("Cube");
	cubeGO.get()->AddComponent<Transform>();
	cubeGO.get()->AddComponent<Mesh>();

	cubeGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();

	currentScene->GetRootSceneGO().get()->children.emplace_back(cubeGO);

	return cubeGO;
}

std::shared_ptr<GameObject> N_SceneManager::CreateSphere()
{
	std::shared_ptr<GameObject> sphereGO = std::make_shared<GameObject>("Sphere");
	sphereGO.get()->AddComponent<Transform>();
	sphereGO.get()->AddComponent<Mesh>();

	sphereGO.get()->parent = currentScene->GetRootSceneGO().get()->weak_from_this();

	currentScene->GetRootSceneGO().get()->children.emplace_back(sphereGO);

	return nullptr;
}

std::shared_ptr<GameObject> N_SceneManager::CreateMF()
{
	return CreateMeshGO("Assets/Meshes/mf.fbx");
}

std::shared_ptr<GameObject> N_SceneManager::CreateTeapot()
{
	return CreateMeshGO("Assets/Meshes/teapot.fbx");
}

uint N_SceneManager::GetNumberGO() const
{
	return static_cast<uint>(currentScene->GetRootSceneGO().get()->children.size());
}

std::vector<std::shared_ptr<GameObject>> N_SceneManager::GetGameObjects()
{
	return currentScene->GetRootSceneGO().get()->children;
}

void N_SceneManager::SetSelectedGO(std::shared_ptr<GameObject> gameObj)
{
	selectedGameObject = gameObj;
}

std::shared_ptr<GameObject> N_SceneManager::GetSelectedGO() const
{
	return selectedGameObject;
}

void Scene::RecurseSceneDraw(std::shared_ptr<GameObject> parentGO)
{
	for (const auto gameObject : parentGO.get()->children)
	{
		gameObject.get()->Draw();
		RecurseSceneDraw(gameObject);
	}
}

void Scene::UpdateGOs(double dt)
{
	for (const auto gameObject : rootSceneGO->children)
	{
		gameObject->Update(dt);
	}
}

void Scene::RecurseUIDraw(std::shared_ptr<GameObject> parentGO, DrawMode mode)
{

	for (const auto gameObject : parentGO.get()->children)
	{
		gameObject.get()->DrawUI(mode);
		RecurseUIDraw(gameObject, mode);
	}
}

void Scene::Draw(DrawMode mode)
{
	RecurseSceneDraw(rootSceneGO);
	RecurseUIDraw(rootSceneGO, mode);
}