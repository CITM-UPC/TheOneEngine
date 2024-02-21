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
    matrixDirty = false;

    view_pos = { 0, 0, 0 };
    view_rot_rad = { 0, 0, 0 };
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
        ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once);

        std::shared_ptr<GameObject> selectedGO = app->sceneManager->GetSelectedGO();

        if (selectedGO != nullptr)
        {
            /*Name*/
            //ImGui::Checkbox("Active", &gameObjSelected->isActive);
            ImGui::SameLine(); ImGui::Text("GameObject:");
            ImGui::SameLine(); ImGui::TextColored({ 0.144f, 0.422f, 0.720f, 1.0f }, selectedGO.get()->GetName().c_str());
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            /*Tag + Layer*/
            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Tag", "Untagged", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }

            ImGui::SameLine();

            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Layer", "Default", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            /*Transform Component*/
            Transform* transform = selectedGO.get()->GetComponent<Transform>();

            if (transform != nullptr && ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SetItemTooltip("Displays and sets game object transformations");

                matrixDirty = false;

                view_pos = transform->getPosition();
                view_rot_rad = transform->getEulerAngles();
                view_sca = transform->getScale();

                //[-pi, pi]
                if (view_rot_rad.x >= 0)
                    view_rot_deg.x = view_rot_rad.x * RADTODEG;
                else
                    view_rot_deg.x = 360 + view_rot_rad.x * RADTODEG;

                //[-pi/2, pi/2]
                if (view_rot_rad.y >= 0)
                    view_rot_deg.y = view_rot_rad.y * RADTODEG;
                else
                    view_rot_deg.y = 360 + view_rot_rad.y * RADTODEG;

                //[-pi, pi]
                if (view_rot_rad.z >= 0)
                    view_rot_deg.z = view_rot_rad.z * RADTODEG;
                else
                    view_rot_deg.z = 360 + view_rot_rad.z * RADTODEG;

                
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
                        transform->setPosition(view_pos);
                        matrixDirty = true;
                    }

                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::DragFloat("##PosY", &view_pos.y, 0.5F, 0, 0, "%.3f", 1))
                    {
                        transform->setPosition(view_pos);
                        matrixDirty = true;
                    }

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::DragFloat("##PosZ", &view_pos.z, 0.5F, 0, 0, "%.3f", 1))
                    {
                        transform->setPosition(view_pos);
                        matrixDirty = true;
                    }

                    ImGui::TableNextRow();

                    // Rotation
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Rotation");

                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::DragFloat("##RotX", &view_rot_deg.x, 1.0f, 0, 0, "%.3f", 1))
                    {
                        view_rot_rad.x = view_rot_deg.x * DEGTORAD;
                        transform->setRotation(view_rot_rad);
                        matrixDirty = true;
                    }

                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::DragFloat("##RotY", &view_rot_deg.y, 1.0f, 0, 0, "%.3f", 1))
                    {
                        view_rot_rad.y = view_rot_deg.y * DEGTORAD;
                        transform->setRotation(view_rot_rad);
                        matrixDirty = true;
                    }

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::DragFloat("##RotZ", &view_rot_deg.z, 1.0f, 0, 0, "%.3f", 1))
                    {
                        view_rot_rad.z = view_rot_deg.z * DEGTORAD;
                        transform->setRotation(view_rot_rad);
                        matrixDirty = true;
                    }

                    ImGui::TableNextRow();

                    // Scale
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Scale");

                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::DragFloat("##ScaleX", &view_sca.x, 0.1F, 0, 0, "%.3f", 1))
                    {
                        transform->setScale(view_sca);
                        matrixDirty = true;
                    }

                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::DragFloat("##ScaleY", &view_sca.y, 0.1F, 0, 0, "%.3f", 1))
                    {
                        transform->setScale(view_sca);
                        matrixDirty = true;
                    }

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::DragFloat("##ScaleZ", &view_sca.z, 0.1F, 0, 0, "%.3f", 1))
                    {
                        transform->setScale(view_sca);
                        matrixDirty = true;
                    }

                    ImGui::EndTable();
                }

                if (matrixDirty)
                    transform->updateMatrix();


                //Check if camera needs to be updated
                Camera* camera = selectedGO.get()->GetComponent<Camera>();
                if (camera && (matrixDirty))
                    camera->UpdateCamera();
                

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }


            /*Mesh Component*/
            Mesh* mesh = selectedGO.get()->GetComponent<Mesh>();

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

                ImGui::Checkbox("Mesh", &mesh->active);
                ImGui::Checkbox("Vertex normals", &mesh->drawNormalsVerts);
                ImGui::Checkbox("Face normals", &mesh->drawNormalsFaces);
                ImGui::Checkbox("Wireframe", &mesh->drawWireframe);
                ImGui::Checkbox("AABB", &mesh->drawAABB);
                ImGui::Checkbox("OBB", &mesh->drawOBB);
                //ImGui::Checkbox("Active checkboard", &mesh->drawChecker);

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }
            

            /*Material Component*/
            //hekbas todo


            /*Texture Component*/
            Texture* texture = selectedGO.get()->GetComponent<Texture>();

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
            Camera* camera = selectedGO.get()->GetComponent<Camera>();

            if (camera != nullptr && ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                bool isDirty = false;

                float fov = static_cast<float>(camera->fov);
                float aspect = static_cast<float>(camera->aspect);
                float zNear = static_cast<float>(camera->zNear);
                float zFar = static_cast<float>(camera->zFar);

                if (ImGui::SliderFloat("FOV", &fov, 20.0, 120.0))
                {
                    camera->fov = fov;
                    isDirty = true;
                }

                if (ImGui::SliderFloat("Aspect", &aspect, 0.1, 10.0))
                {
                    camera->aspect = aspect;
                    isDirty = true;
                }

                ImGui::Text("Clipping Planes");
                if (ImGui::SliderFloat("Near", &zNear, 0.01, 10.0))
                {
                    camera->zNear = zNear;
                    isDirty = true;
                }
                
                if (ImGui::SliderFloat("Far ", &zFar, 1.0, 10000.0))
                {
                    camera->zFar = zFar;
                    isDirty = true;
                }
                
                if (isDirty) camera->UpdateCamera();


                ImGui::Checkbox("Draw Frustrum", &camera->drawFrustum);

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }
        }

        ImGui::End();
	}	

    ImGui::PopStyleVar();

	return true;
}