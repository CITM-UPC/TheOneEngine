#pragma once
#include "Particle.h"
#include "SingleOrRandom.h"
#include "Defs.h"
#include <vector>

class Emmiter;

class InitializeEmmiterModule {
public:
	virtual void Reset() {};

	virtual void Initialize(Particle* particle) {};

	virtual const SingleOrRandom<vec3> getVector() { return SingleOrRandom<vec3>(); };

	virtual json SaveModule() { return json(); };

	virtual void LoadModule(const json& moduleJSON) { };

	enum InitializeEmmiterModuleType {
		SET_SPEED,
		SET_COLOR
	};

	InitializeEmmiterModuleType type;

protected:
	Emmiter* owner;
};


class SetSpeed : public InitializeEmmiterModule {
public:
	SetSpeed();

	const SingleOrRandom<vec3> getVector() {
		return speed;
	};

	void Initialize(Particle* particle);

	json SaveModule();

	void LoadModule(const json& moduleJSON);

	SingleOrRandom<vec3> speed;
};

class SetColor : public InitializeEmmiterModule {
public:
	SetColor();

	const SingleOrRandom<vec3> getVector() {
		return color;
	};

	void Initialize(Particle* particle);

	json SaveModule();

	void LoadModule(const json& moduleJSON);

	SingleOrRandom<vec3> color;
};