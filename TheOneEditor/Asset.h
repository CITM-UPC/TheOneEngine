#ifndef __ASSET_H__
#define __ASSET_H__
#pragma once

#include "Defs.h"

#include <string>
#include <vector>

class GameObject;

class Asset
{
public:
	enum class AssetType
	{
		MESH,
		TEXTURE,
		UNKNOWN,
	};


	Asset(AssetType type, uint UID, std::string source_file);
	virtual ~Asset();

	uint GetUID() const;
	AssetType GetType() const;
	const char* GetOriginalFile() const;
	const char* GetResourceFile() const;
	const char* GetExtension() const;
	const char* GetName() const;
	const uint GetPreviewTexID() const;
	const uint GetNumInstances() const;

	void SetOriginalFile(const char* new_path);
	void SetResourceFile(const char* new_path); // for temporal scene 
	void SetUID(uint UID);
	void SetName(const char* name);
	void SetPreviewTexID(uint ID);

	bool IsInMemory() const;
	bool LoadToMemory();
	void Release();


	virtual void OnOverwrite() = 0;
	virtual void OnDelete() = 0;
	virtual void CreateInspectorNode() {};


protected:

	// Utils
	virtual bool LoadInMemory() = 0;
	virtual void FreeMemory() = 0;
	virtual void Repath() {};

protected:
	uint instances = 0;
	uint previewTexID = 0;
	uint UID = 0;
	AssetType type = AssetType::UNKNOWN;

	std::vector<GameObject*> users; // Asset notifies all interested objects of overwrites/deletes
	std::string resource_file = "";
	std::string original_file = "";
	std::string name = "";
	std::string extension = "";
};

#endif // !__ASSET_H__