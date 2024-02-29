#include "Listener.h"
#include "EngineCore.h"

Listener::Listener(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Listener), AudioObject(containerGO)
{
	//goID = audio->RegisterGameObject(containerGO->GetName());
	SetTransform(containerGO);
}

Listener::~Listener()
{

}

void Listener::SetListener(AkGameObjectID goID)
{
	// JULS: Need to change it as I shouldn't access from the app in the editor
	this->goID = goID;
	audio->SetDefaultListener(this->goID);
}

void Listener::SetPosition(std::shared_ptr<GameObject> containerGO)
{
	AkSoundPosition tTransform;
	tTransform.SetPosition({ -containerGO.get()->GetComponent<Transform>()->getPosition().x, containerGO.get()->GetComponent<Transform>()->getPosition().y, -containerGO.get()->GetComponent<Transform>()->getPosition().z });

	if (AK::SoundEngine::SetPosition(goID, tTransform) != AK_Success)
	{
		// I should put this function and rotation in virtual to change logs in Listener and Source
		LOG(LogType::LOG_AUDIO, "ERROR setting position to the Listener");
	}
}

void Listener::SetRotation(std::shared_ptr<GameObject> containerGO)
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
		LOG(LogType::LOG_AUDIO, "ERROR setting rotation to the Listener (default listener)");
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
