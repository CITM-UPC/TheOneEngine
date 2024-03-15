#include "PanelProject.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"
#include "SceneManager.h"
#include "..\TheOneEngine\EngineCore.h"

#include <filesystem>
#include <string>

#define BIT(x) (1 << x)

namespace fs = std::filesystem;

PanelProject::PanelProject(PanelType type, std::string name) : Panel(type, name)
{
	directoryPath = ASSETS_PATH;
}

PanelProject::~PanelProject() {}


bool PanelProject::Draw()
{
	std::vector<FileInfo> files = ListFiles(directoryPath);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

	if (ImGui::Begin("Project"))
	{
		int width = ImGui::GetContentRegionAvail().x;

		static ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable;
		/*ImGui::CheckboxFlags("ImGuiTableFlags_NoKeepColumnsVisible", &tableFlags, ImGuiTableFlags_NoKeepColumnsVisible);
		ImGui::CheckboxFlags("ImGuiTableFlags_BordersInnerV", &tableFlags, ImGuiTableFlags_BordersInnerV);
		ImGui::CheckboxFlags("ImGuiTableFlags_BordersOuterV", &tableFlags, ImGuiTableFlags_BordersOuterV);*/

		if (ImGui::BeginTable("table", 2, tableFlags))
		{
			// We could also set ImGuiTableFlags_SizingFixedFit on the table and all columns will default to ImGuiTableColumnFlags_WidthFixed.
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			//ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.0f);

			// Directory Tree View ----------------------------
			ImGui::TableNextColumn();
			if (ImGui::CollapsingHeader("Assets", ImGuiTreeNodeFlags_DefaultOpen))
			{
				uint32_t count = 0;
				for (const auto& entry : fs::recursive_directory_iterator(directoryPath))
					count++;

				static int selection_mask = 0;

				auto clickState = DirectoryTreeViewRecursive(directoryPath, &count, &selection_mask);

				if (clickState.first)
				{
					// Update selection state
					// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
					if (ImGui::GetIO().KeyCtrl)
						selection_mask ^= BIT(clickState.second);		// CTRL+click to toggle
					else //if (!(selection_mask & (1 << clickState.second))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
						selection_mask = BIT(clickState.second);		// Click to single-select
				}
			}
			
			// Inspector ----------------------------
			ImGui::TableNextColumn();

			ImGui::Text("%s", directoryPath.c_str());

			ImGui::Separator();

			for (const auto& file : files) {
				// Load icon texture
				GLuint iconTexture = 0; // You need to load the icon texture using OpenGL

				// Display icon and filename
				ImGui::Image((void*)(intptr_t)iconTexture, ImVec2(24, 24));
				//Add Here to center image with text
				ImGui::Text("%s", file.name.c_str());
			}


			ImGui::EndTable();
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}

	return true;
}

std::pair<bool, uint32_t> PanelProject::DirectoryTreeViewRecursive(const fs::path& path, uint32_t* count, int* selection_mask)
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

	bool any_node_clicked = false;
	uint32_t node_clicked = 0;

	for (const auto& entry : fs::directory_iterator(path))
	{
		ImGuiTreeNodeFlags node_flags = base_flags;
		const bool is_selected = (*selection_mask & BIT(*count)) != 0;
		if (is_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		std::string name = entry.path().string();

		auto lastSlash = name.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		name = name.substr(lastSlash, name.size() - lastSlash);

		bool entryIsFile = !fs::is_directory(entry.path());
		if (entryIsFile)
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)(*count), node_flags, name.c_str());

		if (ImGui::IsItemClicked())
		{
			node_clicked = *count;
			any_node_clicked = true;
		}

		(*count)--;

		if (!entryIsFile)
		{
			if (node_open)
			{
				auto clickState = DirectoryTreeViewRecursive(entry.path(), count, selection_mask);

				if (!any_node_clicked)
				{
					any_node_clicked = clickState.first;
					node_clicked = clickState.second;
				}

				ImGui::TreePop();
			}
			else
			{
				for (const auto& e : fs::recursive_directory_iterator(entry.path()))
					(*count)--;
			}
		}
	}

	return { any_node_clicked, node_clicked };
}

bool PanelProject::DragAndDrop()
{
	//if (ImGui::BeginDragDropSource())
	//{
	//	if (childGO != engine->N_sceneManager->currentScene->GetRootSceneGO())
	//	{
	//		ImGui::SetDragDropPayload(fileSelected, &childGO, sizeof(GameObject));
	//	}

	//	ImGui::EndDragDropSource();
	//}
	//if (ImGui::BeginDragDropTarget())
	//{
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(engine->N_sceneManager->GetSelectedGO().get()->GetName().c_str()))
	//	{
	//		GameObject* dragging = *(GameObject**)payload->Data;

	//		if (!dragging->IsPrefab())
	//			dragging->Create / SetPrefab();

	//		//CreatePrefab(dragging);
	//	}
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(fileSelected)
	//	{
	//		//Create a FileNode class

	//		GameObject* dragging = *(GameObject**)payload->Data;

	//		//CreatePrefab(dragging);
	//	}
	//	ImGui::EndDragDropTarget();
	//}

	return false;
}

std::vector<FileInfo> PanelProject::ListFiles(const std::string& path)
{
	std::vector<FileInfo> files;
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_regular_file()) {
			FileInfo fileInfo;
			fileInfo.name = entry.path().filename().string();
			//fileInfo.fileType = GetIconPath(entry.path().extension().string()); //Change to GetType by extension
			files.push_back(fileInfo);
		}
	}
	return files;
}