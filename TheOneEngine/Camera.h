#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"
#include "Transform.h"

#include "..\TheOneEditor\Log.h"
#include "..\TheOneEditor\Input.h"

#include <memory>


struct Plane
{
    //plane eq -> ax + by + cz + d = 0
    glm::vec3 normal;
    double distance;
};

struct Frustum : public Plane
{
    Plane nearPlane;  //do NOT change name to near
    Plane farPlane;   //do NOT change name to far
    Plane left;
    Plane right;
    Plane top;
    Plane bot;

    static Plane ComputePlane(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
        Plane plane;
        plane.normal = glm::normalize(glm::cross(b - a, c - a));
        plane.distance = glm::dot(plane.normal, a);
        return plane;
    }

    static Plane FromNumbers(glm::vec4 numbers)
    {
        Plane plane;
        plane.normal.x = numbers.x;
        plane.normal.y = numbers.y;
        plane.normal.z = numbers.z;
        plane.distance = numbers.w;
        return plane;
    }

    void Update(const glm::mat4& vpm)
    {
        // Extract frustum planes from the combined view-projection matrix

        glm::vec4 row1 = glm::vec4(vpm[0][0], vpm[1][0], vpm[2][0], vpm[3][0]);
        glm::vec4 row2 = glm::vec4(vpm[0][1], vpm[1][1], vpm[2][1], vpm[3][1]);
        glm::vec4 row3 = glm::vec4(vpm[0][2], vpm[1][2], vpm[2][2], vpm[3][2]);
        glm::vec4 row4 = glm::vec4(vpm[0][3], vpm[1][3], vpm[2][3], vpm[3][3]);
       
        left = FromNumbers(row4 + row1);
        right = FromNumbers(row4 - row1);       
        bot = FromNumbers(row4 + row2);
        top = FromNumbers(row4 - row2);
        nearPlane = FromNumbers(row4 + row3);
        farPlane = FromNumbers(row4 - row3);
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

    //update
    void UpdateCamera();
    void UpdateCameraVectors();
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    void UpdateViewProjectionMatrix();
    void UpdateFrustum();

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

    bool drawFrustum;
};
#endif // !__CAMERA_H__