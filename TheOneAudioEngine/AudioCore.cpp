#include "AudioCore.h"
#include "..\TheOneEditor\Log.h"

#include <AK/SoundEngine/Common/AkMemoryMgr.h>      // Memory Manager interface
#include <AK/SoundEngine/Common/AkModule.h>         // Default memory manager

#include <AK/SoundEngine/Common/IAkStreamMgr.h>     
#include <AK/Tools/Common/AkPlatformFuncs.h>        // Thread defines
#include <AkFilePackageLowLevelIODeferred.h>        // Sample low-level I/O implementation

#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

#include <AK/MusicEngine/Common/AkMusicEngine.h>    // Music Engine

#include <AK/SpatialAudio/Common/AkSpatialAudio.h>  // Spatial Audio

// Include for communication between Wwise and the game -- Not needed in the release version
#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif // AK_OPTIMIZED

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
    /*=====================MEMORY=====================*/ 
    AkMemSettings memSettings;
    AK::MemoryMgr::GetDefaultSettings(memSettings);

    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
    {
        assert(!"Could not create the memory manager.");
        return false;
    }

    /*=====================STREAMING=====================*/
    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);

    // Customize the Stream Manager settings here.
    // (...)

    if (!AK::StreamMgr::Create(stmSettings))
    {
        assert(!"Could not create the Streaming Manager");
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
        assert(!"Could not create the streaming device and Low-Level I/O system");
        return false;
    }

    /*=====================AUDIO=====================*/
    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
    {
        assert(!"Could not initialize the Sound Engine.");
        return false;
    }

    /*=====================MUSIC=====================*/
    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);

    if (AK::MusicEngine::Init(&musicInit) != AK_Success)
    {
        assert(!"Could not initialize the Music Engine.");
        return false;
    }

    /*=====================SPATIAL AUDIO=====================*/
    // Initialize Spatial Audio -> Using default initialization parameters
    AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings. 

    if (AK::SpatialAudio::Init(&settings) != AK_Success)
    {
        assert(!"Could not initialize the Spatial Audio.");
        return false;
    }

    /*=====================COMMUNICATION=====================*/
#ifndef AK_OPTIMIZED
//

// Initialize communications (not in release build!)

//

    AkCommSettings commSettings;
    AK::Comm::GetDefaultInitSettings(commSettings);
    if (AK::Comm::Init(commSettings) != AK_Success)
    {
        assert(!"Could not initialize communication.");
        return false;
    }
#endif // AK_OPTIMIZED

	return true;
}