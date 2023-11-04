#include "PanelScene.h"
#include "App.h"
#include "Gui.h"
#include "imgui.h"

PanelScene::PanelScene(PanelType type) : Panel(type) {}

PanelScene::~PanelScene() {}

bool PanelScene::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Scene", &enabled, settingsFlags))
	{
        ImGuiIO& io = ImGui::GetIO();

        ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once); //Sets window size only once with ImGuiCond_Once, if fixed size erase it.

        ImGui::Begin("Scene");
        //{
        //    // Using a Child allow to fill all the space of the window.
        //    // It also alows customization
        //    ImGui::BeginChild("GameRender");
        //    // Get the size of the child (i.e. the whole draw size of the windows).
        //    ImVec2 wsize = ImGui::GetWindowSize();
        //    // Because I use the texture from OpenGL, I need to invert the V from the UV.
        //    ImGui::Image((ImTextureID)tex, wsize, ImVec2(0, 1), ImVec2(1, 0));
        //    ImGui::EndChild();
        //}
        //ImGui::End();

        ImGui::End();
	}

	return true;
}