#pragma once
#include "Particle.h"
#include "SingleOrRandom.h"
#include <vector>

class Emmiter;

class UpdateEmmiterModule {
public:
	virtual void Reset() {};

	virtual void Update(double dt, Particle* particle) {};

	enum UpdateEmmiterModuleType {
		CHANGE_COLOR
	};

	UpdateEmmiterModuleType type;

protected:
	Emmiter* owner;
};
