#include "ComponentScript.h"

ComponentScript::ComponentScript(std::shared_ptr<GameObject> containerGO, const char* path) : 
	Component(containerGO, ComponentType::Script), path_(path) {}

ComponentScript::~ComponentScript() {}
