#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "UIDGen.h"
#include "BBox.hpp"


#include "../TheOneEditor/SceneManager.h"

#include "Math.h"


GameObject::GameObject(std::string name) :
	name(name),
	parent(),
	children(),
	components(),
	enabled(true),
	isStatic(false),
	aabb()
{
	// hekbas - shared_from_this() must NOT be called in the constructor!!!
	// uncomenting the following line causes undefined behaviour
	//AddComponent(ComponentType::Transform);

	Enable();
}


GameObject::~GameObject() {}

void GameObject::Update(double dt)
{
	// Update enabled Components
	for (const auto& component : components)
	{
		if (component && component->IsEnabled())
			component->Update();
	}

	// Update childs
	for (const auto& child : children)
		child->Update(dt);

	// Recalculate AABBs
	aabb = CalculateAABB();
}

void GameObject::Draw()
{
	for (const auto& component : components)
	{
		if (component && component->IsEnabled())
			component->DrawComponent();
	}

	//if (drawAABB)
		DrawAABB();
}


// Component ----------------------------------------
void GameObject::RemoveComponent(ComponentType type)
{
	for (auto it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == type)
		{
			it = components.erase(it);
			break;
		}
	}
}

//std::vector<std::shared_ptr<Component>>& GameObject::GetComponents()
//{
//	return components;
//}


// AABB -------------------------------------
void GameObject::GenerateAABBFromMesh()
{
	Mesh* mesh = GetComponent<Mesh>();

	glGenBuffers(1, &mesh->mesh.vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->mesh.vertex_buffer_id);

	switch (mesh->mesh.format)
	{
	case Formats::F_V3:
		glBufferData(GL_ARRAY_BUFFER, sizeof(V3) * mesh->mesh.numVerts, mesh->meshData.vertex_data.data(), GL_STATIC_DRAW);
		for (const auto& v : std::span((V3*)mesh->meshData.vertex_data.data(), mesh->meshData.vertex_data.size())) {
			aabb.min = glm::min(aabb.min, vec3(v.v));
			aabb.max = glm::max(aabb.max, vec3(v.v));
		}
		break;

	case Formats::F_V3C4:
		glBufferData(GL_ARRAY_BUFFER, sizeof(V3C4) * mesh->mesh.numVerts, mesh->meshData.vertex_data.data(), GL_STATIC_DRAW);
		for (const auto& v : std::span((V3C4*)mesh->meshData.vertex_data.data(), mesh->meshData.vertex_data.size())) {
			aabb.min = glm::min(aabb.min, vec3(v.v));
			aabb.max = glm::max(aabb.max, vec3(v.v));
		}
		break;

	case Formats::F_V3T2:
		glBufferData(GL_ARRAY_BUFFER, sizeof(V3T2) * mesh->mesh.numVerts, mesh->meshData.vertex_data.data(), GL_STATIC_DRAW);
		for (const auto& v : std::span((V3T2*)mesh->meshData.vertex_data.data(), mesh->meshData.vertex_data.size())) {
			aabb.min = glm::min(aabb.min, vec3(v.v));
			aabb.max = glm::max(aabb.max, vec3(v.v));
		}
		break;
	}

	//GLenum error = glGetError();
	//if (error != GL_NO_ERROR) {
	//    // Print the raw error code
	//    fprintf(stderr, "OpenGL error code: %d\n", error);
	//
	//    // Print the corresponding error string
	//    const char* errorString = reinterpret_cast<const char*>(gluErrorString(error));
	//    fprintf(stderr, "OpenGL error: %s\n", errorString ? errorString : "Unknown");
	//
	//    assert(false); // Trigger an assertion failure for debugging
	//}

}

AABBox GameObject::CalculateAABB()
{
	AABBox aabb = this->aabb;

	for (const auto& child : children)
	{
		AABBox childAABB = (child.get()->GetComponent<Transform>()->GetTransform() * child.get()->CalculateAABB()).AABB();
		aabb.min = glm::min(aabb.min, childAABB.min);
		aabb.max = glm::max(aabb.max, childAABB.max);
	}

	return aabb;
}

