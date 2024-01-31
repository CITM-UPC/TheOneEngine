#ifndef __SCRIPTINGTRANSFORM_H__
#define __SCRIPTINGTRANSFORM_H__

class lua_State;
namespace luabridge
{
	class LuaRef;
}

class ScriptingTransform {
public:
	ScriptingTransform() {};
	~ScriptingTransform() {};

	luabridge::LuaRef	GetPosition(unsigned int go_UUID, lua_State* lua_state);
	void				SetPosition(unsigned int go_UUID, float x, float y, float z);
	void				Translate(unsigned int go_UUID, float x, float y, float z);

	luabridge::LuaRef	GetRotation(unsigned int go_UUID, lua_State* lua_state);
	void				SetRotation(unsigned int go_UUID, float x, float y, float z);
	void				Rotate(unsigned int go_UUID, float x, float y, float z);
};

#endif