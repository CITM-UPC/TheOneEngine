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
		UNKNOWN
	};


	Asset(AssetType type, uint ID, std::string path);
	virtual ~Asset();

	AssetType GetType() const;

	uint GetID() const;
	void SetID(uint ID);

	const char* GetPath() const;
	void SetPath(const char* new_path);

	const char* GetName() const;
	void SetName(const char* name);

	const char* GetExtension() const;

	const uint GetNumInstances() const;


protected:
	AssetType type = AssetType::UNKNOWN;

	uint ID = 0;
	
	std::string path = "";
	std::string name = "";
	std::string extension = "";

	uint instances = 0;
};

#endif // !__ASSET_H__