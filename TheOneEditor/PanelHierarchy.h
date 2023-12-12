#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__
#pragma once

#include "Panel.h"

#include "..\TheOneEngine\GameObject.h"

#include <string>
#include <vector>
#include <list>
#include <memory>

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy(PanelType type, std::string name);
	~PanelHierarchy();

	void RecurseShowChildren(std::shared_ptr<GameObject> parent);

	bool Draw();

	void ContextMenu(std::shared_ptr<GameObject> go);

private:
	std::shared_ptr<GameObject> drag;
};

#endif // !__PANEL_HIERARCHY_H__