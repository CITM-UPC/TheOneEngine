#ifndef __COMPONENT_SCRIPT_H__
#define __COMPONENT_SCRIPT_H__

#include "Component.h"
#include <string>

class ComponentScript : public Component {
public:
	// path must be relative
	ComponentScript(std::shared_ptr<GameObject> containerGO, const char* path);
	virtual ~ComponentScript();

	//TODO: Create a save/load function
	std::string GetPath() const { return path_; };
	std::string GetScriptName() const { return script_name_; };

private:
	std::string path_;
	std::string script_name_;
	//TODO: Store here variables from the script
};

#endif