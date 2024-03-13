#ifndef __PANEL_GAME_H__
#define __PANEL_GAME_H__
#pragma once

#include "Panel.h"
#include "../TheOneEngine/Framebuffer.h"

#include <vector>
#include <memory>

class GameObject;
class Camera;
class Scene;

class PanelGame : public Panel
{
public:
	PanelGame(PanelType type, std::string name);
	~PanelGame();
	void Start();

	bool Start();
	bool Draw();

	bool AddCameraToRenderList(GameObject* cameraGO);

	// hekbas implement camera list
	std::vector<GameObject*> gameCameras;
	Camera* primaryCamera = nullptr;

	Scene* current;
	std::shared_ptr<FrameBuffer> frameBuffer;

	glm::vec2 viewportSize = { 0.0f, 0.0f };

private:

	bool isHovered;
	bool isFocused;
};

#endif // !__PANEL_SCENE_H__