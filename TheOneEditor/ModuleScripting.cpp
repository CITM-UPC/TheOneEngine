#include "ModuleScripting.h"
#include "../TheOneEngine/ScriptData.h"
#include "../TheOneEngine/luadefs.h"
#include "../TheOneEngine/ComponentScript.h"
#include "App.h"

//Scripting functions
#include "ScriptingTransform.h"
#include "ScriptingGameObject.h"
#include "ScriptingInput.h"


Scripting::Scripting(App* app) : Module(app){}

Scripting::~Scripting() {}

bool Scripting::Awake() {
	luastate_ = luaL_newstate();
	luaL_openlibs(luastate_);
	PopulateLuaState();

	return true;
}

bool Scripting::CleanUp() {
	current_script_.reset();
	instances_.clear();
	return true;
}

bool Scripting::Update(double dt) {
	//TODO: Update inspector variables
	if (app->IsPlaying()) {
		stopped = false;
		for (auto it = instances_.begin(); it != instances_.end();) {
			std::shared_ptr<ScriptData> script = (*it).lock();
			if (!script) {
				it = instances_.erase(it);
				continue;
			}
			current_script_ = script;
			if (!script->awoken) { // We wake the script regardless of it being active or not
				script->table_class["Awake"]();
				script->awoken = true;
			}
			else if (script->owner->IsEnabled()) { 
				if (!script->started) {
					script->table_class["Start"]();
					script->started = true;
				}
				else
					script->table_class["Update"]();
			}
			++it;
		}
	}
	// Set Scripts to not started so we call start again
	else if (!app->IsInGameState() && !stopped) {
		stopped = true;
		for (auto it = instances_.begin(); it != instances_.end();) {
			std::shared_ptr<ScriptData> script = (*it).lock();
			if (!script) {
				it = instances_.erase(it);
				continue;
			}
			script->started = false;
		}
	}
	return true;
}

void Scripting::CreateScript(ComponentScript* component) {
	instances_.push_back(component->data);
	CompileScriptTable(component->data.get());
}

const std::weak_ptr<ScriptData> Scripting::GetCurrentScript() const {
	return current_script_;
}

void Scripting::PopulateLuaState() {
	luabridge::getGlobalNamespace(luastate_)
		.beginNamespace("Scripting")
		// Transform Scripting
		.beginClass<ScriptingTransform>("Transform")
		.addConstructor<void(*) (void)>()
		.addFunction("GetPosition", &ScriptingTransform::GetPosition)
		.addFunction("SetPosition", &ScriptingTransform::SetPosition)
		.addFunction("Translate", &ScriptingTransform::Translate)
		.addFunction("GetRotation", &ScriptingTransform::GetRotation)
		.addFunction("SetRotation", &ScriptingTransform::SetRotation)
		.addFunction("Rotate", &ScriptingTransform::Rotate)
		.addFunction("RotateOnAxis", &ScriptingTransform::RotateOnAxis)
		.endClass()
		// GameObject Scripting
		.beginClass<ScriptingGameObject>("GameObject")
		.addConstructor<void(*) (void)>()
		.addFunction("GetMyUID", &ScriptingGameObject::GetMyUID)
		.endClass()
		// Input Scripting
		.beginClass<ScriptingInput>("Input")
		.addConstructor<void(*) (void)>()
		.addFunction("GetKeyState", &ScriptingInput::GetKeyState)
		.addFunction("IsKeyDown", &ScriptingInput::IsKeyDown)
		.addFunction("IsKeyUp", &ScriptingInput::IsKeyUp)
		.addFunction("IsKeyRepeat", &ScriptingInput::IsKeyRepeat)
		.addFunction("IsKeyIdle", &ScriptingInput::IsKeyIdle)
		.addFunction("GetMBState", &ScriptingInput::GetMBState)
		.addFunction("IsMBDown", &ScriptingInput::IsMBDown)
		.addFunction("IsMBUp", &ScriptingInput::IsMBUp)
		.addFunction("IsMBRepeat", &ScriptingInput::IsMBRepeat)
		.addFunction("ISMBIdle", &ScriptingInput::ISMBIdle)
		.addFunction("GetMouseMovementX", &ScriptingInput::GetMouseMovementX)
		.addFunction("GetMouseMovementY", &ScriptingInput::GetMouseMovementY)
		.endClass()
		.endNamespace();
}

void Scripting::CompileScriptTable(ScriptData* script) {
	if (luastate_) {
		int compiled = luaL_dofile(luastate_, script->path.c_str());
		if (compiled == LUA_OK) {
			std::string get_table_name = "GetTable" + script->name;
			luabridge::LuaRef get_table = luabridge::getGlobal(luastate_, get_table_name.c_str());

			if (!get_table.isNil()) {
				luabridge::LuaRef script_table(get_table());
				script->table_class = script_table;
			}
		} else {
			std::string error = lua_tostring(luastate_, -1);
			LOG(LogType::LOG_ERROR, "Failed to compile script %s, error: %s", script->name.data(), error.data());
		}
	}
}
