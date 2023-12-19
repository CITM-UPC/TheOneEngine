#include "GameObject.h"
#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "UIDGen.h"

#include "Math.h"
#include "BBox.hpp"

GameObject::GameObject(std::string name)
	: name(name),
	parent(),
	children(),
	components(),
	enabled(true),
	isStatic(false),
	index(-1)
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
}

void GameObject::Draw()
{
	for (const auto& component : components)
	{
		if (component && component->IsEnabled())
			component->DrawComponent();
		
	}

	drawAABBox(aabb());
	if (_graphic.get()) _graphic->draw();
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

AABBox GameObject::aabb() const {
	AABBox aabbox;
	if (_graphic.get()) aabbox = _graphic->aabb;
	else if (children.empty()) {
		aabbox.min = vec3(0);
		aabbox.max = vec3(0);
	}

	for (const auto& child : children) {
		const auto child_aabb = (child.transform() * child.aabb()).AABB();
		aabbox.min = glm::min(aabbox.min, child_aabb.min);
		aabbox.max = glm::max(aabbox.max, child_aabb.max);
	}

	return aabbox;
}

static inline void glVec3(const vec3& v) { glVertex3dv(&v.x); }

static void drawAABBox(const AABBox& aabb) {
	glLineWidth(2);
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
}

//GameObject::GameObject() : _transform(glm::identity<mat4>()) {}
//GameObject::GameObject(std::shared_ptr<Graphic> graphic) : _transform(glm::identity<mat4>()), _graphic(graphic) {}
//
//void GameObject::rotate(double degrees, const vec3& axis) {
//	_transform = glm::rotate(_transform, glm::radians(degrees), axis);
//}
//
//void GameObject::translate(const vec3& dv) {
//	_transform = glm::translate(_transform, dv);
//}