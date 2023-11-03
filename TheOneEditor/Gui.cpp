#include "App.h"
#include "Gui.h"
#include "Window.h"
#include "Log.h"

#include "Panel.h"
#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "PanelProject.h"
#include "PanelScene.h"
#include "PanelSettings.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"


Gui::Gui(App* app) : Module(app)
{
    
}

Gui::~Gui()
{}

bool Gui::Awake()
{
    LOG("Creating Panels");

    panelAbout = new PanelAbout(PanelType::ABOUT);
    panels.push_back(panelAbout);

    panelConsole = new PanelConsole(PanelType::CONSOLE);
    panels.push_back(panelConsole);

    panelHierarchy = new PanelHierarchy(PanelType::HIERARCHY);
    panels.push_back(panelHierarchy);

    panelInspector = new PanelInspector(PanelType::INSPECTOR);
    panels.push_back(panelInspector);

    panelProject = new PanelProject(PanelType::PROJECT);
    panels.push_back(panelProject);

    panelScene = new PanelScene(PanelType::SCENE);
    panels.push_back(panelScene);

    panelSettings = new PanelSettings(PanelType::STATS);
    panels.push_back(panelSettings);

    return true;
}

bool Gui::Start()
{
    LOG("Creating IMGUI context");

    IMGUI_CHECKVERSION();

    ImGuiContext* contextui = ImGui::CreateContext();
    
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(app->window->window, app->window->glContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Enable Panels
    app->gui->panelConsole->SetState(true);
    app->gui->panelHierarchy->SetState(true);
    app->gui->panelInspector->SetState(true);
    app->gui->panelProject->SetState(true);

    LOG("*Error test");
    LOG("!Warning test");

    return true;
}

bool Gui::PreUpdate()
{
    bool ret = true;

    // Clears GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(app->window->window);
    ImGui::NewFrame();

    // hekbas TODO get input here?

    // Dockspace
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
        MainWindowDockspace();

    return ret;
}

bool Gui::Update(double dt)
{
    bool ret = true;

    // Creates the Main Menu Bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ret = MainMenuFile();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            MainMenuEdit();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Assets"))
        {
            MainMenuAssets();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GameObject"))
        {
            MainMenuGameObject();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Component"))
        {
            MainMenuComponent();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            MainMenuWindow();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            MainMenuHelp();
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }

    return ret;
}

bool Gui::PostUpdate()
{
    bool ret = true;

    // Iterate Panels & Draw
    for (const auto& panel : panels)
    {
        if (panel->GetState())
            panel->Draw();
    }

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
        ImGui::End();

    return ret;
}

bool Gui::CleanUp()
{
    LOG("Cleaning up IMGUI");
    bool ret = true;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // hekbas check cleanup

    return ret;
}

void Gui::Draw()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();

    // hekbas look into this
    /*if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }*/
}

void Gui::HandleInput(SDL_Event* event) 
{
    ImGui_ImplSDL2_ProcessEvent(event);
}

void Gui::OpenURL(const char* url) const
{
    // hekbas need shellapi
    //ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

void Gui::MainWindowDockspace()
{
    // Resize
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    // Flags
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    // Style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    static bool p_open = true;
    ImGui::Begin("DockSpace", &p_open, window_flags);
    ImGui::PopStyleVar(3);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        static auto first_time = true;
        if (first_time)
        {
            first_time = false;

            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            auto dock_id_top = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.2f, nullptr, &dockspace_id);
            auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);
            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.15f, nullptr, &dockspace_id);
            auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.20f, nullptr, &dockspace_id);

            ImGui::DockBuilderDockWindow("Scene", dockspace_id); //Takes the name of a window
            ImGui::DockBuilderDockWindow("Inspector", dock_id_right); //Takes the name of a window
            ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left); //Takes the name of a window
            ImGui::DockBuilderDockWindow("Project", dock_id_down); //Takes the name of a window
            ImGui::DockBuilderDockWindow("Console", dock_id_down); //Takes the name of a window
            ImGui::DockBuilderFinish(dockspace_id);
        }
    }
}


// Main Menu Bar ----------------------------------------------

bool Gui::MainMenuFile()
{
    bool ret = true;

    if (ImGui::MenuItem("New", 0, false, false)) {}
    if (ImGui::MenuItem("Open", "Ctrl+O", false, false)) {}
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Save", "Ctrl+S", false, false)) {}
    if (ImGui::MenuItem("Save As..", 0, false, false)) {}

    ImGui::Separator();

    if (ImGui::MenuItem("Exit"))
        ret = false;

    return ret;
}

