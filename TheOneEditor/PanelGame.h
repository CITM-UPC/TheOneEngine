#ifndef __PANEL_GAME_H__
#define __PANEL_GAME_H__
#pragma once

#include "Panel.h"

class PanelGame : public Panel
{
public:
	PanelGame(PanelType type, std::string name);
	~PanelGame();

	bool Draw();

	// hekbas implement camera list
	//std::vector<GameObject> gameCameras;
};

#endif // !__PANEL_SCENE_H__