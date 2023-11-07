#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Defs.h"
#include "Asset.h"

class GameObject;

enum class ComponentType
{
	Transform,
	Mesh,
	MeshRenderer,
	Camera,
	Unknown
};

class Component
{
public:

	Component(GameObject* ContainerGO, ComponentType type);
	virtual ~Component();
	virtual void Enable();
	virtual void Disable();
	virtual void Update() {};

	ComponentType GetType() const;
	GameObject* GetContainerGameObject() const;

	bool isEnabled() const;
	
	virtual void CreateInspectorNode() = 0;
	virtual void DrawComponent() {};

protected:
	GameObject* GO;
	ComponentType type;
	bool enabled;
};

#endif // !__COMPONENT_H__