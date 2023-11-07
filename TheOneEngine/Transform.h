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

    vec3f getForward();
    vec3f getUp();
    vec3f getRight();

    mat4f getMatrix();

    void setPosition(const vec3f& newPosition);
    void setScale(const vec3f& newScale);

    vec3f getPosition() const;
    quatf getRotation() const;
    quatf getLocalRotation() const;
    vec3f getScale() const;

private:
    vec3f position;
    quatf rotation;
    quatf localRotation;
    vec3f scale;
    vec3f localScale;
    mat4f globalMatrix; // Stores the global transformations
};

#endif //__TRANSFORM_H__
