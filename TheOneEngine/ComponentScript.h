#ifndef __COMPONENT_SCRIPT_H__
#define __COMPONENT_SCRIPT_H__

#include "Component.h"
#include <string>

class ComponentScript : public Component {
public:
	ComponentScript(std::shared_ptr<GameObject> containerGO, const char* path);
	virtual ~ComponentScript();

	//TODO: Create a save/load function

private:
	std::string path_;
	//TODO: Store here variables from the script
};

#endif