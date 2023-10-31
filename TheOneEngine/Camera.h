#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "Defs.h"

struct Camera
{
	double fov;
	double aspect;
	double zNear;
	double zFar;

	vec3 eye;		// Position
	vec3 center;	// Look at
	vec3 up;		// Orientation

	mat4 LookAt() const;
	mat4 Move() const;
	//mat4 Orbit() const;

	Camera();
};
#endif // !__CAMERA_H__