#ifndef __SCRIPTINGTRANSFORM_H__
#define __SCRIPTINGTRANSFORM_H__

struct lua_State;
namespace luabridge
{
	class LuaRef;
}

class ScriptingTransform {
public:
	ScriptingTransform() {};
	~ScriptingTransform() {};

	luabridge::LuaRef	GetPosition(unsigned int go_UUID, lua_State* lua_state) const;
	void				SetPosition(unsigned int go_UUID, float x, float y, float z);
	void				Translate(unsigned int go_UUID, float x, float y, float z);

	luabridge::LuaRef	GetRotation(unsigned int go_UUID, lua_State* lua_state) const;
	void				SetRotation(unsigned int go_UUID, float x, float y, float z);
	void				Rotate(unsigned int go_UUID, float x, float y, float z);
	void				RotateOnAxis(unsigned int go_UUID, float axis_x, float axis_y, float axis_z, float angle);
};

#endif