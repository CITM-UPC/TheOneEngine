#include "PanelHierarchy.h"
#include "App.h"
#include "SceneManager.h"
#include "Gui.h"
#include "imgui.h"

PanelHierarchy::PanelHierarchy(PanelType type, std::string name) : Panel(type, name) {}

PanelHierarchy::~PanelHierarchy() {}

void PanelHierarchy::ShowChildren(std::shared_ptr<GameObject> parent)
{
	for (const auto childGO : parent.get()->children)
	{
		if (childGO.get()->children.capacity() != 0)
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
	uint treeFlags = 0;

	if (ImGui::Begin(name.c_str(), &enabled, settingsFlags))
	{
		if (ImGui::BeginChild("GameObjects", ImVec2(325, 0), true))
		{
			uint counter = 0;
			for (const auto gameObject : app->sceneManager->GetGameObjects())
			{
				if (gameObject.get()->children.size() == 0)
					treeFlags |= ImGuiTreeNodeFlags_Leaf;
				
				if (ImGui::TreeNodeEx(gameObject.get()->GetName().data(), treeFlags))
				{
					if (ImGui::IsItemClicked(0))
					{
						app->sceneManager->SetSelectedGO(gameObject);
						drag = gameObject;
						//treeFlags |= ImGuiTreeNodeFlags_Selected;
					}
					ShowChildren(gameObject);

					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Duplicate"))
						{
							//Historn: Add Duplicate function
						}
							
						if (ImGui::MenuItem("Remove"))
						{
							gameObject.get()->Disable(); //Historn: Change to remove function
						}

						ImGui::EndPopup();
					}

					ImGui::TreePop();
				}
			}

			ImGui::EndChild();
		}		
		ImGui::SameLine();

		if (drag && ImGui::IsMouseReleased(0))
			drag = nullptr;


		ImGui::End();
	}

	return true;
}