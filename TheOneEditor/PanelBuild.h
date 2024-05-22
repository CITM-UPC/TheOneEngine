#ifndef __PANEL_BUILD_H__
#define __PANEL_BUILD_H__
#pragma once

#include "Panel.h"

class PanelBuild : public Panel
{
public:
	PanelBuild(PanelType type, std::string name);
	~PanelBuild();

	bool Draw();

public:
	std::string directoryPath;
};

#endif // !__PANEL_PROJECT_H__