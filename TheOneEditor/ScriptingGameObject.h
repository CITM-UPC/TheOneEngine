#ifndef __SCRIPTINGGAMEOBJECT_H__
#define __SCRIPTINGGAMEOBJECT_H__

#include <string>

class ScriptingGameObject {
public:
	ScriptingGameObject() {};
	~ScriptingGameObject() {};

	unsigned int GetMyUID() const;
	unsigned int FindGOByName(std::string name) const;
	void DestroyGameObject(unsigned int go_UUID);
	unsigned int InstantiateGameObject(unsigned int go_UUID);
};

#endif

