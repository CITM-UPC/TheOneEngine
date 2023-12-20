#include "PanelScene.h"
#include "App.h"
#include "Gui.h"
#include "Renderer3D.h"
#include "Window.h"
#include "imgui.h"

#include "..\TheOneEngine\EngineCore.h"

PanelScene::PanelScene(PanelType type, std::string name) : Panel(type, name) {}

PanelScene::~PanelScene() {}

bool PanelScene::Draw()
{
    // Flag to track whether Button 2 was clicked
    static bool showCameraSettings = false;

	ImGuiWindowFlags settingsFlags = 0;
    settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

    //ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

    ImGui::SetNextWindowBgAlpha(.0f);
	if (ImGui::Begin("Scene", &enabled, settingsFlags))
	{
        // Top Bar --------------------------
        if (ImGui::BeginMenuBar())
        {
            //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginMenu("Camera"))
            {
                // Camera settings
                ImGui::TextWrapped("Scene Camera");
                
                Camera* camera = app->renderer3D->cameraGO.get()->GetComponent<Camera>();

                float fov = static_cast<float>(camera->fov);
                float aspect = static_cast<float>(camera->aspect);
                float zNear = static_cast<float>(camera->zNear);
                float zFar = static_cast<float>(camera->zFar);

                ImGui::SliderFloat("Field of View", &fov, 20.0, 120.0);
                ImGui::SliderFloat("Aspect Ratio", &aspect, 0.1, 10.0);
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
        CalculateSizeAspectRatio(regionSize.x, regionSize.y, width, height);

        // Viewport coordinates
        int x = static_cast<int>(windowPos.x);
        int y = SDLWindowHeight - windowPos.y - windowSize.y;

        app->engine->OnWindowResize(x, y, width, height);

        Camera* camera = app->renderer3D->cameraGO.get()->GetComponent<Camera>();
        app->engine->Render(EngineCore::RenderModes::DEBUG, camera);
	}

    ImGui::End();
    ImGui::PopStyleVar();

	return true;
}

void PanelScene::CalculateSizeAspectRatio(int maxWidth, int maxHeight, int& width, int& height)
{
    // Calculate the aspect ratio of the given rectangle
    double aspectRatio = static_cast<double>(maxWidth) / static_cast<double>(maxHeight);

    // Calculate the aspect ratio of a 16:9 rectangle
    double targetAspectRatio = 16.0 / 9.0;

    if (aspectRatio <= targetAspectRatio) {
        // The given rectangle is wider, so the width is limited
        width = maxWidth;
        height = static_cast<int>(std::round(width / targetAspectRatio));
    }
    else {
        // The given rectangle is taller, so the height is limited
        height = maxHeight;
        width = static_cast<int>(std::round(height * targetAspectRatio));
    }
}