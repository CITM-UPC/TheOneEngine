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

    //commented register of the game objects, they must be done in the normal engine

    ////registering music to game object
    //if (AK::SoundEngine::RegisterGameObj(GAME_OBJECT_ID_BACKGROUNDMUSIC, "Music1") == AK_Success)
    //    LOG(LogType::LOG_AUDIO, "Game Object BackgroundMusic Succesfully Registered");
    //else
    //    LOG(LogType::LOG_AUDIO, "Game Object BackgroundMusic ERROR on Register");

    ////registering spatialsound1 to game object
    //if (AK::SoundEngine::RegisterGameObj(GAME_OBJECT_ID_SPATIALSOUND1, "SpatialSound1") == AK_Success)
    //    LOG(LogType::LOG_AUDIO, "Game Object Spatial Sound 1 Succesfully Registered");
    //else
    //    LOG(LogType::LOG_AUDIO, "Game Object Spatial Sound 1 ERROR on Register");

    ////registering spatialsound2 to game object
    //if (AK::SoundEngine::RegisterGameObj(GAME_OBJECT_ID_SPATIALSOUND2, "SpatialSound2") == AK_Success)
    //    LOG(LogType::LOG_AUDIO, "Game Object Spatial Sound 2 Succesfully Registered");
    //else
    //    LOG(LogType::LOG_AUDIO, "Game Object Spatial Sound 2 ERROR on Register");

    //commenting set default listener, it must be set in normal engine
    //SetDefaultListener(GAME_OBJECT_ID_BACKGROUNDMUSIC);

    //creating audio events
    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    {
        audioEvents[i] = new AudioEvent();
    }
}

void AudioCore::Update(double dt)
{
    //always call this function on update to make things work
    AK::SoundEngine::RenderAudio();


    if (isGameOn)
    {
        ////music 1
        ////if its turn of the music1 (nextSong == true) and neither of both songs are playing
        //if (nextSong && !music1->IsEventPlaying() && !music2->IsEventPlaying())
        //{
        //    AK::SoundEngine::PostEvent(AK::EVENTS::MUSIC1, GAME_OBJECT_ID_BACKGROUNDMUSIC, AkCallbackType::AK_EndOfEvent, music1->event_call_back, (void*)music1);
        //    music1->playing_id = 1L;
        //    nextSong = false;
        //}
        ////music 2
        ////if its turn of the music2 (nextSong == false) and neither of both songs are playing
        //else if (!nextSong && !music1->IsEventPlaying() && !music2->IsEventPlaying())
        //{
        //    AK::SoundEngine::PostEvent(AK::EVENTS::MUSIC2, GAME_OBJECT_ID_BACKGROUNDMUSIC, AkCallbackType::AK_EndOfEvent, music2->event_call_back, (void*)music2);
        //    music2->playing_id = 1L;
        //    nextSong = true;
        //}

        ////spatial sound 1
        //if (!spatial1->IsEventPlaying())
        //{
        //    AK::SoundEngine::PostEvent(AK::EVENTS::SPATIAL1, GAME_OBJECT_ID_SPATIALSOUND1, AkCallbackType::AK_EndOfEvent, spatial1->event_call_back, (void*)spatial1);
        //    spatial1->playing_id = 1L;
        //}

        ////spatial sound 2
        //if (!spatial2->IsEventPlaying())
        //{
        //    AK::SoundEngine::PostEvent(AK::EVENTS::SPATIAL2, GAME_OBJECT_ID_SPATIALSOUND2, AkCallbackType::AK_EndOfEvent, spatial2->event_call_back, (void*)spatial2);
        //    spatial2->playing_id = 1L;
        //}
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
        std::string log = "Game Object " + name + " Succesfully Registered";
        LOG(LogType::LOG_AUDIO, log.c_str());
        gameObjectIDs.push_back((AkGameObjectID)gameObjectIDs.size());
        return gameObjectIDs.size() - 1;
    }
    else
    {
        std::string log = "Game Object " + name + " ERROR on Register";
        LOG(LogType::LOG_AUDIO, log.c_str());
        return -1;
    }
}

void AudioCore::PlayEvent(AkUniqueID eventToPlay, AkGameObjectID goID)
{
    for (size_t i = 0; i < MAX_AUDIO_EVENTS; i++)
    {
        if (audioEvents[i]->playing_id == 0L)
        {
            AK::SoundEngine::PostEvent(eventToPlay, goID, AkCallbackType::AK_EndOfEvent, audioEvents[i]->event_call_back, (void*)audioEvents[i]);
            audioEvents[i]->playing_id = 1L;
            return;
        }
    }
    std::string log = "Maximum amount of audio events at the same time reached: " + MAX_AUDIO_EVENTS;
    LOG(LogType::LOG_AUDIO, log.c_str());
}

void AudioCore::PlayEngine()
{
    isGameOn = true;


    //commented cz its the hardcoded from engines
    ////music 1
    //AK::SoundEngine::PostEvent(AK::EVENTS::MUSIC1, GAME_OBJECT_ID_BACKGROUNDMUSIC, AkCallbackType::AK_EndOfEvent, music1->event_call_back, (void*)music1);
    //music1->playing_id = 1L;

    ////spatial sound 1
    //AK::SoundEngine::PostEvent(AK::EVENTS::SPATIAL1, GAME_OBJECT_ID_SPATIALSOUND1, AkCallbackType::AK_EndOfEvent, spatial1->event_call_back, (void*)spatial1);
    //spatial1->playing_id = 1L;

    ////spatial sound 2
    //AK::SoundEngine::PostEvent(AK::EVENTS::SPATIAL2, GAME_OBJECT_ID_SPATIALSOUND2, AkCallbackType::AK_EndOfEvent, spatial2->event_call_back, (void*)spatial2);
    //spatial2->playing_id = 1L;
}

void AudioCore::PauseEngine()
{
    isGameOn = false;

    //will STOP all sounds (not pause)

    for (size_t i = 0; i < gameObjectIDs.size(); i++)
    {
        AK::SoundEngine::StopAll(gameObjectIDs[i]);
    }


    //old code not deleted yet just in case
    AK::SoundEngine::StopAll(GAME_OBJECT_ID_BACKGROUNDMUSIC);
    AK::SoundEngine::StopAll(GAME_OBJECT_ID_SPATIALSOUND1);
    AK::SoundEngine::StopAll(GAME_OBJECT_ID_SPATIALSOUND2);
}

void AudioCore::SetAudioGameObjectTransform(AkGameObjectID goID, float posx, float posy, float posz, float ofx, float ofy, float ofz, float otx, float oty, float otz)
{
    //SINCE OPENGL AND WWISE USE DIFFERENT POSITIVE X AND Z POSITIONS HERE WILL BE CHANGED HERE
    AkSoundPosition tTransform;
    tTransform.SetPosition({ -posx, posy, -posz });
    tTransform.SetOrientation({ ofx, ofy, ofz }, { otx, oty, otz });
    if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "ERROR setting position to backgroundmusic (default listener)");
    }
}

void AudioCore::SetAudioGameObjectPosition(AkGameObjectID goID, float posx, float posy, float posz)
{
    //SINCE OPENGL AND WWISE USE DIFFERENT POSITIVE X AND Z POSITIONS HERE WILL BE CHANGED HERE
    AkSoundPosition tTransform;
    tTransform.SetPosition({ -posx, posy, -posz });
    tTransform.SetOrientation({ 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
    if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "ERROR setting position to backgroundmusic (default listener)");
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