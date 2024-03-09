#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__
#pragma once

#include "Defs.h"
#include "Component.h"
#include "BBox.hpp"
#include "Script.h"
#include "Log.h"

#include <string>
#include <vector>
#include <list>
#include <memory>

enum class DrawMode
{
    GAME,
    EDITOR
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:

    GameObject(std::string name = "gameObject");
    ~GameObject();

    void Update(double dt);
    void Draw();
    void DrawUI(const DrawMode mode);


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

    template <typename TComponent>
    bool AddCopiedComponent(TComponent* ref)
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

        std::unique_ptr<Component> newComponent = std::make_unique<TComponent>(shared_from_this(), ref);
        newComponent->Enable(); // hekbas: Enable the component if necessary?
        components.push_back(std::move(newComponent));

        return true;
    }
    
    bool AddScript(std::string name)
    {
        Component* component = this->GetComponent<Script>();

        // Check for already existing Component
        if (component != nullptr && this->GetComponent<Script>()->scriptName == name)
        {
            LOG(LogType::LOG_WARNING, "Component already applied");
            LOG(LogType::LOG_INFO, "-GameObject [Name: %s] ", name.data());
            LOG(LogType::LOG_INFO, "-Component  [Type: Script] ");

            return false;
        }

        std::unique_ptr<Component> newComponent = std::make_unique<Script>(shared_from_this(), name);
        newComponent->Enable(); // hekbas: Enable the component if necessary?
        components.push_back(std::move(newComponent));

        return true;
    }

    void RemoveComponent(ComponentType type);

    std::vector<Component*> GetAllComponents(bool tunometecabrasalamambiche = true);

    // AABB
    void GenerateAABBFromMesh();
    AABBox CalculateAABB();
    AABBox CalculateAABBWithChildren();
    void DrawAABB();
    void DrawOBB();


    // Get/Set
    bool IsEnabled() const;
    void Enable();
    void Disable();

    void Delete();

    std::string GetName() const;
    void SetName(const std::string& name);

    bool IsStatic() const;
    void SetStatic(bool isStatic);
    bool HasCameraComponent();

    void CreateUID();
    uint32 GetUID() { return UID; }

    json SaveGameObject();
    void LoadGameObject(const json& gameObjectJSON);

public:
    std::weak_ptr<GameObject> parent;
    std::vector<std::shared_ptr<GameObject>> children;
    bool isStatic;

    bool hasAudioObject = false;
    int audioOjectID = -1;

private:
    std::string name;
    std::vector<std::unique_ptr<Component>> components;
    uint32_t UID;
    bool enabled;
    AABBox aabb;
};

#endif // !__GAME_OBJECT_H__