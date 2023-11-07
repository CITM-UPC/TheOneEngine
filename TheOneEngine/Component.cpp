#include "Component.h"
#include "GameObject.h"


Component::Component(std::shared_ptr<GameObject> containerGO, ComponentType type)
	: containerGO(containerGO),
	type(type),
	enabled(true)
{}

Component::~Component()
{
	Disable();
}

void Component::Enable()
{
	enabled = true;
}

void Component::Disable()
{
	enabled = false;
}

bool Component::IsEnabled() const
{
	return enabled;
}

ComponentType Component::GetType() const
{
	return type;
}

std::shared_ptr<GameObject> Component::GetContainerGameObject() const
{
	return containerGO;
}