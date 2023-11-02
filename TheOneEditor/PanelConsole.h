#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__
#pragma once

#include "Panel.h"

#define LOG_ERROR "*"
#define LOG_WARNING "!"

class PanelConsole : public Panel
{
public:
	PanelConsole(PanelType type);
	~PanelConsole();

	bool Draw();
};

#endif // !__PANEL_CONSOLE_H__