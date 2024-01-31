#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "ComponentScript.h"
#include "UIDGen.h"
#include "../TheOneEditor/SceneManager.h"

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
	// hekbas - shared_from_this() must NOT be called in the constructor!!!
	// uncomenting the following line causes undefined behaviour
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
template<typename TComponent>
std::vector<TComponent*> GameObject::GetAllComponents() {
	static_assert(std::is_base_of<Component, TComponent>::value, "TComponent must inherit from Component");
	std::vector<TComponent>* ret;

	for (const auto& component : components) {
		if (dynamic_cast<TComponent*>(component.get()))
			ret->push_back(static_cast<TComponent*>(component.get()));
	}

	return ret;
}

template<typename TComponent>
bool GameObject::AddComponent() {
	static_assert(std::is_base_of<Component, TComponent>::value, "TComponent must inherit from Component");
	std::unique_ptr<Component> new_component = std::make_unique<TComponent>(shared_from_this());
	if (new_component->IsUnique()) {
		Component* component = this->GetComponent<TComponent>();

		// Check for already existing Component
		if (component != nullptr) {
			LOG(LogType::LOG_WARNING, "Component already applied");
			LOG(LogType::LOG_INFO, "-GameObject [Name: %s] ", name.data());
			LOG(LogType::LOG_INFO, "-Component  [Type: %s] ", component->GetName().data());

			new_component.reset();
			return false;
		}
	}

	new_component->Enable(); // hekbas: Enable the component if necessary?
	components.push_back(std::move(new_component));

	return true;
}

void GameObject::AddScriptComponent(const char* path) {
	std::unique_ptr<Component> new_component = std::make_unique<ComponentScript>(shared_from_this(), path);
	new_component->Enable(); 
	components.push_back(std::move(new_component));

}

// Only for Components of unique type
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

void GameObject::RemoveComponent(uint UID) {
	for (auto it = components.begin(); it != components.end(); ++it) {
		if ((*it)->GetUID() == UID) {
			it = components.erase(it);
			break;
		}
	}
}

//std::vector<std::shared_ptr<Component>>& GameObject::GetComponents()
//{
//	return components;
//}


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

void GameObject::Delete()
{


	for (const auto& component : components)
		component.get_deleter();

	for (const auto& child : children)
		child.~shared_ptr();
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

void GameObject::CreateUID()
{
	UID = UIDGen::GenerateUID();
}

json GameObject::SaveGameObject()
{
	json gameObjectJSON;

	if (auto pGO = parent.lock())
	{
		gameObjectJSON["ParentUID"] = pGO.get()->UID;
	}

	gameObjectJSON["UID"] = UID;
	gameObjectJSON["Name"] = name;
	gameObjectJSON["Static"] = isStatic;
	gameObjectJSON["Enabled"] = enabled;

	if (!components.empty())
	{
		json componentsJSON;

		for (const auto& component : components)
		{
			componentsJSON.push_back(component.get()->SaveComponent());
		}
		gameObjectJSON["Components"] = componentsJSON;
	}

	if (!children.empty())
	{
		json childrenGOJSON;

		for (const auto& go : children)
		{
			childrenGOJSON.push_back(go.get()->SaveGameObject());
		}
		gameObjectJSON["GameObjects"] = childrenGOJSON;
	}

	return gameObjectJSON;
}

void GameObject::LoadGameObject(const json& gameObjectJSON)
{
	// Load basic properties
	if (gameObjectJSON.contains("UID"))
	{
		UID = gameObjectJSON["UID"];
	}

	if (gameObjectJSON.contains("Name"))
	{
		name = gameObjectJSON["Name"];
	}

	if (gameObjectJSON.contains("Static"))
	{
		isStatic = gameObjectJSON["Static"];
	}

	if (gameObjectJSON.contains("Enabled"))
	{
		enabled = gameObjectJSON["Enabled"];
	}

	// Load components
	if (gameObjectJSON.contains("Components"))
	{
		const json& componentsJSON = gameObjectJSON["Components"];

		for (const auto& componentJSON : componentsJSON)
		{
			// TODO: Load Scripts
			// Assuming each component has a LoadComponent function
			if (componentJSON["Type"] == ComponentType::Transform)
			{
				this->AddComponent<Transform>();
				this->GetComponent<Transform>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == ComponentType::Camera)
			{
				this->AddComponent<Camera>();
				this->GetComponent<Camera>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == ComponentType::Mesh)
			{
				this->AddComponent<Mesh>();
				this->GetComponent<Mesh>()->LoadComponent(componentJSON);
			}
		}
	}

	// Load child game objects
	if (gameObjectJSON.contains("GameObjects"))
	{
		const json& childrenGOJSON = gameObjectJSON["GameObjects"];

		for (const auto& childJSON : childrenGOJSON)
		{
			auto childGameObject = std::make_shared<GameObject>("Empty GO");
			childGameObject->LoadGameObject(childJSON);

			// Add the loaded child game object to the current game object
			childGameObject.get()->parent = this->weak_from_this().lock();
			this->children.emplace_back(childGameObject);
		}
	}
}