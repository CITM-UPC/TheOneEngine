#include "PanelHierarchy.h"
#include "App.h"
#include "SceneManager.h"
#include "Gui.h"
#include "imgui.h"

PanelHierarchy::PanelHierarchy(PanelType type, std::string name) : Panel(type, name) {}

PanelHierarchy::~PanelHierarchy() {}

void PanelHierarchy::ShowChildren(std::shared_ptr<GameObject> parent)
{
	for (const auto childGO : parent.get()->GetChildren())
	{
		if (childGO.get()->GetChildren().capacity() != 0)
		{
			if (ImGui::Selectable(childGO.get()->GetName().data(), app->sceneManager->GetSelectedGO() == childGO))
			{
				app->sceneManager->SetSelectedGO(childGO);
			}
			ShowChildren(childGO);
		}
	}
}

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
				ShowChildren(gameObject);
			}

			ImGui::EndChild();
		}		
		ImGui::SameLine();

		ImGui::End();
	}

	return true;
}