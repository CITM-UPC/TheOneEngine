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
		
		
	}
	return true;
}

bool AudioManager::CleanUp()
{
	audio->CleanUp();
	delete audio;
	return true;
}

// JULS: Not sure tho if they should be added like this
void AudioManager::PlayAudio()
{
	for (const auto& audioComponent : audioComponents) {
		if (audioComponent->GO->GetComponent<Source>()) {

		}
	}
}

void AudioManager::StopAudio()
{
	for (const auto& audioComponent : audioComponents) {
		if (audioComponent->GO->GetComponent<Source>()) {

		}
	}
}

void AudioManager::PauseAudio()
{
	for (const auto& audioComponent : audioComponents) {
		if (audioComponent->GO->GetComponent<Source>()) {

		}
	}
}

void AudioManager::ResumeAudio()
{
	for (const auto& audioComponent : audioComponents) {
		if (audioComponent->GO->GetComponent<Source>()) {

		}
	}
}

// Implementation of AddAudioObject
void AudioManager::AddAudioObject(std::shared_ptr<AudioComponent> audioGO)
{
	audioComponents.push_back(audioGO);

	// Ensure that the component is properly initialized before enabling it
	//audioGO->GetGameObject()->GetComponent<;
}

