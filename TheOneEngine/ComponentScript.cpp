#include "ComponentScript.h"
#include "FileUtils.h"

ComponentScript::ComponentScript(std::shared_ptr<GameObject> containerGO, const char* path) : 
	Component(containerGO, ComponentType::Script) {
	unique = false;
	path_ = Fileutils::GetWorkingDir() + path;
	script_name_ = Fileutils::GetNameFromPath(path);
}

ComponentScript::~ComponentScript() {}