AABBox GameObject::CalculateAABBWithChildren()
{
	AABBox aabb = CalculateAABB();
	// Transform the AABB to the coordinate system of the parent objects
	mat4 parentTransform = GetComponent<Transform>()->CalculateWorldTransform();
	OBBox obb = parentTransform * aabb;
	return obb.AABB();
}

static inline void glVec3(const vec3& v) { glVertex3dv(&v.x); }

void GameObject::DrawAABB()
{
	glLineWidth(1);
	glColor4ub(255, 255, 255, 64);

	glBegin(GL_LINE_STRIP);
	glVec3(aabb.a());
	glVec3(aabb.b());
	glVec3(aabb.c());
	glVec3(aabb.d());
	glVec3(aabb.a());

	glVec3(aabb.e());
	glVec3(aabb.f());
	glVec3(aabb.g());
	glVec3(aabb.h());
	glVec3(aabb.e());
	glEnd();

	glBegin(GL_LINES);
	glVec3(aabb.h());
	glVec3(aabb.d());
	glVec3(aabb.f());
	glVec3(aabb.b());
	glVec3(aabb.g());
	glVec3(aabb.c());
	glEnd();

	glColor4ub(255, 255, 255, 255);
}

void GameObject::DrawOBB()
{
	glColor3f(1, 0, 1);
	glLineWidth(2);
	vec3f* obb_points = nullptr;
	//obb.GetCornerPoints(obb_points);
	//
	//glBegin(GL_LINES);
	//
	//glVertex3f(obb.CornerPoint(0).x, obb.CornerPoint(0).y, obb.CornerPoint(0).z);
	//glVertex3f(obb.CornerPoint(1).x, obb.CornerPoint(1).y, obb.CornerPoint(1).z);
	//
	//glVertex3f(obb.CornerPoint(0).x, obb.CornerPoint(0).y, obb.CornerPoint(0).z);
	//glVertex3f(obb.CornerPoint(4).x, obb.CornerPoint(4).y, obb.CornerPoint(4).z);
	//
	//glVertex3f(obb.CornerPoint(0).x, obb.CornerPoint(0).y, obb.CornerPoint(0).z);
	//glVertex3f(obb.CornerPoint(2).x, obb.CornerPoint(2).y, obb.CornerPoint(2).z);
	//
	//glVertex3f(obb.CornerPoint(2).x, obb.CornerPoint(2).y, obb.CornerPoint(2).z);
	//glVertex3f(obb.CornerPoint(3).x, obb.CornerPoint(3).y, obb.CornerPoint(3).z);
	//
	//glVertex3f(obb.CornerPoint(1).x, obb.CornerPoint(1).y, obb.CornerPoint(1).z);
	//glVertex3f(obb.CornerPoint(5).x, obb.CornerPoint(5).y, obb.CornerPoint(5).z);
	//
	//glVertex3f(obb.CornerPoint(1).x, obb.CornerPoint(1).y, obb.CornerPoint(1).z);
	//glVertex3f(obb.CornerPoint(3).x, obb.CornerPoint(3).y, obb.CornerPoint(3).z);
	//
	//glVertex3f(obb.CornerPoint(4).x, obb.CornerPoint(4).y, obb.CornerPoint(4).z);
	//glVertex3f(obb.CornerPoint(5).x, obb.CornerPoint(5).y, obb.CornerPoint(5).z);
	//
	//glVertex3f(obb.CornerPoint(4).x, obb.CornerPoint(4).y, obb.CornerPoint(4).z);
	//glVertex3f(obb.CornerPoint(6).x, obb.CornerPoint(6).y, obb.CornerPoint(6).z);
	//
	//glVertex3f(obb.CornerPoint(2).x, obb.CornerPoint(2).y, obb.CornerPoint(2).z);
	//glVertex3f(obb.CornerPoint(6).x, obb.CornerPoint(6).y, obb.CornerPoint(6).z);
	//
	//glVertex3f(obb.CornerPoint(5).x, obb.CornerPoint(5).y, obb.CornerPoint(5).z);
	//glVertex3f(obb.CornerPoint(7).x, obb.CornerPoint(7).y, obb.CornerPoint(7).z);
	//
	//glVertex3f(obb.CornerPoint(6).x, obb.CornerPoint(6).y, obb.CornerPoint(6).z);
	//glVertex3f(obb.CornerPoint(7).x, obb.CornerPoint(7).y, obb.CornerPoint(7).z);
	//
	//glVertex3f(obb.CornerPoint(3).x, obb.CornerPoint(3).y, obb.CornerPoint(3).z);
	//glVertex3f(obb.CornerPoint(7).x, obb.CornerPoint(7).y, obb.CornerPoint(7).z);
	//
	//glLineWidth(1);
	//glEnd();
}


