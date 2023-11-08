#include "Component.h"
#include "GameObject.h"


Component::Component(std::shared_ptr<GameObject> containerGO, ComponentType type) :
	type(type),
	enabled(true)
{
	std::weak_ptr<GameObject> weakPtr = containerGO;
	this->containerGO = weakPtr;
}

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

std::weak_ptr<GameObject> Component::GetContainerGameObject() const
{
	return containerGO;
}