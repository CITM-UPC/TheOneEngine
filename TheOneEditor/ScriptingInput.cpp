#include "ScriptingInput.h"
#include "App.h"
#include "Input.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_mouse.h"
#include <string>
#include "../TheOneEngine/FileUtils.h"

int MouseButtonFromName(std::string button) {
	Fileutils::NormalizePath(button, true);
	int ret = SDL_BUTTON_X1;
	if (button == "left") ret = SDL_BUTTON_LEFT;
	else if (button == "right") ret = SDL_BUTTON_RIGHT;
	else if (button == "middle") ret = SDL_BUTTON_MIDDLE;

	return ret;
}

int ScriptingInput::GetKeyState(const char* key) const {
	SDL_Scancode code = SDL_GetScancodeFromName(key);
	if (code != SDL_SCANCODE_UNKNOWN) {
		return app->input->GetKey(code);
	}
	else {
		return KEY_UNKNOWN;
	} //TODO: Throw an error maybe
}

bool ScriptingInput::IsKeyDown(const char* key) const {
	return GetKeyState(key) == KEY_DOWN;
}

bool ScriptingInput::IsKeyUp(const char* key) const {
	return GetKeyState(key) == KEY_UP;
}

bool ScriptingInput::IsKeyRepeat(const char* key) const {
	return GetKeyState(key) == KEY_REPEAT;
}

bool ScriptingInput::IsKeyIdle(const char* key) const {
	return GetKeyState(key) == KEY_IDLE;
}

int ScriptingInput::GetMBState(const char* mousebutton) const {
	int mouseid = MouseButtonFromName(mousebutton);
	if (mouseid != SDL_BUTTON_X1) {
		return app->input->GetMouseButton(mouseid);
	}
	else {
		return KEY_UNKNOWN; //TODO: Throw an error maybe?
	}
}

bool ScriptingInput::IsMBDown(const char* mousebutton) const {
	return GetMBState(mousebutton) == KEY_DOWN;
}

bool ScriptingInput::IsMBUp(const char* mousebutton) const {
	return GetMBState(mousebutton) == KEY_UP;
}

bool ScriptingInput::IsMBRepeat(const char* mousebutton) const {
	return GetMBState(mousebutton) == KEY_REPEAT;
}

bool ScriptingInput::ISMBIdle(const char* mousebutton) const {
	return GetMBState(mousebutton) == KEY_IDLE;
}

int ScriptingInput::GetMouseMovementX() const {
	return app->input->GetMouseXMotion();
}

int ScriptingInput::GetMouseMovementY() const {
	return app->input->GetMouseYMotion();
}
