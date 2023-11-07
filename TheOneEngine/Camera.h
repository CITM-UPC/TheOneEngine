#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "Defs.h"
#include "Transform.h"

class Camera {
public:
    Camera() : transform(), aspect(1), fov(60), zNear(0.1), zFar(1000), yaw(0), pitch(0) 
    {
        viewMatrix = mat4f(1.0f);
        worldUp = vec3f(0,1,0);
        forward = transform.getForward();
        right = transform.getRight();
    }

    void updateViewMatrix() 
    {
        viewMatrix = glm::inverse(transform.getMatrix());
    }

    const mat4f& getViewMatrix() 
    {
        return viewMatrix;
    }

    void translate(const vec3f& translation, bool local = true) 
    {
        transform.translate(translation, local);
        updateViewMatrix();
    }

    void rotate(const vec3f& axis, float angle, bool local = true) 
    {
        transform.rotate(axis, angle, local);
        updateViewMatrix();
    }
    
    void rotate(const vec3f& eulerRotation, bool local = true) 
    {
        transform.rotate(eulerRotation, local);
        updateViewMatrix();
    }

	void updateCameraVectors() 
    {
        // Update the forward, right, and up vectors based on the new orientation
        forward = transform.getForward();
        right = transform.getRight();
        up = transform.getUp();

        // Update the eye and center vectors
        eye = transform.getPosition();
        center = eye + forward;
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
	vec3f worldUp;
};
#endif // !__CAMERA_H__