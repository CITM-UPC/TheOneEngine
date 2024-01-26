#include "pch.h"
#include "TankMovement.h"

TankMovement::TankMovement() : CPPScript()
{
}

TankMovement::~TankMovement()
{
}

void TankMovement::Start()
{
	//GameObject* root = SceneManager::GetRootSceneGO();

	/*GameObject* wheels = containerGO.lock().get()->FindWithName("Lower_Tank");
	if (wheels != nullptr)
	{
		wheels_transform = (Transform*)wheels->GetComponent<Transform>();
	}

	GameObject* turret = containerGO.lock().get()->FindWithName("TankTurret");
	if (turret != nullptr)
	{
		turret_transform = (Transform*)turret->GetComponent<Transform>();
	}*/
}

void TankMovement::Update(double dt)
{
}
