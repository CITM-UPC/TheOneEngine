#include "PanelHierarchy.h"
#include "App.h"
#include "SceneManager.h"
#include "Gui.h"
#include "imgui.h"

PanelHierarchy::PanelHierarchy(PanelType type, std::string name) : Panel(type, name) {}

PanelHierarchy::~PanelHierarchy() {}

void PanelHierarchy::RecurseShowChildren(std::shared_ptr<GameObject> parent)
{
	uint treeFlags = 0;
	for (const auto childGO : parent.get()->children)
	{
		if (childGO.get()->children.size() == 0)
			treeFlags |= ImGuiTreeNodeFlags_Leaf;

		if (ImGui::TreeNodeEx(childGO.get()->GetName().data(), treeFlags))
		{
			if (ImGui::Selectable(childGO.get()->GetName().data(), app->sceneManager->GetSelectedGO() == childGO))
			{
				app->sceneManager->SetSelectedGO(childGO);
			}
			RecurseShowChildren(childGO);

			ImGui::TreePop();
		}
		//ContextMenu(childGO);
	}
}

bool PanelHierarchy::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;
	uint treeFlags = 0;

	if (ImGui::Begin(name.c_str(), &enabled, settingsFlags))
	{
		if (ImGui::BeginChild("GameObjects", ImVec2(325, 0), true))
		{
			for (const auto gameObject : app->sceneManager->GetGameObjects())
			{
				if (gameObject.get()->children.size() == 0)
					treeFlags |= ImGuiTreeNodeFlags_Leaf;

				if (ImGui::TreeNodeEx(gameObject.get()->GetName().data(), treeFlags))
				{
					if (ImGui::Selectable(gameObject.get()->GetName().data(), app->sceneManager->GetSelectedGO() == gameObject))
					{
						app->sceneManager->SetSelectedGO(gameObject);
					}
					RecurseShowChildren(gameObject);

					ImGui::TreePop();
				}
				ContextMenu(gameObject);
			}

			ImGui::EndChild();
		}
		ImGui::SameLine();

		//Release GameObject
		if (drag && ImGui::IsMouseReleased(0))
			drag = nullptr;

		ImGui::End();
	}

	return true;
}

void PanelHierarchy::ContextMenu(std::shared_ptr<GameObject> go)
{
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Duplicate"))
		{
			//Historn: Add Duplicate function
		}

		if (ImGui::MenuItem("Remove"))
		{
			go.get()->Disable(); //Historn: Change to remove function
		}

		ImGui::EndPopup();
	}
}
