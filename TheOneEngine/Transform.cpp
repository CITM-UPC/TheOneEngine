#include "Transform.h"

Transform::Transform() : position(0.0f), localRotation(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f), isDirty(true) 
{
    localMatrix = mat4f(1.0f);
    globalMatrix = mat4f(1.0f);
    globalRotation = { 1, 0, 0, 0 };
    localRotation = { 1, 0, 0, 0 };
}

void Transform::translate(const vec3f& translation, bool local) 
{
    if (local) {
        //position += vec3f(getMatrix() * vec4f(translation, 1.0));
        localMatrix = glm::translate(localMatrix, translation);
    }
    else {
        //position += translation;
        globalMatrix = glm::translate(globalMatrix, translation);
    }
    isDirty = true;
}

void Transform::rotate(const vec3f& axis, float angle, bool local) 
{
    quatf rotationQuat = glm::angleAxis(glm::radians(angle), axis);
    if (local) {
        localRotation = rotationQuat * localRotation;
        localRotation = glm::normalize(localRotation);
        //localMatrix *= glm::mat4_cast(localRotation);
    }
    else {
        globalRotation = rotationQuat * globalRotation;
        globalRotation = glm::normalize(globalRotation);
        //globalMatrix *= glm::mat4_cast(globalRotation);
    }
    isDirty = true;
}

void Transform::rotateEulerAngles(const vec3f& eulerRotation, bool local) {
    if (local) {
        localRotation = glm::normalize(glm::quat(glm::radians(eulerRotation))) * localRotation;
    }
    else {
        globalRotation = glm::normalize(glm::quat(glm::radians(eulerRotation))) * globalRotation;
    }
    isDirty = true;
}

void Transform::scaleBy(const vec3f& scaling, bool local) {
    if (local) {
        //scale *= scaling;
        localMatrix = glm::scale(localMatrix, scaling);
    }
    else {
        globalMatrix = glm::scale(globalMatrix, scaling);
    }
    isDirty = true;
}

mat4f Transform::getMatrix() 
{
    if (isDirty) {
        globalMatrix = mat4f(1.0f);
        globalMatrix = glm::translate(globalMatrix, position);
        globalMatrix *= glm::mat4_cast(localRotation);
        globalMatrix *= glm::mat4_cast(globalRotation);
        globalMatrix = glm::scale(globalMatrix, scale);
        globalMatrix = globalMatrix * localMatrix;
        isDirty = false;
    }
    return globalMatrix;
}

void Transform::setPosition(const vec3f& newPosition) 
{
    position = newPosition;
    isDirty = true;
}

void Transform::setLocalRotation(const quatf& newRotation) 
{
    localRotation = newRotation;
    isDirty = true;
}

void Transform::setGlobalRotation(quatf& newRotation) {
    globalRotation = newRotation;
    isDirty = true;
}

void Transform::setScale(const vec3f& newScale) 
{
    scale = newScale;
    isDirty = true;
}

vec3f Transform::getPosition() const
{
    return position;
}

quatf Transform::getLocalRotation() const
{
    return localRotation;
}

quatf Transform::getGlobalRotation() const
{
    return globalRotation;
}

vec3f Transform::getScale() const
{
    return scale;
}
