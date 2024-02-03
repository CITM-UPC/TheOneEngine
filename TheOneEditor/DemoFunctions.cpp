#include "DemoFunctions.h"
#include "App.h"
#include "ModuleScripting.h"
#include "SceneManager.h"
#include "../TheOneEngine/GameObject.h"
#include "../TheOneEngine/Transform.h"

std::shared_ptr<GameObject> Demo::CreateTank() {

	// Tank base
	std::shared_ptr<GameObject> tank = app->sceneManager->CreateCube();
	auto transform = tank->GetComponent<Transform>();
	transform->setScale({ 1,1,2 });
	transform->setPosition({ 0,1,0 });
	ComponentScript* script = tank->AddScriptComponent("Assets\\Scripts\\DemoTankMovement.lua");
	app->scripting->CreateScript(script);
	tank->SetName("Tank");

	// Turret
	std::shared_ptr<GameObject> root = app->sceneManager->GetRootSceneGO();
	std::shared_ptr<GameObject> turret = app->sceneManager->CreateCube();

	// We remove it from root and add it to Tank Base
	for (auto child = root->children.begin(); child != root->children.end(); ++child) {
		if ((*child) == turret) {
			root->children.erase(child);
			break;
		}
	}
	tank->children.push_back(turret);

	transform = turret->GetComponent<Transform>();
	transform->setScale({ 0.5, 0.5, 1 });
	transform->setPosition({ 0, 0.5, 0.5 });
	script = turret->AddScriptComponent("Assets\\Scripts\\DemoTurretMovement.lua");
	app->scripting->CreateScript(script);
	turret->SetName("Turret");

	return tank;
}

std::shared_ptr<GameObject> Demo::CreateBullet() {
	return std::shared_ptr<GameObject>();
}
