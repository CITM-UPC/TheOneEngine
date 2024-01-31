#include "ComponentScript.h"
#include "FileUtils.h"
#include "GameObject.h"

ComponentScript::ComponentScript(std::shared_ptr<GameObject> containerGO) :
    Component(containerGO, ComponentType::Script) {
    unique = false;
    // These need to be loaded
    path_ = "NULL";
    script_name_ = "NULL";
}

ComponentScript::ComponentScript(std::shared_ptr<GameObject> containerGO, const char* path) :
	Component(containerGO, ComponentType::Script) {
	unique = false;
	path_ = Fileutils::GetWorkingDir() + path; // We need an absolute path
	script_name_ = Fileutils::GetNameFromPath(path);
}

ComponentScript::~ComponentScript() {}

json ComponentScript::SaveComponent() {
	json savejson;

    savejson["Name"] = name;
    savejson["UID"] = UID;
    savejson["Path"] = path_;
    savejson["ScriptName"] = script_name_;

    return savejson;
}

void ComponentScript::LoadComponent(const json& scriptjson) {
    if (scriptjson.contains("Name")) {
        name = scriptjson["Name"];
    }    
    if (scriptjson.contains("UID")) {
        UID = scriptjson["UID"];
    }
    if (scriptjson.contains("Path")) {
        path_ = scriptjson["Path"];
    }    
    if (scriptjson.contains("ScriptName")) {
        script_name_ = scriptjson["ScriptName"];
    }
}
