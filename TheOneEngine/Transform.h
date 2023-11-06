#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#pragma once

#include "Defs.h"

class Transform {
public:
    Transform();

    void translate(const vec3f& translation, bool local = true);
    void rotate(const vec3f& axis, float angle, bool local = true);
    void rotateEulerAngles(const vec3f& eulerRotation, bool local = true);
    void scaleBy(const vec3f& scaling, bool local = true);

    mat4f getMatrix();

    void setPosition(const vec3f& newPosition);
    void setLocalRotation(const quatf& newRotation);
    void setGlobalRotation(quatf& newRotation);
    void setScale(const vec3f& newScale);

    vec3f getPosition() const;
    quatf getLocalRotation() const;
    quatf getGlobalRotation() const;
    vec3f getScale() const;

private:
    vec3f position;
    quatf localRotation;
    quatf globalRotation;
    vec3f scale;
    mat4f localMatrix; // Stores the local transformations
    mat4f globalMatrix; // Stores the global transformations
    bool isDirty; // Flag to check if the global matrix needs updating
};

#endif //__TRANSFORM_H__
