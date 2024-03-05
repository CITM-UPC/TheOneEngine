#include "BuilderApp.h"
#include "BuilderInput.h"
#include "BuilderWindow.h"
//#include "SceneManager.h"

#include "../TheOneEngine/Log.h"
#include <string>
#include <iostream>
#include <filesystem>

#define MAX_KEYS 300
#define SCREEN_SIZE 1

BuilderInput::BuilderInput(BuilderApp* app) : BuilderModule(app)
{
	keyboard = new B_KEY_STATE[MAX_KEYS];
	memset(keyboard, B_KEY_IDLE, sizeof(B_KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, B_KEY_IDLE, sizeof(B_KEY_STATE) * MAX_MOUSE_BUTTONS);
}

BuilderInput::~BuilderInput()
{
	delete[] keyboard;
}

bool BuilderInput::Awake()
{
	bool ret = true;
	LOG(LogType::LOG_INFO, "# Initializing Input Events...");

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) == 0)
	{
		LOG(LogType::LOG_OK, "-Init SDL Input Event subsystem");
	}
	else
	{
		LOG(LogType::LOG_ERROR, "-SDL Input Event subsystem could not be initialized! %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool BuilderInput::PreUpdate()
{
	bool ret = true;

	ret = processSDLEvents();

	return ret;
}

bool BuilderInput::Update(double dt)
{
	bool ret = true;

	return ret;
}

bool BuilderInput::processSDLEvents()
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == B_KEY_IDLE)
				keyboard[i] = B_KEY_DOWN;
			else
				keyboard[i] = B_KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == B_KEY_REPEAT || keyboard[i] == B_KEY_DOWN)
				keyboard[i] = B_KEY_UP;
			else
				keyboard[i] = B_KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for (int i = 0; i < 5; ++i)
	{
		if (buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == B_KEY_IDLE)
				mouse_buttons[i] = B_KEY_DOWN;
			else
				mouse_buttons[i] = B_KEY_REPEAT;
		}
		else
		{
			if (mouse_buttons[i] == B_KEY_REPEAT || mouse_buttons[i] == B_KEY_DOWN)
				mouse_buttons[i] = B_KEY_UP;
			else
				mouse_buttons[i] = B_KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	static SDL_Event event;

	while (SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
		case SDL_MOUSEWHEEL:
			mouse_z = event.wheel.y;
			break;

		case SDL_MOUSEMOTION:
			mouse_x = event.motion.x / SCREEN_SIZE;
			mouse_y = event.motion.y / SCREEN_SIZE;

			mouse_x_motion = event.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = event.motion.yrel / SCREEN_SIZE;
			break;

		case SDL_QUIT: return false;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE: return false;
			}
			break;

		case SDL_WINDOWEVENT:

			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				app->window->OnResizeWindow(event.window.data1, event.window.data2);
			}
			break;

		}
	}
	return true;
}