#include "PanelHierarchy.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

PanelHierarchy::PanelHierarchy(PanelType type, std::string name) : Panel(type, name) {}

PanelHierarchy::~PanelHierarchy() {}

bool PanelHierarchy::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Hierarchy", &enabled, settingsFlags))
	{


		ImGui::End();
	}	

	return true;
}