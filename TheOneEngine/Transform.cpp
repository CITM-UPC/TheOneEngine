#include "Transform.h"
#include "GameObject.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"

Transform::Transform(std::shared_ptr<GameObject> containerGO)
    : Component(containerGO, ComponentType::Transform),
    globalMatrix(1.0f),
    position(0.0f), rotation(1, 0, 0, 0), scale(1.0f),
    localScale(1.0f), localRotation(1, 0, 0, 0)
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
    dirty_ = true;
}

void Transform::rotate(const vec3& axis, double angle, bool local)
{
    glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);

    if (local) {
        localRotation *= rotationQuat;
    }
    else {
        rotation *= rotationQuat;
    }
    dirty_ = true;
}

void Transform::rotate(const vec3& eulerAngles, bool local)
{
    glm::quat rotationQuat = glm::quat(glm::radians(eulerAngles));

    if (local) {
        localRotation *= rotationQuat;
    }
    else {
        rotation *= rotationQuat;
    }
    dirty_ = true;
}

void Transform::rotate(const glm::quat& rotation_quat, bool local) {
    if (local)
        localRotation *= rotation_quat;
    else
        rotation *= rotation_quat;

    dirty_ = true;
}

void Transform::scaleBy(const vec3& scaling, bool local) 
{
    if (local) {
        localScale *= scaling;
    }
    else {
        scale *= scaling;
    }
    dirty_ = true;

}


// Get / Set ----------------------------------------------------
vec3 Transform::getForward() 
{
    return glm::normalize(globalMatrix[2]);
}

vec3 Transform::getUp() 
{
    return glm::normalize(globalMatrix[1]);
}

vec3 Transform::getRight() 
{
    return glm::normalize(globalMatrix[0]);
}

bool Transform::isDirty() const {
    return dirty_;
}

mat4 Transform::getMatrix() const
{
    return globalMatrix;
}

mat4 Transform::getMatrixLocal() const {
    return localMatrix;
}

void Transform::updateMatrix(const mat4& parent_global)
{
    localMatrix = mat4(1.0f);
    localMatrix = glm::translate(localMatrix, position);
    localMatrix *= glm::mat4_cast(localRotation);
    localMatrix = glm::scale(localMatrix, localScale);
    localMatrix = glm::scale(localMatrix, scale);
    globalMatrix = parent_global * localMatrix;
    updateGlobalTRS();
    dirty_ = false;
}

vec3 Transform::getPosition() const
{
    return position;
}

vec3 Transform::getGlobalPosition() const {
    return globalPosition;
}

void Transform::setPosition(const vec3& newPosition) 
{
    position = newPosition;
    dirty_ = true;
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

void Transform::setRotation(const vec3& newRotation, bool local)
{
    glm::quat rotationQuat = glm::quat(glm::radians(newRotation));

    if (local) {
        localRotation = rotationQuat;
    }
    else {
        rotation = rotationQuat;
    }

    dirty_ = true;
}

void Transform::setRotation(const vec3& axis, double angle, bool local) {
    glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);

    if (local) {
        localRotation = rotationQuat;
    }
    else {
        rotation = rotationQuat;
    }

    dirty_ = true;
}

void Transform::setRotation(const glm::quat& rotation_quat, bool local) {
    if (local)
        localRotation *= rotation_quat;
    else
        rotation *= rotation_quat;

    dirty_ = true;
}

void Transform::updateGlobalTRS() {
    vec3 skew;
    vec4 perspective;
    glm::decompose(globalMatrix, scale, rotation, globalPosition, skew, perspective);
}

vec3 Transform::getScale() const
{
    return scale;
}

void Transform::setScale(const vec3& newScale)
{
    scale = newScale;
    dirty_ = true;
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
    updateMatrix(); // For gameobjects outside scene
    dirty_ = true;
}
