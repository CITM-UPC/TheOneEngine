#include "PanelAbout.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

PanelAbout::PanelAbout(PanelType type) : Panel(type) {}

PanelAbout::~PanelAbout() {}

bool PanelAbout::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags |= ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("About TheOneEngine", &enabled, settingsFlags))
	{
		ImGui::Text("TheOneEngine by Hector Bascones Zamora & Arnau Jimenez Gallego.");
		ImGui::Text("This is a demo for the subject of Game Engines, CITM - UPC");
	}	

	return true;
}