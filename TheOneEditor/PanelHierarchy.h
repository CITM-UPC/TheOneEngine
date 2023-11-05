#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__
#pragma once

#include "Panel.h"

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy(PanelType type, std::string name);
	~PanelHierarchy();

	bool Draw();
};

#endif // !__PANEL_HIERARCHY_H__