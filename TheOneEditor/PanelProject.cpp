#include "PanelProject.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

PanelProject::PanelProject(PanelType type) : Panel(type) {}

PanelProject::~PanelProject() {}

bool PanelProject::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Project", &enabled, settingsFlags))
	{


		ImGui::End();
	}	

	return true;
}