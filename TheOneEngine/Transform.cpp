#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"

Transform::Transform(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Transform),
    transformMatrix(1.0f),
    position(0.0f),
    rotation(1, 0, 0, 0),
    scale(1.0f)
{}

Transform::Transform(std::shared_ptr<GameObject> containerGO, mat4 transform) : Component(containerGO, ComponentType::Transform),
    transformMatrix(transform)
{
    DecomposeTransform();
}

Transform::~Transform() {}


// Transform ----------------------------------------------------

void Transform::Translate(const vec3& translation, const HandleSpace& space)
{
    if (space == HandleSpace::GLOBAL)
        transformMatrix = CalculateWorldTransform();

    transformMatrix = glm::translate(transformMatrix, translation);
    position = transformMatrix[3];

    UpdateCameraIfPresent();
}

// void Transform::SetPosition(const vec3& newPosition, const HandleSpace& space)
// {
// 	if (space == HandleSpace::GLOBAL)
// 		transformMatrix;
//
// 	transformMatrix[3] = glm::vec4(newPosition, 1.0f);
// 	position = transformMatrix[3];
//
// 	UpdateCameraIfPresent();
// }

//void Transform::SetPosition(const vec3& newPosition, const HandleSpace& space)
//{
//    mat4 transform = transformMatrix;
//    if (space == HandleSpace::GLOBAL)
//        transform = CalculateWorldTransform();
//
//    transform[3] = glm::vec4(newPosition, 1.0f);
//
//	if (space == HandleSpace::GLOBAL)
//        transformMatrix = WorldToLocalTransform(CalculateWorldTransform(), transform);
//
//    position = transformMatrix[3];
//
//    UpdateCameraIfPresent();
//}

void Transform::SetPosition(const vec3& newPosition, const HandleSpace& space)
{
    mat4 transform = space == HandleSpace::GLOBAL ? CalculateWorldTransform() : transformMatrix;
    transform[3] = glm::vec4(newPosition, 1.0f);

    if (space == HandleSpace::GLOBAL)
        transformMatrix = WorldToLocalTransform(containerGO.lock().get(), transform);

    position = transformMatrix[3];

    UpdateCameraIfPresent();
}

// Function to apply transformation to object B in world coordinates and then adjust its transform to be relative to A
glm::mat4 applyAndAdjustTransformationRelativeToA(glm::mat4 worldTransformA, glm::mat4 transformB, glm::mat4 modification)
{
    // Calculate the new world transformation for B
    glm::mat4 worldTransformB = worldTransformA * transformB;

    // Apply the modification to the world transformation of B
    glm::mat4 modifiedWorldTransformB = worldTransformB * modification;

    // Calculate the inverse transformation matrix of A
    glm::mat4 inverseTransformA = glm::inverse(worldTransformA);

    // Adjust the modified transform to be relative to A
    glm::mat4 finalTransformB = modifiedWorldTransformB * inverseTransformA;

    return finalTransformB;
}

void Transform::Rotate(const vec3& eulerAngles, const HandleSpace& space)
{
	// Get global or local transform
	if (space == HandleSpace::GLOBAL)
		transformMatrix = CalculateWorldTransform();

	// Extract rotation from matrix
	const glm::mat3 rotMtx(
		vec3(transformMatrix[0]) / scale[0],
		vec3(transformMatrix[1]) / scale[1],
		vec3(transformMatrix[2]) / scale[2]);

	// Convert rotation to quat
	rotation = glm::normalize(glm::quat_cast(rotMtx));

	// Set rotation AngleAxis from quat
	vec3 rotationEuler = glm::radians(glm::eulerAngles(rotation));

	quat quaternion = glm::angleAxis(glm::radians(eulerAngles.x), vec3(1, 0, 0));
	rotation = glm::normalize(quaternion * rotation);
	quaternion = glm::angleAxis(glm::radians(eulerAngles.y), vec3(0, 1, 0));
	rotation = glm::normalize(quaternion * rotation);
	quaternion = glm::angleAxis(glm::radians(eulerAngles.z), vec3(0, 0, 1));
	rotation = glm::normalize(quaternion * rotation);

	transformMatrix = glm::translate(mat4(1.0f), position) * glm::mat4_cast(rotation) * glm::scale(mat4(1.0f), scale);
}

