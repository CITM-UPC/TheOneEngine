#include "MonoRegisterer.h"
#include "MonoManager.h"
#include "Defs.h"

#include "Transform.h"

#include "..\mono\include\mono\jit\jit.h"
#include "..\mono\include\mono\metadata\assembly.h"
#include "..\mono\include\mono\metadata\attrdefs.h"

#include <glm/vec3.hpp>

//Constructors
static GameObject* GetGameObjectPtr()
{
	return MonoManager::GetCurrentGameObjectPtr();
}

//Transform
static void GetTransform(GameObject* GOptr, vec3f* position)
{
	//	WARNING

	//This function has been implemented as a PLACEHOLDER due to the Transform
	//component being under rework at the time of this implementation. This
	//currently works thanks to the new tracking system through pointers instead
	//of UIDs, but still MUST be updated AS SOON as the Transform rework is finished.

	*position = (vec3f)GOptr->GetComponent<Transform>()->position;
}
static void SetTransform(GameObject* GOptr, vec3f* position)
{
	//	WARNING

	//This function has been implemented as a PLACEHOLDER due to the Transform
	//component being under rework at the time of this implementation. This
	//currently works thanks to the new tracking system through pointers instead
	//of UIDs, but still MUST be updated AS SOON as the Transform rework is finished.

	GOptr->GetComponent<Transform>()->position = vec3(*position);
}

static vec3f GetTransformForward(GameObject* GOptr)
{
	//	WARNING

	//This function has been implemented as a PLACEHOLDER due to the Transform
	//component being under rework at the time of this implementation. This
	//currently works thanks to the new tracking system through pointers instead
	//of UIDs, but still MUST be updated AS SOON as the Transform rework is finished.

	 return (vec3f)GOptr->GetComponent<Transform>()->getForward();
}

static void PrintCSharpPtrInfo(GameObject* GOptr) //Debug purposes only
{
	std::cout << "Recieved pointer to " << GOptr->GetName() << " from C# is: " << GOptr << std::endl;
}

void MonoRegisterer::RegisterFunctions()
{
	mono_add_internal_call("InternalCalls::GetGameObjectPtr", GetGameObjectPtr);
	mono_add_internal_call("InternalCalls::GetTransform", GetTransform);
	mono_add_internal_call("InternalCalls::SetTransform", SetTransform);
	mono_add_internal_call("InternalCalls::GetTransformForward", GetTransformForward);
	mono_add_internal_call("InternalCalls::PrintCSharpPtrInfo", PrintCSharpPtrInfo);
}
