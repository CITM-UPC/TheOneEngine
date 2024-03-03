#ifndef __BUILDER_INPUT_H__
#define __BUILDER_INPUT_H__
#pragma once

#include "BuilderModule.h"

#define MAX_MOUSE_BUTTONS 5

enum B_KEY_STATE
{
	B_KEY_IDLE = 0,
	B_KEY_DOWN,
	B_KEY_REPEAT,
	B_KEY_UP
};

class BuilderInput : public BuilderModule
{
public:
	BuilderInput(BuilderApp* app);

	virtual ~BuilderInput();

	bool Awake();

	bool PreUpdate();
	bool Update(double dt);

	bool processSDLEvents();

	B_KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	B_KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

private:
	B_KEY_STATE* keyboard;
	B_KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;
	float dx;
	float dy;
};

#endif // !__BUILDER_INPUT_H__