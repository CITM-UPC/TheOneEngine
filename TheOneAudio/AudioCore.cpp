#include "AudioCore.h"
#include "../TheOneEngine/Log.h"

AudioEvent::AudioEvent()
{
    playing_id = 0L;
    event_call_back = &AudioCore::EventCallBack;
}

bool AudioEvent::IsEventPlaying()
{
    return playing_id != 0L;
}

AudioCore::AudioCore()
{
    nextSong = false;
}

bool AudioCore::InitEngine()
{
    if (InitMemoryManager())  LOG(LogType::LOG_AUDIO, "Initialized the Memory Manager.");
    else LOG(LogType::LOG_AUDIO, "Could not initialize the Memory Manager.");

    if (InitStreamingManager()) LOG(LogType::LOG_AUDIO, "Initialized the Streaming Manager.");
    else LOG(LogType::LOG_AUDIO, "Could not initialize the Streaming Manager.");

    if (InitSoundEngine()) LOG(LogType::LOG_AUDIO, "Initialized the Sound Engine.");
    else LOG(LogType::LOG_AUDIO, "Could not initialize the Sound Engine.");

    if (InitMusicEngine()) LOG(LogType::LOG_AUDIO, "Initialized the Music Engine.");
    else LOG(LogType::LOG_AUDIO, "Could not initialize the Music Engine.");

    g_lowLevelIO.SetBasePath(AKTEXT("Assets\\Audio\\Wwise Project\\GeneratedSoundBanks\\Windows"));
    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(us)"));

    //bank init
    AkBankID bankID;
    if (AK::SoundEngine::LoadBank(BANKNAME_INIT, bankID) == AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Init bank loaded");
    }
    else
    {
        LOG(LogType::LOG_AUDIO, "Could not load init bank");
        return false;
    }

    if (AK::SoundEngine::LoadBank(BANKNAME_THEONEENGINE, bankID) == AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "TheOneEngine bank loaded");
    }
    else
    {
        LOG(LogType::LOG_AUDIO, "Could not load TheOneEngine bank");
        return false;
    }

    if (InitSpatialAudio()) LOG(LogType::LOG_AUDIO, "Initialized the Spatial Audio.");
    else LOG(LogType::LOG_AUDIO, "Could not initialize the Spatial Audio.");

#ifndef AK_OPTIMIZED
    if (InitCommunication()) LOG(LogType::LOG_AUDIO, "Initialized communication.");
    else LOG(LogType::LOG_AUDIO, "Could not initialize communication.");
#endif // AK_OPTIMIZED

    return true;
}

bool AudioCore::InitMemoryManager()
{
    AkMemSettings memSettings;
    AK::MemoryMgr::GetDefaultSettings(memSettings);

    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not create the memory manager.");
        return false;
    }

    return true;
}

bool AudioCore::InitStreamingManager()
{
    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);

    // Customize the Stream Manager settings here.
    // (...)

    if (!AK::StreamMgr::Create(stmSettings))
    {
        LOG(LogType::LOG_AUDIO, "Could not create the Streaming Manager");
        return false;
    }

    // Create a streaming device.
    // Note that you can override the default low-level I/O module with your own. 
    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

    // Customize the streaming device settings here.
    // (...)

    // CAkFilePackageLowLevelIODeferred::Init() creates a streaming device
    // in the Stream Manager, and registers itself as the File Location Resolver.
    if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not create the streaming device and Low-Level I/O system");
        return false;
    }

    return true;
}

bool AudioCore::InitSoundEngine()
{
    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not initialize the Sound Engine.");
        return false;
    }

    return true;
}

bool AudioCore::InitMusicEngine()
{
    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);

    if (AK::MusicEngine::Init(&musicInit) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not initialize the Music Engine.");
        return false;
    }
    return true;
}

bool AudioCore::InitSpatialAudio()
{
    // Initialize Spatial Audio -> Using default initialization parameters
    AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings. 

    if (AK::SpatialAudio::Init(settings) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not initialize the Spatial Audio.");
        return false;
    }
    return true;
}

