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

json ItemUI::SaveUIElement()
{
	json uiElementJSON;

	uiElementJSON["Rect"] = { rect.x, rect.y, rect.w, rect.h };
	uiElementJSON["Type"] = (int)type;
	uiElementJSON["Interactuable"] = interactuable;

	return uiElementJSON;
}

void ItemUI::LoadUIElement(const json& UIElementJSON)
{
	if (UIElementJSON.contains("Rect"))
	{
		rect.x = UIElementJSON["Rect"][0];
		rect.y = UIElementJSON["Rect"][1];
		rect.w = UIElementJSON["Rect"][2];
		rect.h = UIElementJSON["Rect"][3];
	}
	if (UIElementJSON.contains("Type")) type = (UiType)UIElementJSON["Type"];
	if (UIElementJSON.contains("Interactuable")) interactuable = UIElementJSON["Interactuable"];
}