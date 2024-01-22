#include "AudioCore.h"
#include "..\TheOneEditor\Log.h"

// Include for communication between Wwise and the game -- Not needed in the release version
#include <AK/Comm/AkCommunication.h>

// We're using the default Low-Level I/O implementation that's part
// of the SDK's sample code, with the file package extension
CAkFilePackageLowLevelIODeferred g_lowLevelIO;

AudioCore::AudioCore()
{
}

AudioCore::~AudioCore()
{
}

bool AudioCore::Awake()
{
    if (InitMemoryManager())  LOG(LogType::LOG_AUDIO, "Initialized the Memory Manager.");
    if (InitStreamingManager()) LOG(LogType::LOG_AUDIO, "Initialized the Streaming Manager.");
    if (InitSoundEngine()) LOG(LogType::LOG_AUDIO, "Initialized the Sound Engine.");
    if (InitMusicEngine()) LOG(LogType::LOG_AUDIO, "Initialized the Music Engine.");
    if (InitSpatialAudio()) LOG(LogType::LOG_AUDIO, "Initialized the Spatial Audio.");
    if (InitCommunication()) LOG(LogType::LOG_AUDIO, "Initialized communication.");

	return true;
}

bool AudioCore::Update(double dt)
{
    AK::SoundEngine::RenderAudio();
    
    return true;
}

bool AudioCore::CleanUp()
{
#ifndef AK_OPTIMIZED
    // Terminate Communication Services
    AK::Comm::Term();
#endif // AK_OPTIMIZED
    
    //AK::SpatialAudio::Term();

    AK::MusicEngine::Term();

    g_lowLevelIO.Term();

    if (AK::IAkStreamMgr::Get())
        AK::IAkStreamMgr::Get()->Destroy();

    AK::MemoryMgr::Term();

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
    AkCommSettings commSettings;
    AK::Comm::GetDefaultInitSettings(commSettings);
    if (AK::Comm::Init(commSettings) != AK_Success)
    {
        LOG(LogType::LOG_AUDIO, "Could not initialize communication.");
        return false;
    }

    return true;
}
