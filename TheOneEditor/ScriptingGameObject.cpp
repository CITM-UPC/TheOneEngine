#include "ScriptingGameObject.h"
#include "App.h"
#include "ModuleScripting.h"
#include "../TheOneEngine/luadefs.h"
#include "../TheOneEngine/ScriptData.h"
#include "../TheOneEngine/ComponentScript.h"
#include "../TheOneEngine/GameObject.h"


unsigned int ScriptingGameObject::GetMyUID() const {
	ComponentScript* component = app->scripting->GetCurrentScript().lock()->owner;
	return component->GetContainerGO()->GetUID();
}
