#include "AudioCore.h"
#include "..\TheOneEditor\Log.h"

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
    isGameOn = false;
    nextSong = false;

    music1 = NULL;
    music2 = NULL;
    spatial1 = NULL;
    spatial2 = NULL;

    GAME_OBJECT_ID_BACKGROUNDMUSIC = 100;
    GAME_OBJECT_ID_SPATIALSOUND1 = 200;
    GAME_OBJECT_ID_SPATIALSOUND2 = 300;
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
        LOG(LogType::LOG_AUDIO, "Could not load MantelEngine bank");
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

    //registering music to game object
    if (AK::SoundEngine::RegisterGameObj(GAME_OBJECT_ID_BACKGROUNDMUSIC, "Music1") == AK_Success)
        LOG(LogType::LOG_AUDIO, "Game Object BackgroundMusic Succesfully Registered");
    else
        LOG(LogType::LOG_AUDIO, "Game Object BackgroundMusic ERROR on Register");

    //registering spatialsound1 to game object
    if (AK::SoundEngine::RegisterGameObj(GAME_OBJECT_ID_SPATIALSOUND1, "SpatialSound1") == AK_Success)
        LOG(LogType::LOG_AUDIO, "Game Object Spatial Sound 1 Succesfully Registered");
    else
        LOG(LogType::LOG_AUDIO, "Game Object Spatial Sound 1 ERROR on Register");

    //registering spatialsound2 to game object
    if (AK::SoundEngine::RegisterGameObj(GAME_OBJECT_ID_SPATIALSOUND2, "SpatialSound2") == AK_Success)
        LOG(LogType::LOG_AUDIO, "Game Object Spatial Sound 2 Succesfully Registered");
    else
        LOG(LogType::LOG_AUDIO, "Game Object Spatial Sound 2 ERROR on Register");

    //set default listener
    AK::SoundEngine::SetDefaultListeners(&GAME_OBJECT_ID_BACKGROUNDMUSIC, 1);

    //creating audio events
    music1 = new AudioEvent();
    music2 = new AudioEvent();
    spatial1 = new AudioEvent();
    spatial2 = new AudioEvent();
}

void AudioCore::Update(double dt)
{
    //always call this function on update to make things work
    AK::SoundEngine::RenderAudio();
    if (isGameOn)
    {
        //music 1
        //if its turn of the music1 (nextSong == true) and neither of both songs are playing
        if (nextSong && !music1->IsEventPlaying() && !music2->IsEventPlaying())
        {
            AK::SoundEngine::PostEvent(AK::EVENTS::MUSIC1, GAME_OBJECT_ID_BACKGROUNDMUSIC, AkCallbackType::AK_EndOfEvent, music1->event_call_back, (void*)music1);
            music1->playing_id = 1L;
            nextSong = false;
        }
        //music 2
        //if its turn of the music2 (nextSong == false) and neither of both songs are playing
        else if (!nextSong && !music1->IsEventPlaying() && !music2->IsEventPlaying())
        {
            AK::SoundEngine::PostEvent(AK::EVENTS::MUSIC2, GAME_OBJECT_ID_BACKGROUNDMUSIC, AkCallbackType::AK_EndOfEvent, music2->event_call_back, (void*)music2);
            music2->playing_id = 1L;
            nextSong = true;
        }

        //spatial sound 1
        if (!spatial1->IsEventPlaying())
        {
            AK::SoundEngine::PostEvent(AK::EVENTS::SPATIAL1, GAME_OBJECT_ID_SPATIALSOUND1, AkCallbackType::AK_EndOfEvent, spatial1->event_call_back, (void*)spatial1);
            spatial1->playing_id = 1L;
        }

        //spatial sound 2
        if (!spatial2->IsEventPlaying())
        {
            AK::SoundEngine::PostEvent(AK::EVENTS::SPATIAL2, GAME_OBJECT_ID_SPATIALSOUND2, AkCallbackType::AK_EndOfEvent, spatial2->event_call_back, (void*)spatial2);
            spatial2->playing_id = 1L;
        }
    }
}



void AudioCore::CleanUp()
{
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

void AudioCore::PlayEngine()
{
    isGameOn = true;

    //music 1
    AK::SoundEngine::PostEvent(AK::EVENTS::MUSIC1, GAME_OBJECT_ID_BACKGROUNDMUSIC, AkCallbackType::AK_EndOfEvent, music1->event_call_back, (void*)music1);
    music1->playing_id = 1L;

    //spatial sound 1
    AK::SoundEngine::PostEvent(AK::EVENTS::SPATIAL1, GAME_OBJECT_ID_SPATIALSOUND1, AkCallbackType::AK_EndOfEvent, spatial1->event_call_back, (void*)spatial1);
    spatial1->playing_id = 1L;

    //spatial sound 2
    AK::SoundEngine::PostEvent(AK::EVENTS::SPATIAL2, GAME_OBJECT_ID_SPATIALSOUND2, AkCallbackType::AK_EndOfEvent, spatial2->event_call_back, (void*)spatial2);
    spatial2->playing_id = 1L;
}

void AudioCore::PauseEngine()
{
    isGameOn = false;
    AK::SoundEngine::StopAll(GAME_OBJECT_ID_BACKGROUNDMUSIC);
    AK::SoundEngine::StopAll(GAME_OBJECT_ID_SPATIALSOUND1);
    AK::SoundEngine::StopAll(GAME_OBJECT_ID_SPATIALSOUND2);
}

void AudioCore::SetListenerTransform(float posx, float posy, float posz, float ofx, float ofy, float ofz, float otx, float oty, float otz)
{
    //SINCE OPENGL AND WWISE USE DIFFERENT POSITIVE X AND Z POSITIONS HERE WILL BE CHANGED HERE
    AkSoundPosition tTransform;
    tTransform.SetPosition({ -posx, posy, -posz });
    tTransform.SetOrientation({ ofx, ofy, ofz }, { otx, oty, otz });
    if (AK::SoundEngine::SetPosition(GAME_OBJECT_ID_BACKGROUNDMUSIC, tTransform) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "ERROR setting position to backgroundmusic (default listener)");
    }
}

void AudioCore::SetSpatial1Transform(float posx, float posy, float posz)
{
    //SINCE OPENGL AND WWISE USE DIFFERENT POSITIVE X AND Z POSITIONS HERE WILL BE CHANGED HERE
    AkSoundPosition tTransform;
    tTransform.SetPosition({ -posx, posy, -posz });
    tTransform.SetOrientation({ 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
    if (AK::SoundEngine::SetPosition(GAME_OBJECT_ID_SPATIALSOUND1, tTransform) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "ERROR setting position to spatialsound1 (emiter 1)");
    }
}

void AudioCore::SetSpatial2Transform(float posx, float posy, float posz)
{
    //SINCE OPENGL AND WWISE USE DIFFERENT POSITIVE X AND Z POSITIONS HERE WILL BE CHANGED HERE
    AkSoundPosition tTransform;
    tTransform.SetPosition({ -posx, posy, -posz });
    tTransform.SetOrientation({ 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
    if (AK::SoundEngine::SetPosition(GAME_OBJECT_ID_SPATIALSOUND2, tTransform) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "ERROR setting position to spatialsound2 (emiter 2)");
    }
}

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