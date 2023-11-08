#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Transform.h"

#include "..\TheOneEditor\Log.h"
#include "..\TheOneEditor\Input.h"

#include <memory>

class Camera : public Component
{
public:

    Camera(std::shared_ptr<GameObject> containerGO);
    ~Camera();

    void translate(const vec3f& translation, bool local = true);
    void rotate(const vec3f& axis, float angle, bool local = true);  
    void rotate(const vec3f& eulerRotation, bool local = true);

    const mat4f& getViewMatrix();
    void updateViewMatrix();

    void updateCameraVectors();

public:

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