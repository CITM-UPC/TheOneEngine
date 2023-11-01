#include "App.h"
#include "Gui.h"
#include "Window.h"
#include "Log.h"

Gui::Gui(App* app) : Module(app)
{
    
}

Gui::~Gui()
{}

bool Gui::Awake()
{
    LOG("Creating IMGUI context");
    bool ret = true;

    show_guiwindow_1 = false;
    show_sceneView_window = false;
    show_inspector_window = true;
    show_hierarchy_window = true;
    show_assets_window = true;
    show_console_window = true;

    return ret;
}

bool Gui::Start()
{
    LOG("Starting IMGUI");
    bool ret = true;

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

    ImGuiWindowFlags flags = 0;

    return ret;
}

bool Gui::PreUpdate()
{
    bool ret = true;

    // Clears GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
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

    // hekbas TODO iterate UIWindows + Draw


    if (show_guiwindow_1)
    {
        GUIWindow1();
    }

    if (show_sceneView_window)
    {
        SceneViewWindow();
    }

    if (show_inspector_window)
    {
        InspectorWindow();
    }

    if (show_hierarchy_window)
    {
        HierarchyWindow();
    }

    if (show_assets_window)
    {
        AssetsWindow();
    }

    if (show_console_window)
    {
        ConsoleWindow();
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

void Gui::MainWindowDockspace()
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

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
            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
            auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);

            ImGui::DockBuilderDockWindow("Scene", dockspace_id); //Takes the name of a window
            ImGui::DockBuilderDockWindow("Inspector", dock_id_right); //Takes the name of a window
            ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left); //Takes the name of a window
            ImGui::DockBuilderDockWindow("Assets", dock_id_down); //Takes the name of a window
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
    if (ImGui::MenuItem("Console")) {}
    if (ImGui::MenuItem("Hierarchy")) {}
    if (ImGui::MenuItem("Inspector")) {}
    if (ImGui::MenuItem("Project")) {}
    if (ImGui::MenuItem("Scene")) {}
}

void Gui::MainMenuHelp()
{
    if (ImGui::BeginMenu("About TheOneEngine"))
    {
        ImGui::Text("TheOneEngine by Hector Bascones Zamora & Arnau Jimenez Gallego.");
        ImGui::Text("This is a demo for the subject of Game Engines, CITM - UPC");
        ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Documentation"))
    {
        //OpenURL("link here");
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


// Windows ----------------------------------------------------

void Gui::GUIWindow1()
{
    ImGuiIO& io = ImGui::GetIO();

    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    static float f = 0.0f;
    static int counter = 0;

    ImGui::SetNextWindowSize(ImVec2(339, 180), ImGuiCond_Once); //Sets window size only once with ImGuiCond_Once, if fixed size erase it.
    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Full Desktop", &full_desktop);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Minimize Window", &minimize_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

void Gui::SceneViewWindow()
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

void Gui::InspectorWindow()
{
    ImGuiIO& io = ImGui::GetIO();

    clear_color = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

    ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once); //Sets window size only once with ImGuiCond_Once, if fixed size erase it.
    ImGui::Begin("Inspector");
    
    /*Transform*/
    ImGui::SeparatorText("Transform");
    static char buf[5] = "0";
    ImGui::Text("Position");
    //ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
    ImGui::PushItemWidth(40.0f);
    ImGui::SameLine();
    ImGui::InputText("x", buf, IM_ARRAYSIZE(buf));
    ImGui::SameLine();
    ImGui::InputText("y", buf, IM_ARRAYSIZE(buf));
    ImGui::SameLine();
    ImGui::InputText("z", buf, IM_ARRAYSIZE(buf));
    
    ImGui::Text("Rotation");
    //ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
    ImGui::PushItemWidth(40.0f);
    ImGui::SameLine();
    ImGui::InputText("x", buf, IM_ARRAYSIZE(buf));
    ImGui::SameLine();
    ImGui::InputText("y", buf, IM_ARRAYSIZE(buf));
    ImGui::SameLine();
    ImGui::InputText("z", buf, IM_ARRAYSIZE(buf));
    
    ImGui::Text("Scale   ");
    //ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
    ImGui::PushItemWidth(40.0f);
    ImGui::SameLine();
    ImGui::InputText("x", buf, IM_ARRAYSIZE(buf));
    ImGui::SameLine();
    ImGui::InputText("y", buf, IM_ARRAYSIZE(buf));
    ImGui::SameLine();
    ImGui::InputText("z", buf, IM_ARRAYSIZE(buf));
    
    ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
    ImGui::Text("Mesh");
    static char mesh_name[32] = "house.fbx";
    ImGui::PushItemWidth(150.0f);
    ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
    ImGui::InputText("", mesh_name, IM_ARRAYSIZE(mesh_name));

    ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
    ImGui::Text("Texture");
    static char texture_name[32] = "texture.png";
    ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
    ImGui::InputText("", texture_name, IM_ARRAYSIZE(texture_name));

    ImGui::PopItemWidth();

    ImGui::End();
}

void Gui::HierarchyWindow()
{

    ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once); //Sets window size only once with ImGuiCond_Once, if fixed size erase it.
    ImGui::Begin("Hierarchy");
    
    ImGui::End();
}

void Gui::AssetsWindow()
{
    ImGui::Begin("Assets");
    
    ImGui::End();
}

void Gui::ConsoleWindow()
{
    ImGui::Begin("Console");
    //console.Draw("Console", &show_console_window);
    ImGui::End();
}