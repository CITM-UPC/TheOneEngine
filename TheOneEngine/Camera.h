#pragma once

#include "Defs.h"

struct Camera
{
	double fov;
	double aspect;
	double zNear;
	double zFar;

	vec3 eye;		// Position
	vec3 center;	// Target
	vec3 up;		// Orientation
	vec3 direction;
	vec3 cameraRight;
	mat4 viewMatrix;

	double yaw, pitch;

	/*We do a cross product to create a right vector and we move along the right vector accordingly. */
	vec3 RightVector();
	vec3 DirectionVector();

	/*Returns view matrix*/
	mat4 LookAt() const;

	mat4 Move() const;
	//mat4 Orbit() const;

	Camera();
};

