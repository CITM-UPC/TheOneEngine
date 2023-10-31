#ifndef __UI_WINDOW_ABOUT__
#define __UI_WINDOW_ABOUT__
#pragma once

#include "UIWindow.h"

class UIWindowAbout : public UIWindow
{
public:
	UIWindowAbout(UIWindowType type);
	~UIWindowAbout();

	bool Draw();
};

#endif // !__UI_WINDOW_ABOUT__