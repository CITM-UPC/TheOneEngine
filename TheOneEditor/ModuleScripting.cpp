#include "ModuleScripting.h"
#include "ScriptData.h"
#include "luadefs.h"
#include "../TheOneEngine/ComponentScript.h"


Scripting::Scripting(App* app) : Module(app){}

Scripting::~Scripting() {}

void Scripting::Init() {
	luastate_ = luaL_newstate();
	luaL_openlibs(luastate_);
}

bool Scripting::CleanUp() {
	for (ScriptData* it : instances_)
		if (it) delete it;
	current_script_ = nullptr;
	return true;
}

bool Scripting::Update(double dt) {
	//TODO: Check if we are in play mode and call every script update/start
	//TODO: Update inspector variables
	return true;
}

void Scripting::CreateScript(ComponentScript* component) {
	//TODO: Create a new instance from component's script path
}

const ScriptData* Scripting::GetCurrentScript() const {
	return current_script_;
}

void Scripting::CompileScriptTable(ScriptData* script) {
	//TODO: Create function table and pass it onto script
}
