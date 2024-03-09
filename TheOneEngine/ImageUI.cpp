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

	//glm::mat4 projectionMatrix = glm::translate(glm::mat4(1.0f), glm::fvec3(x, y, 0)) * glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::fvec3(GetRect().w, GetRect().h, 1));
	//glPushMatrix();
	//glMultMatrixf(glm::value_ptr(projectionMatrix));

	//glLineWidth(2.0f);

	glEnable(GL_TEXTURE_2D);

	//image.get()->bind();

	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);


	glBegin(GL_QUADS);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);  // Bottom-left corner
	glVertex2f(rect.x - width / 2, rect.y + height / 2);

	glTexCoord2f(1.0f, 1.0f);  // Bottom-right corner
	glVertex2f(rect.x + width / 2, rect.y + height / 2);

	glTexCoord2f(1.0f, 0.0f);  // Top-right corner
	glVertex2f(rect.x + width / 2, rect.y - height / 2);

	glTexCoord2f(0.0f, 0.0f);  // Top-left corner
	glVertex2f(rect.x - width / 2, rect.y - height / 2);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);


	//glPopMatrix();
}