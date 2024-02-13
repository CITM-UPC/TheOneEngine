#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__
#pragma once

#include "Panel.h"
#include "..\TheOneEngine\Defs.h"

class PanelInspector : public Panel
{
public:
	PanelInspector(PanelType type, std::string name);
	~PanelInspector();

	bool Draw();

private:
	bool needRefresh_pos;
	bool needRefresh_rot;
	bool needRefresh_sca;

	vec3f view_pos;
	vec3f view_rot_rad;
	vec3f view_rot_deg;
	vec3f view_sca;
};

#endif // !__PANEL_INSPECTOR_H__