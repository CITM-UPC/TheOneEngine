#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__
#pragma once

#include "Panel.h"
#include "../TheOneEngine/Defs.h"

class GameObject;

class PanelInspector : public Panel
{
public:
	PanelInspector(PanelType type, std::string name);
	~PanelInspector();

	void OnSelectGO(std::shared_ptr<GameObject> gameObj);
	bool Draw();

	void ChooseScriptNameWindow();

private:
	bool matrixDirty;

	vec3f view_pos;
	vec3f view_rot_rad;
	vec3f view_rot_deg;
	vec3f view_sca;

	bool chooseScriptNameWindow;

	GameObject* selectedGO = nullptr;
};

#endif // !__PANEL_INSPECTOR_H__