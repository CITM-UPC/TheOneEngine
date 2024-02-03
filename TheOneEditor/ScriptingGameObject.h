#ifndef __SCRIPTINGGAMEOBJECT_H__
#define __SCRIPTINGGAMEOBJECT_H__


class ScriptingGameObject {
public:
	ScriptingGameObject() {};
	~ScriptingGameObject() {};

	unsigned int GetMyUID() const;
	void DestroyGameObject(unsigned int go_UUID);
};

#endif

