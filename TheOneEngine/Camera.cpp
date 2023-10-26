#include "Camera.h"

#include <glm/ext/matrix_transform.hpp>

Camera::Camera() : fov(60), aspect(4.0/3.0), zNear(0.1), zFar(100), eye(10, 2, 10), center(0, 1, 0), up(0, 1, 0) {}

glm::dmat4 Camera::LookAt() const
{
	return glm::lookAt(eye, center, up);
}

//glm::dmat4 Camera::Move() const
//{
//    glm::vec3 translation(1.0f, 2.0f, 3.0f);
//
//    glm::dmat4 translationMatrix = glm::translate(glm::dmat4(1.0f), translation);
//
//    return translationMatrix;
//}

//glm::dmat4 Camera::Orbit() const {
//	return glm::rotate(eye, center, up);
//}