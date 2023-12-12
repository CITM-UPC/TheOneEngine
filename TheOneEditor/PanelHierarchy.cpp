#include "PanelHierarchy.h"
#include "App.h"
#include "SceneManager.h"
#include "Gui.h"
#include "imgui.h"

#include <variant>

PanelHierarchy::PanelHierarchy(PanelType type, std::string name) : Panel(type, name), open_selected(false) {}

PanelHierarchy::~PanelHierarchy() {}

void PanelHierarchy::RecurseShowChildren(std::shared_ptr<GameObject> parent)
{
	uint treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (ImGui::IsItemClicked(0) && !ImGui::IsItemToggledOpen())
	{
		app->sceneManager->SetSelectedGO(parent);
		drag = parent;
		LOG(LogType::LOG_INFO, "SelectedGO: %s", app->sceneManager->GetSelectedGO().get()->GetName().c_str());
	}
	if (drag && ImGui::IsMouseReleased(0) && drag != parent)
	{
		drag.get()->parent = parent;
		drag = nullptr;
	}
	if (parent == app->sceneManager->GetSelectedGO())
	{
		treeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	ContextMenu(parent);
	
	for (const auto childGO : parent.get()->children)
	{
		if (childGO.get()->children.size() == 0)
			treeFlags |= ImGuiTreeNodeFlags_Leaf;

		//Parent is not detected if not opened
		if (childGO == app->sceneManager->GetSelectedGO() || parent == app->sceneManager->GetSelectedGO())
		{
			treeFlags |= ImGuiTreeNodeFlags_Selected;
		}
		else
		{
			treeFlags &= ~ImGuiTreeNodeFlags_Selected;
		}

		if (ImGui::TreeNodeEx(childGO.get()->GetName().data(), treeFlags))
		{
			//treeFlags &= ~ImGuiTreeNodeFlags_Selected;
			RecurseShowChildren(childGO);

			ImGui::TreePop();
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
			RecurseShowChildren(app->sceneManager->GetRootSceneGO());

			ImGui::EndChild();
		}
		ImGui::SameLine();

		//Release GameObject
		/*if (drag && ImGui::IsMouseReleased(0))
			drag = nullptr;*/

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
			//Historn: Change to remove function
			LOG(LogType::LOG_INFO, "Use Count: %d", go.use_count());
			//go.get()->Delete();
		}

		ImGui::EndPopup();
	}
}
