#include "SpawnEmmiterModule.h"
#include "Emmiter.h"

ConstantSpawnRate::ConstantSpawnRate(Emmiter* owner)
{
	type = CONSTANT;
	this->owner = owner;
	duration = 10;
	spawnRate = 0.5f;
	timeFromLastSpawn = 0;
}

void ConstantSpawnRate::Update(double dt)
{
	if (spawnRate <= 0.001) return;

	timeFromLastSpawn += dt;

	if (timeFromLastSpawn >= 1 / spawnRate) {
		int ticks = 0;

		while (timeFromLastSpawn >= 1 / spawnRate) {
			timeFromLastSpawn -= 1 / spawnRate;
			ticks++;
		}

		owner->SpawnParticles(ticks);
	}
}

SingleBurstSpawn::SingleBurstSpawn(Emmiter* owner)
{
	type = SINGLE_BURST;
	this->owner = owner;
	duration = 10;
	amount = 10;
	activated = false;
}

void SingleBurstSpawn::Reset()
{
	activated = false;
}

void SingleBurstSpawn::Update(double dt)
{
	if (activated) return;
	owner->SpawnParticles(amount);

	activated = true;
}

ConstantBurstSpawn::ConstantBurstSpawn(Emmiter* owner)
{
	type = CONSTANT_BURST;
	this->owner = owner;
	duration = 10;
	spawnRate = 0.5f;
	timeFromLastSpawn = 0;
	amount = 10;
}

void ConstantBurstSpawn::Update(double dt)
{
	if (spawnRate <= 0.001) return;

	timeFromLastSpawn += dt;

	if (timeFromLastSpawn >= 1 / spawnRate) {
		int ticks = 0;

		while (timeFromLastSpawn >= 1 / spawnRate) {
			timeFromLastSpawn -= 1 / spawnRate;
			ticks++;
		}

		owner->SpawnParticles(ticks * amount);
	}
}
