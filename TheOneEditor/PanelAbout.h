#ifndef __PANEL_ABOUT_H__
#define __PANEL_ABOUT_H__
#pragma once

#include "Panel.h"

class PanelAbout : public Panel
{
public:
	PanelAbout(PanelType type, std::string name);
	~PanelAbout();

	bool Draw();
};

#endif // !__PANEL_ABOUT_H__