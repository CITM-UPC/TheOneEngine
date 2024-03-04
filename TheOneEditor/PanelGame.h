#ifndef __PANEL_GAME_H__
#define __PANEL_GAME_H__
#pragma once

#include "Panel.h"

#include <vector>

class GameObject;
class Camera;

class PanelGame : public Panel
{
public:
	PanelGame(PanelType type, std::string name);
	~PanelGame();

	bool Start();
	bool Draw();

	bool AddCameraToRenderList(GameObject* cameraGO);

	// hekbas implement camera list
	std::vector<GameObject*> gameCameras;

	GameObject* selectedGO = nullptr;
	Camera* cameraToRender = nullptr;
};

#endif // !__PANEL_SCENE_H__