#include "PanelGame.h"
#include "App.h"
#include "Gui.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Window.h"
#include "imgui.h"

#include "../TheOneEngine/EngineCore.h"

PanelGame::PanelGame(PanelType type, std::string name) : Panel(type, name),cameraToRender() {}

PanelGame::~PanelGame() {}

bool PanelGame::Start()
{
 	for (const auto GO : app->scenemanager->N_sceneManager->GetGameObjects())
	{
		if (GO->HasCameraComponent()) { gameCameras.push_back(GO.get()); cameraToRender = GO->GetComponent<Camera>(); }
	}
	return true;
}

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
		//Get selected GO
		selectedGO = app->scenemanager->N_sceneManager->GetSelectedGO().get();

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
				for (auto camerasGO : gameCameras)
				{
					if(ImGui::MenuItem(camerasGO->GetName().c_str()))
					{
						cameraToRender = camerasGO->GetComponent<Camera>();
					}
				}
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

        engine->OnWindowResize(x, y, width, height);

		// Render Game cameras
		/*for (const auto GO : app->scenemanager->N_sceneManager->GetGameObjects())
		{
			Camera* gameCam = GO.get()->GetComponent<Camera>();

            if (gameCam == nullptr) continue;
            engine->Render(gameCam);
        }*/
		engine->Render(cameraToRender);
    }

	ImGui::End();
	ImGui::PopStyleVar();

	return true;
}

bool PanelGame::AddCameraToRenderList(GameObject* cameraGO)
{
	gameCameras.push_back(cameraGO);
	return true;
}