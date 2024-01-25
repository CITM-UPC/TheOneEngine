#ifndef __SCRIPT_H__
#define __SCRIPT_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"

#include <vector>
#include <memory>
#include <string>

//struct __declspec(dllexport) InspectorScriptData {
//	friend class Script;
//	enum DataType {
//		INT, // DONE
//		INT2,
//		INT3,
//		FLOAT, // DONE
//		FLOAT2,
//		FLOAT3,
//		STRING,
//		BOOL, // DONE
//		PREFAB, // DONE
//		GAMEOBJECT, // DONE
//	};
//
//	enum ShowMode {
//		NONE,
//		INPUT_INT,
//		DRAGABLE_INT,
//		SLIDER_INT,
//		INPUT_FLOAT,
//		DRAGABLE_FLOAT,
//		SLIDER_FLOAT,
//		CHECKBOX,
//	};
//
//	InspectorScriptData(const std::string& variable_name, const DataType& variable_type, void* ptr, const ShowMode& mode) {
//		this->variable_name = variable_name;
//		this->variable_type = variable_type;
//		this->ptr = ptr;
//		this->show_as = mode;
//	}
//
//	std::string variable_name;
//	DataType variable_type;
//	ShowMode show_as;
//	void* ptr = nullptr;
//
//	GameObject** parent = nullptr;
//private:
//	float min_slider = 0;
//	float max_slider = 0;
//};

class __declspec(dllexport) Script : public Component
{
    friend class Transform;
    friend class Mesh;
    friend class Camera;
    friend class Texture;
    friend class GameObject;
    friend class Component;
public:

    Script(std::shared_ptr<GameObject> containerGO);
    virtual ~Script();

	virtual void Awake() {}
	virtual void Start() {}

	virtual void PreUpdate() {}
	virtual void Update(double dt) {}
	virtual void PostUpdate() {}

	virtual void OnDisable() {}
	virtual void OnEnable() {}

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

	/*GameObject* Instance = nullptr;

	GameObject*(*InstantiateScript)();
	void (*DestroyScript)(Script*);*/

	//Historn: Try to create a script in the DLL Like TankMovement.h/.cpp and add functionality
	/*template<typename T>
	void Bind()
	{
		InstantiateScript = []() { return new T(); };
		DestroyScript = [](Script* script) { delete script->Instance; script->Instance = nullptr; };
	}*/

public:
    bool active;
private:
    std::string path;
	//InspectorScriptData scriptData;
};
#endif // !__SCRIPT_H__