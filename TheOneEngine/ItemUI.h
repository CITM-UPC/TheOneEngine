#ifndef __ITEMUI_H__
#define __ITEMUI_H__
#pragma once

#include "GameObject.h"

enum class UiType {
	IMAGE,
	UNKNOWN
};

struct Rect2D
{
	float x, y, w, h;
};

class ItemUI
{
public:
	ItemUI(std::shared_ptr<GameObject> containerGO, UiType type, bool interactuable = false, Rect2D rect = { 0,0,1,1 });
	virtual ~ItemUI();

	virtual void Draw2D();

	Rect2D GetRect() const;
	void SetSize(float width, float height);

protected:

	Rect2D rect;
	bool interactuable;

	std::shared_ptr<GameObject> containerGO;
	UiType type;


};

#endif // !__ITEMUI_H__