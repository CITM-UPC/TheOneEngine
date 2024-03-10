#include "GamepadInput.h"

std::unordered_map<int, std::unique_ptr<GamepadInput::Controller>> GamepadInput::availableGamepads;

void GamepadInput::GamepadConnected(SDL_ControllerDeviceEvent& event)
{

}

void GamepadInput::GamepadDisconnected(SDL_ControllerDeviceEvent& event)
{

}

void GamepadInput::Shutdown()
{

}

void GamepadInput::Update()
{

}