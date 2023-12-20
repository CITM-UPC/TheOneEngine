#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__
#pragma once

#include "Panel.h"

class PanelScene : public Panel
{
public:
	PanelScene(PanelType type, std::string name);
	~PanelScene();

	bool Draw();

	void CalculateSizeAspectRatio(int maxWidth, int maxHeight, int& width, int& height);
};

#endif // !__PANEL_SCENE_H__