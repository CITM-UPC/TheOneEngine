#include "Script.h"
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
    scriptJSON["Active"] = active;
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
        active = scriptJSON["Active"];
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
    
}
