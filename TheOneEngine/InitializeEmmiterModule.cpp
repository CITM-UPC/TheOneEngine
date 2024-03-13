#include "InitializeEmmiterModule.h"

SetSpeed::SetSpeed()
{
	type = SET_SPEED;
}

void SetSpeed::Initialize(Particle* particle)
{
	if (speed.usingSingleValue) {
		particle->speed = speed.singleValue;
	}
	else {
		vec3 randomVec = vec3{
			randomFloat(speed.rangeValue.lowerLimit.x, speed.rangeValue.upperLimit.x),
			randomFloat(speed.rangeValue.lowerLimit.y, speed.rangeValue.upperLimit.y),
			randomFloat(speed.rangeValue.lowerLimit.z, speed.rangeValue.upperLimit.z) };

		particle->speed = randomVec;
	}
}

SetColor::SetColor()
{
	type = SET_COLOR;
}

void SetColor::Initialize(Particle* particle)
{
	if (color.usingSingleValue) {
		particle->color = color.singleValue;
	}
	else {
		vec3 randomVec = vec3{
			randomFloat(color.rangeValue.lowerLimit.r, color.rangeValue.upperLimit.r),
			randomFloat(color.rangeValue.lowerLimit.g, color.rangeValue.upperLimit.g),
			randomFloat(color.rangeValue.lowerLimit.b, color.rangeValue.upperLimit.b) };

		particle->color = randomVec;
	}
}
