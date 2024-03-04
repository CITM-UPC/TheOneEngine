#include "Listener.h"
#include "EngineCore.h"

#include <memory>

Listener::Listener(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Listener)
{
	//this->Enable();
	this->GO = containerGO;
	SetTransform(containerGO);
}

Listener::~Listener()
{

}

void Listener::SetListener(AkGameObjectID goID)
{
	std::shared_ptr<AudioManager> am = std::make_shared<AudioManager>();
	am->audio->SetDefaultListener(goID);
}

void Listener::SetTransform(std::shared_ptr<GameObject> containerGO)
{
	AkSoundPosition tTransform;
	AkVector pos;
	pos.X = -containerGO.get()->GetComponent<Transform>()->getPosition().x;
	pos.Y = containerGO.get()->GetComponent<Transform>()->getPosition().y;
	pos.Z = -containerGO.get()->GetComponent<Transform>()->getPosition().z;

	tTransform.SetPosition(pos);

	AkVector forward;
	forward.X = -containerGO.get()->GetComponent<Transform>()->getForward().x;
	forward.Y = -containerGO.get()->GetComponent<Transform>()->getForward().y;
	forward.Z = -containerGO.get()->GetComponent<Transform>()->getForward().z;

	AkVector up;
	up.X = -containerGO.get()->GetComponent<Transform>()->getUp().x;
	up.Y = -containerGO.get()->GetComponent<Transform>()->getUp().y;
	up.Z = -containerGO.get()->GetComponent<Transform>()->getUp().z;

	tTransform.SetOrientation(forward, up);

	if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
	{
		// I should put this function and rotation in virtual to change logs in Listener and Source
		LOG(LogType::LOG_AUDIO, "ERROR setting transform to the Listener");
	}
}

// JULS: TODO -> SaveComponent and LoadComponent Listener
json Listener::SaveComponent()
{
	return json();
}

void Listener::LoadComponent(const json& transformJSON)
{
}
