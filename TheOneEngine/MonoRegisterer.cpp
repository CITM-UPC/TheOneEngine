#include "MonoRegisterer.h"
#include "MonoManager.h"

#include "..\mono\include\mono\jit\jit.h"
#include "..\mono\include\mono\metadata\assembly.h"
#include "..\mono\include\mono\metadata\attrdefs.h"

#include <glm/vec3.hpp>

//Constructors
static void GetUID(unsigned long* uuid)
{
	*uuid = MonoManager::GetCurrentUID();
}

//Transform
static void GetTransform(unsigned long uuid, glm::vec3* position)
{
	//	WARNING

	//This function is left pending to implement due to the current Transform and
	//access to the Game Objects through the SceneManager being under rework.
	//This are now just a placeholder, but MUST be implemented AS SOON as the
	//Transform and SceneManager reworks are finished.
}
static void SetTransform(unsigned long uuid, glm::vec3* position)
{
	//	WARNING

	//This function is left pending to implement due to the current Transform and
	//access to the Game Objects through the SceneManager being under rework.
	//This are now just a placeholder, but MUST be implemented AS SOON as the
	//Transform and SceneManager reworks are finished.
}

void MonoRegisterer::RegisterFunctions()
{
	mono_add_internal_call("InternalCalls::GetUID", GetUID);
	mono_add_internal_call("InternalCalls::GetTransform", GetTransform);
	mono_add_internal_call("InternalCalls::SetTransform", SetTransform);
}
