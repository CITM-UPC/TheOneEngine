#include "Script.h"
#include "MonoManager.h"
#include "GameObject.h"

Script::Script(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Mesh)
{
	//Init things
	monoBehaviourInstance = MonoManager::InstantiateClass("Class1", containerGO->GetUID());

	//MonoManager::CallScriptFunction(monoBehaviourInstance, "PrintFloatVar");
}

Script::~Script()
{
}

void Script::DrawComponent()
{
}

json Script::SaveComponent()
{
	return json();
}

void Script::LoadComponent(const json& scriptJSON)
{
}
