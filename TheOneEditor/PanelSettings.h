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
	RENDERER,
	HARDWARE,
	SOFTWARE
};

class PanelSettings : public Panel
{
public:

	PanelSettings(PanelType type, std::string name);
	~PanelSettings();

	bool Draw();
	void AddFpsValue(int fps);

private:

	void Performance();
	void Window();
	void Input();
	void Renderer();
	void Hardware();
	void Software();
	

private:

	SelectedSetting selected = SelectedSetting::PERFORMANCE;

	std::vector<int> fpsHistory;
	std::vector<double> delayHistory;

	const char* displayModes[4] =
	{
		"Windowed",
		"Full Screen",
		"Full Screen Desktop",
		"Borderless"
	};
	const char* resolutions[8] =
	{
		"3840x2160",
		"2560x1440",
		"1920x1080",
		"1280x720",
		"854x480",
		"640x360",
		"426x240",
		"Native"
	};
	const char* fpsList[8] =
	{
		"30",
		"60",
		"120",
		"144",
		"160",
		"180",
		"240",
		"Unlimited"
	};
};

#endif // !__PANEL_SETTINGS_H__