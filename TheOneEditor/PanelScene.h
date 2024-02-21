#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__
#pragma once

#include "Panel.h"

#include<vector>

class Ray;
class Camera;

class PanelScene : public Panel
{
public:
	PanelScene(PanelType type, std::string name);
	~PanelScene();

	bool Draw();

	Ray GetScreenRay(int x, int y, Camera* camera, int width, int height);

public:
	bool isHovered;
	std::vector<Ray> rays;

	bool drawMesh;
	bool drawWireframe;
	bool drawNormalsVerts;
	bool drawNormalsFaces;
	bool drawAABB;
	bool drawOBB;
	bool drawChecker;
};

#endif // !__PANEL_SCENE_H__