#ifndef __SCRIPT_H__
#define __SCRIPT_H__
#pragma once

#include "Defs.h"
#include "GameObject.h"

#include <vector>
#include <memory>
#include <string>

class __declspec(dllexport) Script : public Component
{
    friend class Transform;
    friend class Mesh;
    friend class Camera;
    friend class Texture;
public:

    Script(std::shared_ptr<GameObject> containerGO);
    virtual ~Script();

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

public:
    bool active;
private:
    std::string path;
    
};
#endif // !__SCRIPT_H__