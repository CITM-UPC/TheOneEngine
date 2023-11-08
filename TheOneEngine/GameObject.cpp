#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

#include "..\TheOneEditor\Log.h"

#include "Math.h"


GameObject::GameObject(std::string name)
	: name(name),
	parent(),
	children(),
	components(),
	enabled(true),
	isStatic(false),
	index(-1)
{
	//hekbas - shared_from_this() should not be called in the constructor!!!
	//AddComponent(ComponentType::Transform);
	Enable();
}

GameObject::~GameObject() {}

void GameObject::Update(double dt)
{
	// Update enabled Components
	for (const auto& component : components)
	{
		if (component && component->IsEnabled())
			component->Update();
	}

	// Update childs
	for (const auto& child : children)
		child->Update(dt);
}

void GameObject::Draw()
{
	for (const auto& component : components)
	{
		if (component && component->IsEnabled())
			component->DrawComponent();
	}
}

// Component ----------------------------------------
std::shared_ptr<Component> GameObject::AddComponent(ComponentType type, int index)
{
	std::shared_ptr<Component> component = nullptr;

	// Check for already existing component of given type
	if (HasComponent(type) == nullptr)
	{
		switch (type)
		{
			case ComponentType::Transform:
				component = std::make_shared<Transform>(shared_from_this());
				break;
			case ComponentType::Camera:
				component = std::make_shared<Camera>(shared_from_this());
				break;
			/*case ComponentType::Mesh:
				component = std::make_shared<Mesh>(shared_from_this());
				break;*/
		}

		if (component)
		{
			if (index == -1)
			{
				components.push_back(component);
			}
			else if (index >= 0 && index < static_cast<int>(components.size()))
			{
				components.insert(components.begin() + index, component);
			}
			// Hekbas: Handle cases where index is out of range
			// ...

			// Enable the component if necessary?
			component->Enable();
		}
		else
		{
			LOG(LogType::LOG_ERROR, "Unable to add Component");
		}
	}
	else
	{
		std::shared_ptr<GameObject> parentSharedPtr = parent.lock();
		component = HasComponent(type);

		LOG(LogType::LOG_WARNING, "Component already applied");
		LOG(LogType::LOG_INFO, "-GameObject [Name: %s] ", parentSharedPtr.get()->GetName());
		LOG(LogType::LOG_INFO, "-Component  [Type: %s] ", component.get()->GetType());		
	}

	return component;
}

void GameObject::RemoveComponent(ComponentType type)
{
	for (auto it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			it = components.erase(it);
			break;
		}
	}
}

std::shared_ptr<Component> GameObject::HasComponent(ComponentType type) const
{
	// Search the components vector for a component of the specified type
	for (const auto& component : components)
	{
		if (component && component->GetType() == type)
			return component;
	}

	// If no matching component is found, return nullptr
	return std::shared_ptr<Component>();
}

std::vector<std::shared_ptr<Component>>& GameObject::GetComponents()
{
	return components;
}


// Get/Set ------------------------------------------
bool GameObject::IsEnabled() const
{
	return enabled;
}

void GameObject::Enable()
{
	enabled = true;

	for (const auto& component : components)
		component->Enable();

	for (const auto& child : children)
		child->Enable();
}

void GameObject::Disable()
{
	enabled = false;

	for (const auto& component : components)
		component->Disable();

	for (const auto& child : children)
		child->Disable();
}

std::string GameObject::GetName() const
{
	return name;
}

void GameObject::SetName(const std::string& newName)
{
	name = newName;
}

bool GameObject::IsStatic() const
{
	return isStatic;
}

void GameObject::SetStatic(bool staticFlag)
{
	isStatic = staticFlag;
}