#pragma once
#include "Defs.h"
#include "..\TheOneAudio\AudioUtility.h"
#include "GameObject.h"
#include "Transform.h"

class GameObject;

class AudioObject {
public:
	AudioObject(std::shared_ptr<GameObject> containerGO);
	virtual ~AudioObject();

	virtual void SetPosition(std::shared_ptr<GameObject> containerGO);
	virtual void SetRotation(std::shared_ptr<GameObject> containerGO);
	void SetTransform(std::shared_ptr<GameObject> containerGO);
	
	AkGameObjectID goID;
private:
	

};