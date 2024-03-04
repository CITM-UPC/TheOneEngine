#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__
#pragma once

#include "Panel.h"

#include "../TheOneEngine/Transform.h"

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

	HandleSpace GetHandleSpace() const { return handleSpace; }
	HandlePosition GetHandlePosition() const { return handlePosition; }

public:
	bool isHovered;
	std::vector<Ray> rays;

	bool drawMesh;
	bool drawWireframe;
	bool drawNormalsVerts;
	bool drawNormalsFaces;
	bool drawAABB;
	bool drawOBB;
	bool drawRaycasting;
	bool drawChecker;

private:
	HandleSpace handleSpace;
	HandlePosition handlePosition;
	int gizmoType;
	int gizmoMode;

	const char* spaces[2] =
	{
		"Local",
		"Global",
	};

	const char* positions[2] =
	{
		"Pivot",
		"Center",
	};
};

#endif // !__PANEL_SCENE_H__