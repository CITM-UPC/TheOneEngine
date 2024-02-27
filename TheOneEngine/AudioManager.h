#pragma once
#include "Defs.h"
#include "AudioObject.h"

class AudioManager {
public:
	AudioManager();

	void Start();
	void Update();
	void CleanUp();



private:
	std::vector<std::unique_ptr<AudioObject>> audios;

};
