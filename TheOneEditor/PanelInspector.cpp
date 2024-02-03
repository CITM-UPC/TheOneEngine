#include "PanelInspector.h"
#include "App.h"
#include "Gui.h"
#include "SceneManager.h"

#include "..\TheOneEngine\Transform.h"
#include "..\TheOneEngine\Mesh.h"
#include "..\TheOneEngine\Camera.h"

#include "imgui.h"
#include "imgui_internal.h"


PanelInspector::PanelInspector(PanelType type, std::string name) : Panel(type, name) 
{
    needRefresh_pos = false; 
    needRefresh_rot = false;
    needRefresh_sca = false;

    view_pos = { 0, 0, 0 };
    view_rot = { 0, 0, 0 };
    view_sca = { 0, 0, 0 };
}

PanelInspector::~PanelInspector() {}

bool PanelInspector::Draw()
{
	ImGuiWindowFlags settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Inspector", &enabled, settingsFlags))
	{
        ImGuiIO& io = ImGui::GetIO();

        ImVec4 clear_color = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

        ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once); //Sets window size only once with ImGuiCond_Once, if fixed size erase it.

        if (app->sceneManager->GetSelectedGO() != nullptr)
        {
            /*Name*/
            //ImGui::Checkbox("Active", &gameObjSelected->isActive);
            ImGui::SameLine(); ImGui::Text("GameObject:");
            ImGui::SameLine(); ImGui::TextColored({ 0.144f, 0.422f, 0.720f, 1.0f }, app->sceneManager->GetSelectedGO().get()->GetName().c_str());
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            /*Tag + Layer*/
            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Tag", "Untagged", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }

            ImGui::SameLine();

            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Layer", "Default", ImGuiComboFlags_HeightSmall)){ ImGui::EndCombo(); }
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            /*Transform Component*/
            Transform* transform = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>();

            if (transform != nullptr && ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SetItemTooltip("Displays and sets game object transformations");

                view_pos = transform->getPosition();
                view_rot = transform->getEulerAngles();
                view_sca = transform->getScale();

                ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;

                if (ImGui::BeginTable("", 4, tableFlags))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthStretch);

                    ImGui::TableNextRow();

                    // Headers
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TableHeader("");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TableHeader("X");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TableHeader("Y");
                    ImGui::TableSetColumnIndex(3);
                    ImGui::TableHeader("Z");

                    ImGui::TableNextRow();

                    // Position
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Position");

                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::DragFloat("##PosX", &view_pos.x, 0.5F, 0, 0, "%.3f", 1))
                    {
                        needRefresh_pos = true;
                    }

                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::DragFloat("##PosY", &view_pos.y, 0.5F, 0, 0, "%.3f", 1))
                    {
                        needRefresh_pos = true;
                    }

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::DragFloat("##PosZ", &view_pos.z, 0.5F, 0, 0, "%.3f", 1))
                    {
                        needRefresh_pos = true;
                    }

                    ImGui::TableNextRow();

                    // Rotation
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Rotation");

                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::DragFloat("##RotX", &view_rot.x, 0.5F, 0, 0, "%.3f", 1))
                    {
                        needRefresh_rot = true;
                    }

                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::DragFloat("##RotY", &view_rot.y, 0.5F, 0, 0, "%.3f", 1))
                    {
                        needRefresh_rot = true;
                    }

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::DragFloat("##RotZ", &view_rot.z, 0.5F, 0, 0, "%.3f", 1))
                    {
                        needRefresh_rot = true;
                    }

                    ImGui::TableNextRow();

                    // Scale
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Scale");

                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::DragFloat("##ScaleX", &view_sca.x, 0.5F, 0, 0, "%.3f", 1))
                    {
                        needRefresh_sca = true;
                    }

                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::DragFloat("##ScaleY", &view_sca.y, 0.5F, 0, 0, "%.3f", 1))
                    {
                        needRefresh_sca = true;
                    }

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::DragFloat("##ScaleZ", &view_sca.z, 0.5F, 0, 0, "%.3f", 1))
                    {
                        needRefresh_sca = true;
                    }

                    ImGui::EndTable();
                }

                if (needRefresh_pos){
                    transform->setPosition(view_pos);
                    transform->updateMatrix();
                }
                else if (needRefresh_rot) {
                    transform->setRotation((vec3)view_rot);
                    transform->updateMatrix();
                }
                else if (needRefresh_sca) {
                    transform->setScale(view_sca);
                    transform->updateMatrix();
                }

                needRefresh_pos = false;
                needRefresh_rot = false;
                needRefresh_sca = false;

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }

            //static char buf[5] = "0";
            //ImGui::Text("Position");
            ////ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
            //ImGui::PushItemWidth(40.0f);
            //ImGui::SameLine();
            //ImGui::InputText("x", buf, IM_ARRAYSIZE(buf));
            //ImGui::SameLine();
            //ImGui::InputText("y", buf, IM_ARRAYSIZE(buf));
            //ImGui::SameLine();
            //ImGui::InputText("z", buf, IM_ARRAYSIZE(buf));

            /*Mesh Component*/
            Mesh* mesh = app->sceneManager->GetSelectedGO().get()->GetComponent<Mesh>();

            if (mesh != nullptr && ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SetItemTooltip("Displays and sets mesh data");
                //ImGui::Checkbox("Active", &mesh->isActive);
                //ImGui::SameLine();  
                ImGui::Text("Name: ");
                ImGui::SameLine();  ImGui::TextColored({ 0.920f, 0.845f, 0.0184f, 1.0f }, mesh->mesh.meshName.c_str());
                ImGui::Separator();
                ImGui::Text("Indexes: ");
                ImGui::SameLine();  ImGui::Text((std::to_string(mesh->mesh.indexs_buffer_id)).c_str());
                ImGui::Text("Vertexs: ");
                ImGui::SameLine();  ImGui::Text(std::to_string(mesh->mesh.numVerts).c_str());
                ImGui::Text("Faces: ");
                ImGui::SameLine();  ImGui::Text(std::to_string(mesh->mesh.numFaces).c_str());

                ImGui::Checkbox("Active Mesh", &mesh->active);
                ImGui::Checkbox("Active vertex normals", &mesh->drawNormalsVerts);
                ImGui::Checkbox("Active face normals", &mesh->drawNormalsFaces);
                ImGui::Checkbox("Active Wireframe", &mesh->drawWireframe);
                ImGui::Checkbox("Active AABB", &mesh->drawAABB);
                ImGui::Checkbox("Active OBB", &mesh->drawOBB);
                //ImGui::Checkbox("Active checkboard", &mesh->drawChecker);

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }
            
            /*Material Component*/



            /*Texture Component*/
            Texture* texture = app->sceneManager->GetSelectedGO().get()->GetComponent<Texture>();

            if (texture != nullptr && ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SetItemTooltip("Displays and sets texture data");
                ImGui::Checkbox("Active Texture", &texture->active);
                ImGui::Text("Name: ");
                ImGui::SameLine();  ImGui::TextColored({ 0.920f, 0.845f, 0.0184f, 1.0f }, (texture->GetName()).c_str());
                ImGui::Separator();
                ImGui::Text("Size: ");
                ImGui::SameLine();  ImGui::Text(std::to_string(texture->width).c_str());
                ImGui::Text("Height: ");
                ImGui::SameLine();  ImGui::Text(std::to_string(texture->height).c_str());

                //ImGui::Text("Tex coords: ");
                //ImGui::SameLine();  ImGui::Text(std::to_string(mesh->mesh.getNumTexCoords()).c_str());

                //if (ImGui::Checkbox("Use Texture", /*&mesh->usingTexture*/true))
                //{
                //    //(mesh->usingTexture) ? mesh->texture = gameObjSelected->GetComponent<Texture2D>() : mesh->texture = nullptr;
                //}

                //ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dpx x %dpx", s->getTexture()->width, s->getTexture()->height);

                // JULS: To show the image of the texture, but need to look at it more.
                //ImTextureID my_tex_id;
                //glGenTextures(GL_TEXTURE_2D, 1, my_tex_id);
                //glTextureParameteri(my_tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                //glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                //glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                //glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, [width of your texture], [height of your texture], false, GL_RGBA, GL_FLOAT, [pointer to first element in array of texture pixel values]);
                //ImGui::Image()

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }

            /*Camera Component*/
            Camera* camera = app->sceneManager->GetSelectedGO().get()->GetComponent<Camera>();

            if (camera != nullptr && ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                float fov = static_cast<float>(camera->fov);
                float aspect = static_cast<float>(camera->aspect);
                float zNear = static_cast<float>(camera->zNear);
                float zFar = static_cast<float>(camera->zFar);

                ImGui::SliderFloat("FOV", &fov, 20.0, 120.0);
                ImGui::SliderFloat("Aspect", &aspect, 0.1, 10.0);
                ImGui::Text("Clipping Plane");
                ImGui::SliderFloat("Near", &zNear, 0.01, 10.0);
                ImGui::SliderFloat("Far ", &zFar, 1.0, 1500.0);

                camera->fov = fov;
                camera->aspect = aspect;
                camera->zNear = zNear;
                camera->zFar = zFar;

                ImGui::Checkbox("Draw Frustrum", &camera->drawFrustum);

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }
        }

        ImGui::End();
	}	

	return true;
}