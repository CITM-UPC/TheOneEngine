#ifndef __PANEL_H__
#define __PANEL_H__
#pragma once


enum class PanelType
{
	ABOUT,

	CONSOLE,
	HIERARCHY,
	INSPECTOR,
	PROJECT,
	SCENE,
	STATS,

	UNKNOWN
};

class Panel
{
public:

	Panel(PanelType type);
	virtual ~Panel();

	virtual bool Draw() = 0;

	virtual bool GetState();
	virtual void SetState(bool state);
	virtual void SwitchState();

	virtual PanelType GetType();

protected:

	bool enabled = false;
	PanelType type = PanelType::UNKNOWN;
};

#endif // !__PANEL_H__