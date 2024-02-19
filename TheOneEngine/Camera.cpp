#include "Camera.h"
#include "Defs.h"

#include "GameObject.h"
#include "Transform.h"
#include "Ray.h"

Camera::Camera(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Camera),
    aspect(1.777), fov(65),
    zNear(0.1), zFar(1000),
    yaw(0), pitch(0),
    viewMatrix(1.0f),
    forward(0, 0, 0), right(0, 0, 0), up(0, 0, 0),
    eye(0, 0, 0), center(0, 0, 0),
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

    UpdateCamera();
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

    UpdateCamera();
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

    UpdateCamera();
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

    UpdateCamera();
}

const mat4f& Camera::getViewMatrix()
{
    return viewMatrix;
}


// update camera

void Camera::UpdateCamera()
{
    UpdateCameraVectors();
    UpdateViewMatrix();
    UpdateProjectionMatrix();
    UpdateViewProjectionMatrix();
    UpdateFrustum();
}

void Camera::UpdateCameraVectors()
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

void Camera::UpdateViewMatrix()
{
    if (auto sharedGO = this->containerGO.lock())
    {
        Camera* camera = sharedGO.get()->GetComponent<Camera>();
        viewMatrix = glm::lookAt(camera->eye, camera->center, camera->up);
    }
    else
    {
        LOG(LogType::LOG_ERROR, "GameObject Container invalid!");
    }
}

void Camera::UpdateProjectionMatrix()
{
    projectionMatrix = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
}

void Camera::UpdateViewProjectionMatrix()
{
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Camera::UpdateFrustum()
{
    frustum.Update(viewProjectionMatrix);
}


Ray Camera::ComputeCameraRay(float x, float y)
{
    glm::mat4 viewProjInverse = glm::inverse(viewProjectionMatrix);

    //glm::vec4 worldOrigin = viewProjInverse * glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 worldDirection = viewProjInverse * glm::vec4(x, y, 1.0f, 1.0f);

    return Ray(eye, glm::normalize(worldDirection));
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
    //hekbas - correct, Frustum can be created from camera
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
    UpdateCamera();
}