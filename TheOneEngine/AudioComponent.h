#pragma once
#include "Defs.h"
#include "..\TheOneAudio\AudioUtility.h"
#include "GameObject.h"
#include "Transform.h"
#include <memory>

class GameObject;
class AudioManager;

class AudioComponent : public std::enable_shared_from_this<AudioComponent> {
public:
	//AudioComponent(std::shared_ptr<GameObject> containerGO);
	//virtual ~AudioComponent();

	virtual void SetTransform(std::shared_ptr<GameObject> containerGO) = 0;
	
	// Get the associated game object
	std::shared_ptr<GameObject> GetGameObject() {
		return GO;
	}

public:
	std::shared_ptr<GameObject> GO;
	AkGameObjectID goID;

	AudioManager* AM;
};