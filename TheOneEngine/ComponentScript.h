#ifndef __COMPONENT_SCRIPT_H__
#define __COMPONENT_SCRIPT_H__

#include "Component.h"

//TODO: Create the scripting component
class ComponentScript : public Component {
public:
	ComponentScript(std::shared_ptr<GameObject> containerGO, const char* path);
	virtual ~ComponentScript();
};

#endif