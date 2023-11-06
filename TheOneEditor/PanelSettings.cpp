#include "PanelSettings.h"
#include "App.h"
#include "Gui.h"
#include "Window.h"
#include "Input.h"
#include "Hardware.h"

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

			if (ImGui::Button("Hardware", buttonSize))
				selected = SelectedSetting::HARDWARE;

			if (ImGui::Button("Software", buttonSize))
				selected = SelectedSetting::SOFTWARE;

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
				case SelectedSetting::HARDWARE:		Hardware();		break;
				case SelectedSetting::SOFTWARE:		Software();		break;
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
	// Display Mode
	int displayMode = (int)app->window->GetDisplayMode();
	
	if (ImGui::Combo("Display Mode", &displayMode, displayModes, 4))
		app->window->SetDisplayMode((DisplayMode)displayMode);

	// Resolution
	int resolution = (int)app->window->GetResolution();

	if (ImGui::Combo("Resolution", &resolution, resolutions, 8))
		app->window->SetResolution((Resolution)resolution);

	// vsync


	// fps
	int maxFPS = (int)app->GetFrameRate();

	if (ImGui::Combo("Maximum FPS", &maxFPS, fpsList, 8))
		app->SetFrameRate(atoi(fpsList[maxFPS]));
}

void PanelSettings::Input()
{
	// Mouse position
	int mouse_x, mouse_y;
	mouse_x = app->input->GetMouseX();
	mouse_y = app->input->GetMouseY();
	ImGui::Text("Mouse Position:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(169, 169, 169, 255), "%i,%i", mouse_x, mouse_y);

	// Mouse Speed
	mouse_x = app->input->GetMouseXMotion();
	mouse_y = app->input->GetMouseYMotion();
	ImGui::Text("Mouse Speed:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(169, 169, 169, 255), "%i,%i", mouse_x, mouse_y);

	// Mouse Wheel
	int wheel = app->input->GetMouseZ();
	ImGui::Text("Mouse Wheel:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(169, 169, 169, 255), "%i", wheel);
}

void PanelSettings::Renderer()
{

}

void PanelSettings::Hardware()
{
	HardwareInfo hardware_info = app->hardware->GetInfo();

	// CPU 
	ImGui::Text("CPU Logic Cores:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", hardware_info.cpu_count);

	ImGui::Text("CPU L1 Cache (Kb):");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 0, 0, 255), "%i", hardware_info.l1_cachekb);

	ImGui::Text("CPU Instruction Support:");
	ImGui::SameLine();

	ImVec4 color = ImVec4(128, 128, 128, 255);

	if (hardware_info.rdtsc) ImGui::TextColored(color, "%s", "rdtsc");
	ImGui::SameLine();
	if (hardware_info.altivec) ImGui::TextColored(color, "%s", "altivec");
	ImGui::SameLine();
	if (hardware_info.now3d) ImGui::TextColored(color, "%s", "now3d");
	ImGui::SameLine();
	if (hardware_info.mmx) ImGui::TextColored(color, "%s", "mmx");
	ImGui::SameLine();
	if (hardware_info.sse) ImGui::TextColored(color, "%s", "sse");
	ImGui::SameLine();
	if (hardware_info.sse2) ImGui::TextColored(color, "%s", "sse2");
	ImGui::SameLine();
	if (hardware_info.sse3) ImGui::TextColored(color, "%s", "sse3");
	ImGui::SameLine();
	if (hardware_info.sse41) ImGui::TextColored(color, "%s", "sse41");
	ImGui::SameLine();
	if (hardware_info.sse42) ImGui::TextColored(color, "%s", "sse42");
	ImGui::SameLine();
	if (hardware_info.avx) ImGui::TextColored(color, "%s", "avx");
	ImGui::SameLine();
	if (hardware_info.avx2)ImGui::TextColored(color, "%s", "avx2");

	// RAM 
	ImGui::Separator();
	ImGui::Text("RAM Memory (Gb)");
	ImGui::SameLine();
	ImGui::TextColored(color, "%f", hardware_info.ram_gb);

	// GPU (Currently NVIDIA only)
	ImGui::Separator();
	ImGui::Text("GPU Vendor");
	ImGui::SameLine();
	ImGui::TextColored(color, "%s", hardware_info.gpu_vendor.data());

	ImGui::Text("GPU Model");
	ImGui::SameLine();
	ImGui::TextColored(color, "%s", hardware_info.gpu_brand.data());

	ImGui::Text("GPU Driver");
	ImGui::SameLine();
	ImGui::TextColored(color, "%s", hardware_info.gpu_driver.data());

	ImGui::Text("VRAM Budget");
	ImGui::SameLine();
	ImGui::TextColored(color, "%f", hardware_info.vram_mb_budget);

	ImGui::Text("VRAM Available");
	ImGui::SameLine();
	ImGui::TextColored(color, "%f", hardware_info.vram_mb_available);

	ImGui::Text("VRAM Usage");
	ImGui::SameLine();
	ImGui::TextColored(color, "%f", hardware_info.vram_mb_usage);
}

void PanelSettings::Software()
{
	// SDL Version
	/*ImGui::Text("SDL Version:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.sdl_version);
	ImGui::Separator();*/
}
// --------------------------------------------------