#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__
#pragma once

#include "Panel.h"

class PanelConsole : public Panel
{
public:
	PanelConsole(PanelType type);
	~PanelConsole();

	bool Draw();
};

#endif // !__PANEL_CONSOLE_H__