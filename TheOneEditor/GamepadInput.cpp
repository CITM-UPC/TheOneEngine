#include "App.h"
#include "Input.h"
#include "Gui.h"
#include "Window.h"
#include "SceneManager.h"
#include "GamepadInput.h"

#include "Log.h"
#include <string>
#include <iostream>
#include <filesystem>

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
    LOG(LogType::LOG_INFO, "Init SDL gamepad event system");
    bool ret = true;

    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
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

    return ret;
}

void GamepadInput::UpdateGamepadInput()
{
    //Placeholder
    //All active gamepads Update inputs here
}