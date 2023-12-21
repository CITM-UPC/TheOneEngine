#include "PanelGame.h"
#include "App.h"
#include "Gui.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Window.h"
#include "imgui.h"

#include "..\TheOneEngine\EngineCore.h"

PanelGame::PanelGame(PanelType type, std::string name) : Panel(type, name) {}

PanelGame::~PanelGame() {}

bool PanelGame::Draw()
{
    ImGuiWindowFlags settingsFlags = 0;
    settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

    //ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

    ImGui::SetNextWindowBgAlpha(.0f);
    if (ImGui::Begin("Game", &enabled, settingsFlags))
    {
        // Top Bar --------------------------
        if (ImGui::BeginMenuBar())
        {
            //if (ImGui::Combo("##Camera", &resolution, gameCameras, 8))

            //if (selectedItem != -1) {
            //    // Do something with the selected item
            //    ImGui::Text("Selected Object: %s", items[selectedItem]);
            //}

            if (ImGui::BeginMenu("Camera"))
            {

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Aspect"))
            {

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

        // Render Game cameras
        for (const auto GO : app->sceneManager->GetGameObjects())
        {
            Camera* gameCam = GO.get()->GetComponent<Camera>();

            if (gameCam == nullptr) continue;
            app->engine->Render(gameCam);
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();

    return true;
}