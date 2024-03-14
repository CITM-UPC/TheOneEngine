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

json SetSpeed::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["UsingSingleValueSpeed"] = speed.usingSingleValue;
	moduleJSON["MinSpeed"] = { speed.rangeValue.lowerLimit.x, speed.rangeValue.lowerLimit.y, speed.rangeValue.lowerLimit.z };
	moduleJSON["MaxSpeed"] = { speed.rangeValue.upperLimit.x, speed.rangeValue.upperLimit.y, speed.rangeValue.upperLimit.z };

	return moduleJSON;
}

void SetSpeed::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("UsingSingleValueSpeed"))
	{
		speed.usingSingleValue = moduleJSON["UsingSingleValueSpeed"];
	}

	if (moduleJSON.contains("MinSpeed"))
	{
		speed.rangeValue.lowerLimit.x = moduleJSON["MinSpeed"][0];
		speed.rangeValue.lowerLimit.y = moduleJSON["MinSpeed"][1];
		speed.rangeValue.lowerLimit.z = moduleJSON["MinSpeed"][2];

	}

	if (moduleJSON.contains("MaxSpeed"))
	{
		speed.rangeValue.upperLimit.x = moduleJSON["MaxSpeed"][0];
		speed.rangeValue.upperLimit.y = moduleJSON["MaxSpeed"][1];
		speed.rangeValue.upperLimit.z = moduleJSON["MaxSpeed"][2];
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

json SetColor::SaveModule()
{
	json moduleJSON;

	moduleJSON["Type"] = type;

	moduleJSON["UsingSingleValueColor"] = color.usingSingleValue;
	moduleJSON["MinColor"] = { color.rangeValue.lowerLimit.x, color.rangeValue.lowerLimit.y, color.rangeValue.lowerLimit.z };
	moduleJSON["MaxColor"] = { color.rangeValue.upperLimit.x, color.rangeValue.upperLimit.y, color.rangeValue.upperLimit.z };

	return moduleJSON;
}

void SetColor::LoadModule(const json& moduleJSON)
{
	if (moduleJSON.contains("Type"))
	{
		type = moduleJSON["Type"];
	}

	if (moduleJSON.contains("UsingSingleValueColor"))
	{
		color.usingSingleValue = moduleJSON["UsingSingleValueColor"];
	}

	if (moduleJSON.contains("MinColor"))
	{
		color.rangeValue.lowerLimit.x = moduleJSON["MinColor"][0];
		color.rangeValue.lowerLimit.y = moduleJSON["MinColor"][1];
		color.rangeValue.lowerLimit.z = moduleJSON["MinColor"][2];

	}

	if (moduleJSON.contains("MaxColor"))
	{
		color.rangeValue.upperLimit.x = moduleJSON["MaxColor"][0];
		color.rangeValue.upperLimit.y = moduleJSON["MaxColor"][1];
		color.rangeValue.upperLimit.z = moduleJSON["MaxColor"][2];
	}
}
