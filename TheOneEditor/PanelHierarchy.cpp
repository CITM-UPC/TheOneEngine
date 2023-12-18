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
	for (const auto childGO : parent.get()->children)
	{
		uint treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (childGO.get()->children.size() == 0)
			treeFlags |= ImGuiTreeNodeFlags_Leaf;

		if (childGO == app->sceneManager->GetSelectedGO())
			treeFlags |= ImGuiTreeNodeFlags_Selected;

		bool isOpen = ImGui::TreeNodeEx(childGO.get()->GetName().data(), treeFlags);

		Reparent(childGO);

		if (ImGui::IsItemClicked(0) && !ImGui::IsItemToggledOpen())
		{
			app->sceneManager->SetSelectedGO(childGO);
			LOG(LogType::LOG_INFO, "SelectedGO: %s", app->sceneManager->GetSelectedGO().get()->GetName().c_str());
		}

		ContextMenu(childGO);

		if (isOpen)
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

void PanelHierarchy::Reparent(std::shared_ptr<GameObject> childGO)
{

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload(app->sceneManager->GetSelectedGO().get()->GetName().c_str(), &childGO, sizeof(GameObject));

		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(app->sceneManager->GetSelectedGO().get()->GetName().c_str()))
		{
			GameObject* dragging = *(GameObject**)payload->Data;
			GameObject* oldParent = dragging->parent.lock().get();
			dragging->parent = childGO.get()->weak_from_this();

			std::shared_ptr<GameObject> newChild = dragging->weak_from_this().lock();

			std::vector<std::shared_ptr<GameObject>>::iterator position = std::find(oldParent->children.begin(), oldParent->children.end(), newChild);
			oldParent->children.erase(position);

			childGO.get()->children.push_back(std::move(newChild));
		}
		ImGui::EndDragDropTarget();
	}

}
