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
    void translate(const vec3f& translation, bool local = true);
    void rotate(const vec3f& axis, float angle, bool local = true);
    void rotate(const vec3f& eulerAngles, bool local = true);
    void scaleBy(const vec3f& scaling, bool local = true);


    // Get / Set -----------------------------------------------------
    vec3f getForward(); // Gets forward vector   
    vec3f getUp();      // Gets up vector  
    vec3f getRight();   // Gets right vector

    // Gets Transformation model Matrix
    mat4f getMatrix();
    void updateMatrix();

    vec3f getPosition() const;
    void setPosition(const vec3f& newPosition); // Sets position in global space
   
    quatf getRotation() const;      // Gets global rotation in quaternion   
    quatf getLocalRotation() const; // Gets local rotation in quaternion
   
    vec3f getEulerAngles() const;       // Gets global rotation of the object in Euler Angles   
    vec3f getLocalEulerAngles() const;  // Gets local rotation of the object in Euler Angles
    //void setRotation(const vec3f& newRotation); // Sets rotation 
    void setRotation(const vec3f& newRotation); // Sets rotation 

    vec3f getScale() const;
    void setScale(const vec3f& newScale); // Sets Scale

    quatf EulerAnglesToQuaternion(const vec3f& eulerAngles);    // Converts Euler Angles to Quaternion


public:

    vec3f position;
    quatf rotation;
    quatf localRotation;
    vec3f eulerAngles;
    vec3f localEulerAngles;
    vec3f scale;
    vec3f localScale;
    mat4f globalMatrix; // Stores the transformations
};

#endif //__TRANSFORM_H__