void Transform::SetRotation(const vec3& eulerAngles, const HandleSpace& space)
{
    // Get global or local transform
    if (space == HandleSpace::GLOBAL)
        transformMatrix = CalculateWorldTransform();

    // Extract rotation from matrix
	const glm::mat3 rotMtx(
		vec3(transformMatrix[0]) / scale[0],
		vec3(transformMatrix[1]) / scale[1],
		vec3(transformMatrix[2]) / scale[2]);
    
    // Convert rotation to quat
	rotation = glm::normalize(glm::quat_cast(rotMtx));

    // vec3(x, y, z)


    // glm::radians

    // Set rotation AngleAxis from quat
    vec3 rotationEuler = glm::eulerAngles(rotation);

	quat quaternion = glm::angleAxis(eulerAngles.x, vec3(1, 0, 0));
    rotation = glm::normalize(quaternion);
	quaternion = glm::angleAxis(eulerAngles.y, vec3(0, 1, 0));
	rotation = glm::normalize(quaternion * rotation);
	quaternion = glm::angleAxis(eulerAngles.z, vec3(0, 0, 1));
	rotation = glm::normalize(quaternion * rotation);

	transformMatrix = glm::translate(mat4(1.0f), position) * glm::mat4_cast(rotation) * glm::scale(mat4(1.0f), scale);


 //   glm::angleAxis;
	//glm::eulerAngles()

 //   //Global
 //   double X = axisX.clone();
 //   double Y = axisY.clone();
 //   double Z = axisZ.clone();

 //   //Local
 //   X = X.applyEuler(cube.rotation);
 //   Y = Y.applyEuler(cube.rotation);
 //   Z = Z.applyEuler(cube.rotation);

 //   if (keys["w"]) {
 //       var quaternion = new THREE.Quaternion();
 //       quaternion.setFromAxisAngle(X, -0.05);
 //       cube.applyQuaternion(quaternion);
 //   }
 //   if (keys["s"]) {
 //       var quaternion = new THREE.Quaternion();
 //       quaternion.setFromAxisAngle(X, 0.05);
 //       cube.applyQuaternion(quaternion);
 //   }
 //   if (keys["a"]) {
 //       var quaternion = new THREE.Quaternion();
 //       quaternion.setFromAxisAngle(Y, -0.05);
 //       cube.applyQuaternion(quaternion);
 //   }
 //   if (keys["d"]) {
 //       var quaternion = new THREE.Quaternion();
 //       quaternion.setFromAxisAngle(Y, 0.05);
 //       cube.applyQuaternion(quaternion);
 //   }
 //   if (keys["q"]) {
 //       var quaternion = new THREE.Quaternion();
 //       quaternion.setFromAxisAngle(Z, 0.05);
 //       cube.applyQuaternion(quaternion);
 //   }
 //   if (keys["e"]) {
 //       var quaternion = new THREE.Quaternion();
 //       quaternion.setFromAxisAngle(Z, -0.05);
 //       cube.applyQuaternion(quaternion);
 //   }
}

//void Transform::SetRotation()
//{
//    glm::quat newRotation = ;
//
//	transformMatrix = glm::translate(
//		glm::mat4(1.0f), position) *
//		glm::mat4_cast(newRotation) *
//		scale;
//}

//hekbas, kiko - Need to fix
void Transform::Scale(const vec3& scaleFactors)
{
    transformMatrix = glm::scale(transformMatrix, scaleFactors);
    scale *= scaleFactors;
}

void Transform::SetScale(const vec3& newScale)
{
	mat4 newScaleMatrix = glm::scale(mat4(1.0f), vec3(newScale.x, newScale.y, newScale.z));
    transformMatrix = glm::translate(mat4(1.0f), position) * glm::mat4_cast(rotation) * newScaleMatrix;

    scale = newScale;
}


// Utils --------------------------------------------------------

void Transform::DecomposeTransform()
{
    position = transformMatrix[3];

    for (int i = 0; i < 3; i++)
        scale[i] = glm::length(vec3(transformMatrix[i]));

    const glm::mat3 rotMtx(
        vec3(transformMatrix[0]) / scale[0],
        vec3(transformMatrix[1]) / scale[1],
        vec3(transformMatrix[2]) / scale[2]);

    rotation = glm::quat_cast(rotMtx);
}

