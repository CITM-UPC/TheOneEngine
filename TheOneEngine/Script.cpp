#include "Script.h"
#include "GameObject.h"
//#include "Transform.h"
//#include "Mesh.h"
//#include "Camera.h"
#include "CPPScript.h"
#include "..\TheOneEditor\Log.h"
#include "..\TheOneEditor\Input.h"

Script::Script(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Script), active(true)
{
}

Script::~Script()
{
}

json Script::SaveComponent()
{
    json scriptJSON;

    scriptJSON["Name"] = name;
    scriptJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        scriptJSON["ParentUID"] = pGO.get()->GetUID();
    }
    scriptJSON["UID"] = UID;
    scriptJSON["Active"] = enabled;
    scriptJSON["Path"] = path;

    return scriptJSON;
}

void Script::LoadComponent(const json& scriptJSON)
{
    // Load basic properties
    if (scriptJSON.contains("UID"))
    {
        UID = scriptJSON["UID"];
    }

    if (scriptJSON.contains("Name"))
    {
        name = scriptJSON["Name"];
    }

    // Load mesh-specific properties
    if (scriptJSON.contains("Active"))
    {
        enabled = scriptJSON["Active"];
    }

    if (scriptJSON.contains("Path"))
    {
        path = scriptJSON["Path"];
    }

    if (!path.empty())
    {
        //Load cpp script file
    }

}

void Script::LoadScript(HMODULE _dllHandle, std::string name)
{
    //Load here script attached
    void* (*Creator)() = (void* (*)())GetProcAddress(_dllHandle, std::string("Create" + std::string(name)).data());
    if (Creator != nullptr) {
        scriptName = std::string(name);
        scriptData_ptr = Creator();

        CPPScript* script = (CPPScript*)scriptData_ptr;
        script->containerGO = containerGO;
        //script->transform = containerGO.lock().get()->GetComponent<Transform>();
        //script->mesh = containerGO.lock().get()->GetComponent<Mesh>();
        //script->camera = containerGO.lock().get()->GetComponent<Camera>();
        script->active = &active;
        script->name = scriptName;
        goScripts.emplace_back(script);
        
    }
}
