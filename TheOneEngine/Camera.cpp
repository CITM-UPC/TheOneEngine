#include "Camera.h"

Camera::Camera() : fov(60), aspect(4.0/3.0), zNear(0.1), zFar(100), eye(0, 0, 0), center(0, 0, 1), up(0, 1, 0), yaw(-90) 
{
    WorldUp = up;
    cameraRight = glm::normalize(glm::cross(center, WorldUp));
    direction = center;
}

Camera::~Camera(){}

glm::dmat4 Camera::LookAt() const
{
	return glm::lookAt(eye, eye + center, up);
}

void Camera::UpdateDirection()
{
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
}

void Camera::updateCameraVectors()
{
    // Update Camera's Focus view point vector to be recomputed in the renderer with gluLookAt()
    center = direction;
    cameraRight = glm::normalize(glm::cross(center, WorldUp));
    up = glm::normalize(glm::cross(cameraRight,center));
}

