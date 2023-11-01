#ifndef __PANEL_STATS_H__
#define __PANEL_STATS_H__
#pragma once

#include "Panel.h"

class PanelStats : public Panel
{
public:
	PanelStats(PanelType type);
	~PanelStats();

	bool Draw();
};

#endif // !__PANEL_STATS_H__