#pragma once

#include "BBox.hpp"

class Graphic {
public:
	AABBox aabb;
	void draw();
	virtual ~Graphic() = default;
};