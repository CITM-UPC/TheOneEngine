#ifndef __LISTENER_H__
#define __LISTENER_H__

#pragma once

#include "Component.h"
#include "AudioObject.h"

class GameObject;

class Listener : public Component, public AudioObject {
public:
    Listener(std::shared_ptr<GameObject> containerGO);
    virtual ~Listener();

    void SetListener(std::string name);

    json SaveComponent();
    void LoadComponent(const json& transformJSON);

private:


};

#endif // !__LISTENER_H__