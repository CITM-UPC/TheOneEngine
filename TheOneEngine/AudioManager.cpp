#include "AudioManager.h"
#include "Source.h"

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
		
		if (audioComponent->GO->GetComponent<Source>()) {

		}
	}
	return true;
}

bool AudioManager::CleanUp()
{
	audio->CleanUp();
	delete audio;
	return true;
}

// Implementation of AddAudioObject
void AudioManager::AddAudioObject(std::shared_ptr<AudioComponent> audioGO)
{
	audioComponents.push_back(audioGO);

	// Ensure that the component is properly initialized before enabling it
	//audioGO->GetGameObject()->GetComponent<;
}

