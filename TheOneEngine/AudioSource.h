#pragma once
#include "Defs.h"
#include "Component.h"
#include "AudioObject.h"

class AudioSource : public Component, public AudioObject // assume CRTP component base classpublic AudioObject
{
public:
    //... engine specific component constructors/destructors here  

    // asume components are activated/deactivated via init/exit// instead of using ctors and dtors
    void Init();

    void Update(float dt); // used in Unity-style component based engines

    void Exit();

    // this will change to use an integral type later
    void Emit(const char* eventName);

    // so that you can position the exact position of the emitter within// game object space.// for advanced implementation, you could have multiple emitter positions// indexed by names on a game object. could be used for animations, etc.
    vec3f m_positionOffset;
}; 