mat4 Transform::CalculateWorldTransform()
{
    mat4 worldTransform = transformMatrix;
    GameObject* parent = containerGO.lock().get()->parent.lock().get();

    while (parent && parent->GetComponent<Transform>())
    {
        worldTransform = parent->GetComponent<Transform>()->transformMatrix * worldTransform;
        parent = parent->parent.lock().get();
    }

    return worldTransform;
}

mat4 Transform::WorldToLocalTransform(GameObject* GO, mat4 modifiedWorldTransform)
{
	GameObject* parent = containerGO.lock().get()->parent.lock().get();

    mat4 localTransform;

	if (parent && parent->GetComponent<Transform>())
	{
		localTransform = WorldToLocalTransform(parent, modifiedWorldTransform);

		localTransform = localTransform * glm::inverse(GO->GetComponent<Transform>()->transformMatrix);
	}
    else
    {
		localTransform = modifiedWorldTransform * glm::inverse(GO->GetComponent<Transform>()->transformMatrix);
    }

	return localTransform;
}

void Transform::UpdateCameraIfPresent()
{
    Camera* camera = containerGO.lock().get()->GetComponent<Camera>();
    if (camera) { camera->UpdateCamera(); }
}


// Get/Set ------------------------------------------------------

vec3 Transform::GetRight() const
{
    return glm::normalize(transformMatrix[0]);
}

vec3 Transform::GetUp() const
{
	return glm::normalize(transformMatrix[1]);
}

vec3 Transform::GetForward() const
{
	return glm::normalize(transformMatrix[2]);
}

vec3 Transform::GetPosition() const
{
	return position;
}

quat Transform::GetRotation() const
{
	return rotation;
}

vec3 Transform::GetScale() const
{
	return scale;
}

mat4 Transform::GetTransform() const
{
	return transformMatrix;
}





#pragma region OLD_CODE
//void Transform::rotate(const vec3& axis, double angle, bool local)
//{
//    glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), axis);
//
//    if (local) {
//        localRotation = rotationQuat;
//        //localRotation = glm::normalize(localRotation);
//        localEulerAngles = glm::eulerAngles(localRotation);
//    }
//    else {
//        rotation = rotationQuat;
//        //rotation = glm::normalize(rotation);
//        this->eulerAngles = glm::eulerAngles(rotation);
//    }
//}
//
//void Transform::rotate(const vec3& eulerAngles, bool local)
//{
//    glm::quat rotationQuat = glm::quat(glm::radians(eulerAngles));
//
//    if (local) {
//        localRotation = rotationQuat;
//        //localRotation = glm::normalize(localRotation);
//        localEulerAngles = glm::eulerAngles(localRotation);
//    }
//    else {
//        rotation = rotationQuat;
//        //rotation = glm::normalize(rotation);
//        this->eulerAngles = glm::eulerAngles(rotation);
//    }
//}
//
//void Transform::scaleBy(const vec3& scaling, bool local) 
//{
//    if (local) {
//        localScale *= scaling;
//    }
//    else {
//        scale *= scaling;
//    }
//}
//
//// Get / Set ----------------------------------------------------
//vec3 Transform::getForward() 
//{
//    updateMatrix();
//    return glm::normalize(transformMatrix[2]);
//}
//
//vec3 Transform::getUp() 
//{
//    updateMatrix();
//    return glm::normalize(transformMatrix[1]);
//}
//
//vec3 Transform::getRight() 
//{
//    updateMatrix();
//    return glm::normalize(transformMatrix[0]);
//}
//
//mat4 Transform::getMatrix() 
//{
//    updateMatrix();
//    return transformMatrix;
//}
//
//void Transform::updateMatrix()
//{
//    transformMatrix = mat4(1.0f);
//    transformMatrix = glm::translate(transformMatrix, position);
//    transformMatrix *= glm::mat4_cast(rotation * localRotation);
//    transformMatrix = glm::scale(transformMatrix, localScale);
//    transformMatrix = glm::scale(transformMatrix, scale);
//}
//
//vec3 Transform::getPosition() const
//{
//    return position;
//}
//
//void Transform::setPosition(const vec3& newPosition) 
//{
//    position = newPosition;
//}
//
//quat Transform::getRotation() const
//{
//    return rotation;
//}
//
//quat Transform::getLocalRotation() const
//{
//    return localRotation;
//}
//
//vec3 Transform::getEulerAngles() const
//{
//    vec3 eulerAngles = glm::eulerAngles(rotation);
//    return eulerAngles;
//}
//
//vec3 Transform::getLocalEulerAngles() const
//{
//    vec3 eulerAngles = glm::eulerAngles(localRotation);
//    return eulerAngles;
//}
//
//void Transform::setRotation(const vec3& newRotation)
//{
//    eulerAngles = newRotation;
//    rotation = EulerAnglesToQuaternion(eulerAngles);
//}
//
//vec3 Transform::getScale() const
//{
//    return scale;
//}
//
//void Transform::setScale(const vec3& newScale)
//{
//    scale = newScale;
//}
//
//
//quat Transform::EulerAnglesToQuaternion(const vec3& eulerAngles)
//{
//    quat quaternion;
//    quaternion = glm::angleAxis(eulerAngles.z, vec3(0, 0, 1));     // Rotate around the Z-axis (yaw)
//    quaternion *= glm::angleAxis(eulerAngles.y, vec3(0, 1, 0));    // Rotate around the Y-axis (pitch)
//    quaternion *= glm::angleAxis(eulerAngles.x, vec3(1, 0, 0));    // Rotate around the X-axis (roll)
//    return quaternion;
//}
#pragma endregion OLD_CODE

