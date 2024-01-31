#ifndef __MODULE_SCRIPTING_H__
#define __MODULE_SCRIPTING_H__

#include "Module.h"
#include <vector>

// LuaBridge's Lua Virtual Machine Class
class lua_State;
class ComponentScript;
struct ScriptData;

class Scripting : public Module {
public:
	// Constructor and destructor
	Scripting(App* app);
	virtual ~Scripting();

	// Regular module functions 
	void Init();
	bool CleanUp() override;
	bool Update(double dt) override;

	// Own functions
	void CreateScript(ComponentScript* component);
	const ScriptData* GetCurrentScript() const;

private:
	void PopulateLuaState();
	void CompileScriptTable(ScriptData* script);

private:
	lua_State* luastate_= nullptr;
	std::vector<ScriptData*> instances_;
	ScriptData* current_script_ = nullptr;




};

#endif