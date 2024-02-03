#include "ScriptingGameObject.h"
#include "App.h"
#include "ModuleScripting.h"
#include "SceneManager.h"
#include "../TheOneEngine/luadefs.h"
#include "../TheOneEngine/ScriptData.h"
#include "../TheOneEngine/ComponentScript.h"
#include "../TheOneEngine/GameObject.h"


unsigned int ScriptingGameObject::GetMyUID() const {
	ComponentScript* component = app->scripting->GetCurrentScript().lock()->owner;
	return component->GetContainerGO()->GetUID();
}

unsigned int ScriptingGameObject::FindGOByName(std::string name) const {
	std::shared_ptr<GameObject> go = app->sceneManager->FindGOByName(name);
	if (go)
		return go->GetUID();
	else
		return 0; //TODO: Throw some error
}

void ScriptingGameObject::DestroyGameObject(unsigned int go_UUID) {
	app->sceneManager->DestroyGameObject(go_UUID);
}

unsigned int ScriptingGameObject::InstantiateGameObject(unsigned int go_UUID) {
	std::shared_ptr<GameObject> newGO = app->sceneManager->InstantiateGameObject(go_UUID);
	if (newGO)
		return newGO->GetUID();
	else
		return 0;
}
