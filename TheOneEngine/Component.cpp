#include "Component.h"
#include "GameObject.h"


Component::Component(GameObject* ContainerGO, ComponentType type)
	: GO(ContainerGO),
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

bool Component::isEnabled() const
{
	return enabled;
}

ComponentType Component::GetType() const
{
	return type;
}

GameObject* Component::GetContainerGameObject() const
{
	return GO;
}