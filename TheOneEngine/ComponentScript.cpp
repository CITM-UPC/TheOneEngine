#include "ComponentScript.h"
#include "FileUtils.h"
#include "GameObject.h"
#include "ScriptData.h"

ComponentScript::ComponentScript(std::shared_ptr<GameObject> containerGO) :
    Component(containerGO, ComponentType::Script) {
    unique = false;
    // Path needs to be loaded
    data = std::make_shared<ScriptData>();
    data->owner = this;
}

ComponentScript::ComponentScript(std::shared_ptr<GameObject> containerGO, const char* path) :
	Component(containerGO, ComponentType::Script) {
	unique = false;
    data = std::make_shared<ScriptData>();
    data->owner = this;
    data->path = Fileutils::GetWorkingDir() + path; // The path must be absolute
    data->name = Fileutils::GetNameFromPath(path);
}

ComponentScript::~ComponentScript() {}

json ComponentScript::SaveComponent() {
	json savejson;

    savejson["Type"] = type;
    savejson["Name"] = name;
    savejson["UID"] = UID;
    savejson["Path"] = data->path;
    savejson["ScriptName"] = data->name;

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
        data->path = scriptjson["Path"];
    }    
    if (scriptjson.contains("ScriptName")) {
        data->name = scriptjson["ScriptName"];
    }
}
