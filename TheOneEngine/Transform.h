#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#pragma once

#include "Defs.h"
#include "Component.h"

#include <memory>

class GameObject;

class Transform : public Component
{
public:

    Transform(std::shared_ptr<GameObject> containerGO);
    virtual ~Transform();

    // Transform ----------------------------------------------------
    void translate(const vec3& translation, bool local = true);
    void rotate(const vec3& axis, double angle, bool local = true);
    void rotate(const vec3& eulerAngles, bool local = true);
    void scaleBy(const vec3& scaling, bool local = true);


    // Get / Set -----------------------------------------------------
    vec3 getForward(); // Gets forward vector   
    vec3 getUp();      // Gets up vector  
    vec3 getRight();   // Gets right vector

    // Gets Transformation model Matrix
    mat4 getMatrix();
    void updateMatrix();
    mat4 GetWorldTransform();

    vec3 getPosition() const;
    void setPosition(const vec3& newPosition); // Sets position in global space
   
    quat getRotation() const;      // Gets global rotation in quaternion   
    quat getLocalRotation() const; // Gets local rotation in quaternion
   
    vec3 getEulerAngles() const;       // Gets global rotation of the object in Euler Angles   
    vec3 getLocalEulerAngles() const;  // Gets local rotation of the object in Euler Angles
    //void setRotation(const vec3f& newRotation); // Sets rotation 
    void setRotation(const vec3& newRotation); // Sets rotation 

    vec3 getScale() const;
    void setScale(const vec3& newScale); // Sets Scale

    quat EulerAnglesToQuaternion(const vec3& eulerAngles);    // Converts Euler Angles to Quaternion

public:

    json SaveComponent();
    void LoadComponent(const json& transformJSON);

public:

    vec3 position;
    quat rotation;
    quat localRotation;
    vec3 eulerAngles;
    vec3 localEulerAngles;
    vec3 scale;
    vec3 localScale;
    mat4 globalMatrix; // Stores the transformations
};

#endif //__TRANSFORM_H__