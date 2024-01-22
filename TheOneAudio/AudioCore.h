#ifndef __AUDIOCORE_H__
#define __AUDIOCORE_H__
#pragma once

#include <AK/SoundEngine/Common/AkMemoryMgr.h>      // Memory Manager interface
#include <AK/SoundEngine/Common/AkModule.h>         // Default memory manager
#include <AK/SoundEngine/Common/IAkStreamMgr.h>     // Streaming Manager
#include <AK/Tools/Common/AkPlatformFuncs.h>        // Thread defines
#include <Common/AkFilePackageLowLevelIODeferred.h> // Sample low-level I/O implementation
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine
#include <AK/MusicEngine/Common/AkMusicEngine.h>    // Music Engine
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>  // Spatial Audio

class AudioCore
{
public:
	AudioCore();

	virtual ~AudioCore();

	bool Awake();

	bool Update(double dt);

	bool CleanUp();
private:
	bool InitMemoryManager();
	bool InitStreamingManager();
	bool InitSoundEngine();
	bool InitMusicEngine();
	bool InitSpatialAudio();
	bool InitCommunication();
};

#endif // !__AUDIOCORE_H__