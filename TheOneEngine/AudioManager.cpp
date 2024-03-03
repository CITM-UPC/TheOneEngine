#include "AudioManager.h"

AudioManager::AudioManager()
{
}

bool AudioManager::Awake()
{
	audio->Awake();
	return true;
}

bool AudioManager::Update(double dt)
{
	audio->Update(dt);
	for (const auto& audioObject : audioObjects) {
		audioObject->SetTransform(audioObject->GetGameObject());
	}
	return true;
}

bool AudioManager::CleanUp()
{
	audio->CleanUp();
	return true;
}

void AudioManager::AddAudioObject(std::shared_ptr<AudioObject> audioGO)
{
	audioObjects.push_back(audioGO);
}

