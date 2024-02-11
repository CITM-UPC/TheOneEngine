#include "PanelScene.h"
#include "App.h"
#include "Gui.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Window.h"
#include "imgui.h"

#include "..\TheOneEngine\EngineCore.h"

PanelScene::PanelScene(PanelType type, std::string name) : Panel(type, name), isHovered(false) {}

PanelScene::~PanelScene() {}

bool PanelScene::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
    settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

    //ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

    ImGui::SetNextWindowBgAlpha(.0f);
	if (ImGui::Begin("Scene", &enabled, settingsFlags))
	{
        if (ImGui::IsWindowHovered())
            isHovered = true;

        // Top Bar --------------------------
        if (ImGui::BeginMenuBar())
        {
            //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginMenu("Camera"))
            {
                // Camera settings
                ImGui::TextWrapped("Scene Camera");
                
                Camera* camera = app->renderer3D->sceneCamera.get()->GetComponent<Camera>();

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

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Gizmo"))
            {
                ImGui::Text("Gizmo Options");
                ImGui::Text("This section isn't finished, my precious...");
                ImGui::EndMenu();
            }

            ImGui::PopStyleVar();
            ImGui::EndMenuBar();
        }


        // Viewport Control --------------------------
        // SDL Window
        int SDLWindowWidth, SDLWindowHeight;
        app->window->GetSDLWindowSize(&SDLWindowWidth, &SDLWindowHeight);
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        // ImGui Window
        ImVec2 regionSize = ImGui::GetContentRegionAvail();

        // Aspect Ratio Size
        int width, height;
        app->gui->CalculateSizeAspectRatio(regionSize.x, regionSize.y, width, height);

        // Viewport coordinates
        int x = static_cast<int>(windowPos.x);
        int y = SDLWindowHeight - windowPos.y - windowSize.y;

        app->engine->OnWindowResize(x, y, width, height);

        // Render Scene Camera 
        Camera* sceneCam = app->renderer3D->sceneCamera.get()->GetComponent<Camera>();
        app->engine->Render(sceneCam);

        // Game cameras Frustum
        for (const auto GO : app->sceneManager->GetGameObjects())
        {
            Camera* gameCam = GO.get()->GetComponent<Camera>();

            if (gameCam != nullptr && gameCam->drawFrustum)
                app->engine->DrawFrustum(gameCam->frustum);
        }
	}

    ImGui::End();
    ImGui::PopStyleVar();

	return true;
}