#pragma once

#include "BBox.hpp"

class Graphic {
public:
	AABBox aabb;
	virtual void draw() = 0;
	virtual ~Graphic() = default;
};