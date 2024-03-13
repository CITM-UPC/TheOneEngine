#pragma once
// only has information of itself
// the emmiter can change part of its values

// instead of having initColor and endColor, having deltaColor and multiplying it with dt and adding it to the color

#include "Types.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"


class Particle {
public:
	float duration;
	float lifetime;

	void Update(double dt);
	void Render();

	vec3 color;
	vec3 deltaColor;

	vec3 position;
	vec3 speed;

	glm::quat rotation;


	vec3 scale = vec3(0.1, 0.1, 0.1);

private:

};