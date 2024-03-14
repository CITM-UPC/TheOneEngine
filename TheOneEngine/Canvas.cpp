#include "Canvas.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "ItemUI.h"
#include "ImageUI.h"

Canvas::Canvas(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Canvas)
{}

Canvas::Canvas(std::shared_ptr<GameObject> containerGO, Canvas* ref) : Component(containerGO, ComponentType::Canvas)
{
	//this->uiElements = ref->GetUiElements();
	for (auto& itemPtr : ref->GetUiElements())
	{
		std::unique_ptr<ItemUI> uniquePtr(itemPtr);
		this->uiElements.push_back(std::move(uniquePtr));
	}
	this->debugDraw = ref->debugDraw;
	this->rect = ref->rect;
}

Canvas::~Canvas() {}

void Canvas::DrawComponent(Camera* camera)
{
	if (!camera)
		return;

	SetTo2DRenderSettings(true);

	if (camera->IsEnabled() && this->IsEnabled())
	{
		float width = 2.0f * camera->frustum._near.distance * glm::radians(camera->fov);
		SetSize(width, width / camera->aspect);
	}

	//glPushMatrix();
	//
	//glLineWidth(2.0f);


	//glBegin(GL_LINES);

	//glColor4f(1.0f, 0.0f, 0.0f, 1.0f);										// X Axis.
	//glVertex2f(rect.x - rect.w / 2, rect.y + rect.h / 2);			glVertex2f(rect.x + rect.w / 2, rect.y + rect.h / 2);
	//glVertex2f(rect.x + rect.w / 2, rect.y + rect.h / 2);			glVertex2f(rect.x + rect.w / 2, rect.y - rect.h / 2);
	//glVertex2f(rect.x + rect.w / 2, rect.y - rect.h / 2);			glVertex2f(rect.x - rect.w / 2, rect.y - rect.h / 2);
	//glVertex2f(rect.x - rect.w / 2, rect.y - rect.h / 2);			glVertex2f(rect.x - rect.w / 2, rect.y + rect.h / 2);

	//glEnd();

	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//glLineWidth(1.0f);

	//glPopMatrix();

	for (auto element = uiElements.rbegin(); element != uiElements.rend(); ++element)
	{
		(*element)->Draw2D();
	}

	SetTo2DRenderSettings(false);
}

void Canvas::SetTo2DRenderSettings(const bool& setTo)
{
	Camera* camera = containerGO.lock().get()->GetComponent<Camera>();

	float width = 2.0f * camera->frustum._near.distance * glm::radians(camera->fov);

	glm::mat4 viewMatrix = glm::transpose(glm::mat4(camera->getViewMatrix()));
	glm::mat4 projectionMatrix = glm::transpose(glm::mat4(camera->projectionMatrix));

	if (setTo)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//gluOrtho2D(0.0, 1.0, 1.0, 0.0);
		glOrtho(-rect.w / 2, rect.w / 2, -(rect.w / camera->aspect) / 2, (rect.w / camera->aspect) / 2, 100.0f, -100.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(viewMatrix));
		/*glPushMatrix();
		glLoadIdentity();*/
		/*glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);*/
	}
	else
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(projectionMatrix));
		glMatrixMode(GL_MODELVIEW);

		/*glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);*/
	}
}

Rect2D Canvas::GetRect() const
{
	return rect;
}

void Canvas::SetSize(float width, float height)
{
	rect.w = width;
	rect.h = height;
}

json Canvas::SaveComponent()
{
	json canvasJSON;

	canvasJSON["UID"] = UID;
	canvasJSON["Name"] = name;
	canvasJSON["Type"] = type;
	canvasJSON["Rect"] = { rect.x, rect.y, rect.w, rect.h };
	canvasJSON["DebugDraw"] = debugDraw;

	if (auto pGO = containerGO.lock())
		canvasJSON["ParentUID"] = pGO.get()->GetUID();

	if (!uiElements.empty())
	{
		json uiElementsJSON;
		for (auto& item : uiElements)
		{
			uiElementsJSON.push_back(item.get()->SaveUIElement());
		}
		canvasJSON["UiElements"] = uiElementsJSON;
	}

	return canvasJSON;
}

void Canvas::LoadComponent(const json& canvasJSON)
{
	if (canvasJSON.contains("UID")) UID = canvasJSON["UID"];
	if (canvasJSON.contains("Name")) name = canvasJSON["Name"];
	if (canvasJSON.contains("Type")) type = canvasJSON["Type"];
	if (canvasJSON.contains("Rect"))
	{
		rect.x = canvasJSON["Rect"][0];
		rect.y = canvasJSON["Rect"][1];
		rect.w = canvasJSON["Rect"][2];
		rect.h = canvasJSON["Rect"][3];
	}
	if (canvasJSON.contains("DebugDraw")) debugDraw = canvasJSON["DebugDraw"];

	if (canvasJSON.contains("UiElements"))
	{
		const json& uiElementsJSON = canvasJSON["UiElements"];

		for (auto& item : uiElementsJSON)
		{			
			if (item["Type"] == (int)UiType::IMAGE)
			{
				int id = this->AddItemUI<ImageUI>();
				this->GetItemUI<ImageUI>(id)->LoadUIElement(item);
			}
			if (item["Type"] == (int)UiType::UNKNOWN)
			{
				//default
			}
		}
	}
}

std::vector<ItemUI*> Canvas::GetUiElements()
{
	std::vector<ItemUI*> tempUiElements;
	for (const auto& item : uiElements)
	{
		tempUiElements.push_back(item.get());
	}
	return tempUiElements;
}
