#include "PanelConsole.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

PanelConsole::PanelConsole(PanelType type) : Panel(type) {}

PanelConsole::~PanelConsole() {}

bool PanelConsole::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags |= ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Console", &enabled, settingsFlags))
	{
		

		ImGui::End();
	}

	return true;
}