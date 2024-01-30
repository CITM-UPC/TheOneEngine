#include "ModuleScripting.h"
#include "ScriptData.h"
#include "luadefs.h"
#include "../TheOneEngine/ComponentScript.h"
#include "App.h"


Scripting::Scripting(App* app) : Module(app){}

Scripting::~Scripting() {}

void Scripting::Init() {
	luastate_ = luaL_newstate();
	luaL_openlibs(luastate_);
}

bool Scripting::CleanUp() {
	current_script_ = nullptr;
	for (ScriptData* it : instances_)
		if (it) delete it;
	instances_.clear();
	return true;
}

bool Scripting::Update(double dt) {
	//TODO: Update inspector variables
	if (app->IsPlaying()) {
		for (ScriptData* script : instances_) {
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
		}
	}
	return true;
}

void Scripting::CreateScript(ComponentScript* component) {
	ScriptData* instance = new ScriptData;
	instance->owner = component;
	instances_.push_back(instance);
	CompileScriptTable(instance);
}

const ScriptData* Scripting::GetCurrentScript() const {
	return current_script_;
}

void Scripting::CompileScriptTable(ScriptData* script) {
	//TODO: Create function table and pass it onto script
}
