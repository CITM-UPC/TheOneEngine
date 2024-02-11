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


class GameObject : public std::enable_shared_from_this<GameObject>
{
public:

    GameObject();
    GameObject(std::string name = "gameObject");
    ~GameObject();

    void Update(double dt);
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

    template <typename TComponent>
    bool AddComponent()
    {
        Component* component = this->GetComponent<TComponent>();

        // Check for already existing Component
        if (component != nullptr)
        {
            LOG(LogType::LOG_WARNING, "Component already applied");
            LOG(LogType::LOG_INFO, "-GameObject [Name: %s] ", name.data());
            LOG(LogType::LOG_INFO, "-Component  [Type: %s] ", component->GetName().data());

            return false;
        }

        std::unique_ptr<Component> newComponent = std::make_unique<TComponent>(shared_from_this());
        newComponent->Enable(); // hekbas: Enable the component if necessary?
        components.push_back(std::move(newComponent));

        return true;
    }

    void RemoveComponent(ComponentType type);

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