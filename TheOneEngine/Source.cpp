#include "Source.h"
#include "EngineCore.h"
#include "AudioManager.h"
#include "..\TheOneEditor\App.h"

Source::Source(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Source), path(path), volume(volume)
{	
	volume = 0.5f;
	path = "";

	// Creating an instance of AudioManager
	std::shared_ptr<AudioManager> am = std::make_shared<AudioManager>();

	goID = am->audio->RegisterGameObject(containerGO->GetName());

	this->GO = containerGO;
	SetTransform(containerGO);

	am->AddAudioObject(std::shared_ptr<AudioComponent>(this));
}

Source::~Source()
{
}

void Source::SetTransform(std::shared_ptr<GameObject> containerGO)
{
	AkSoundPosition tTransform;
	AkVector pos;
	pos.X = -containerGO.get()->GetComponent<Transform>()->getPosition().x;
	pos.Y = containerGO.get()->GetComponent<Transform>()->getPosition().y;
	pos.Z = -containerGO.get()->GetComponent<Transform>()->getPosition().z;

	tTransform.SetPosition(pos);

	AkVector forward;
	forward.X = containerGO.get()->GetComponent<Transform>()->getForward().x;
	forward.Y = containerGO.get()->GetComponent<Transform>()->getForward().y;
	forward.Z = containerGO.get()->GetComponent<Transform>()->getForward().z;

	AkVector up;
	up.X = containerGO.get()->GetComponent<Transform>()->getUp().x;
	up.Y = containerGO.get()->GetComponent<Transform>()->getUp().y;
	up.Z = containerGO.get()->GetComponent<Transform>()->getUp().z;

	tTransform.SetOrientation(forward, up);

	if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
	{
		// I should put this function and rotation in virtual to change logs in Listener and Source
		LOG(LogType::LOG_AUDIO, "ERROR setting transform to AudioSource: %s", containerGO->GetName());
	}
}

// JULS: TODO -> SaveComponent and LoadComponent AudioSource
json Source::SaveComponent()
{
	return json();
}

void Source::LoadComponent(const json& transformJSON)
{
}
