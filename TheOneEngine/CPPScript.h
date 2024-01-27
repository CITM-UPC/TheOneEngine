#pragma once

#include <vector>
#include <memory>
#include <string>

class GameObject;
class Transform;

class __declspec(dllexport) CPPScript
{
public:

	CPPScript();
	virtual ~CPPScript();

	virtual void Awake() {}
	virtual void Start() {}

	virtual void PreUpdate() {}
	virtual void Update(double dt) = 0;
	virtual void PostUpdate() {}

	virtual void OnDisable() {}
	virtual void OnEnable() {}

	virtual void CleanUp() {}

public:
	GameObject* containerGO = nullptr;
	Transform* transform = nullptr;
	bool active = true;
	std::string name;
};

typedef CPPScript* (*NewScript)();