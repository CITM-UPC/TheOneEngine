#ifndef __MODULE_SCRIPTING_H__
#define __MODULE_SCRIPTING_H__

#include "Module.h"
#include <vector>
#include <memory>

// LuaBridge's Lua Virtual Machine Class
struct lua_State;
class ComponentScript;
struct ScriptData;

class Scripting : public Module {
public:
	// Constructor and destructor
	Scripting(App* app);
	virtual ~Scripting();

	// Regular module functions 
	bool Awake() override;
	bool CleanUp() override;
	bool Update(double dt) override;

	// Own functions
	void CreateScript(ComponentScript* component);
	const std::weak_ptr<ScriptData> GetCurrentScript() const;

private:
	void PopulateLuaState();
	void CompileScriptTable(ScriptData* script);

private:
	lua_State* luastate_= nullptr;
	std::vector<std::weak_ptr<ScriptData>> instances_;
	std::weak_ptr<ScriptData> current_script_;
	bool stopped = true;




};

#endif