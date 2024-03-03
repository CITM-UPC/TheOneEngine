#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__
#pragma once

#include "Panel.h"

#include "../TheOneEngine/GameObject.h"

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

	bool ReparentDragDrop(std::shared_ptr<GameObject> childGO);

private:
	bool open_selected;
	bool reparent;
};

#endif // !__PANEL_HIERARCHY_H__