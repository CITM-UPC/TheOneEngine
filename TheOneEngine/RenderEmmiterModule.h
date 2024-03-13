#pragma once
#include "Particle.h"
#include "SingleOrRandom.h"
#include <vector>
#include "Billboard.h"

class Emmiter;

class RenderEmmiterModule {
public:
	virtual void Reset() {};

	virtual void Update(Particle* particle, vec3 cameraPosition) {};

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

	BillboardType type;

	void Update(Particle* particle, vec3 cameraPosition) override;
};