#include "AudioManager.h"

AudioManager::AudioManager()
{
	audio = new AudioCore();
}

bool AudioManager::Awake()
{
	audio->Awake();
	return true;
}

bool AudioManager::Update(double dt)
{
	audio->Update(dt);
	for (const auto& audioComponent : audioComponents) {
		audioComponent->SetTransform(audioComponent->GetGameObject());
	}
	return true;
}

bool AudioManager::CleanUp()
{
	audio->CleanUp();
	delete audio;
	return true;
}

void AudioManager::AddAudioObject(std::shared_ptr<AudioComponent> audioGO)
{
	audioComponents.push_back(audioGO);
}

