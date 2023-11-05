#include "PanelSettings.h"
#include "App.h"
#include "Gui.h"
#include "Window.h"

#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"


PanelSettings::PanelSettings(PanelType type, std::string name) : Panel(type, name), fpsHistory(MAX_HISTORY_SIZE), delayHistory(MAX_HISTORY_SIZE) {}

PanelSettings::~PanelSettings() {}


void PanelSettings::AddFpsValue(int fps)
{
	fpsHistory.push_back(fps);

	if (fpsHistory.size() > MAX_HISTORY_SIZE)
	{
		fpsHistory.erase(fpsHistory.begin());
	}
}


bool PanelSettings::Draw()
{
	ImGuiWindowFlags settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	if (ImGui::Begin("Settings", &enabled, settingsFlags))
	{
		// LEFT - Buttons
		ImVec2 leftChildSize = ImVec2(ImGui::GetWindowSize().x * 0.2, ImGui::GetWindowSize().y);
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowSize().x * 0.2, 20);
	
		if (ImGui::BeginChild("", leftChildSize, true))
		{
			if (ImGui::Button("Performance", buttonSize))
				selected = SelectedSetting::PERFORMANCE;

			if (ImGui::Button("Window", buttonSize))
				selected = SelectedSetting::WINDOW;

			if (ImGui::Button("Input", buttonSize))
				selected = SelectedSetting::INPUT;

			if (ImGui::Button("Renderer", buttonSize))
				selected = SelectedSetting::RENDERER;

			ImGui::EndChild();
		}

		ImGui::SameLine();

		// RIGHT - Display selected
		settingsFlags &= ~(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		if (ImGui::BeginChild("Selected", ImVec2(ImGui::GetWindowSize().x * 0.8f, ImGui::GetWindowSize().y), false, settingsFlags))
		{
			switch (selected)
			{
				case SelectedSetting::PERFORMANCE:	Performance();	break;
				case SelectedSetting::WINDOW:		Window();		break;
				case SelectedSetting::INPUT:		Input();		break;
				case SelectedSetting::RENDERER:		Renderer();		break;
				default: Performance(); break;
			}

			ImGui::EndChild();
		}

		ImGui::End();
	}

	ImGui::PopStyleVar();

	return true;
}



// Settings -----------------------------------------
void PanelSettings::Performance()
{
	// FPS cap
	int frameRate = app->GetFrameRate();
	ImGui::Text("Frame Rate");
	ImGui::SameLine();
	if (ImGui::SliderInt("", &frameRate, 0, app->window->GetDisplayRefreshRate()))
		app->SetFrameRate(frameRate);

	ImGui::Separator();

	ImPlotFlags plotFlags = ImPlotFlags_NoInputs | ImPlotFlags_NoFrame | ImPlotFlags_NoLegend;
	ImPlotAxisFlags axisFlags = ImPlotAxisFlags_NoTickMarks;
	app->gui->PlotChart("FPS", fpsHistory, plotFlags, axisFlags);
}

void PanelSettings::Window()
{
	int displayMode = (int)app->window->GetDisplayMode();
	
	if (ImGui::Combo("Display Mode", &displayMode, displayModes, 4))
		app->window->SetDisplayMode((DisplayMode)displayMode);

	int resolution = (int)app->window->GetResolution();

	if (ImGui::Combo("Resolution", &resolution, resolutions, 8))
		app->window->SetResolution((Resolution)resolution);
}

void PanelSettings::Input()
{
	ImGui::Text("Input");
}

void PanelSettings::Renderer()
{
	ImGui::Text("Renderer");
}
// --------------------------------------------------