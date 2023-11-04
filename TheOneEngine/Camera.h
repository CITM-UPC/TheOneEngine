#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "Defs.h"

class Camera
{
public:
	Camera();
	~Camera();

	/*Returns view matrix*/
	mat4 LookAt() const;

	void updateCameraVectors();

public:

	double fov;
	double aspect;
	double zNear;
	double zFar;

	vec3 eye;		// Position
	vec3 center;	// Target
	vec3 up;		// Orientation
	vec3 direction;
	vec3 cameraRight;
	vec3 WorldUp;

	double yaw, pitch;
	float zoom;
};
#endif // !__CAMERA_H__