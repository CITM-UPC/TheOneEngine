#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__
#pragma once

#include "Panel.h"

#include <filesystem>

enum class FileDropType {
	MODEL3D,
	TEXTURE,
	FOLDER,
	SCRIPT,
	SCENE,
	PREFAB,

	UNKNOWN
};

struct FileInfo {
	std::string name;
	FileDropType fileType;
};

class PanelProject : public Panel
{
public:
	PanelProject(PanelType type, std::string name);
	~PanelProject();

	bool Draw();
	std::pair<bool, uint32_t> DirectoryTreeViewRecursive(const std::filesystem::path& path, uint32_t* count, int* selection_mask);

	//Function to move files through the folders and some to the scene
	bool DragAndDrop();

	//Function that shows the files of a selected folder (take into account selected Directory in DirectoryTreeViewRecursive)
	bool ShowAssetFiles();

	// Function to list files in a directory
	std::vector<FileInfo> ListFiles(const std::string& path);

public:
	std::string directoryPath;
};

#endif // !__PANEL_PROJECT_H__