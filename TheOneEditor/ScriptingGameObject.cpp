#include "ScriptingGameObject.h"
#include "luadefs.h"
#include "App.h"
#include "ModuleScripting.h"
#include "ScriptData.h"
#include "../TheOneEngine/ComponentScript.h"
#include "../TheOneEngine/GameObject.h"


unsigned int ScriptingGameObject::GetMyUID() {
	ComponentScript* component = app->scripting->GetCurrentScript()->owner;
	return component->GetContainerGO()->GetUID();
}
