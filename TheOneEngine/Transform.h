#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#pragma once

#include "Defs.h"

class Transform {
public:
    Transform();

    void translate(const vec3f& translation, bool local = true);
    void rotate(const vec3f& axis, float angle, bool local = true);
    void rotate(const vec3f& eulerAngles, bool local = true);
    void scaleBy(const vec3f& scaling, bool local = true);

    //Gets forward vector
    vec3f getForward();

    //Gets up vector
    vec3f getUp(); 

    //Gets right vector
    vec3f getRight();

    /*Gets Transformation model Matrix*/
    void updateMatrix();
    mat4f getMatrix();

    /*Sets position to a new position in the global space*/
    void setPosition(const vec3f& newPosition);

    /*Sets scale to a new scale*/
    void setScale(const vec3f& newScale);

    /*Gets position*/
    vec3f getPosition() const;

    /*Gets global rotation of the object in quaternion*/
    quatf getRotation() const;

    /*Gets global rotation of the object in Euler Angles*/
    vec3f getEulerAngles() const;

    /*Gets local rotation of the object in quaternion*/
    quatf getLocalRotation() const;

    /*Gets local rotation of the object in Euler Angles*/
    vec3f getLocalEulerAngles() const;

    /*Gets scale*/
    vec3f getScale() const;

    /*Converts Euler Angles to Quaternion*/
    quatf EulerAnglesToQuaternion(const vec3f& eulerAngles);

private:
    vec3f position;
    quatf rotation;
    quatf localRotation;
    vec3f scale;
    vec3f localScale;
    mat4f globalMatrix; // Stores the transformations
};

#endif //__TRANSFORM_H__
