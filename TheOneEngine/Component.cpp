#include "Component.h"
#include "GameObject.h"


Component::Component(std::shared_ptr<GameObject> containerGO, ComponentType type) :
	type(type),
	enabled(true)
{
	std::weak_ptr<GameObject> weakPtr = containerGO;
	this->containerGO = weakPtr;

	CreateNameFromType(type);
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

std::string Component::GetName()
{
	return name;
}

void Component::CreateNameFromType(ComponentType type)
{
	switch (type)
	{
		case ComponentType::Transform: name = "Transform"; break;
		case ComponentType::Camera: name = "Camera"; break;
		case ComponentType::Mesh: name = "Mesh"; break;
		case ComponentType::MeshRenderer: name = "MeshRenderer"; break;
		case ComponentType::Unknown: name = "Unknown"; break;
		default: name = "Specified Type was not found"; break;
	}
}

ComponentType Component::GetType() const
{
	return type;
}

std::weak_ptr<GameObject> Component::GetContainerGameObject() const
{
	return containerGO;
}