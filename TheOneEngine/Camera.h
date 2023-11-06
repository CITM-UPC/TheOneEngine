#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "Defs.h"
#include "Transform.h"

class Camera {
public:
    Camera() : transform(), aspect(1), fov(60), zNear(0.1), zFar(1000) {
        viewMatrix = mat4f(1.0f);
        forward = glm::normalize(center - eye);
        right = glm::normalize(glm::cross(up, forward));
    }

    void updateViewMatrix() {
        viewMatrix = glm::inverse(transform.getMatrix());
    }

    const mat4f& getViewMatrix() {
        return viewMatrix;
    }

    void translate(const vec3f& translation, bool local = true) {
        transform.translate(translation, local);
        updateViewMatrix();
    }

    void rotate(const vec3f& axis, float angle, bool local = true) {
        transform.rotate(axis, angle, local);
        updateViewMatrix();
    }
    
    void rotateEulerAngles(const vec3f& eulerRotation, bool local = true) {
        transform.rotateEulerAngles(eulerRotation, local);
        updateViewMatrix();
    }

	void updateCameraVectors() {
		eye = vec3f(glm::inverse(viewMatrix)[3]);
		center = eye + vec3f(glm::inverse(viewMatrix)[2]);
		up = vec3f(glm::inverse(viewMatrix)[1]);
        forward = glm::normalize(center - eye);
        right = glm::normalize(glm::cross(up, forward));
	}
    
    void updateForward() {
        forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        forward.y = sin(glm::radians(pitch));
        forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        center = eye + glm::normalize(forward);
    }

public:
    Transform transform; // The camera's transform
    double fov;
    double aspect;
    double zNear;
    double zFar;

    float yaw, pitch;

    mat4f viewMatrix;
	vec3f eye;		// Position
	vec3f center;	// LookAt
	vec3f up;		// Orientation
	vec3f forward;
	vec3f right;
};
#endif // !__CAMERA_H__