#include "Camera.h"

Camera::Camera(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Camera),
    aspect(1.777), fov(65), zNear(0.1), zFar(15000),
    yaw(0), pitch(0),
    viewMatrix(1.0f),
    forward(), right(), up(),
    eye(), center(),
    drawFrustum(true)
{
    Transform* transform = containerGO.get()->GetComponent<Transform>();

    if (transform)
    {
        forward = transform->getForward();
        right = transform->getRight();
        up = transform->getUp();
        eye = transform->getPosition();
        center = eye - forward; //hekbas maybe +

        updateCameraVectors();
        updateViewMatrix();
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }  
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

void Camera::setRotation(const vec3f& axis, float angle, bool local) {
    if (auto sharedGO = this->containerGO.lock()) {
        sharedGO.get()->GetComponent<Transform>()->setRotation(axis, angle, local);
    }
    else {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }

    updateViewMatrix();
}

void Camera::rotate(const vec3f& eulerRotation, bool local)
{
    if (auto sharedGO = this->containerGO.lock())
    {
        sharedGO.get()->GetComponent<Transform>()->rotate((vec3)eulerRotation, local);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }

    updateViewMatrix();
}

void Camera::setRotation(const vec3f& eulerRotation, bool local) {
    if (auto sharedGO = this->containerGO.lock()) {
        sharedGO.get()->GetComponent<Transform>()->setRotation((vec3)eulerRotation, local);
    }
    else {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }

    updateViewMatrix();
}

const mat4f& Camera::getViewMatrix() const
{
    return viewMatrix;
}

const mat4f& Camera::getProjMatrix() const     {
    return projectionMatrix;
}

void Camera::updateViewMatrix()
{
    if (auto sharedGO = this->containerGO.lock())
    {
        //sharedGO.get()->GetComponent<Transform>()->updateMatrix();
        viewMatrix = glm::inverse(sharedGO.get()->GetComponent<Transform>()->getMatrix());
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }

    UpdateProjectionMatrix();
    UpdateViewProjectionMatrix();
    UpdateFrustum();
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
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }
}

void Camera::UpdateFrustum()
{
    frustum.update(viewProjectionMatrix);
}

void Camera::UpdateProjectionMatrix()
{
    projectionMatrix = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
}

void Camera::UpdateViewProjectionMatrix()
{
    viewProjectionMatrix = projectionMatrix * (mat4)viewMatrix;
}

json Camera::SaveComponent()
{
    json cameraJSON;

    cameraJSON["Name"] = name;
    cameraJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        cameraJSON["ParentUID"] = pGO.get()->GetUID();
    }
    cameraJSON["UID"] = UID;
    cameraJSON["FOV"] = fov;
    cameraJSON["Aspect"] = aspect;
    cameraJSON["zNear"] = zNear;
    cameraJSON["zFar"] = zFar;
    cameraJSON["Yaw"] = yaw;
    cameraJSON["Pitch"] = pitch;

    //Maybe not necessary to serialize
    /*cameraJSON["Frustum"]["nearTopLeft"] = { frustum.nearTopLeft.x, frustum.nearTopLeft.y, frustum.nearTopLeft.z };
    cameraJSON["Frustum"]["nearTopRight"] = { frustum.nearTopRight.x, frustum.nearTopRight.y, frustum.nearTopRight.z };
    cameraJSON["Frustum"]["nearBottomLeft"] = { frustum.nearBottomLeft.x, frustum.nearBottomLeft.y, frustum.nearBottomLeft.z };
    cameraJSON["Frustum"]["nearBottomRight"] = { frustum.nearBottomRight.x, frustum.nearBottomRight.y, frustum.nearBottomRight.z };

    cameraJSON["Frustum"]["farTopLeft"] = { frustum.farTopLeft.x, frustum.farTopLeft.y, frustum.farTopLeft.z };
    cameraJSON["Frustum"]["farTopRight"] = { frustum.farTopRight.x, frustum.farTopRight.y, frustum.farTopRight.z };
    cameraJSON["Frustum"]["farBottomLeft"] = { frustum.farBottomLeft.x, frustum.farBottomLeft.y, frustum.farBottomLeft.z };
    cameraJSON["Frustum"]["farBottomRight"] = { frustum.farBottomRight.x, frustum.farBottomRight.y, frustum.farBottomRight.z };*/

    return cameraJSON;
}

void Camera::LoadComponent(const json& cameraJSON)
{
    // Load basic properties
    if (cameraJSON.contains("UID"))
    {
        UID = cameraJSON["UID"];
    }

    if (cameraJSON.contains("Name"))
    {
        name = cameraJSON["Name"];
    }

    // Load parent UID and set parent
    /*if (cameraJSON.contains("ParentUID"))
    {
        uint32_t parentUID = cameraJSON["ParentUID"];
        if (auto parentGameObject = SceneManager::GetInstance().FindGOByUID(parentUID))
        {
            containerGO = parentGameObject;
        }
    }*/

    // Load camera-specific properties
    if (cameraJSON.contains("FOV"))
    {
        fov = cameraJSON["FOV"];
    }

    if (cameraJSON.contains("Aspect"))
    {
        aspect = cameraJSON["Aspect"];
    }

    if (cameraJSON.contains("zNear"))
    {
        zNear = cameraJSON["zNear"];
    }

    if (cameraJSON.contains("zFar"))
    {
        zFar = cameraJSON["zFar"];
    }

    if (cameraJSON.contains("Yaw"))
    {
        yaw = cameraJSON["Yaw"];
    }

    if (cameraJSON.contains("Pitch"))
    {
        pitch = cameraJSON["Pitch"];
    }

    // Implement additional logic to handle other camera-specific properties as needed
    // ...

    // Optional: Recalculate view and projection matrices based on loaded data
    UpdateProjectionMatrix();
    UpdateViewProjectionMatrix();
    UpdateFrustum();
}