bool AudioCore::InitCommunication()
{
#ifndef AK_OPTIMIZED
    // Initialize communications (not in release build!)

    AkCommSettings commSettings;
    AK::Comm::GetDefaultInitSettings(commSettings);
    if (AK::Comm::Init(commSettings) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not initialize the communications.");
        return false;
    }
#endif // AK_OPTIMIZED

    return true;
}

void AudioCore::Awake()
{
    if (InitEngine())
        LOG(LogType::LOG_AUDIO, "Initialized the Audio Engine.");
    else
        LOG(LogType::LOG_AUDIO, "Could not initialize the Audio Engine.");

    //creating audio events
    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    {
        audioEvents.push_back(new AudioEvent);
    }

    SetGlobalVolume(globalVolume);

}

void AudioCore::Update(double dt)
{
    //always call this function on update to make things work
    AK::SoundEngine::RenderAudio();


    if (state == EngineState::PLAYING)
    {

    }
}

void AudioCore::CleanUp()
{
    //must delete vectors and stuff
    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    {
        //function to delete them here
        if (audioEvents[i] != NULL)
        {
            delete(audioEvents[i]);
        }
    }
    
    audioEvents.clear();


#ifndef AK_OPTIMIZED
    AK::Comm::Term();
#endif // AK_OPTIMIZED
    //commented cz theres no term function xd
    //AK::SpatialAudio::Term();
    AK::MusicEngine::Term();
    AK::SoundEngine::Term();

    g_lowLevelIO.Term();

    if (AK::IAkStreamMgr::Get())
    {
        AK::IAkStreamMgr::Get()->Destroy();
    }

    AK::MemoryMgr::Term();

}

void AudioCore::SetDefaultListener(AkGameObjectID goID)
{
    AK::SoundEngine::SetDefaultListeners(&goID, 1);
}

AkGameObjectID AudioCore::RegisterGameObject(std::string name)
{
    if (AK::SoundEngine::RegisterGameObj((AkGameObjectID)gameObjectIDs.size(), name.c_str()) == AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Game Object %s SUCCESS on Register", name.c_str());
        gameObjectIDs.push_back((AkGameObjectID)gameObjectIDs.size());
        return gameObjectIDs.size() - 1;
    }
    else
    {
        LOG(LogType::LOG_AUDIO, "Game Object %s ERROR on Register", name.c_str());
        return -1;
    }
}

void AudioCore::PlayEvent(AkUniqueID event, AkGameObjectID goID)
{
    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    {
        //if we found unused audio event we use it
        if (audioEvents[i]->playing_id == 0L)
        {
            AK::SoundEngine::PostEvent(event, goID, AkCallbackType::AK_EndOfEvent, audioEvents[i]->event_call_back, (void*)audioEvents[i]);
            audioEvents[i]->playing_id = 1L;
            return;
        }
    }
    LOG(LogType::LOG_AUDIO, "Maximum amount of audio events at the same time reached: %d", MAX_AUDIO_EVENTS);
}

void AudioCore::StopEvent(AkUniqueID event, AkGameObjectID goID)
{
    AK::SoundEngine::ExecuteActionOnEvent(event, AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Stop, gameObjectIDs[goID]);
}

void AudioCore::PauseEvent(AkUniqueID event, AkGameObjectID goID)
{
    AK::SoundEngine::ExecuteActionOnEvent(event, AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause, gameObjectIDs[goID]);
}

void AudioCore::ResumeEvent(AkUniqueID event, AkGameObjectID goID)
{
    AK::SoundEngine::ExecuteActionOnEvent(event, AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Resume, gameObjectIDs[goID]);
}

