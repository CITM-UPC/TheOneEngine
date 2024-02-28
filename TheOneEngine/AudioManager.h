#pragma once
#include "Defs.h"
#include "AudioObject.h"

class AudioManager {
public:
	AudioManager();

	bool Start();
	bool Update();
	bool CleanUp();

	void AddAudioObject(AudioObject audioGO);


private:
	std::vector<std::unique_ptr<AudioObject>> audios;

};
