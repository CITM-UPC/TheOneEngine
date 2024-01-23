#ifndef __SCRIPT_H__
#define __SCRIPT_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"

#include <vector>
#include <memory>
#include <string>

struct __declspec(dllexport) InspectorScriptData {
	friend class Script;
	enum DataType {
		INT, // DONE
		INT2,
		INT3,
		FLOAT, // DONE
		FLOAT2,
		FLOAT3,
		STRING,
		BOOL, // DONE
		PREFAB, // DONE
		GAMEOBJECT, // DONE
	};

	enum ShowMode {
		NONE,
		INPUT_INT,
		DRAGABLE_INT,
		SLIDER_INT,
		INPUT_FLOAT,
		DRAGABLE_FLOAT,
		SLIDER_FLOAT,
		CHECKBOX,
	};

	InspectorScriptData(const std::string& variable_name, const DataType& variable_type, void* ptr, const ShowMode& mode) {
		this->variable_name = variable_name;
		this->variable_type = variable_type;
		this->ptr = ptr;
		this->show_as = mode;
	}

	std::string variable_name;
	DataType variable_type;
	ShowMode show_as;
	void* ptr = nullptr;

	GameObject** parent = nullptr;
private:
	float min_slider = 0;
	float max_slider = 0;
};

class __declspec(dllexport) Script : public Component
{
    friend class Transform;
    friend class Mesh;
    friend class Camera;
    friend class Texture;
public:

    Script(std::shared_ptr<GameObject> containerGO);
    virtual ~Script();

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

	GameObject* Instance = nullptr;

	std::function<void()> InstantiateFunction;
	std::function<void()> DestroyInstanceFunction;

	std::function<void(GameObject*)> StartFunction;
	std::function<void(GameObject*)> PreUpdateFunction;
	std::function<void(GameObject*, double)> UpdateFunction;
	std::function<void(GameObject*)> PostUpdateFunction;

	//Historn: Try to create a script in the DLL Like TankMovement.h/.cpp and add functionality
	template<typename T>
	void Bind()
	{
		InstantiateFunction = [&]() { Instance = new T(); };
		DestroyInstanceFunction = [&]() { delete (T*)Instance; Instance = nullptr; };

		StartFunction = [](GameObject* instance) { ((T*)instance)->OnStart(); };
		PreUpdateFunction = [](GameObject* instance) { ((T*)instance)->OnPreUpdate(); };
		UpdateFunction = [](GameObject* instance, double dt) { ((T*)instance)->OnUpdate(dt); };
		PostUpdateFunction = [](GameObject* instance) { ((T*)instance)->OnPostUpdate(); };
	}

public:
    bool active;
private:
    std::string path;
	InspectorScriptData scriptData;
};
#endif // !__SCRIPT_H__