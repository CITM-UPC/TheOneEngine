#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__
#pragma once

#include "Panel.h"

#include <filesystem>

class PanelProject : public Panel
{
public:
	PanelProject(PanelType type, std::string name);
	~PanelProject();

	bool Draw();
	std::pair<bool, uint32_t> DirectoryTreeViewRecursive(const std::filesystem::path& path, uint32_t* count, int* selection_mask);

public:
	std::string directoryPath;
};

#endif // !__PANEL_PROJECT_H__