#include "Source.h"

Source::Source(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Source), AudioObject(containerGO), path(path), volume(volume)
{
	volume = 50;
	path = "";
	SetTransform(containerGO);
}

Source::~Source()
{
}

json Source::SaveComponent()
{
	return json();
}

void Source::LoadComponent(const json& transformJSON)
{
}
