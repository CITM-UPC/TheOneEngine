#ifndef __AUDIOCORE_H__
#define __AUDIOCORE_H__
#pragma once

class AudioCore
{
public:
	AudioCore();

	virtual ~AudioCore();

	bool Awake();

	bool PreUpdate();

	bool Update(double dt);

private:
	bool InitMemoryManager();
	bool InitStreamingManager();
	bool InitSoundEngine();
	bool InitMusicEngine();
	bool InitSpatialAudio();
	bool InitCommunication();
};

#endif // !__AUDIOCORE_H__