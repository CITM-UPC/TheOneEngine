#pragma once

#include "Component.h"
#include "Emmiter.h"
#include <vector>
#include "Transform.h"

class ParticleSystem : public Component {

public:
	ParticleSystem(std::shared_ptr<GameObject> containerGO);
	ParticleSystem(std::shared_ptr<GameObject> containerGO, ParticleSystem* ref);
	~ParticleSystem();

	void Update() override;
	void DrawComponent() override;

	void Play();
	void Stop();
	void Replay();

	void ClearEmmiters();
	Emmiter* AddEmmiter();
	Emmiter* AddEmmiter(Emmiter* ref);

	json SaveComponent();
	void LoadComponent(const json& transformJSON);

	bool IsON();

	Transform* GetTransform();

	std::vector<std::unique_ptr<Emmiter>> emmiters;

protected:

private:
	Transform* transform;

	bool isON;
};