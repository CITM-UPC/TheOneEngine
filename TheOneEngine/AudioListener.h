#pragma once
#include "..\TheOneAudio\AudioCore.h"
#include "AudioObject.h"
#include "Component.h"

// I usually just define ID 1 to always be the Listener. 
// In general, only 1 listener is needed to exist at a time.
// However, there always need to exist one listener in order to hear sound.
//const AudioObjectID DEFAULT_LISTENER_OBJECT = 1;
//
class AudioListener : public Component, public AudioObject
{
public:
    //...

    void Init();

    void Update(float dt); // used in Unity-style component based engines

    void Exit();

    void SetListenerTransform(const vec3& pos);
    void SetListenerTransform(float posx, float posy, float posz, float ofx, float ofy, float ofz, float otx, float oty, float otz);


    vec3 m_positionOffset;
};