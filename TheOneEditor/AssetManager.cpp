#include "AssetManager.h"
#include "App.h"
#include "Log.h"
#include "Defs.h"

#include "Importer.h"
#include "Asset.h"

#include <assimp/cimport.h>


// Get Assimp LOGS and print to console
void MyAssimpCallback(const char* msg, char* userData)
{
	LOG("[Assimp]: %s", msg);
}

AssetManager::AssetManager(App* app) : Module(app) {}

AssetManager::~AssetManager() {}

bool AssetManager::Awake()
{
	// --- Stream LOG messages to MyAssimpCallback, that sends them to console ---
	struct aiLogStream stream;
	stream.callback = MyAssimpCallback;
	aiAttachLogStream(&stream);


	//importers.push_back(new ImporterMesh());
	//importers.push_back(new ImporterTexture());

	return true;
}

bool AssetManager::Start()
{
	// Add file filters
	filters.push_back("fbx");
	filters.push_back("png");
	filters.push_back("jpg");

	return true;
}

bool AssetManager::Update(double dt)
{
	return true;
}

bool AssetManager::CleanUp()
{
	static_assert(static_cast<int>(Asset::AssetType::UNKNOWN) == 2, "Asset Clean Up needs to be updated");

	// Delete assets 
	for (std::map<uint, AssetMesh*>::iterator it = meshes.begin(); it != meshes.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = meshes.erase(it);
	}

	meshes.clear();

	for (std::map<uint, AssetTexture*>::iterator it = textures.begin(); it != textures.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = textures.erase(it);
	}

	textures.clear();

	// Delete importers 
	for (uint i = 0; i < importers.size(); ++i)
	{
		delete importers[i];
	}

	importers.clear();

	// Detach assimp log stream 
	aiDetachAllLogStreams();

	return true;
}



// ============================ IMPORT ============================

Asset* AssetManager::ImportMesh(Importer::ImportData& IData)
{
	Asset* mesh = nullptr;
	ImporterMesh* IMesh = GetImporter<ImporterMesh>();

	// --- Load the mesh directly from the lib (only declaration)---
	if (IData.path && IMesh)
	{
		mesh = IMesh->Load(IData.path);
	}

	return mesh;
}

Asset* AssetManager::ImportTexture(Importer::ImportData& IData)
{
	Asset* texture = nullptr;
	ImporterTexture* ITex = GetImporter<ImporterTexture>();

	// --- If the resource is already in library, load from there ---
	if (IsFileImported(IData.path))
		texture = ITex->Load(IData.path);


	// --- Else call relevant importer ---
	else
	{
		std::string new_path = IData.path;

		if (IData.dropped)
			new_path = DuplicateIntoGivenFolder(IData.path, app->gui->panelProject->GetcurrentDirectory()->GetResourceFile());

		IData.path = new_path.c_str();
		texture = ITex->Import(IData);
	}

	return texture;
}



// ============================ LOAD ============================

Asset* AssetManager::GetAsset(uint UID, bool loadinmemory) // loadinmem is used only when absolutely needed
{
	Asset* asset = nullptr;

	static_assert(static_cast<int>(Asset::AssetType::UNKNOWN) == 2, "Asset Get Switch needs to be updated");

	asset = asset ? asset : (meshes.find(UID) == meshes.end() ? asset : (*meshes.find(UID)).second);
	asset = asset ? asset : (textures.find(UID) == textures.end() ? asset : (*textures.find(UID)).second);

	if (asset && loadinmemory)
		asset->LoadToMemory();
	else if (!asset)
		LOG("![Warning]: Could not load: %i", UID);

	return asset;
}

Asset* AssetManager::CreateAsset(Asset::AssetType type, std::string source_file)
{
	// Note you CANNOT create a meta resource through this function, use CreateResourceGivenUID instead

	static_assert(static_cast<int>(Asset::AssetType::UNKNOWN) == 2, "Asset Creation Switch needs to be updated");

	Asset* resource = nullptr;

	switch (type)
	{
	case Asset::AssetType::MESH:
		resource = (Asset*)new AssetMesh(App->GetRandom().Int(), source_file);
		meshes[resource->GetUID()] = (AssetMesh*)resource;
		break;

	case Asset::AssetType::TEXTURE:
		resource = (Asset*)new AssetTexture(App->GetRandom().Int(), source_file);
		textures[resource->GetUID()] = (AssetTexture*)resource;
		break;

	case Asset::AssetType::UNKNOWN:
		LOG("![Warning]: Detected unsupported resource type");
		break;

	default:
		LOG("![Warning]: Detected unsupported resource type");
		break;
	}

	return resource;
}

Asset* AssetManager::CreateAssetGivenUID(Asset::AssetType type, std::string source_file, uint UID)
{
	Asset* resource = nullptr;

	static_assert(static_cast<int>(Asset::AssetType::UNKNOWN) == 2, "Asset Creation Switch needs to be updated");

	switch (type)
	{
	case Asset::AssetType::MESH:
		resource = (Asset*)new AssetMesh(UID, source_file);
		meshes[resource->GetUID()] = (AssetMesh*)resource;
		break;

	case Asset::AssetType::TEXTURE:
		resource = (Asset*)new AssetTexture(UID, source_file);
		textures[resource->GetUID()] = (AssetTexture*)resource;
		break;

	case Asset::AssetType::UNKNOWN:
		LOG("![Warning]: Detected unsupported resource type");
		break;

	default:
		LOG("![Warning]: Detected unsupported resource type");
		break;
	}

	return resource;
}

uint AssetManager::GetFileFormatVersion()
{
	return fileFormatVersion;
}