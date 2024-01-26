#pragma once

class GameObject;
class Component;
class Transform;
class Camera;
class Mesh;
class Texture;

class __declspec(dllexport) CPPScript
{
	friend class Transform;
	friend class Mesh;
	friend class Camera;
	friend class Texture;
	friend class GameObject;
	friend class Component;
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
	GameObject* parent = nullptr;
};