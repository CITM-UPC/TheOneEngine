#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__
#pragma once

#include "Component.h"
//#include "Transform.h"
//#include "AssetMesh.h"
//#include "Texture.h"

#include <string>
#include <vector>
#include <list>
#include <memory>


class GameObject
{
public:

	GameObject(std::string name);
	virtual ~GameObject();

	void Update(float dt);
	void Draw();

	void Enable();
	void Disable();
	bool isEnabled() const;

	// Components
	template <typename TComponent>
	TComponent* GetComponent()
	{
		for (const auto& component : components)
		{
			if (component && dynamic_cast<TComponent*>(component))
				return static_cast<TComponent*>(component);
		}			

		return nullptr;
	}

	Component* AddComponent(ComponentType type, int index = -1);
	void RemoveComponent(ComponentType type);
	Component* HasComponent(ComponentType type) const;
	std::vector<Component*>& GetComponents();

	// Get/Set
	std::string	GetName() const;
	void SetName(const char* name);

	void SetStatic(bool isStatic);
	bool IsStatic() const;

private:

	std::string name;
	GameObject* parent;
	std::vector<GameObject*> childs;
	std::vector<Component*> components;
	bool enabled;
	bool isStatic;
	int index;	
};

#endif // !__GAME_OBJECT_H__