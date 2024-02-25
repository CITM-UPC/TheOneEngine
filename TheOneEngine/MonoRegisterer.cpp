#include "MonoRegisterer.h"
#include "MonoManager.h"

#include "..\mono\include\mono\jit\jit.h"
#include "..\mono\include\mono\metadata\assembly.h"
#include "..\mono\include\mono\metadata\attrdefs.h"

static void GetUID(unsigned long* uuid)
{
	*uuid = MonoManager::GetCurrentUID();
}

void MonoRegisterer::RegisterFunctions()
{
	mono_add_internal_call("InternalCalls::GetUID", GetUID);
}
