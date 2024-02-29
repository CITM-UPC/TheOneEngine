#include "Source.h"
#include "EngineCore.h"

Source::Source(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Source), AudioObject(containerGO), path(path), volume(volume)
{
	volume = 50;
	path = "";
	goID = audio->RegisterGameObject(containerGO->GetName());
	SetTransform(containerGO);
}

Source::~Source()
{
}

void Source::SetPosition(std::shared_ptr<GameObject> containerGO)
{
	AkSoundPosition tTransform;
	tTransform.SetPosition({ -containerGO.get()->GetComponent<Transform>()->getPosition().x, containerGO.get()->GetComponent<Transform>()->getPosition().y, -containerGO.get()->GetComponent<Transform>()->getPosition().z });

	if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
	{
		// I should put this function and rotation in virtual to change logs in Listener and Source
		LOG(LogType::LOG_AUDIO, "ERROR setting position to AudioSource: %s", containerGO->GetName());
	}
}

void Source::SetRotation(std::shared_ptr<GameObject> containerGO)
{
	AkSoundPosition tTransform;
	AkVector forward;
	forward.X = containerGO.get()->GetComponent<Transform>()->getForward().x;
	forward.Y = containerGO.get()->GetComponent<Transform>()->getForward().y;
	forward.X = containerGO.get()->GetComponent<Transform>()->getForward().z;

	AkVector up;
	up.X = containerGO.get()->GetComponent<Transform>()->getUp().x;
	up.Y = containerGO.get()->GetComponent<Transform>()->getUp().y;
	up.Z = containerGO.get()->GetComponent<Transform>()->getUp().z;

	tTransform.SetOrientation(forward, up);

	if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
	{
		LOG(LogType::LOG_AUDIO, "ERROR setting rotation to AudioSource: %s", containerGO->GetName());
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
