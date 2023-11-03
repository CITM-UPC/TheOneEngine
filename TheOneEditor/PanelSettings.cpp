#include "PanelSettings.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

PanelSettings::PanelSettings(PanelType type) : Panel(type) {}

PanelSettings::~PanelSettings() {}

bool PanelSettings::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	if (ImGui::Begin("Settings", &enabled, settingsFlags))
	{
		// LEFT - Buttons
		ImVec2 leftChildSize = ImVec2(ImGui::GetWindowSize().x * 0.2, ImGui::GetWindowSize().y);
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowSize().x * 0.2, 20);
	
		if (ImGui::BeginChild("", leftChildSize, true))
		{
			if (ImGui::Button("Performance", buttonSize))
				selected = SelectedSetting::PERFORMANCE;

			if (ImGui::Button("Window", buttonSize))
				selected = SelectedSetting::WINDOW;

			if (ImGui::Button("Input", buttonSize))
				selected = SelectedSetting::INPUT;

			if (ImGui::Button("Renderer", buttonSize))
				selected = SelectedSetting::RENDERER;

			ImGui::EndChild();
		}

		ImGui::SameLine();

		// RIGHT - Display selected
		settingsFlags &= ~(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		if (ImGui::BeginChild("Selected", ImVec2(ImGui::GetWindowSize().x * 0.8f, ImGui::GetWindowSize().y), true, settingsFlags))
		{
			switch (selected)
			{
				case SelectedSetting::PERFORMANCE:	Performance();	break;
				case SelectedSetting::WINDOW:		Window();		break;
				case SelectedSetting::INPUT:		Input();		break;
				case SelectedSetting::RENDERER:		Renderer();		break;
				default: Performance(); break;
			}

			ImGui::EndChild();
		}

		ImGui::End();
	}

	ImGui::PopStyleVar();

	return true;
}

void PanelSettings::Performance()
{
	ImGui::Text("Performance");
}

void PanelSettings::Window()
{
	ImGui::Text("Window");
}

void PanelSettings::Input()
{
	ImGui::Text("Input");
}

void PanelSettings::Renderer()
{
	ImGui::Text("Renderer");
}