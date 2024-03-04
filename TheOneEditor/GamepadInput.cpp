#include "App.h"
#include "GamepadInput.h"
#include "Gui.h"
#include "Window.h"
#include "SceneManager.h"

#include "../TheOneEngine/Log.h"
#include <iostream>

GamepadInput::GamepadInput(App* app) : Module(app)
{
	memset(&pads[0], 0, sizeof(Gamepad) * MAX_GAMEPAD);
}

GamepadInput::~GamepadInput()
{
	delete[] pads;
}

bool GamepadInput::Awake()
{
	bool ret = true;
	if (SDL_InitSubSystem(SDL_INIT_EVENTS) == 0)
	{
		LOG(LogType::LOG_OK, "-Init SDL gamepad event system");
	}
	else if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
    {
        LOG(LogType::LOG_ERROR, "SDL_INIT_GAMECONTROLLER could not initialize! SDL_Error: %s\n", SDL_GetError());
        ret = false;
    }

    return ret;
}

bool GamepadInput::PreUpdate()
{
    bool ret = true;

	UpdateGamepadInput();

    return ret;
}

bool GamepadInput::Update(double dt)
{
    //Placeholder
    bool ret = true;

	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
		case(SDL_CONTROLLERDEVICEADDED):
		{
			HandleDeviceConnection(event.cdevice.which);
			break;
		}
		case(SDL_CONTROLLERDEVICEREMOVED):
		{
			HandleDeviceRemoval(event.cdevice.which);
			break;
		}
		case(SDL_QUIT):
		{
			return false;
			break;
		}
		}
	}

    return ret;
}

void GamepadInput::HandleDeviceConnection(int index)
{
	LOG(LogType::LOG_OK, "gamepad connected");
	if (SDL_IsGameController(index))
	{
		for (int i = 0; i < MAX_GAMEPAD; ++i)
		{
			Gamepad& pad = pads[i];

			if (pad.enabled == false)
			{
				if (pad.controller = SDL_GameControllerOpen(index))
				{
					pad.enabled = true;
					pad.left_dz = pad.right_dz = 0.1f;
					pad.index = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pad.controller));
				}
			}
		}
	}
}

void GamepadInput::HandleDeviceRemoval(int index)
{
	LOG(LogType::LOG_WARNING, "gamepad disconnected");
	for (int i = 0; i < MAX_GAMEPAD; ++i)
	{
		Gamepad& pad = pads[i];
		if (pad.enabled && pad.index == index)
		{
			SDL_GameControllerClose(pad.controller);
			memset(&pad, 0, sizeof(Gamepad));
		}
	}
}

void GamepadInput::UpdateGamepadInput()
{
    //Placeholder
    //All active gamepads Update inputs here
    for (int i = 0; i < MAX_GAMEPAD; ++i)
    {
        Gamepad& pad = pads[i];

        if(pad.enabled)
        {
			pad.a = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_A) == 1;
			pad.b = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_B) == 1;
			pad.x = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_X) == 1;
			pad.y = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_Y) == 1;
			pad.l1 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == 1;
			pad.r1 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == 1;
			pad.l3 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_LEFTSTICK) == 1;
			pad.r3 = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK) == 1;
			pad.up = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_UP) == 1;
			pad.down = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1;
			pad.left = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1;
			pad.right = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1;

			pad.start = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_START) == 1;
			pad.guide = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_GUIDE) == 1;
			pad.back = SDL_GameControllerGetButton(pad.controller, SDL_CONTROLLER_BUTTON_BACK) == 1;

			pad.l2 = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) / 32767.0f;
			pad.r2 = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) / 32767.0f;

			pad.left_x = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_LEFTX)) / 32767.0f;
			pad.left_y = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_LEFTY)) / 32767.0f;
			pad.right_x = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_RIGHTX)) / 32767.0f;
			pad.right_y = float(SDL_GameControllerGetAxis(pad.controller, SDL_CONTROLLER_AXIS_RIGHTY)) / 32767.0f;

			pad.left_x = (fabsf(pad.left_x) > pad.left_dz) ? pad.left_x : 0.0f;
			pad.left_y = (fabsf(pad.left_y) > pad.left_dz) ? pad.left_y : 0.0f;
			pad.right_x = (fabsf(pad.right_x) > pad.right_dz) ? pad.right_x : 0.0f;
			pad.right_y = (fabsf(pad.right_y) > pad.right_dz) ? pad.right_y : 0.0f;
        }
    }
}