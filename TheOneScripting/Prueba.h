#pragma once
#include "pch.h"
#include "TheOneScripting.h"

class THEONESCRIPTING_API Prueba : public CPPScript
{
public:
	Prueba();
	virtual ~Prueba();

	void Start();

	void Update(double dt);

public:
	/*GameObject* goPrueba = nullptr;
	Transform* transformPrueba = nullptr;*/
};

THEONE_API Prueba* CreatePrueba() {
	Prueba* prueba = new Prueba();
	// To show in inspector here

	return prueba;
}