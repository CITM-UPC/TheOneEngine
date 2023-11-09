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
			uint counter = 0;
			for (const auto gameObject : app->sceneManager->GetGameObjects())
			{
				if (ImGui::Selectable(gameObject.get()->GetName().data(), app->sceneManager->GetSelectedGO() == gameObject))
				{
					app->sceneManager->SetSelectedGO(gameObject);
				}
			}

			ImGui::EndChild();
		}		
		ImGui::SameLine();

		ImGui::End();
	}

	return true;
}