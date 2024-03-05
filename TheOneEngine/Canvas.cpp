#include "Canvas.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

Canvas::Canvas(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Canvas)
{}

Canvas::~Canvas() {}

void Canvas::DrawComponent()
{
	SetTo2DRenderSettings(true);

	Camera* camera = containerGO.lock().get()->GetComponent<Camera>();

	if (camera)
	{
		if (camera->IsEnabled() && this->IsEnabled())
		{
			float width = 2.0f * camera->frustum._near.distance * glm::radians(camera->fov);
			SetSize(width, width / camera->aspect);
		}
	}
	else
		return;

	glPushMatrix();
	
	glLineWidth(2.0f);


	glBegin(GL_LINES);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);										// X Axis.
	glVertex2f(rect.x - rect.w / 2, rect.y + rect.h / 2);			glVertex2f(rect.x + rect.w / 2, rect.y + rect.h / 2);
	glVertex2f(rect.x + rect.w / 2, rect.y + rect.h / 2);			glVertex2f(rect.x + rect.w / 2, rect.y - rect.h / 2);
	glVertex2f(rect.x + rect.w / 2, rect.y - rect.h / 2);			glVertex2f(rect.x - rect.w / 2, rect.y - rect.h / 2);
	glVertex2f(rect.x - rect.w / 2, rect.y - rect.h / 2);			glVertex2f(rect.x - rect.w / 2, rect.y + rect.h / 2);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);

	glPopMatrix();

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
		glOrtho(-width / 2, width / 2, -(width / camera->aspect) / 2, (width / camera->aspect) / 2, 100.0f, -100.0f);
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
    json transformJSON;

    /*transformJSON["Name"] = name;
    transformJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
        transformJSON["ParentUID"] = pGO.get()->GetUID();

    transformJSON["UID"] = UID;
    transformJSON["Position"] = { position.x, position.y, position.z };
    transformJSON["Rotation"] = { rotation.w, rotation.x, rotation.y, rotation.z };
    transformJSON["Scale"] = { scale.x, scale.y, scale.z };*/

    return transformJSON;
}

// hekbas - Fix this
void Canvas::LoadComponent(const json& transformJSON)
{
    //// Load basic properties
    //if (transformJSON.contains("UID"))
    //{
    //    UID = transformJSON["UID"];
    //}
}