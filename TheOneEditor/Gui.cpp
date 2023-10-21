#include "App.h"
#include "Gui.h"
#include "Window.h"
#include "Log.h"

Gui::Gui(App* app) : Module(app)
{
    
}

// Destructor
Gui::~Gui()
{}

bool Gui::Awake()
{
    LOG("Creating IMGUI context");
    bool ret = true;

    show_guiwindow_1 = true;
    show_inspector_window = true;

    return ret;
}

bool Gui::Start()
{
    LOG("Starting IMGUI");
    bool ret = true;

    // Setup Dear ImGui context should go on Init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiContext* contextui = ImGui::GetCurrentContext();
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

    /*Clears GUI*/
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (show_gui)
    {
        GeneralWindowDockspace();
    }

    if (show_guiwindow_1)
    {
        GUIWindow1();
    }

    if (show_inspector_window)
    {
        InspectorWindow();
    }


    return ret;
}

bool Gui::Update(float dt)
{
    bool ret = true;



    return ret;
}

bool Gui::PostUpdate()
{
    bool ret = true;

    
    return ret;
}

bool Gui::CleanUp()
{
    LOG("Cleaning up IMGUI");
    bool ret = true;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return ret;
}

void Gui::RenderGui()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::HandleInput(SDL_Event* event) 
{
    ImGui_ImplSDL2_ProcessEvent(event);
}

void Gui::GeneralWindowDockspace()
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

            ImGui::DockBuilderDockWindow("Inspector", dock_id_right); //Takes the name of a window
            //ImGui::DockBuilderDockWindow("Hello, world!", dock_id_down);
            ImGui::DockBuilderFinish(dockspace_id);
        }
    }

    ImGui::End();
}

void Gui::GUIWindow1() {
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

void Gui::InspectorWindow()
{
    ImGuiIO& io = ImGui::GetIO();

    clear_color = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

    ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once); //Sets window size only once with ImGuiCond_Once, if fixed size erase it.
    ImGui::Begin("Inspector");

    /*Transform*/
    ImGui::Text("Transform");
    //ImGui::TextEx("Escribe");

    ImGui::End();
}