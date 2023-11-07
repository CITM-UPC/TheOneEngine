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
    ~GameObject();

    void Update(double dt);
    void Draw();

    // Components
    template <typename TComponent>
    std::shared_ptr<TComponent> GetComponent()
    {
        for (const auto& component : components)
        {
            if (component && dynamic_cast<TComponent*>(component.get()))
                return std::static_pointer_cast<TComponent>(component);
        }

        return nullptr;
    }

    std::shared_ptr<Component> AddComponent(ComponentType type, int index = -1);
    void RemoveComponent(ComponentType type);
    std::shared_ptr<Component> HasComponent(ComponentType type) const;
    std::vector<std::shared_ptr<Component>>& GetComponents();

    // Get/Set
    bool IsEnabled() const;
    void Enable();
    void Disable();

    std::string GetName() const;
    void SetName(const std::string& name);

    bool IsStatic() const;
    void SetStatic(bool isStatic);

private:
    std::string name;
    std::weak_ptr<GameObject> parent;
    std::vector<std::shared_ptr<GameObject>> children;
    std::vector<std::shared_ptr<Component>> components;
    bool enabled;
    bool isStatic;
    int index;
};

#endif // !__GAME_OBJECT_H__