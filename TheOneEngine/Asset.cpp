#include "Asset.h"
#include "Defs.h"


Asset::Asset(AssetType type, uint ID, std::string path)
{
	this->type = type;
	this->ID = ID;
	this->path = path;
}

Asset::~Asset() {}

Asset::AssetType Asset::GetType() const { return type; }

uint Asset::GetID() const { return ID; }
void Asset::SetID(uint ID) { this->ID = ID; }

const char* Asset::GetPath() const { return path.c_str(); }
void Asset::SetPath(const char* new_path) { path = new_path; }

const char* Asset::GetName() const { return name.c_str(); }
void Asset::SetName(const char* name) { this->name = name; }

const char* Asset::GetExtension() const { return extension.c_str(); }

const uint Asset::GetNumInstances() const { return instances; }