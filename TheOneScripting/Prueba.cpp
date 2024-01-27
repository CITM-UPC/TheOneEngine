#include "pch.h"
#include "Prueba.h"

Prueba::Prueba() : CPPScript()
{
}

Prueba::~Prueba()
{
}

void Prueba::Start()
{

}

void Prueba::Update(double dt)
{
	transform->rotate({ 0, 1, 0 }, rotationAngle);
	rotationAngle += rotationSpeed * dt;
}
