#pragma once

#include "TheOneScripting.h"

class THEONESCRIPTING_API TankMovement : public Script
{
	TankMovement();
	virtual ~TankMovement();

	void Start();

	void Update(double dt);

public:

	GameObject bullet;
	Transform* wheels_transform = nullptr;
	Transform* turret_transform = nullptr;

	float max_velocity_forward = 8.0f;
	float max_velocity_backward = -8.0f;

	float velocity = 0.0f;
	float friction_force = 0.05f;
	float acceleration = 4.0f;
	float recoil = 0.0f;
	float recoil_velocity = 6.0f;
	vec3f recoil_direction = { 0,0,0 };
	float turning_velocity = 30.0f;

private:

	float angle = 0.0f;
	vec3f to_look = { 0,0,0 };
};

THEONE_API TankMovement* CreateTankMovement() {
	TankMovement* tank = new TankMovement();
	// To show in inspector here

	return tank;
}

//void* (*Creator)() = (void* (*)())GetProcAddress(App->scripts_dll, std::string("Create" + std::string(name)).data());