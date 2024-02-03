#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__
#pragma once

#include "Defs.h"
#include "Component.h"

#include "..\TheOneEditor\Log.h"

#include <string>
#include <vector>
#include <list>
#include <memory>

class ComponentScript;

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:

    GameObject(std::string name = "gameObject");
    ~GameObject();

    void Update(double dt);
    void UpdateTransform(mat4 parent_matrix = mat4(1.0f), bool dirty = false);
    void Draw();

    // Components
    template <typename TComponent>
    TComponent* GetComponent()
    {
        for (const auto& component : components)
        {
            if (dynamic_cast<TComponent*>(component.get()))
                return static_cast<TComponent*>(component.get());
        }
        return nullptr;
    }

    std::vector<Component*> GetAllComponents();

    template <typename TComponent>
    Component* AddComponent();

    ComponentScript* AddScriptComponent(const char* path);

    void RemoveComponent(ComponentType type);
    void RemoveComponent(uint UID);

    // Get/Set
    bool IsEnabled() const;
    void Enable();
    void Disable();

    void Delete();

    std::string GetName() const;
    void SetName(const std::string& name);

    bool IsStatic() const;
    void SetStatic(bool isStatic);

    void CreateUID();
    uint32 GetUID() { return UID; }

    json SaveGameObject();
    void LoadGameObject(const json& gameObjectJSON);

public:
    std::weak_ptr<GameObject> parent;
    std::vector<std::shared_ptr<GameObject>> children;
    bool isStatic;

private:
    std::string name;
    std::vector<std::unique_ptr<Component>> components;
    uint32_t UID;
    bool enabled;
    int index;
};

#endif // !__GAME_OBJECT_H__
