#include "Transform.h"

Transform::Transform(std::shared_ptr<GameObject> containerGO)
    : Component(containerGO, ComponentType::Transform),
    globalMatrix(1.0f),
    position(0.0f), rotation(1, 0, 0, 0), scale(1.0f), eulerAngles(0,0,0),
    localScale(1.0f), localRotation(1, 0, 0, 0), localEulerAngles(0, 0, 0)
{}

Transform::~Transform() {}


// Transform ----------------------------------------------------
void Transform::translate(const vec3f& translation, bool local) 
{
    if (local) {
        position += localRotation * translation;
    }
    else {
        position += rotation * translation;
    }
}

void Transform::rotate(const vec3f& axis, float angle, bool local)
{
    glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);

    if (local) {
        localRotation = rotationQuat;
        localRotation = glm::normalize(localRotation);
        localEulerAngles = glm::eulerAngles(localRotation);
    }
    else {
        rotation = rotationQuat;
        rotation = glm::normalize(rotation);
        this->eulerAngles = glm::eulerAngles(rotation);
    }
}

void Transform::rotate(const vec3f& eulerAngles, bool local)
{
    glm::quat rotationQuat = glm::quat(glm::radians(eulerAngles));

    if (local) {
        localRotation = rotationQuat;
        localRotation = glm::normalize(localRotation);
        localEulerAngles = glm::eulerAngles(localRotation);
    }
    else {
        rotation = rotationQuat;
        rotation = glm::normalize(rotation);
        this->eulerAngles = glm::eulerAngles(rotation);
    }
}

void Transform::scaleBy(const vec3f& scaling, bool local) 
{
    if (local) {
        localScale *= scaling;
    }
    else {
        scale *= scaling;
    }
}


// Get / Set ----------------------------------------------------
vec3f Transform::getForward() 
{
    updateMatrix();
    return glm::normalize(globalMatrix[2]);
}

vec3f Transform::getUp() 
{
    updateMatrix();
    return glm::normalize(globalMatrix[1]);
}

vec3f Transform::getRight() 
{
    updateMatrix();
    return glm::normalize(globalMatrix[0]);
}

mat4f Transform::getMatrix() 
{
    updateMatrix();
    return globalMatrix;
}

void Transform::updateMatrix()
{
    globalMatrix = mat4f(1.0f);
    globalMatrix = glm::translate(globalMatrix, position);
    globalMatrix *= glm::mat4_cast(rotation * localRotation);
    globalMatrix = glm::scale(globalMatrix, localScale);
    globalMatrix = glm::scale(globalMatrix, scale);
}

vec3f Transform::getPosition() const
{
    return position;
}

void Transform::setPosition(const vec3f& newPosition) 
{
    position = newPosition;
}

quatf Transform::getRotation() const
{
    return rotation;
}

quatf Transform::getLocalRotation() const
{
    return localRotation;
}

vec3f Transform::getEulerAngles() const
{
    vec3f eulerAngles = glm::eulerAngles(rotation);
    return eulerAngles;
}

vec3f Transform::getLocalEulerAngles() const
{
    vec3f eulerAngles = glm::eulerAngles(localRotation);
    return eulerAngles;
}

vec3f Transform::getScale() const
{
    return scale;
}

void Transform::setScale(const vec3f& newScale)
{
    scale = newScale;
}


quatf Transform::EulerAnglesToQuaternion(const vec3f& eulerAngles)
{
    quatf quaternion;
    quaternion = glm::angleAxis(eulerAngles.z, glm::vec3(0, 0, 1));     // Rotate around the Z-axis (yaw)
    quaternion *= glm::angleAxis(eulerAngles.y, glm::vec3(0, 1, 0));    // Rotate around the Y-axis (pitch)
    quaternion *= glm::angleAxis(eulerAngles.x, glm::vec3(1, 0, 0));    // Rotate around the X-axis (roll)
    return quaternion;
}