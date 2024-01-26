#include "AudioObject.h"

void AudioObject::Init() {
    m_audioObjectID = AudioCore::Instance().RegisterAudioObject();
    ASSERT(m_audioObjectID != AK_INVALID_GAME_OBJECT);
}

void AudioObject::Exit() {
    AudioCore::Instance().UnregisterAudioObject(m_audioObjectID);
}