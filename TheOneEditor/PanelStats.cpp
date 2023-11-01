#include "PanelStats.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

PanelStats::PanelStats(PanelType type) : Panel(type) {}

PanelStats::~PanelStats() {}

bool PanelStats::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Stats", &enabled, settingsFlags))
	{


		ImGui::End();
	}

	return true;
}