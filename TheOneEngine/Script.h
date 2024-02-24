#ifndef __SCRIPT_H__
#define __SCRIPT_H__
#pragma once

#include "Defs.h"
#include "Component.h"

#include "mono\metadata\assembly.h"

#include <vector>
#include <memory>
#include <string>

class Script : public Component
{
public:

    Script(std::shared_ptr<GameObject> containerGO);
    virtual ~Script();

    void DrawComponent();

    json SaveComponent();
    void LoadComponent(const json& scriptJSON);

public:

    MonoObject* monoBehaviourInstance = nullptr;

};

#endif // !__SCRIPT_H__
