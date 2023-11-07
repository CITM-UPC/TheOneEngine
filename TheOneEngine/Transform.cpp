#include "Transform.h"

Transform::Transform() : position(0.0f), scale(1.0f), localScale(1.0f), globalMatrix(1.0f) {
    rotation = { 1, 0, 0, 0 };
    localRotation = { 1, 0, 0, 0 };
}

void Transform::translate(const vec3f& translation, bool local) 
{
    if (local) {
        position += localRotation * translation;
    }
    else {
        position += translation;
    }
}

void Transform::rotate(const vec3f& axis, float angle, bool local)
{
    glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);
    if (local) {
        localRotation = rotationQuat * localRotation;
        localRotation = glm::normalize(localRotation);
    }
    else {
        rotation = rotationQuat * rotation;
        rotation = glm::normalize(rotation);
    }
}

void Transform::rotate(const vec3f& eulerAngles)
{
    rotation = glm::quat(glm::radians(eulerAngles));
}

void Transform::rotateLocal(const glm::vec3& eulerAngles) {
    localRotation = glm::quat(glm::radians(eulerAngles));
}

void Transform::scaleBy(const vec3f& scaling, bool local) {
    if (local) {
        localScale *= scaling;
    }
    else {
        scale *= scaling;
    }
}

vec3f Transform::getForward() {
    // Calculate the forward vector based on the current orientation
    return glm::normalize(glm::vec3(2.0f * (rotation.x * rotation.z + rotation.w * rotation.y),
        2.0f * (rotation.y * rotation.x - rotation.w * rotation.x),
        1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y)));
}

vec3f Transform::getUp() {
    // Calculate the up vector based on the current orientation
    return glm::normalize(glm::vec3(2.0f * (rotation.x * rotation.y - rotation.w * rotation.z),
        1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z),
        2.0f * (rotation.y * rotation.z + rotation.w * rotation.x)));
}

vec3f Transform::getRight() {
    /*return glm::normalize(glm::vec3(1.0f - 2.0f * (localRotation.y * localRotation.y + localRotation.z * localRotation.z),
        2.0f * (localRotation.x * localRotation.y + localRotation.w * localRotation.z),
        2.0f * (localRotation.x * localRotation.z - localRotation.w * localRotation.y)));*/

    return glm::normalize(glm::cross(getForward(), getUp()));
}

mat4f Transform::getMatrix() 
{
    globalMatrix = mat4f(1.0f);
    globalMatrix = glm::translate(globalMatrix, position);
    globalMatrix *= glm::mat4_cast(localRotation);
    globalMatrix *= glm::mat4_cast(rotation); // Apply global rotation
    globalMatrix = glm::scale(globalMatrix, localScale);
    globalMatrix = glm::scale(globalMatrix, scale);
    return globalMatrix;
}

void Transform::setPosition(const vec3f& newPosition) 
{
    position = newPosition;
}

void Transform::setScale(const vec3f& newScale) 
{
    scale = newScale;
}

vec3f Transform::getPosition() const
{
    return position;
}

quatf Transform::getRotation() const
{
    return rotation;
}

quatf Transform::getLocalRotation() const
{
    return localRotation;
}

vec3f Transform::getScale() const
{
    return scale;
}
