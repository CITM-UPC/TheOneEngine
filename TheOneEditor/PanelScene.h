#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__
#pragma once

#include "Panel.h"

class PanelScene : public Panel
{
public:
	PanelScene(PanelType type);
	~PanelScene();

	bool Draw();
};

#endif // !__PANEL_SCENE_H__