#include "App.h"
#include "Window.h"


Window::Window(App* app) : Module(app), window(nullptr), glContext(nullptr) {}

Window::~Window() {}

bool Window::Awake()
{
    window = initSDLWindowWithOpenGL();
    glContext = createSdlGlContext(window);
    initOpenGL();
    return true;
}

bool Window::Start()
{

    return true;
}


bool Window::CleanUp()
{
    if (glContext) SDL_GL_DeleteContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}

SDL_Window* Window::initSDLWindowWithOpenGL()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) throw std::exception(SDL_GetError());

    SDL_version compiled;
    SDL_VERSION(&compiled);
    std::cout << "SDL Compiled with " << std::to_string(compiled.major) << '.' << std::to_string(compiled.minor) << '.' << std::to_string(compiled.patch);

    SDL_version linked;
    SDL_GetVersion(&linked);
    std::cout << "SDL Linked with " << std::to_string(linked.major) << '.' << std::to_string(linked.minor) << '.' << std::to_string(linked.patch);

    // setup SDL window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // GL 3.1 + GLSL 130
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    std::string title = std::string(TITLE) + "_" + VERSION;

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    auto Width = displayMode.w;
    auto Height = displayMode.h;
    auto window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL);
    if (!window) throw std::exception(SDL_GetError());

    return window;
}

SDL_GLContext Window::createSdlGlContext(SDL_Window* window)
{
    auto gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) throw std::exception(SDL_GetError());
    if (SDL_GL_MakeCurrent(window, gl_context) != 0) throw std::exception(SDL_GetError());
    if (SDL_GL_SetSwapInterval(1) != 0) throw std::exception(SDL_GetError());
    return gl_context;
}

void Window::initOpenGL()
{
    auto glew_init_error = glewInit();
    if (glew_init_error != GLEW_OK) throw std::exception((char*)glewGetErrorString(glew_init_error));
    if (!GLEW_VERSION_3_1) throw std::exception("OpenGL 3.1 Not Supported!");
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.25, 0.25, .25, 1);
}

uint Window::GetDisplayRefreshRate()
{
    SDL_DisplayMode desktopDisplay;

    if (SDL_GetDesktopDisplayMode(0, &desktopDisplay) == 0)
        refreshRate = desktopDisplay.refresh_rate;
    else
        LOG("*SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());

    return refreshRate;
}