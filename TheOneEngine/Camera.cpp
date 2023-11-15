#include "Camera.h"


Camera::Camera(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Camera),
    aspect(1.777), fov(65), zNear(0.1), zFar(15000),
    yaw(0), pitch(0),
    viewMatrix(1.0f),
    forward(), right(), up(),
    eye(), center()
{
    Transform* transform = containerGO.get()->GetComponent<Transform>();

    if (transform)
    {
        forward = transform->getForward();
        right = transform->getRight();
        up = transform->getUp();
        eye = transform->getPosition();
        center = eye - forward; //hekbas maybe +
    }
    //if (auto sharedGO = this->containerGO.lock())
    //{
    //    forward = sharedGO.get()->GetComponent<Transform>().get()->getForward();
    //    right = sharedGO.get()->GetComponent<Transform>().get()->getRight();
    //    up = sharedGO.get()->GetComponent<Transform>().get()->getUp();
    //    eye = sharedGO.get()->GetComponent<Transform>().get()->getPosition();
    //    center = eye - forward; //hekbas maybe +
    //}
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }

    updateCameraVectors();
}

Camera::~Camera() {}


void Camera::translate(const vec3f& translation, bool local)
{
    if (auto sharedGO = this->containerGO.lock())
    {
        sharedGO.get()->GetComponent<Transform>()->translate(translation, local);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }

    updateViewMatrix();
}

void Camera::setPosition(const vec3f& newPosition)
{
    if (auto sharedGO = this->containerGO.lock())
    {
        sharedGO.get()->GetComponent<Transform>()->setPosition(newPosition);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }

    updateViewMatrix();
}

void Camera::rotate(const vec3f& axis, float angle, bool local)
{
    if (auto sharedGO = this->containerGO.lock())
    {
        sharedGO.get()->GetComponent<Transform>()->rotate(axis, angle, local);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }

    updateViewMatrix();
}

void Camera::rotate(const vec3f& eulerRotation, bool local)
{
    if (auto sharedGO = this->containerGO.lock())
    {
        sharedGO.get()->GetComponent<Transform>()->rotate(eulerRotation, local);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }

    updateViewMatrix();
}


const mat4f& Camera::getViewMatrix()
{
    return viewMatrix;
}

void Camera::updateViewMatrix()
{
    if (auto sharedGO = this->containerGO.lock())
    {
        viewMatrix = glm::inverse(sharedGO.get()->GetComponent<Transform>()->getMatrix());
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }
}


void Camera::updateCameraVectors()
{
    if (auto sharedGO = this->containerGO.lock())
    {
        Transform* transform = sharedGO.get()->GetComponent<Transform>();

        forward = transform->getForward();
        right = transform->getRight();
        up = transform->getUp();

        eye = transform->getPosition();
        center = eye + forward;
    }

    /*if (auto sharedGO = this->containerGO.lock())
    {
        forward = sharedGO.get()->GetComponent<Transform>().get()->getForward();
        right = sharedGO.get()->GetComponent<Transform>().get()->getRight();
        up = sharedGO.get()->GetComponent<Transform>().get()->getUp();

        eye = sharedGO.get()->GetComponent<Transform>().get()->getPosition();
        center = eye + forward;
    }*/
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }
}