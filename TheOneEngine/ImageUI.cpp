#include "ImageUI.h"
#include "Canvas.h"

ImageUI::ImageUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::IMAGE, name, false, rect)
{
	this->name = "Image";
	imagePath = "Assets/Meshes/HUD.png";
	image = std::make_unique<Texture>(imagePath, containerGO);
}

ImageUI::ImageUI(std::shared_ptr<GameObject> containerGO, const std::string& path, std::string name, Rect2D rect) : ItemUI(containerGO, UiType::IMAGE, name, false, rect), imagePath(path)
{
	image = std::make_unique<Texture>(imagePath);
}

ImageUI::~ImageUI(){}

void ImageUI::Draw2D()
{
	auto canvas = containerGO.get()->GetComponent<Canvas>();

	float posX = canvas->GetRect().x + GetRect().x;
	float posY = canvas->GetRect().y + GetRect().y;

	float width = (canvas->GetRect().w * imageRect.w);
	float height = (canvas->GetRect().h * imageRect.h);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	image.get()->bind();

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 1.0f);  // Top-left corner of the texture
	glVertex2f(posX - width / 2, posY + height / 2);
	
	glTexCoord2f(1.0f, 1.0f);  // Top-right corner of the texture
	glVertex2f(posX + width / 2, posY + height / 2);

	glTexCoord2f(1.0f, 0.0f);  // Bottom-right corner of the texture
	glVertex2f(posX + width / 2, posY - height / 2);

	glTexCoord2f(0.0f, 0.0f);  // Bottom-left corner of the texture
	glVertex2f(posX - width / 2, posY - height / 2);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
}

json ImageUI::SaveUIElement()
{
	json uiElementJSON;

	uiElementJSON["ID"] = (unsigned int)id;
	uiElementJSON["Name"] = name.c_str();
	uiElementJSON["Rect"] = { imageRect.x, imageRect.y, imageRect.w, imageRect.h };
	uiElementJSON["Type"] = (int)type;
	uiElementJSON["Interactuable"] = interactuable;

	uiElementJSON["ImagePath"] = imagePath;

	return uiElementJSON;
}

void ImageUI::LoadUIElement(const json& UIElementJSON)
{
	if (UIElementJSON.contains("ID")) id = (unsigned int)UIElementJSON["ID"];
	if (UIElementJSON.contains("Name")) name = UIElementJSON["Name"];
	if (UIElementJSON.contains("Rect"))
	{
		imageRect.x = UIElementJSON["Rect"][0];
		imageRect.y = UIElementJSON["Rect"][1];
		imageRect.w = UIElementJSON["Rect"][2];
		imageRect.h = UIElementJSON["Rect"][3];
	}
	if (UIElementJSON.contains("Type")) type = (UiType)UIElementJSON["Type"];
	if (UIElementJSON.contains("Interactuable")) interactuable = UIElementJSON["Interactuable"];

	if (UIElementJSON.contains("ImagePath")) imagePath = UIElementJSON["ImagePath"];
	image = std::make_unique<Texture>(imagePath);
}