#ifndef __PANEL_SETTINGS_H__
#define __PANEL_SETTINGS_H__
#pragma once

#include "Panel.h"
#include <vector>

#define MAX_HISTORY_SIZE 240

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
	void AddFpsValue(int fps);

private:

	void Performance();
	void Window();
	void Input();
	void Renderer();

	

private:

	SelectedSetting selected = SelectedSetting::PERFORMANCE;

	std::vector<int> fpsHistory;
	std::vector<double> delayHistory;
};

#endif // !__PANEL_SETTINGS_H__