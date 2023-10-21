#pragma once
#include "Module.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "Defs.h"

class Gui : public Module
{
public:
	Gui(App* app);
	~Gui();

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void RenderGui();

	void HandleInput(SDL_Event* event);

private:

	/*Methods for windows*/

	/*Main Dockspace*/
	void GeneralWindowDockspace(); 
	void GeneralMenuFile();
	void GeneralMenuEdit();
	void GeneralMenuWindows();
	void GeneralMenuAbout();

	void GUIWindow1();
	void InspectorWindow();
	void ConsoleWindow();

private:

	/*Create bools for each window to show and the general that contains all of them*/
	bool show_gui = true;


	/*GuiWindow1 Vars*/
	bool show_guiwindow_1 = false;
	bool full_desktop = false;
	bool minimize_window = false;

	/*Inspector Window Vars*/
	bool show_inspector_window = false;
	
	/*Console Window Vars*/
	bool show_console_window = false;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};