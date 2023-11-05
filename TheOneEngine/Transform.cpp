#include "Transform.h"

Transform::Transform()
{
	position = { 0, 0, 0 };
	rotation = { 0, 0, 0, 1 };
	scale	 = { 1, 1, 1 };

	eulerAngles = { 0, 0, 0 };

	right =	  { 1, 0, 0 };
	up =	  { 0, 1, 0 };
	forward = { 0, 0, 1 };

	referenceFrameMat = glm::mat3(1); //Identity
	transformMat = mat4(1);
}

Transform::~Transform(){}

void Transform::MoveTo(vec3 position, Space referenceFrame)
{
}

void Transform::Move(vec3 displacement, Space referenceFrame)
{
	transformMat[0].w = transformMat[0].w * displacement.x;
	transformMat[1].w = transformMat[1].w * displacement.y;
	transformMat[2].w = transformMat[2].w * displacement.z;
}

void Transform::RotateTo(vec3 axis)
{
}

void Transform::Rotate(vec3 axis, Space referenceFrame)
{
}

void Transform::ScaleTo(vec3 axis, Space referenceFrame)
{
}

void Transform::Scale(vec3 axis, Space referenceFrame)
{
	transformMat[0].x = transformMat[0].x * axis.x;
	transformMat[1].y = transformMat[1].y * axis.y;
	transformMat[2].z = transformMat[2].z * axis.z;
}
