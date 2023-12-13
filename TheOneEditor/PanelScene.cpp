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

    ImGui::SetNextWindowBgAlpha(.0f);
	if (ImGui::Begin("Scene", &enabled, settingsFlags))
	{
        ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once); //Sets window size only once with ImGuiCond_Once, if fixed size erase it.

        int width, height;
        CalculateMaxSize_16x9(ImGui::GetWindowWidth(), ImGui::GetWindowHeight(), width, height);


        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        
        int x = static_cast<int>(windowPos.x);
        int y = 720 - static_cast<int>(windowPos.y + windowSize.y);


        //glViewport(x, y, width, height);

        app->engine->OnWindowResize(x, y, width, height);


        Camera* camera = app->renderer3D->cameraGO.get()->GetComponent<Camera>();
        app->engine->Render(EngineCore::RenderModes::DEBUG, camera);

        ImGui::End();
	}

	return true;
}

void PanelScene::CalculateMaxSize_16x9(int maxWidth, int maxHeight, int& width, int& height)
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