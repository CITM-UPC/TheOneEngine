#pragma once
#include "Defs.h"
#include "..\TheOneAudio\AudioCore.h"
#include "AudioObject.h"

class AudioManager {
public:
	AudioManager();

	bool Awake();
	bool Update(double dt);
	bool CleanUp();

	void AddAudioObject(std::shared_ptr<AudioObject> audioGO);

	AudioCore* audio = nullptr;

	AudioCore* GetAudioCore() { return audio; }

private:
	std::vector<std::shared_ptr<AudioObject>> audioObjects;
};
