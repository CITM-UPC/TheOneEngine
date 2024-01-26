#pragma once
#include "..\TheOneAudio\AudioUtils.h"
#include "..\TheOneAudio\AudioCore.h"

using AudioObjectID = AkGameObjectID;

class AudioObject
{
public:
    AudioObject() : m_audioObjectID{ -1 } {}
    virtual ~AudioObject() {}

    void Init();

    void Exit();

    AudioObjectID GetAudioObjectID() const { return m_audioObjectID; };

    // these will be changed to use integral values later using the preprocessorvoid SetRTPC(const char* name, float value);void SetSwitch(const char* switchGroupName, const char* switchName);

protected:
    AudioObjectID m_audioObjectID;
};