#include "MonoRegisterer.h"
#include "MonoManager.h"
#include "Defs.h"

#include "EngineCore.h"
#include "Transform.h"
#include "N_SceneManager.h"

#include "../mono/include/mono/jit/jit.h"
#include "../mono/include/mono/metadata/assembly.h"
#include "../mono/include/mono/metadata/attrdefs.h"

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
}
