#include "Collider2D.h"
#include "Defs.h"

#include "GameObject.h"
#include "Transform.h"


Collider2D::Collider2D(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Collider2D)
{
    colliderType = ColliderType::Circle;
}

Collider2D::Collider2D(std::shared_ptr<GameObject> containerGO, ColliderType colliderType) :
    Component(containerGO, ComponentType::Collider2D),
    colliderType(colliderType)
{}

Collider2D::~Collider2D() {}


json Collider2D::SaveComponent()
{
    json cameraJSON;

    cameraJSON["Name"] = name;
    cameraJSON["Type"] = type;

    if (auto pGO = containerGO.lock())
        cameraJSON["ParentUID"] = pGO.get()->GetUID();

    return cameraJSON;
}

void Collider2D::LoadComponent(const json& cameraJSON)
{
    // Load basic properties
    if (cameraJSON.contains("UID")) UID = cameraJSON["UID"];
    if (cameraJSON.contains("Name")) name = cameraJSON["Name"];

    // Load parent UID and set parent
    /*if (cameraJSON.contains("ParentUID"))
    {
        uint32_t parentUID = cameraJSON["ParentUID"];
        if (auto parentGameObject = SceneManager::GetInstance().FindGOByUID(parentUID))
        {
            containerGO = parentGameObject;
        }
    }*/
}