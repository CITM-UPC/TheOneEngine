#pragma once
#include "Defs.h"
#include "..\TheOneAudio\AudioUtility.h"
#include "GameObject.h"
#include "Transform.h"

class GameObject;

class AudioObject {
public:
	//AudioObject(std::shared_ptr<GameObject> containerGO);
	//virtual ~AudioObject();

	virtual void SetTransform(std::shared_ptr<GameObject> containerGO) = 0;
	
	// Get the associated game object
	std::shared_ptr<GameObject> GetGameObject() {
		return GO;
	}

public:
	std::shared_ptr<GameObject> GO;
	AkGameObjectID goID;


	

};