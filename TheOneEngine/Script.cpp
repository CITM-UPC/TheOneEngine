#include "Script.h"

Script::Script(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Mesh)
{
	//Init things
	//monoBehaviourInstance = ;
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