void Gui::MainMenuEdit()
{
    if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false)) {}
    if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {}  // Disabled item

    ImGui::Separator();

    if (ImGui::MenuItem("Cut", "Ctrl+X", false, false)) {}
    if (ImGui::MenuItem("Copy", "Ctrl+C", false, false)) {}
    if (ImGui::MenuItem("Paste", "Ctrl+V", false, false)) {}
}

void Gui::MainMenuAssets()
{
    if (ImGui::BeginMenu("Create"))
    {
        if (ImGui::MenuItem("Folder", 0, false, false)) {}

        ImGui::Separator();

        if (ImGui::MenuItem("Script", 0, false, false)) {}
        if (ImGui::MenuItem("Shader", 0, false, false)) {}

        ImGui::EndMenu();
    }
}

void Gui::MainMenuGameObject()
{
    if (ImGui::MenuItem("Create Empty", "Ctrl+Shift+N")) {}

    if (ImGui::BeginMenu("3D Object", "Ctrl+Shift+N"))
    {
        if (ImGui::MenuItem("Square", 0, false, false)) {}
        if (ImGui::MenuItem("Sphere", 0, false, false)) {}

        ImGui::EndMenu();
    }
}

void Gui::MainMenuComponent()
{
    if (ImGui::MenuItem("Mesh Renderer", 0, false, false)) {}
}

void Gui::MainMenuWindow()
{
    if (ImGui::MenuItem("Console")) { app->gui->panelConsole->SwitchState(); }
    if (ImGui::MenuItem("Hierarchy")) { app->gui->panelHierarchy->SwitchState(); }
    if (ImGui::MenuItem("Inspector")) { app->gui->panelInspector->SwitchState(); }
    if (ImGui::MenuItem("Project")) { app->gui->panelProject->SwitchState(); }
    if (ImGui::MenuItem("Scene")) { app->gui->panelScene->SwitchState(); }
    if (ImGui::MenuItem("Settings")) { app->gui->panelSettings->SwitchState(); }
}

void Gui::MainMenuHelp()
{
    if (ImGui::MenuItem("About TheOneEngine"))
    {
        app->gui->panelAbout->SwitchState();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Documentation"))
    {
        OpenURL("link here");
    }
    
    ImGui::Separator();

    HardwareInfo hardware_info = app->hardware->GetInfo();

    // SDL Version
    ImGui::Text("SDL Version:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.sdl_version);
    ImGui::Separator();

    // CPU 
    ImGui::Text("CPU Logic Cores:");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", hardware_info.cpu_count);

    ImGui::Text("CPU L1 Cache (Kb):");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "%i", hardware_info.l1_cachekb);

    ImGui::Text("CPU Instruction Support:");
    ImGui::SameLine();

    if (hardware_info.rdtsc)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "rdtsc");
    ImGui::SameLine();
    if (hardware_info.altivec)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "altivec");
    ImGui::SameLine();
    if (hardware_info.now3d)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "now3d");
    ImGui::SameLine();
    if (hardware_info.mmx)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "mmx");
    ImGui::SameLine();
    if (hardware_info.sse)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "sse");
    ImGui::SameLine();
    if (hardware_info.sse2)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "sse2");
    ImGui::SameLine();
    if (hardware_info.sse3)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "sse3");
    ImGui::SameLine();
    if (hardware_info.sse41)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "sse41");
    ImGui::SameLine();
    if (hardware_info.sse42)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "sse42");
    ImGui::SameLine();
    if (hardware_info.avx)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "avx");
    ImGui::SameLine();
    if (hardware_info.avx2)
        ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", "avx2");

    // RAM 
    ImGui::Separator();
    ImGui::Text("RAM Memory (Gb)");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.ram_gb);

    // GPU (Currently NVIDIA only)
    ImGui::Separator();
    ImGui::Text("GPU Vendor");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.gpu_vendor.data());

    ImGui::Text("GPU Model");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.gpu_brand.data());

    ImGui::Text("GPU Driver");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s", hardware_info.gpu_driver.data());

    ImGui::Text("VRAM Budget");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.vram_mb_budget);

    ImGui::Text("VRAM Available");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.vram_mb_available);

    ImGui::Text("VRAM Usage");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "%f", hardware_info.vram_mb_usage);
}