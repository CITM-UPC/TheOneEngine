#pragma once
#include "Particle.h"
#include "SingleOrRandom.h"
#include "Defs.h"
#include <vector>

class Emmiter;

class UpdateEmmiterModule {
public:
	virtual void Reset() {};

	virtual void Update(double dt, Particle* particle) {};

	virtual json SaveModule() { return json(); };

	virtual void LoadModule(const json& moduleJSON) { };

	enum UpdateEmmiterModuleType {
		CHANGE_COLOR
	};

	UpdateEmmiterModuleType type;

protected:
	Emmiter* owner;
};
