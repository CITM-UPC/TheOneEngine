#include "Asset.h"
#include "App.h"
#include "ResourceManager.h"
#include "Log.h"


Asset::Asset(AssetType type, uint UID, std::string source_file)
{
	this->type = type;
	this->UID = UID;
	original_file = source_file;
}

Asset::~Asset()
{
}

uint Asset::GetUID() const
{
	return UID;
}

void Asset::SetUID(uint UID)
{
	this->UID = UID;
}

Asset::AssetType Asset::GetType() const
{
	return type;
}

const char* Asset::GetOriginalFile() const
{
	return original_file.c_str();
}

const char* Asset::GetResourceFile() const
{
	return resource_file.c_str();
}

const char* Asset::GetExtension() const
{
	return extension.c_str();
}

const char* Asset::GetName() const
{
	return name.c_str();
}

const uint Asset::GetPreviewTexID() const
{
	return previewTexID;
}

const uint Asset::GetNumInstances() const
{
	return instances;
}

void Asset::SetOriginalFile(const char* new_path)
{
	original_file = new_path;
	// Redo resource file
	Repath();
}

void Asset::SetResourceFile(const char* new_path)
{
	resource_file = new_path;
}

bool Asset::IsInMemory() const
{
	return instances > 0;
}

bool Asset::LoadToMemory()
{
	if (instances > 0)
	{
		instances++;
	}
	else
	{
		instances = LoadInMemory() ? 1 : 0;
	}

	return instances > 0;
}

void Asset::Release()
{
	//hekbas review code

	if (instances != 0)
	{
		if (--instances == 0)
		{
			FreeMemory();
		}
	}
	else
		LOG("![Warning]: Trying to release an already released resource: %s", name.c_str());
}


void Asset::SetName(const char* name)
{
	this->name = name;
}

void Asset::SetPreviewTexID(uint ID)
{
	previewTexID = ID;
}