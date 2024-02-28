#include "Listener.h"

Listener::Listener(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Listener), AudioObject(containerGO)
{
	SetTransform(containerGO);
}

Listener::~Listener()
{

}

void Listener::SetListener(std::string name)
{
	// JULS: Need to change it as I shouldn't access from the app in the editor
	//app->engine->audio->SetDefaultListener(app->engine->audio->RegisterGameObject(name))
}

json Listener::SaveComponent()
{
	return json();
}

void Listener::LoadComponent(const json& transformJSON)
{
}
