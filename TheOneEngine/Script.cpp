#include "Script.h"
#include "MonoManager.h"
#include "GameObject.h"

Script::Script(std::shared_ptr<GameObject> containerGO, std::string name) : Component(containerGO, ComponentType::Script), scriptName(name)
{
	//Init things
	monoBehaviourInstance = MonoManager::InstantiateClass(name.c_str(), containerGO->GetUID());
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
