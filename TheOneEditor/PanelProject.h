#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__
#pragma once

#include "Panel.h"

class PanelProject : public Panel
{
public:
	PanelProject(PanelType type, std::string name);
	~PanelProject();

	bool Draw();
};

#endif // !__PANEL_PROJECT_H__