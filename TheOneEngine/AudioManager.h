#pragma once
#include "Defs.h"
#include "AudioObject.h"

class AudioManager {
public:
	AudioManager();

	bool Start();
	bool Update();
	bool CleanUp();

	void AddAudioObject(std::shared_ptr<AudioObject> audioGO);


private:
	std::vector<std::shared_ptr<AudioObject>> audioObjects;

};
