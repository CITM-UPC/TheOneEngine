#pragma once

#include <vector>
#include <memory>
#include <string>

class GameObject;
class Transform;

class __declspec(dllexport) CPPScript
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
	bool active = true;
	std::string name;
};