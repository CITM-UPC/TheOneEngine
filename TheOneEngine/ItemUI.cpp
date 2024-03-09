#include "ItemUI.h"

ItemUI::ItemUI(std::shared_ptr<GameObject> containerGO, UiType type, bool interactuable, Rect2D rect) : containerGO(containerGO), type(type), interactuable(interactuable), rect(rect)
{
}

ItemUI::~ItemUI()
{
}

void ItemUI::Draw2D()
{

}

Rect2D ItemUI::GetRect() const
{
	return rect;
}

void ItemUI::SetSize(float width, float height)
{
	rect.w = width;
	rect.h = height;
}