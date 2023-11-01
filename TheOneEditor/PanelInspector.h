#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__
#pragma once

#include "Panel.h"

class PanelInspector : public Panel
{
public:
	PanelInspector(PanelType type);
	~PanelInspector();

	bool Draw();
};

#endif // !__PANEL_INSPECTOR_H__