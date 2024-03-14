#pragma once
#include "Particle.h"
#include "SingleOrRandom.h"
#include <vector>
#include "Defs.h"
#include "Billboard.h"

class Emmiter;

class RenderEmmiterModule {
public:
	virtual void Reset() {};

	virtual void Update(Particle* particle, vec3 cameraPosition) {};

	virtual json SaveModule() { return json(); };

	virtual void LoadModule(const json& moduleJSON) { };

	enum RenderEmmiterModuleType {
		BILLBOARD
	};

	RenderEmmiterModuleType type;

protected:
	Emmiter* owner;
};


class BillboardRender : public RenderEmmiterModule {
public:
	BillboardRender(Emmiter* owner);

	BillboardType billboardType;

	void Update(Particle* particle, vec3 cameraPosition) override;

	json SaveModule();

	void LoadModule(const json& moduleJSON);

};