#include "AudioListener.h"

void AudioListener::Init()
{
    //m_audioObjectID = DEFAULT_LISTENER_OBJECT;
    //AKRESULT result = AK::SoundEngine::RegisterGameObj(DEFAULT_LISTENER_OBJECT);
    //ASSERT(result == AK_Success);
    //result = AK::SoundEngine::SetDefaultListeners(&m_audioObjectID, 1);
    //ASSERT(result == AK_Success);
    //
    //m_positionOffset = { 0,0,0 };
}

void AudioListener::Update(float dt)
{
}

void AudioListener::Exit()
{
    AudioObject::Exit(); // call base class Exit
}

void AudioListener::SetListenerTransform(const vec3& pos)
{
    AkSoundPosition tTransform;
    tTransform.SetPosition({ -pos.x, pos.y, -pos.z });
}

void AudioListener::SetListenerTransform(float posx, float posy, float posz, float ofx, float ofy, float ofz, float otx, float oty, float otz)
{
    //SINCE OPENGL AND WWISE USE DIFFERENT POSITIVE X AND Z POSITIONS HERE WILL BE CHANGED HERE
    AkSoundPosition tTransform;
    tTransform.SetPosition({ -posx, posy, -posz });
    tTransform.SetOrientation({ ofx, ofy, ofz }, { otx, oty, otz });
    //if (AK::SoundEngine::SetPosition(GAME_OBJECT_ID_BACKGROUNDMUSIC, tTransform) != AK_Success)
    //{
    //    LOG(LogType::LOG_AUDIO, "ERROR setting position to backgroundmusic (default listener)");
    //}
}
