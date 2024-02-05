#ifndef __COMPONENT_SCRIPT_H__
#define __COMPONENT_SCRIPT_H__

#include "Component.h"
#include <string>
struct ScriptData;

class ComponentScript : public Component {
public:
	ComponentScript(std::shared_ptr<GameObject> containerGO);
	// path must be relative
	ComponentScript(std::shared_ptr<GameObject> containerGO, const char* path);
	virtual ~ComponentScript();

	json SaveComponent() override;
	void LoadComponent(const json& scriptjson) override;

public:
	std::shared_ptr<ScriptData> data;

private:
	//TODO: Store here variables from the script
};

#endif