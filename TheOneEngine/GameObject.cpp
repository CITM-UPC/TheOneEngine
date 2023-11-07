#include "GameObject.h"
#include "..\TheOneEditor\Log.h"
//#include "Transform.h"
//#include "MeshRenderer.h"
//hekbas add other components...
#include "AssetMesh.h"

#include "Math.h"


GameObject::GameObject(std::string name)
	: name(name),
	parent(nullptr),
	childs(),
	components(),
	enabled(true),
	isStatic(false),
	index(-1)
{
	AddComponent(ComponentType::Transform);
	Enable();
}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if (*it)
			delete* it;
	}
	components.clear();
}

void GameObject::Update(float dt)
{
	/*if (GetComponent<Transform>()->update_transform)
		this->OnUpdateTransform();*/

	// --- Update components ---
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i] && components[i]->isEnabled())
			components[i]->Update();
	}

	// --- Update child game objects
	for (std::vector<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it)
	{
		(*it)->Update(dt);
	}
}

void GameObject::Draw()
{
	for (const auto& component : components)
	{
		if (component && component->isEnabled())
			component->DrawComponent();
	}
}

Component* GameObject::AddComponent(ComponentType type, int index)
{
	Component* component = nullptr;

	// --- Check if there is already a component of the type given ---

	if (HasComponent(type) == nullptr)
	{
		switch (type)
		{
		/*case ComponentType::Transform:
			component = new Transform(this);
			break;
		case ComponentType::Mesh:
			component = new Mesh(this);
			break;*/
		}

		if (component)
		{
			// if index is specified
			if (index == -1)
				components.push_back(component);
			else
			{
				// Resize if needed
				if (index + 1 > components.size())
					components.resize(index + 1);

				// Delete component at index
				if (components[index])
				{
					delete components[index];
					components[index] = nullptr;
				}

				// Insert component at index
				components[index] = component;
			}				
		}
	}
	else
	{
		LOG(LogType::LOG_ERROR, "Component already applied");
		LOG(LogType::LOG_INFO, "-GameObject [Name: %s] ", parent->name);
		LOG(LogType::LOG_INFO, "-Component  [Type: %s] ", component->GetType());
		component = HasComponent(type);
	}

	return component;
}

void GameObject::RemoveComponent(ComponentType type)
{
	for (auto it = components.begin(); it != components.end(); ++it)
	{
		Component* component = *it;

		if (component->GetType() == type)
		{
			delete component;
			it = components.erase(it);
			break;
		}
	}
}

Component* GameObject::HasComponent(ComponentType type) const
{
	// --- Search for given type of component ---
	Component* component = nullptr;

	for (uint i = 0; i < components.size(); ++i)
	{
		if (components[i] && components[i]->GetType() == type)
		{
			component = components[i];
			break;
		}
	}

	return component;
}

std::vector<Component*>& GameObject::GetComponents()
{
	return components;
}

void GameObject::Enable()
{
	enabled = true;

	for (int i = 0; i < components.size(); ++i)
		components[i]->Enable();

	for (int i = 0; i < childs.size(); ++i)
		childs[i]->Enable();
}

void GameObject::Disable()
{
	enabled = false;

	for (int i = 0; i < components.size(); ++i)
		components[i]->Disable();

	for (int i = 0; i < childs.size(); ++i)
		childs[i]->Disable();
}

std::string GameObject::GetName() const
{
	return name;
}

bool GameObject::isEnabled() const
{
	return enabled;
}

void GameObject::SetName(const char* name)
{
	if (name && name != "root")
		this->name = name;
}