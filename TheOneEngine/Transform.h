#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#pragma once

#include "Defs.h"

class Transform
{
public:
	enum class Space
	{
		LOCAL,
		WORLD,
	};

	Transform();
	~Transform();

	//Moves the object to a position.
	void MoveTo(vec3 position, Space referenceFrame = Space::WORLD);

	//Moves the object in his local space.
	void Move(vec3 displacement, Space referenceFrame = Space::LOCAL);

	//Rotates the object for its rotation to be the one given by 'axis'.
	void RotateTo(vec3 axis);

	//Rotates the object in 'axis' increments.
	void Rotate(vec3 axis, Space referenceFrame = Space::LOCAL);

	void ScaleTo(vec3 axis, Space referenceFrame = Space::WORLD);

	void Scale(vec3 axis, Space referenceFrame = Space::LOCAL);

public:

	vec3 position;
	
	//Rotation should be Quaternion
	vec4 rotation;
	vec3 eulerAngles;

	vec3 scale;

	//Reference frame vectors
	vec3 forward; //z
	vec3 right; //x
	vec3 up; //y

	glm::mat3x3 referenceFrameMat;
	mat4 transformMat;
};

#endif //__TRANSFORM_H__
