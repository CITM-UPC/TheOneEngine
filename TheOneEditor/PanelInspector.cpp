#include "PanelInspector.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "SceneManager.h"


PanelInspector::PanelInspector(PanelType type, std::string name) : Panel(type, name) {}

PanelInspector::~PanelInspector() {}

bool PanelInspector::Draw()
{
	ImGuiWindowFlags settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Inspector", &enabled, settingsFlags))
	{
        ImGuiIO& io = ImGui::GetIO();

        ImVec4 clear_color = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

        ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once); //Sets window size only once with ImGuiCond_Once, if fixed size erase it.
        ImGui::Begin("Inspector");

        if (app->sceneManager->GetSelectedGO() != nullptr)
        {
            std::shared_ptr<Transform> transform = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>();
            std::shared_ptr<Mesh> mesh = app->sceneManager->GetSelectedGO().get()->GetComponent<Mesh>();

            ImGui::Checkbox("Enable", &app->sceneManager->GetSelectedGO().get()->enabled);
            ImGui::SameLine(); 
            ImGui::InputText(" ", &app->sceneManager->GetSelectedGO().get()->name);

            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Tag", "Untagged", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }

            ImGui::SameLine();

            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Layer", "Default", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }

            /*Transform Component*/
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SetItemTooltip("Displays and sets game object transformations");
                ImGui::Checkbox("Active", &transform->enabled);
                if (ImGui::BeginTable("", 4))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    
                    ImGui::Text("Position");
                    ImGui::Text("Rotation");
                    ImGui::Text("Scale");

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputFloat("x", &transform.get()->position.x);
                    ImGui::InputFloat("x", &transform.get()->localEulerAngles.x);
                    ImGui::InputFloat("x", &transform.get()->localScale.x);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::InputFloat("y", &transform.get()->position.y);
                    ImGui::InputFloat("y", &transform.get()->localEulerAngles.y);
                    ImGui::InputFloat("y", &transform.get()->localScale.y);

                    ImGui::TableSetColumnIndex(3);
                    ImGui::InputFloat("z", &transform.get()->position.z);
                    ImGui::InputFloat("z", &transform.get()->localEulerAngles.z);
                    ImGui::InputFloat("z", &transform.get()->localScale.z);

                    ImGui::EndTable();
                }

            }
            
            /*Mesh Component*/
            if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SetItemTooltip("Displays and sets mesh data");
                ImGui::Checkbox("Active", &mesh->enabled);
                //ImGui::SameLine();  
                ImGui::Text("Name: ");
                ImGui::SameLine();  ImGui::TextColored({ 0.920f, 0.845f, 0.0184f, 1.0f }, app->sceneManager->GetSelectedGO().get()->GetComponent<Mesh>().get()->GetName().c_str());
                ImGui::Separator();
                ImGui::Text("Indexes: ");
                ImGui::SameLine();  ImGui::Text(/*app->sceneManager->GetSelectedGO().get()->GetComponent<Mesh>().get()*/"0");
                ImGui::Text("Normals: ");
                ImGui::SameLine();  ImGui::Text(/*std::to_string(mesh->getNumNormals()).c_str()*/"244");
                ImGui::Text("Vertexs: ");
                ImGui::SameLine();  ImGui::Text(/*std::to_string(mesh->getNumVerts()).c_str()*/"244");
                ImGui::Text("Faces: ");
                ImGui::SameLine();  ImGui::Text(/*std::to_string(mesh->getNumFaces()).c_str()*/"244");
                ImGui::Text("Tex coords: ");
                ImGui::SameLine();  ImGui::Text(/*std::to_string(mesh->getNumTexCoords()).c_str()*/"244");
                ImGui::Separator();
                //if (ImGui::Checkbox("Use Texture", /*&mesh->usingTexture*/true))
                //{
                //    //(mesh->usingTexture) ? mesh->texture = gameObjSelected->GetComponent<Texture2D>() : mesh->texture = nullptr;
                //}
                //ImGui::Checkbox("Draw vertex normals", /*&mesh->drawVertexNormals*/);
                //ImGui::Checkbox("Draw face normals", /*&mesh->drawFaceNormals*/);

            }
            
            /*Texture Component*/
            if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SetItemTooltip("Displays and sets texture data");  
                ImGui::Text("Name: ");
                ImGui::SameLine();  ImGui::TextColored({ 0.920f, 0.845f, 0.0184f, 1.0f }, /*app->sceneManager->GetSelectedGO().get()->GetComponent<Mesh>().get()->GetName().c_str()*/"Texture");
                ImGui::Separator();
                ImGui::Text("Width: ");
                ImGui::SameLine();  ImGui::Text(/*app->sceneManager->GetSelectedGO().get()->GetComponent<Mesh>().get()*/"1024");
                ImGui::Text("Height: ");
                ImGui::SameLine();  ImGui::Text(/*std::to_string(mesh->getNumNormals()).c_str()*/"1024");
                ImGui::Separator();
            }

            /*ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
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

            ImGui::PopItemWidth();*/

        }

        ImGui::End();
	}	

    ImGui::PopStyleVar();

	return true;
}