json Transform::SaveComponent()
{
    json transformJSON;

    /*transformJSON["Name"] = name;
    transformJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        transformJSON["ParentUID"] = pGO.get()->GetUID();
    }
    transformJSON["UID"] = UID;
    transformJSON["Position"] = { position.x, position.y, position.z };
    transformJSON["Rotation"] = { rotation.w, rotation.x, rotation.y, rotation.z };
    transformJSON["LocalRotation"] = { localRotation.w, localRotation.x, localRotation.y, localRotation.z };
    transformJSON["Scale"] = { scale.x, scale.y, scale.z };
    transformJSON["LocalScale"] = { localScale.x, localScale.y, localScale.z };*/

    return transformJSON;
}

void Transform::LoadComponent(const json& transformJSON)
{
    //// Load basic properties
    //if (transformJSON.contains("UID"))
    //{
    //    UID = transformJSON["UID"];
    //}

    //if (transformJSON.contains("Name"))
    //{
    //    name = transformJSON["Name"];
    //}

    //// Load parent UID and set parent
    ///*if (transformJSON.contains("ParentUID"))
    //{
    //    uint32_t parentUID = transformJSON["ParentUID"];

    //    if (auto parentGameObject = SceneManager::GetInstance().FindGOByUID(parentUID))
    //    {
    //        containerGO = parentGameObject;
    //    }
    //}*/

    //// Load transformation properties
    //if (transformJSON.contains("Position"))
    //{
    //    const auto& positionArray = transformJSON["Position"];
    //    position = { positionArray[0], positionArray[1], positionArray[2] };
    //}

    //if (transformJSON.contains("Rotation"))
    //{
    //    const auto& rotationArray = transformJSON["Rotation"];
    //    rotation = quat(rotationArray[1], rotationArray[2], rotationArray[3], rotationArray[0]);
    //}

    //if (transformJSON.contains("LocalRotation"))
    //{
    //    const auto& localRotationArray = transformJSON["LocalRotation"];
    //    localRotation = quat(localRotationArray[1], localRotationArray[2], localRotationArray[3], localRotationArray[0]);
    //}

    //if (transformJSON.contains("Scale"))
    //{
    //    const auto& scaleArray = transformJSON["Scale"];
    //    scale = { scaleArray[0], scaleArray[1], scaleArray[2] };
    //}

    //if (transformJSON.contains("LocalScale"))
    //{
    //    const auto& localScaleArray = transformJSON["LocalScale"];
    //    localScale = { localScaleArray[0], localScaleArray[1], localScaleArray[2] };
    //}

    //// Update the transformation matrix
    //updateMatrix();
}
