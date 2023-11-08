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

	ImVec2 mainViewportPos = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(ImVec2(mainViewportPos.x, mainViewportPos.y), ImGuiCond_Appearing, ImVec2(0.5, 0.8));

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
	ImGui::Text("Display Mode");
	ImGui::SameLine();
	if (ImGui::Combo("##Display Mode", &displayMode, displayModes, 4))
		app->window->SetDisplayMode((DisplayMode)displayMode);

	// Resolution
	int resolution = (int)app->window->GetResolution();
	ImGui::Text("Resolution");
	ImGui::SameLine();
	if (ImGui::Combo("##Resolution", &resolution, resolutions, 8))
		app->window->SetResolution((Resolution)resolution);

	// Vsync
	bool vsync = app->engine->GetVSync();
	ImGui::Text("V-Sync");
	ImGui::SameLine();
	if (ImGui::Checkbox("##V-Sync", &vsync))
		app->engine->SetVSync(vsync);

	ImGui::SameLine();

	// Fps
	int maxFPS = (int)app->GetFrameRate();
	ImGui::Text("Maximum FPS");
	ImGui::SameLine();
	if (ImGui::Combo("##Maximum FPS", &maxFPS, fpsList, 8))
		app->SetFrameRate(atoi(fpsList[maxFPS]));
}

void PanelSettings::Input()
{
	ImVec4 grey = ImVec4(0.5, 0.5, 0.5, 1);

	// Mouse position
	int mouse_x, mouse_y;
	mouse_x = app->input->GetMouseX();
	mouse_y = app->input->GetMouseY();
	ImGui::Text("Mouse Position:");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%i,%i", mouse_x, mouse_y);

	// Mouse Speed
	mouse_x = app->input->GetMouseXMotion();
	mouse_y = app->input->GetMouseYMotion();
	ImGui::Text("Mouse Speed:");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%i,%i", mouse_x, mouse_y);

	// Mouse Wheel
	int wheel = app->input->GetMouseZ();
	ImGui::Text("Mouse Wheel:");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%i", wheel);
}

void PanelSettings::Renderer()
{

}

void PanelSettings::Hardware()
{
	HardwareInfo hardware_info = app->hardware->GetInfo();
	ImVec4 isSupported;
	ImVec4 grey = ImVec4(0.5, 0.5, 0.5, 1);
	ImVec4 green = ImVec4(0, 0.8, 0, 1);
	ImVec4 red = ImVec4(0.8, 0, 0, 1);


	// CPU 
	ImGui::Text("CPU");
	ImGui::Text(" Logic Cores:");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%i", hardware_info.cpu_count);

	ImGui::Text(" L1 Cache (Kb):");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%i", hardware_info.l1_cachekb);

	ImGui::Text(" Instruction Support:");
	ImGui::SameLine();
	isSupported = hardware_info.rdtsc == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "rdtsc");
	ImGui::SameLine();
	isSupported = hardware_info.altivec == true ? green : red;
    ImGui::TextColored(isSupported, "%s", "altivec");
	ImGui::SameLine();
	isSupported = hardware_info.now3d == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "now3d");
	ImGui::SameLine();
	isSupported = hardware_info.mmx == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "mmx");

	isSupported = hardware_info.sse == true ? green : red;
	ImGui::TextColored(isSupported, "\t\t\t\t\t  %s", "sse");
	ImGui::SameLine();
	isSupported = hardware_info.sse2 == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "sse2");
	ImGui::SameLine();
	isSupported = hardware_info.sse3 == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "sse3");
	ImGui::SameLine();
	isSupported = hardware_info.sse41 == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "sse41");
	ImGui::SameLine();
	isSupported = hardware_info.sse42 == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "sse42");

	isSupported = hardware_info.avx == true ? green : red;
	ImGui::TextColored(isSupported, "\t\t\t\t\t  %s", "avx");
	ImGui::SameLine();
	isSupported = hardware_info.avx2 == true ? green : red;
	ImGui::TextColored(isSupported, "%s", "avx2");


	// RAM 
	ImGui::Separator();
	ImGui::Text("RAM");
	ImGui::Text("  Memory");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%.1f GB", hardware_info.ram_gb);


	// GPU (Currently NVIDIA only)
	ImGui::Separator();
	ImGui::Text("GPU");
	ImGui::Text(" Vendor");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%s", hardware_info.gpu_vendor.data());

	ImGui::Text(" Model");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%s", hardware_info.gpu_brand.data());

	ImGui::Text(" Driver");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%s", hardware_info.gpu_driver.data());

	ImGui::Text(" VRAM Budget");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%.1f MB", hardware_info.vram_mb_budget);

	ImGui::Text(" VRAM Usage");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%.1f MB", hardware_info.vram_mb_usage);

	ImGui::Text(" VRAM Available");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%.1f MB", hardware_info.vram_mb_available);

	
}

void PanelSettings::Software()
{
	HardwareInfo hardware_info = app->hardware->GetInfo();
	ImVec4 grey = ImVec4(0.5, 0.5, 0.5, 1);

	// SDL Version
	ImGui::Text("SDL Version");
	ImGui::SameLine();
	ImGui::TextColored(grey, "%s", hardware_info.sdl_version);
	ImGui::Separator();
}
// --------------------------------------------------