#include "Transform.h"

Transform::Transform() : position(0.0f), localRotation(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f), isDirty(true) 
{
    globalMatrix = mat4f(1.0f);
}

void Transform::translate(const vec3f& translation, bool local = true) 
{
    if (local) {
        localMatrix = glm::translate(localMatrix, translation);
    }
    else {
        globalMatrix = glm::translate(globalMatrix, translation);
    }
    isDirty = true;
}

void Transform::rotate(const vec3f& axis, float angle, bool local = true) 
{
    quatf rotationQuat = glm::angleAxis(glm::radians(angle), axis);
    if (local) {
        localRotation = rotationQuat * localRotation;
        localRotation = glm::normalize(localRotation);
        localMatrix *= glm::mat4_cast(rotationQuat);
    }
    else {
        globalMatrix = glm::rotate(globalMatrix, glm::radians(angle), axis);
    }
    isDirty = true;
}

void Transform::scaleBy(const vec3f& scaling, bool local = true) {
    if (local) {
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

void Transform::setScale(const vec3f& newScale) 
{
    scale = newScale;
    isDirty = true;
}