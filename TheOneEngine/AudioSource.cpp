#include "AudioObject.h"
#include "AudioSource.h"

void AudioSource::Init() {
    AudioObject::Init(); // call base class init.
    m_positionOffset = { 0, 0,0 };
}

void AudioSource::Update(float dt)
{
    //// assume Transform is the component that contains // the world position information
    //Transform* transform = this->GetSiblingComponent<Transform>();
    //
    //// retrieve position and normalized up and forward vectors from transform.
    //vec3 position = transform->GetPosition();
    //vec3 upVector = transform->GetUpVector().Normalize();
    //vec3 forwardVector = transform->GetForwardVector().Normalize();
    //
    //AkSoundPosition audioTransform;
    //audioTransform.SetOrientation(forwardVector, upVector);
    //audioTransform.SetPosition(position + this->m_positionOffset);
    //
    //AKRESULT result = AK::SoundEngine::SetPosition(
    //    this->GetAudioObjectID(), audioTransform);
    //ASSERT(result == AK_Success);
}   //

void AudioSource::Exit() 
{
    AudioObject::Exit(); // call base class exit.
}