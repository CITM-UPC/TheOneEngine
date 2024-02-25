#include "Listener.h"

Listener::Listener(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Listener), AudioObject(containerGO)
{

}

Listener::~Listener()
{
}

json Listener::SaveComponent()
{
	return json();
}

void Listener::LoadComponent(const json& transformJSON)
{
}
