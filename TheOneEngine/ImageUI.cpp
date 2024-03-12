#include "ImageUI.h"
#include "Canvas.h"

ImageUI::ImageUI(std::shared_ptr<GameObject> containerGO, Rect2D rect) : ItemUI(containerGO, UiType::IMAGE, false, rect)
{
	imagePath = "Assets/Meshes/HUD.png";
	image = std::make_unique<Texture>(imagePath, containerGO);
}

ImageUI::ImageUI(std::shared_ptr<GameObject> containerGO, const std::string& path, Rect2D rect) : ItemUI(containerGO, UiType::IMAGE, false, rect), imagePath(path)
{
	image = std::make_unique<Texture>(imagePath);
}

ImageUI::~ImageUI(){}

void ImageUI::Draw2D()
{
	auto canvas = containerGO.get()->GetComponent<Canvas>();

	rect.x = canvas->GetRect().x + GetRect().x;
	rect.y = canvas->GetRect().y + GetRect().y;

	float width = (canvas->GetRect().w * rect.w) / 100;
	float height = (canvas->GetRect().h * rect.h) / 100;

	//glLineWidth(2.0f);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//glm::mat4 projectionMatrix = glm::translate(glm::mat4(1.0f), glm::fvec3(x, y, 0)) * glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::fvec3(GetRect().w, GetRect().h, 1));
	//glPushMatrix();
	//glMultMatrixf(glm::value_ptr(projectionMatrix));


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	image.get()->bind();


	glBegin(GL_QUADS);

	glTexCoord2f(1.0f, 0.0f);  // Bottom-right corner of the texture
	glVertex2f(rect.x - width / 2, rect.y + height / 2);

	glTexCoord2f(0.0f, 0.0f);  // Bottom-left corner of the texture
	glVertex2f(rect.x + width / 2, rect.y + height / 2);

	glTexCoord2f(0.0f, 1.0f);  // Top-left corner of the texture
	glVertex2f(rect.x + width / 2, rect.y - height / 2);

	glTexCoord2f(1.0f, 1.0f);  // Top-right corner of the texture
	glVertex2f(rect.x - width / 2, rect.y - height / 2);

	glEnd();

	/*glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);


	//glPopMatrix();
}

json ImageUI::SaveUIElement()
{
	json uiElementJSON;

	uiElementJSON["Rect"] = { rect.x, rect.y, rect.w, rect.h };
	uiElementJSON["Type"] = (int)type;
	uiElementJSON["Interactuable"] = interactuable;

	uiElementJSON["ImagePath"] = imagePath;

	return uiElementJSON;
}

void ImageUI::LoadUIElement(const json& UIElementJSON)
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

	if (UIElementJSON.contains("ImagePath")) imagePath = UIElementJSON["ImagePath"];
	image = std::make_unique<Texture>(imagePath);
}