#ifndef __PANEL_GAME_H__
#define __PANEL_GAME_H__
#pragma once

#include "Panel.h"
#include "../TheOneEngine/Defs.h"
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

	bool Draw();

	Camera* primaryCamera = nullptr;

	Scene* current;
	std::shared_ptr<FrameBuffer> frameBuffer;

	vec2f viewportSize = { 0.0f, 0.0f };

	GameObject* selectedGO = nullptr;
private:

	bool isHovered;
	bool isFocused;
};

#endif // !__PANEL_SCENE_H__