// JULS: This functions do not work, but I should check them
void AudioCore::PlayEngine()
{
    //if (state == EngineState::PAUSED)
    //{
    //    //resume stuff
    //    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    //    {
    //        if (audioEvents[i] != NULL)
    //        {
    //            AK::SoundEngine::ExecuteActionOnPlayingID(AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Resume, audioEvents[i]->playing_id);
    //        }
    //    }
    //}
    //else if (state == EngineState::STOPPED)
    //{
    //    //resume stuff
    //    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    //    {
    //        if (audioEvents[i] != NULL)
    //        {
    //            AK::SoundEngine::ExecuteActionOnPlayingID(AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Resume, audioEvents[i]->playing_id);
    //        }
    //    }
    //}

    //resume stuff
    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    {
        if (audioEvents[i] != NULL)
        {
            AK::SoundEngine::ExecuteActionOnPlayingID(AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Resume, audioEvents[i]->playing_id);
        }
    }

    //state = EngineState::PLAYING;
}

void AudioCore::PauseEngine()
{
    //state = EngineState::PAUSED;

    //will PAUSE all sounds
    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    {
        if (audioEvents[i] != NULL)
        {
            AK::SoundEngine::ExecuteActionOnPlayingID(AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause, audioEvents[i]->playing_id);
        }
    }
}

void AudioCore::StopEngine()
{
    //state = EngineState::STOPPED;

    //will STOP all sounds (not pause)
    for (size_t i = 0; i < gameObjectIDs.size(); i++)
    {
        AK::SoundEngine::StopAll(gameObjectIDs[i]);
    }
}

void AudioCore::SetGlobalVolume(float volume)
{
    if (volume < 0.0f)
    {
        volume = 0.0f;
    }
    else if (volume > 100.0f)
    {
        volume = 100.0f;
    }
    globalVolume = volume;
    AK::SoundEngine::SetOutputVolume(AK::SoundEngine::GetOutputID(AK_INVALID_UNIQUE_ID, 0.0f), (AkReal32)(volume * 0.01f));
}

void AudioCore::SetAudioObjectVolume(AkGameObjectID goID, float volume)
{
    if (volume < 0.0f)
    {
        volume = 0.0f;
    }
    else if (volume > 100.0f)
    {
        volume = 100.0f;
    }

    // JULS: ID of the source, ID of the listener, volume
    //AK::SoundEngine::SetGameObjectOutputBusVolume(id, AK::SoundEngine::list (AkReal32)(volume * 0.01f));
}


// Juls: moved this to AudioComponent
//void AudioCore::SetAudioGameObjectTransform(AkGameObjectID goID, float posx, float posy, float posz, float ofx, float ofy, float ofz, float otx, float oty, float otz)
//{
//    //SINCE OPENGL AND WWISE USE DIFFERENT POSITIVE X AND Z POSITIONS HERE WILL BE CHANGED HERE
//    AkSoundPosition tTransform;
//    tTransform.SetPosition({ -posx, posy, -posz });
//    tTransform.SetOrientation({ ofx, ofy, ofz }, { otx, oty, otz });
//    if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
//    {
//        LOG(LogType::LOG_AUDIO, "ERROR setting position to backgroundmusic (default listener)");
//    }
//}
//
//void AudioCore::SetAudioGameObjectPosition(AkGameObjectID goID, float posx, float posy, float posz)
//{
//    //SINCE OPENGL AND WWISE USE DIFFERENT POSITIVE X AND Z POSITIONS HERE WILL BE CHANGED HERE
//    AkSoundPosition tTransform;
//    tTransform.SetPosition({ -posx, posy, -posz });
//    tTransform.SetOrientation({ 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
//    if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
//    {
//        LOG(LogType::LOG_AUDIO, "ERROR setting position to backgroundmusic (default listener)");
//    }
//}


void AudioCore::EventCallBack(AkCallbackType in_eType, AkCallbackInfo* in_pCallbackInfo)
{
    AudioEvent* a_event = (AudioEvent*)in_pCallbackInfo->pCookie;

    switch (in_eType)
    {
    case(AkCallbackType::AK_EndOfEvent):
    {
        a_event->playing_id = 0L;
        break;
    }
    }
}