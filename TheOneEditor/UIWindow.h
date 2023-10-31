#ifndef __UI_WINDOW_H__
#define __UI_WINDOW_H__
#pragma once

enum class UIWindowType
{
	ABOUT,
	CONSOLE,
	HIERARCHY,
	INSPECTOR,
	SETTINGS,
	UNKNOWN
};

class UIWindow
{
public:
	UIWindow(UIWindowType type);
	virtual ~UIWindow();

	virtual bool Draw() = 0;

	virtual void SetEnabled(bool enabled);

	virtual bool isEnabled();

	virtual UIWindowType GetType();

protected:
	bool enabled = false;
	UIWindowType type = UIWindowType::UNKNOWN;
};

#endif // !__UI_WINDOW_H__