#include "Transform.h"
#include "GameObject.h"

Transform::Transform(std::shared_ptr<GameObject> containerGO)
    : Component(containerGO, ComponentType::Transform),
    globalMatrix(1.0f),
    position(0.0f), rotation(1, 0, 0, 0), scale(1.0f), eulerAngles(0,0,0),
    localScale(1.0f), localRotation(1, 0, 0, 0), localEulerAngles(0, 0, 0)
{}

Transform::~Transform() {}


// Transform ----------------------------------------------------
void Transform::translate(const vec3& translation, bool local) 
{
    if (local) {
        position += localRotation * translation;
    }
    else {
        position += rotation * translation;
    }
}

void Transform::rotate(const vec3& axis, double angle, bool local)
{
    glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);

    if (local) {
        localRotation = rotationQuat;
        //localRotation = glm::normalize(localRotation);
        localEulerAngles = glm::eulerAngles(localRotation);
    }
    else {
        rotation = rotationQuat;
        //rotation = glm::normalize(rotation);
        this->eulerAngles = glm::eulerAngles(rotation);
    }
}

void Transform::rotate(const vec3& eulerAngles, bool local)
{
    glm::quat rotationQuat = glm::quat(glm::radians(eulerAngles));

    if (local) {
        localRotation = rotationQuat;
        //localRotation = glm::normalize(localRotation);
        localEulerAngles = glm::eulerAngles(localRotation);
    }
    else {
        rotation = rotationQuat;
        //rotation = glm::normalize(rotation);
        this->eulerAngles = glm::eulerAngles(rotation);
    }
}

void Transform::scaleBy(const vec3& scaling, bool local) 
{
    if (local) {
        localScale *= scaling;
    }
    else {
        scale *= scaling;
    }
}


// Get / Set ----------------------------------------------------
vec3 Transform::getForward() 
{
    updateMatrix();
    return glm::normalize(globalMatrix[2]);
}

vec3 Transform::getUp() 
{
    updateMatrix();
    return glm::normalize(globalMatrix[1]);
}

vec3 Transform::getRight() 
{
    updateMatrix();
    return glm::normalize(globalMatrix[0]);
}

mat4 Transform::getMatrix() 
{
    updateMatrix();
    return globalMatrix;
}

void Transform::updateMatrix()
{
    globalMatrix = mat4(1.0f);
    globalMatrix = glm::translate(globalMatrix, position);
    globalMatrix *= glm::mat4_cast(rotation * localRotation);
    globalMatrix = glm::scale(globalMatrix, localScale);
    globalMatrix = glm::scale(globalMatrix, scale);
}

// Traverse the hierarchy to multiply the transformation matrices of parent objects
mat4 Transform::GetWorldTransform()
{
    mat4 worldTransform = globalMatrix;   
    GameObject* parent = containerGO.lock().get()->parent.lock().get();

    while (parent->GetName() != "Scene")
    {
        worldTransform = parent->GetComponent<Transform>()->globalMatrix * worldTransform;
        parent = parent->parent.lock().get();
    }

    return worldTransform;
}

vec3 Transform::getPosition() const
{
    return position;
}

void Transform::setPosition(const vec3& newPosition) 
{
    position = newPosition;
}

quat Transform::getRotation() const
{
    return rotation;
}

quat Transform::getLocalRotation() const
{
    return localRotation;
}

vec3 Transform::getEulerAngles() const
{
    vec3 eulerAngles = glm::eulerAngles(rotation);
    return eulerAngles;
}

vec3 Transform::getLocalEulerAngles() const
{
    vec3 eulerAngles = glm::eulerAngles(localRotation);
    return eulerAngles;
}

void Transform::setRotation(const vec3& newRotation)
{
    eulerAngles = newRotation;
    rotation = EulerAnglesToQuaternion(eulerAngles);
}

vec3 Transform::getScale() const
{
    return scale;
}

void Transform::setScale(const vec3& newScale)
{
    scale = newScale;
}


quat Transform::EulerAnglesToQuaternion(const vec3& eulerAngles)
{
    quat quaternion;
    quaternion = glm::angleAxis(eulerAngles.z, vec3(0, 0, 1));     // Rotate around the Z-axis (yaw)
    quaternion *= glm::angleAxis(eulerAngles.y, vec3(0, 1, 0));    // Rotate around the Y-axis (pitch)
    quaternion *= glm::angleAxis(eulerAngles.x, vec3(1, 0, 0));    // Rotate around the X-axis (roll)
    return quaternion;
}

json Transform::SaveComponent()
{
    json transformJSON;

    transformJSON["Name"] = name;
    transformJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        transformJSON["ParentUID"] = pGO.get()->GetUID();
    }
    transformJSON["UID"] = UID;
    transformJSON["Position"] = { position.x, position.y, position.z };
    transformJSON["Rotation"] = { rotation.w, rotation.x, rotation.y, rotation.z };
    transformJSON["LocalRotation"] = { localRotation.w, localRotation.x, localRotation.y, localRotation.z };
    transformJSON["Scale"] = { scale.x, scale.y, scale.z };
    transformJSON["LocalScale"] = { localScale.x, localScale.y, localScale.z };

    return transformJSON;
}

void Transform::LoadComponent(const json& transformJSON)
{
    // Load basic properties
    if (transformJSON.contains("UID"))
    {
        UID = transformJSON["UID"];
    }

    if (transformJSON.contains("Name"))
    {
        name = transformJSON["Name"];
    }

    // Load parent UID and set parent
    /*if (transformJSON.contains("ParentUID"))
    {
        uint32_t parentUID = transformJSON["ParentUID"];

        if (auto parentGameObject = SceneManager::GetInstance().FindGOByUID(parentUID))
        {
            containerGO = parentGameObject;
        }
    }*/

    // Load transformation properties
    if (transformJSON.contains("Position"))
    {
        const auto& positionArray = transformJSON["Position"];
        position = { positionArray[0], positionArray[1], positionArray[2] };
    }

    if (transformJSON.contains("Rotation"))
    {
        const auto& rotationArray = transformJSON["Rotation"];
        rotation = quat(rotationArray[1], rotationArray[2], rotationArray[3], rotationArray[0]);
    }

    if (transformJSON.contains("LocalRotation"))
    {
        const auto& localRotationArray = transformJSON["LocalRotation"];
        localRotation = quat(localRotationArray[1], localRotationArray[2], localRotationArray[3], localRotationArray[0]);
    }

    if (transformJSON.contains("Scale"))
    {
        const auto& scaleArray = transformJSON["Scale"];
        scale = { scaleArray[0], scaleArray[1], scaleArray[2] };
    }

    if (transformJSON.contains("LocalScale"))
    {
        const auto& localScaleArray = transformJSON["LocalScale"];
        localScale = { localScaleArray[0], localScaleArray[1], localScaleArray[2] };
    }

    // Update the transformation matrix
    updateMatrix();
}
