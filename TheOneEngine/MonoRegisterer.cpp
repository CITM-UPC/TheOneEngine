#include "MonoRegisterer.h"
#include "MonoManager.h"
#include "Defs.h"

#include "Transform.h"

#include "../mono/include/mono/jit/jit.h"
#include "../mono/include/mono/metadata/assembly.h"
#include "../mono/include/mono/metadata/attrdefs.h"

#include <glm/vec3.hpp>

//Constructors
static GameObject* GetGameObjectPtr()
{
	return MonoManager::GetCurrentGameObjectPtr();
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

static void Translate(GameObject* GOptr, vec3f* increment)
{
	GOptr->GetComponent<Transform>()->Translate((vec3)*increment, HandleSpace::GLOBAL);
}

static vec3f GetTransformForward(GameObject* GOptr)
{
	return (vec3f)GOptr->GetComponent<Transform>()->GetForward();
}

void MonoRegisterer::RegisterFunctions()
{
	mono_add_internal_call("InternalCalls::GetGameObjectPtr", GetGameObjectPtr);

	mono_add_internal_call("InternalCalls::GetPosition", GetPosition);
	mono_add_internal_call("InternalCalls::SetPosition", SetPosition);
	mono_add_internal_call("InternalCalls::Translate", Translate);
	mono_add_internal_call("InternalCalls::GetTransformForward", GetTransformForward);
}
