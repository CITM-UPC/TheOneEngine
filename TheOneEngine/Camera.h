#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Transform.h"

#include "..\TheOneEditor\Log.h"
#include "..\TheOneEditor\Input.h"

#include <memory>

struct Frustum
{
    glm::vec3 nearTopLeft;
    glm::vec3 nearTopRight;
    glm::vec3 nearBottomLeft;
    glm::vec3 nearBottomRight;

    glm::vec3 farTopLeft;
    glm::vec3 farTopRight;
    glm::vec3 farBottomLeft;
    glm::vec3 farBottomRight;

    void update(const glm::mat4& viewProjectionMatrix)
    {
        // Extract frustum planes from the combined view-projection matrix
        nearTopLeft = extractPlane(viewProjectionMatrix, 3, 0, 2);
        nearTopRight = extractPlane(viewProjectionMatrix, 3, 1, 2);
        nearBottomLeft = extractPlane(viewProjectionMatrix, 3, 0, 3);
        nearBottomRight = extractPlane(viewProjectionMatrix, 3, 1, 3);

        farTopLeft = extractPlane(viewProjectionMatrix, 2, 0, 2);
        farTopRight = extractPlane(viewProjectionMatrix, 2, 1, 2);
        farBottomLeft = extractPlane(viewProjectionMatrix, 2, 0, 3);
        farBottomRight = extractPlane(viewProjectionMatrix, 2, 1, 3);
    }

    glm::vec3 extractPlane(const glm::mat4& mat, int row, int col1, int col2)
    {
        glm::vec3 normal;
        normal.x = mat[col1][0] - mat[row][0];
        normal.y = mat[col1][1] - mat[row][1];
        normal.z = mat[col1][2] - mat[row][2];
        return glm::normalize(normal);
    }
};

class Camera : public Component
{
public:

    Camera(std::shared_ptr<GameObject> containerGO);
    ~Camera();

    void translate(const vec3f& translation, bool local = true);
    void setPosition(const vec3f& newPosition);
    void rotate(const vec3f& axis, float angle, bool local = true);  
    void rotate(const vec3f& eulerRotation, bool local = true);

    const mat4f& getViewMatrix();
    void updateViewMatrix();

    void updateCameraVectors();

    void UpdateFrustum();
    void UpdateProjectionMatrix();
    void UpdateViewProjectionMatrix();

public:

    double fov;
    double aspect;
    double zNear;
    double zFar;

    float yaw, pitch;

	vec3f eye;		// Position
	vec3f center;	// LookAt
	vec3f up;		// Orientation
	vec3f forward;
	vec3f right;

    Frustum frustum;
    mat4f viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
};
#endif // !__CAMERA_H__