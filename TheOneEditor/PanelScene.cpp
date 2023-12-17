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
	ImGuiWindowFlags settingsFlags = 0;
    settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    //ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

    ImGui::SetNextWindowBgAlpha(.0f);
	if (ImGui::Begin("Scene", &enabled, settingsFlags))
	{
        // SDL Window
        int SDLWindowWidth, SDLWindowHeight;
        app->window->GetSDLWindowSize(&SDLWindowWidth, &SDLWindowHeight);

        // ImGui Window
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
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