#ifndef __SCRIPTDATA_H__
#define __SCRIPTDATA_H__

#include "../TheOneEngine/ComponentScript.h"
#include "luadefs.h"

struct ScriptData {
	ComponentScript* owner = nullptr;
	luabridge::LuaRef table_class = 0;
	bool started = false;
};
#endif