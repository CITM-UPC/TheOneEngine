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

    if (show_guiwindow_1)
    {
        GUIWindow1();
    }

    /*if (show_inspector_window)
    {
        InspectorWindow();
    }*/


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

void Gui::GUIWindow1() {
    ImGuiIO& io = ImGui::GetIO();

    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    static float f = 0.0f;
    static int counter = 0;

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

    ImGui::Begin("Inspector");

    /*Transform*/
    ImGui::Text("");

    ImGui::End();
}