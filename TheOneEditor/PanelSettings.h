#ifndef __PANEL_SETTINGS_H__
#define __PANEL_SETTINGS_H__
#pragma once

#include "Panel.h"

enum class SelectedSetting
{
	PERFORMANCE,
	WINDOW,
	INPUT,
	RENDERER
};

class PanelSettings : public Panel
{
public:

	PanelSettings(PanelType type);
	~PanelSettings();

	bool Draw();

	void Performance();
	void Window();
	void Input();
	void Renderer();

private:

	SelectedSetting selected = SelectedSetting::PERFORMANCE;

};

#endif // !__PANEL_SETTINGS_H__