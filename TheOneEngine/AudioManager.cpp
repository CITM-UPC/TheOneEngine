#include "AudioManager.h"

AudioManager::AudioManager()
{
}

bool AudioManager::Start()
{
	return true;
}

bool AudioManager::Update()
{
	// JULS: Should check the state of the engine (PLAY, PAUSE, STOP) (Can be found in AudioCore)
	for (const auto& audio : audios) {
		// JULS: It takes the gameobject, so tricky
		//audio->SetPosition(audio.get)
	}
	return true;
}

bool AudioManager::CleanUp()
{
	return true;
}

void AudioManager::AddAudioObject(AudioObject audioGO)
{

}