// Get/Set ------------------------------------------
bool GameObject::IsEnabled() const
{
	return enabled;
}

void GameObject::Enable()
{
	enabled = true;

	for (const auto& component : components)
		component->Enable();

	for (const auto& child : children)
		child->Enable();
}

void GameObject::Disable()
{
	enabled = false;

	for (const auto& component : components)
		component->Disable();

	for (const auto& child : children)
		child->Disable();
}

void GameObject::Delete()
{
	for (const auto& component : components)
		component.get_deleter();

	for (const auto& child : children)
		child.~shared_ptr();
}

std::string GameObject::GetName() const
{
	return name;
}

void GameObject::SetName(const std::string& newName)
{
	name = newName;
}

bool GameObject::IsStatic() const
{
	return isStatic;
}

void GameObject::SetStatic(bool staticFlag)
{
	isStatic = staticFlag;
}

void GameObject::CreateUID()
{
	UID = UIDGen::GenerateUID();
}


// Save/Load ------------------------------------------
json GameObject::SaveGameObject()
{
	json gameObjectJSON;

	if (auto pGO = parent.lock())
	{
		gameObjectJSON["ParentUID"] = pGO.get()->UID;
	}

	gameObjectJSON["UID"] = UID;
	gameObjectJSON["Name"] = name;
	gameObjectJSON["Static"] = isStatic;
	gameObjectJSON["Enabled"] = enabled;

	if (!components.empty())
	{
		json componentsJSON;

		for (const auto& component : components)
		{
			componentsJSON.push_back(component.get()->SaveComponent());
		}
		gameObjectJSON["Components"] = componentsJSON;
	}

	if (!children.empty())
	{
		json childrenGOJSON;

		for (const auto& go : children)
		{
			childrenGOJSON.push_back(go.get()->SaveGameObject());
		}
		gameObjectJSON["GameObjects"] = childrenGOJSON;
	}

	return gameObjectJSON;
}

void GameObject::LoadGameObject(const json& gameObjectJSON)
{
	// Load basic properties
	if (gameObjectJSON.contains("UID"))
	{
		UID = gameObjectJSON["UID"];
	}

	if (gameObjectJSON.contains("Name"))
	{
		name = gameObjectJSON["Name"];
	}

	if (gameObjectJSON.contains("Static"))
	{
		isStatic = gameObjectJSON["Static"];
	}

	if (gameObjectJSON.contains("Enabled"))
	{
		enabled = gameObjectJSON["Enabled"];
	}

	// Load components
	if (gameObjectJSON.contains("Components"))
	{
		const json& componentsJSON = gameObjectJSON["Components"];

		for (const auto& componentJSON : componentsJSON)
		{

			// Assuming each component has a LoadComponent function
			if (componentJSON["Type"] == 0)
			{
				this->AddComponent<Transform>();
				this->GetComponent<Transform>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == 1)
			{
				this->AddComponent<Camera>();
				this->GetComponent<Camera>()->LoadComponent(componentJSON);
			}
			else if (componentJSON["Type"] == 2)
			{
				this->AddComponent<Mesh>();
				this->GetComponent<Mesh>()->LoadComponent(componentJSON);
			}
		}
	}

	// Load child game objects
	if (gameObjectJSON.contains("GameObjects"))
	{
		const json& childrenGOJSON = gameObjectJSON["GameObjects"];

		for (const auto& childJSON : childrenGOJSON)
		{
			auto childGameObject = std::make_shared<GameObject>("Empty GO");
			childGameObject->LoadGameObject(childJSON);

			// Add the loaded child game object to the current game object
			childGameObject.get()->parent = this->weak_from_this().lock();
			this->children.emplace_back(childGameObject);
		}
	}
}