#include "PanelHierarchy.h"
#include "App.h"
#include "SceneManager.h"
#include "Gui.h"
#include "imgui.h"

PanelHierarchy::PanelHierarchy(PanelType type, std::string name) : Panel(type, name) {}

PanelHierarchy::~PanelHierarchy() {}

bool PanelHierarchy::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name.c_str(), &enabled, settingsFlags))
	{
		if (ImGui::BeginChild("GameObjects", ImVec2(325, 0), true))
		{
			for (const auto gameObject : app->sceneManager->GetGameObjects())
			{

			}

			ImGui::EndChild();
		}		
		ImGui::SameLine();

		ImGui::End();
	}

	return true;
}