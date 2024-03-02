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
	for (const auto& audioObject : audioObjects) {
		audioObject->SetTransform(audioObject->GetGameObject());
	}
	return true;
}

bool AudioManager::CleanUp()
{
	return true;
}

void AudioManager::AddAudioObject(std::shared_ptr<AudioObject> audioGO)
{
	audioObjects.push_back(audioGO);
}

