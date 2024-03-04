#ifndef __LISTENER_H__
#define __LISTENER_H__

#pragma once

#include "Component.h"
#include "AudioComponent.h"

class GameObject;

class Listener : public Component, public AudioComponent {
public:
    Listener(std::shared_ptr<GameObject> containerGO);
    virtual ~Listener();

    void SetListener(AkGameObjectID goID, std::unique_ptr<AudioManager> audioManager);
    void SetTransform(std::shared_ptr<GameObject> containerGO);

    json SaveComponent();
    void LoadComponent(const json& transformJSON);

private:


};

#endif // !__LISTENER_H__