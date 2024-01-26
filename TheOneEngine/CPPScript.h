#pragma once

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"

#include <vector>
#include <memory>
#include <string>

class CPPScript
{
	friend class GameObject;
	friend class Script;
public:

	CPPScript();
	virtual ~CPPScript();

	virtual void Awake() {}
	virtual void Start() {}

	virtual void PreUpdate() {}
	virtual void Update(double dt) {}
	virtual void PostUpdate() {}

	virtual void OnDisable() {}
	virtual void OnEnable() {}

	virtual void CleanUp() {}

public:
	std::weak_ptr<GameObject> containerGO;
	Transform* transform = nullptr;
	Mesh* mesh = nullptr;
	Camera* camera = nullptr;
	bool active;
	std::string name;
};