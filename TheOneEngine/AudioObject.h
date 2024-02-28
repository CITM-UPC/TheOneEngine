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

	void SetPosition(std::shared_ptr<GameObject> containerGO);
	void SetRotation(std::shared_ptr<GameObject> containerGO);
	void SetTransform(std::shared_ptr<GameObject> containerGO);

private:
	AkGameObjectID goID;

};