#ifndef GAMEPAD_INPUT_H
#define GAMEPAD_INPUT_H

#include "Module.h"

#define MAX_GAMEPAD 4

struct _SDL_GameController;

struct Gamepad
{
	//Input Data
	bool start, back, guide;
	bool x, y, a, b, l1, r1, l3, r3;
	bool up, down, left, right;
	float l2, r2;
	float left_x, left_y, left_dz, right_x, right_y, right_dz;

	//Controller Data
	bool enabled = false;
	int index;
	SDL_GameController* controller;
};

class GamepadInput : public Module
{
public:
	GamepadInput(App* app);

	virtual ~GamepadInput();

	bool Awake();

	bool PreUpdate();
	bool Update(double dt);

private:
	void HandleDeviceConnection(int index);
	void HandleDeviceRemoval(int index);
	void UpdateGamepadInput();

private: 
	Gamepad pads[MAX_GAMEPAD];
};

#endif //GAMEPAD_INPUT_H