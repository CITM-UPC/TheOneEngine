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

	void GUIWindow1();
	void InspectorWindow();

private:

	/*Methods for windows*/


private:

	/*Create bools for each window to show*/
	bool show_gui = true;

	/*GUIWINDOW1 BOOLS*/
	bool show_guiwindow_1 = false;
	bool full_desktop = false;
	bool minimize_window = false;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};