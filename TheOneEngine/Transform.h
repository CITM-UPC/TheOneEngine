#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#pragma once

#include "Defs.h"
#include "Component.h"

#include <memory>

enum class HandleSpace
{
    LOCAL,
    GLOBAL
};

enum class HandlePosition
{
    PIVOT,
    CENTER
};

class GameObject;

class Transform : public Component
{
public:

    Transform(std::shared_ptr<GameObject> containerGO);
    Transform(std::shared_ptr<GameObject> containerGO, mat4 transform);
    virtual ~Transform();
   

    //Transform
    void Translate(const vec3& translation, const HandleSpace& space = HandleSpace::LOCAL);
    void SetPosition(const vec3& newPosition, const HandleSpace& space = HandleSpace::LOCAL);

    void SetRotation(const vec3& eulerAngles);
    void Rotate(const vec3& eulerAngles, const HandleSpace& space = HandleSpace::LOCAL);
    void RotateInspector(const vec3& eulerAngles);

    void Scale(const vec3& scaleFactors);
    void SetScale(const vec3& newScale); 


    // Utils
    void DecomposeTransform();   
    mat4 CalculateWorldTransform();
    mat4 WorldToLocalTransform(GameObject* GO, mat4 modifiedWorldTransform);
    void RotateOMEGALUL(const vec3& eulerAngles, const HandleSpace& space);
    void ExtractBasis(const glm::mat4& transformMatrix, glm::mat3& basis);
    glm::vec3 ChangeBasis(const glm::vec3& rotationVectorA, const glm::mat3& basisA, const glm::mat3& basisB);

    void UpdateCameraIfPresent();
    

    // Get / Set
    // Traverse the hierarchy to multiply the transformation matrices of parent objects

	vec3 GetForward() const;
	vec3 GetUp() const;
	vec3 GetRight() const;

    void SetRight(vec3 newRight);
    void SetUp(vec3 newUp);
    void SetForward(vec3 newForward);

	vec3 GetPosition() const;
	quat GetRotation() const;
	vec3 GetScale() const;

    mat4 GetTransform() const;
    void SetTransform(mat4 transform);

    vec3 GetRotationEuler() const;
    


    // Transform -----------------------------------------------------  
    /*void rotate(const vec3& axis, double angle, bool local = true);
    void rotate(const vec3& eulerAngles, bool local = true);
    void scaleBy(const vec3& scaling, bool local = true);*/

    // Get / Set -----------------------------------------------------
//    vec3 getForward(); // Gets forward vector   
//    vec3 getUp();      // Gets up vector  
//    vec3 getRight();   // Gets right vector
//
//    // Gets Transformation model Matrix
//    mat4 getMatrix();
//    void updateMatrix();
//    
//
//    vec3 getPosition() const;
//    void setPosition(const vec3& newPosition); // Sets position in global space
//   
//    quat getRotation() const;      // Gets global rotation in quaternion   
//    quat getLocalRotation() const; // Gets local rotation in quaternion
//   
//    vec3 getEulerAngles() const;       // Gets global rotation of the object in Euler Angles   
//    vec3 getLocalEulerAngles() const;  // Gets local rotation of the object in Euler Angles
//    //void setRotation(const vec3f& newRotation); // Sets rotation 
//    void setRotation(const vec3& newRotation); // Sets rotation 
//
//    vec3 getScale() const;
//    void setScale(const vec3& newScale); // Sets Scale
//
//    quat EulerAnglesToQuaternion(const vec3& eulerAngles);    // Converts Euler Angles to Quaternion

public:

    json SaveComponent();
    void LoadComponent(const json& transformJSON);

private:

    mat4 transformMatrix;

    vec3 position;
    quat rotation;
    vec3 scale;
};

#endif //__TRANSFORM_H__