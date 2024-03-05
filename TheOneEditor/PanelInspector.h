#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__
#pragma once

#include "Panel.h"
#include "../TheOneEngine/Defs.h"

#include "../TheOneEditor/Assets/Audio/Wwise Project/GeneratedSoundBanks/Wwise_IDs.h"

class GameObject;

class PanelInspector : public Panel
{
public:
	PanelInspector(PanelType type, std::string name);
	~PanelInspector();

	void OnSelectGO(std::shared_ptr<GameObject> gameObj);
	bool Draw();

	void ChooseScriptNameWindow();
	void ChooseEventWindow();

private:
	bool matrixDirty;

	vec3f view_pos;
	vec3f view_rot_rad;
	vec3f view_rot_deg;
	vec3f view_sca;

	bool chooseScriptNameWindow;
	bool chooseEventWindow;

	GameObject* selectedGO = nullptr;

	AkUniqueID selectedEvent;
};

#endif // !__PANEL_INSPECTOR_H__