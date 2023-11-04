#include "PanelInspector.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"
#include "imgui_internal.h"


PanelInspector::PanelInspector(PanelType type) : Panel(type) {}

PanelInspector::~PanelInspector() {}

bool PanelInspector::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Inspector", &enabled, settingsFlags))
	{
        ImGuiIO& io = ImGui::GetIO();

        ImVec4 clear_color = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

        ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once); //Sets window size only once with ImGuiCond_Once, if fixed size erase it.
        ImGui::Begin("Inspector");

        /*Transform*/
        ImGui::SeparatorText("Transform");
        static char buf[5] = "0";
        ImGui::Text("Position");
        //ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
        ImGui::PushItemWidth(40.0f);
        ImGui::SameLine();
        ImGui::InputText("x", buf, IM_ARRAYSIZE(buf));
        ImGui::SameLine();
        ImGui::InputText("y", buf, IM_ARRAYSIZE(buf));
        ImGui::SameLine();
        ImGui::InputText("z", buf, IM_ARRAYSIZE(buf));

        ImGui::Text("Rotation");
        //ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
        ImGui::PushItemWidth(40.0f);
        ImGui::SameLine();
        ImGui::InputText("x", buf, IM_ARRAYSIZE(buf));
        ImGui::SameLine();
        ImGui::InputText("y", buf, IM_ARRAYSIZE(buf));
        ImGui::SameLine();
        ImGui::InputText("z", buf, IM_ARRAYSIZE(buf));

        ImGui::Text("Scale   ");
        //ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
        ImGui::PushItemWidth(40.0f);
        ImGui::SameLine();
        ImGui::InputText("x", buf, IM_ARRAYSIZE(buf));
        ImGui::SameLine();
        ImGui::InputText("y", buf, IM_ARRAYSIZE(buf));
        ImGui::SameLine();
        ImGui::InputText("z", buf, IM_ARRAYSIZE(buf));

        ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
        ImGui::Text("Mesh");
        static char mesh_name[32] = "house.fbx";
        ImGui::PushItemWidth(150.0f);
        ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
        ImGui::InputText("", mesh_name, IM_ARRAYSIZE(mesh_name));

        ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
        ImGui::Text("Texture");
        static char texture_name[32] = "texture.png";
        ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
        ImGui::InputText("", texture_name, IM_ARRAYSIZE(texture_name));

        ImGui::PopItemWidth();

        ImGui::End();
	}	

	return true;
}