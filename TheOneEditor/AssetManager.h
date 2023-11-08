#ifndef __ASSET_MANAGER_H__
#define __ASSET_MANAGER_H__
#pragma once

#include "Module.h"
#include "../TheOneEngine/EngineCore.h"
#include "Importer.h"

#include <string>
#include <map>


class Asset;
class MeshLoader;
class AssetTexture;


enum class AssetType
{
	MeshLoader,
	Texture
};

class AssetManager : public Module
{
	friend class ImporterMesh;
	//friend class ImporterTexture;

public:

	AssetManager(App* app);
	virtual ~AssetManager();

	bool Awake();
	bool Start();
	
	bool Update(double dt);
	bool CleanUp();

public:

	// Importing
	Asset* ImportMesh(Importer::ImportData& IData);
	Asset* ImportTexture(Importer::ImportData& IData);

	// For consistency, use this only on resource manager/importers 
	template<typename TImporter>
	TImporter* GetImporter()
	{
		for (uint i = 0; i < importers.size(); ++i)
		{
			if (importers[i]->GetType() == TImporter::GetType())
				return ((TImporter*)(importers[i]));
		}
		return nullptr;
	}

	// Asset Handling
	Asset* GetAsset(uint ID, bool loadinmemory = true);
	Asset* CreateAsset(AssetType type, std::string source_file);
	Asset* CreateAssetFromID(AssetType type, std::string source_file, uint ID);


	// Getter
	uint GetFileFormatVersion();

private:

	// Available importers
	std::vector<Importer*> importers;
	std::vector<std::string> filters;

	uint fileFormatVersion = 2;

	// Available resources
	std::map<uint, MeshLoader*> meshes;
	std::map<uint, AssetTexture*> textures;
};

#endif // !__ASSET_MANAGER_H__