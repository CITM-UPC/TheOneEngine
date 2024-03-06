#include "PanelGame.h"
#include "App.h"
#include "Gui.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Window.h"
#include "imgui.h"

#include "../TheOneEngine/EngineCore.h"

PanelGame::PanelGame(PanelType type, std::string name) : Panel(type, name)
{
	frameBuffer = std::make_shared<FrameBuffer>(1280, 720, true);
}

PanelGame::~PanelGame() {}

void PanelGame::Start()
{
	current = app->scenemanager->N_sceneManager->currentScene;
}

bool PanelGame::Draw()
{
	if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
		(frameBuffer->getWidth() != viewportSize.x || frameBuffer->getHeight() != viewportSize.y))
	{
		frameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		if(primaryCamera)
			primaryCamera->aspect = viewportSize.x / viewportSize.y;
	}


	// Render Game cameras
	for (const auto GO : app->scenemanager->N_sceneManager->GetGameObjects())
	{
		if (GO->HasCameraComponent()) { gameCameras.push_back(GO.get()); }

	}

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);

	if (ImGui::Begin("Game", &enabled, settingsFlags))
	{
		isHovered = ImGui::IsWindowHovered();
		isFocused = ImGui::IsWindowFocused();

		ImVec2 availWindowSize = ImGui::GetContentRegionAvail();

		// Top Bar --------------------------
		if (ImGui::BeginMenuBar())
		{
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

		for (const auto& cam : gameCameras)
		{
			Camera* gameCam = cam->GetComponent<Camera>();

            if (gameCam && !gameCam->primaryCam) continue;
			primaryCamera = gameCam;
        }

		//ALL DRAWING MUST HAPPEN BETWEEN FB BIND/UNBIND-------------------------------------------------
		{
			frameBuffer->Bind();


			frameBuffer->Clear();
			frameBuffer->ClearBuffer(-1);
			// Draw
			engine->Render(primaryCamera);

			// Game cameras Frustum
			for (const auto GO : app->scenemanager->N_sceneManager->GetGameObjects())
			{
				Camera* gameCam = GO.get()->GetComponent<Camera>();

				if (gameCam != nullptr && gameCam->drawFrustum)
					engine->DrawFrustum(gameCam->frustum);
			}
			current->Draw();


			frameBuffer->Unbind();
		}

		//Draw FrameBuffer Texture
		viewportSize = { availWindowSize.x, availWindowSize.y };
		ImGui::Image((ImTextureID)frameBuffer->getColorBufferTexture(), ImVec2{ viewportSize.x, viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

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