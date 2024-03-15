#include "ItemUI.h"

ItemUI::ItemUI(std::shared_ptr<GameObject> containerGO, UiType type, bool interactuable, Rect2D rect) : containerGO(containerGO), type(type), interactuable(interactuable), imageRect(rect)
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
	return imageRect;
}

void ItemUI::SetSize(float width, float height)
{
	imageRect.w = width;
	imageRect.h = height;
}

json ItemUI::SaveUIElement()
{
	json uiElementJSON;

	uiElementJSON["Rect"] = { imageRect.x, imageRect.y, imageRect.w, imageRect.h };
	uiElementJSON["Type"] = (int)type;
	uiElementJSON["Interactuable"] = interactuable;

	return uiElementJSON;
}

void ItemUI::LoadUIElement(const json& UIElementJSON)
{
	if (UIElementJSON.contains("Rect"))
	{
		imageRect.x = UIElementJSON["Rect"][0];
		imageRect.y = UIElementJSON["Rect"][1];
		imageRect.w = UIElementJSON["Rect"][2];
		imageRect.h = UIElementJSON["Rect"][3];
	}
	if (UIElementJSON.contains("Type")) type = (UiType)UIElementJSON["Type"];
	if (UIElementJSON.contains("Interactuable")) interactuable = UIElementJSON["Interactuable"];
}