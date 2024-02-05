#ifndef __SCRIPTDATA_H__
#define __SCRIPTDATA_H__

#include "ComponentScript.h"
#include "luadefs.h"

struct ScriptData {
	ComponentScript* owner = nullptr;
	luabridge::LuaRef table_class = 0;
	bool started = false;
	bool awoken = false;
	std::string name;
	std::string path;
};
#endif