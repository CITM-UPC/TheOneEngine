#ifndef __AUDIOCORE_H__
#define __AUDIOCORE_H__
#pragma once

#include <iostream>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>      // Memory Manager interface
#include <AK/SoundEngine/Common/AkModule.h>         // Default memory manager

#include <AK/SoundEngine/Common/IAkStreamMgr.h>     // Streaming Manager
#include <AK/Tools/Common/AkPlatformFuncs.h>        // Thread defines
#include <AK/SoundEngine/Common/AkFilePackageLowLevelIODeferred.h> // Sample low-level I/O implementation

#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

#include <AK/MusicEngine/Common/AkMusicEngine.h>    // Music Engine

#include <AK/SpatialAudio/Common/AkSpatialAudio.h>  // Spatial Audio

// Include for communication between Wwise and the game -- Not needed in the release version
#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif // AK_OPTIMIZED


class AudioCore
{
public:
	AudioCore();

	void Awake();

	void Update(double dt);

	void CleanUp();
private:
	bool InitMemoryManager();
	bool InitStreamingManager();
	bool InitSoundEngine();
	bool InitMusicEngine();
	bool InitSpatialAudio();
	bool InitCommunication();

public:
	// We're using the default Low-Level I/O implementation that's part
	// of the SDK's sample code, with the file package extension
	//CAkFilePackageLowLevelIODeferred g_lowLevelIO;
};

#endif // !__AUDIOCORE_H__