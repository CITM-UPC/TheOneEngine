#include "MonoRegisterer.h"
#include "MonoManager.h"
#include "Defs.h"

#include "EngineCore.h"
#include "Transform.h"
#include "Canvas.h"
#include "N_SceneManager.h"

#include <glm/vec3.hpp>

//Constructors
static GameObject* GetGameObjectPtr()
{
	return MonoManager::GetCurrentGameObjectPtr();
}

//Input
static bool GetKeyboardButton(int id)
{
	return engine->inputManager->GetKey(id) == KEY_REPEAT;
}

static bool GetControllerButton(int controllerButton, int gamePad)
{
	auto inputToPass = (SDL_GameControllerButton)controllerButton;

	auto result = engine->inputManager->GetGamepadButton(gamePad, inputToPass);

	if (result == InputManagerNamespace::KEY_IDLE)
	{
		LOG(LogType::LOG_WARNING, "Button %i is idle", controllerButton);
	}

	return result == InputManagerNamespace::KEY_DOWN;
}
static void GetControllerJoystick(int joystick, vec2f* joyResult, int gamePad)
{
	if (joystick) //value is 1, so it means right
	{
		joyResult->x = engine->inputManager->pads[gamePad].right_x;
		joyResult->y = engine->inputManager->pads[gamePad].right_y;
	}
	else
	{
		joyResult->x = engine->inputManager->pads[gamePad].left_x;
		joyResult->y = engine->inputManager->pads[gamePad].left_y;
	}
}

//Transform
static vec3f GetPosition(GameObject* GOptr)
{
	return (vec3f)GOptr->GetComponent<Transform>()->GetPosition();
}
static void SetPosition(GameObject* GOptr, vec3f* position)
{
	GOptr->GetComponent<Transform>()->SetPosition((vec3)*position);
}

static vec3f GetRotation(GameObject* GOptr)
{
	return (vec3f)GOptr->GetComponent<Transform>()->GetRotationEuler();
}
static void SetRotation(GameObject* GOptr, vec3f* rotation)
{
	GOptr->GetComponent<Transform>()->SetRotation((vec3)*rotation);
}

static void Translate(GameObject* GOptr, vec3f* finalPos)
{
	//This implementation is temporary, engine Transform.Translate is not working properly.

	GOptr->GetComponent<Transform>()->SetPosition((vec3)*finalPos, HandleSpace::GLOBAL);
}

static vec3f GetTransformForward(GameObject* GOptr)
{
	return (vec3f)GOptr->GetComponent<Transform>()->GetForward();
}

//GameObject
static GameObject* InstantiateBullet(vec3f* initialPosition, vec3f* direction)
{
	engine->N_sceneManager->CreateTeapot();
	GameObject* go = engine->N_sceneManager->objectsToAdd.back().get();

	SetPosition(go, initialPosition);
	SetRotation(go, direction);

	go->AddScript("Bullet");

	return go;
}

static void DestroyGameObject(GameObject* objectToDestroy)
{
	objectToDestroy->Delete(engine->N_sceneManager->objectsToDelete);
}

//Helpers
static float GetAppDeltaTime()
{
	return (float)engine->dt;
}
static void ExitApplication()
{
	engine->inputManager->shutDownEngine = true;
}

static void LoadScene(MonoString* sceneName)
{
	std::string name = MonoRegisterer::MonoStringToUTF8(sceneName);

	engine->N_sceneManager->LoadScene(name);
}

static void MoveSelectedButton(GameObject* containerGO, int direction)
{
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();

	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::BUTTONIMAGE && uiElements[i]->GetState() == UiState::HOVERED)
		{
			for (int j = i + direction; j != i; j += direction)
			{
				if (j < 0)
					j = uiElements.size() - 1;
				else if (j >= uiElements.size())
					j = 0;

				if (uiElements[j]->GetType() == UiType::BUTTONIMAGE)
				{
					uiElements[i]->SetState(UiState::IDLE);
					uiElements[j]->SetState(UiState::HOVERED);
					break;
				}
			}
			break;
		}
	}
}

static int GetSelectiedButton(GameObject* containerGO)
{
	std::vector<ItemUI*> uiElements = containerGO->GetComponent<Canvas>()->GetUiElements();
	int ret = -1;
	for (size_t i = 0; i < uiElements.size(); i++)
	{
		if (uiElements[i]->GetType() == UiType::BUTTONIMAGE)
		{
			ret++;
			if (uiElements[i]->GetState() == UiState::HOVERED)
				return ret;
		}
	}
	return ret;
}

void MonoRegisterer::RegisterFunctions()
{
	mono_add_internal_call("InternalCalls::GetGameObjectPtr", GetGameObjectPtr);

	mono_add_internal_call("InternalCalls::GetKeyboardButton", GetKeyboardButton);
	mono_add_internal_call("InternalCalls::GetControllerButton", GetControllerButton);
	mono_add_internal_call("InternalCalls::GetControllerJoystick", GetControllerJoystick);

	mono_add_internal_call("InternalCalls::GetPosition", GetPosition);
	mono_add_internal_call("InternalCalls::SetPosition", SetPosition);
	mono_add_internal_call("InternalCalls::GetRotation", GetRotation);
	mono_add_internal_call("InternalCalls::SetRotation", SetRotation);
	mono_add_internal_call("InternalCalls::Translate", Translate);
	mono_add_internal_call("InternalCalls::GetTransformForward", GetTransformForward);

	mono_add_internal_call("InternalCalls::InstantiateBullet", InstantiateBullet);
	mono_add_internal_call("InternalCalls::DestroyGameObject", DestroyGameObject);

	mono_add_internal_call("InternalCalls::GetAppDeltaTime", GetAppDeltaTime);
	mono_add_internal_call("InternalCalls::ExitApplication", ExitApplication);

	mono_add_internal_call("InternalCalls::LoadScene", LoadScene);

	mono_add_internal_call("InternalCalls::MoveSelectedButton", MoveSelectedButton);
	mono_add_internal_call("InternalCalls::GetSelectiedButton", GetSelectiedButton);
}

bool MonoRegisterer::CheckMonoError(MonoError& error)
{
	bool hasError = !mono_error_ok(&error);
	if (hasError)
	{
		unsigned short errorCode = mono_error_get_error_code(&error);
		const char* errorMessage = mono_error_get_message(&error);
		std::cout << "Mono Error!" << std::endl;
		std::cout << "Error Code: " << errorCode << std::endl;
		std::cout << "Error Message: " << errorMessage << std::endl;
		mono_error_cleanup(&error);
	}
	return hasError;
}

std::string MonoRegisterer::MonoStringToUTF8(MonoString* monoString)
{
	if (monoString == nullptr || mono_string_length(monoString) == 0)
		return "";

	MonoError error;
	char* utf8 = mono_string_to_utf8_checked(monoString, &error);
	if (CheckMonoError(error))
		return "";
	std::string result(utf8);
	mono_free(utf8);
	return result;
}