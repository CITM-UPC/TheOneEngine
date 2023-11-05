#include "App.h"
#include "Window.h"
#include "Log.h"


Window::Window(App* app) : Module(app), window(nullptr), glContext(nullptr) {}

Window::~Window() {}

bool Window::Awake()
{
    bool ret = true;

    if (!initSDLWindowWithOpenGL())
        return false;

    if (!createSdlGlContext())
        return false;

    if (!initOpenGL())
        return false;   

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

bool Window::initSDLWindowWithOpenGL()
{
    LOG(LogType::LOG_INFO, "# Initializing SDL Window with OpenGL");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        LOG(LogType::LOG_ERROR, "-Initializing SDL video subsystem: %s", SDL_GetError());
        return false;
    }
    LOG(LogType::LOG_OK, "-Initializing SDL video subsystem");


    SDL_version compiled;
    SDL_VERSION(&compiled);
    LOG(LogType::LOG_OK, "-SDL Compiled with %d.%d.%d", compiled.major, compiled.minor, compiled.patch);

    SDL_version linked;
    SDL_GetVersion(&linked);
    LOG(LogType::LOG_OK, "-SDL Linked with %d.%d.%d", linked.major, linked.minor, linked.patch);

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
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

    if (!window)
    {
        LOG(LogType::LOG_ERROR, "-Creating SDL window: %s", SDL_GetError());
        return false;
    }
    LOG(LogType::LOG_OK, "-Creating SDL Window");

    return true;
}

// hekbas SDL_Surface needed?
// hekbas MUST move glContext and OpenGL Init to ENGINE
bool Window::createSdlGlContext()
{
    LOG(LogType::LOG_INFO, "# Creating GLContext");

    glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        LOG(LogType::LOG_ERROR, "-Creating OpenGL context: %s", SDL_GetError());
        return false;
    }
    LOG(LogType::LOG_OK, "-Creating OpenGL context");

    if (SDL_GL_MakeCurrent(window, glContext) != 0)
    {
        LOG(LogType::LOG_ERROR, "-Setting the current OpenGL context : % s", SDL_GetError());
        return false;
    }
    LOG(LogType::LOG_OK, "-Setting the current OpenGL context");

    if (SDL_GL_SetSwapInterval(1) != 0)
    {
        LOG(LogType::LOG_ERROR, "-Setting swap interval for vsync: %s", SDL_GetError());
        return false;
    }
    LOG(LogType::LOG_OK, "-Setting swap interval for vsync");

    return true;
}

bool Window::initOpenGL()
{
    LOG(LogType::LOG_INFO, "# Initializing OpenGL");

    auto glew_init_error = glewInit();
    if (glew_init_error != GLEW_OK)
    {
        LOG(LogType::LOG_ERROR, "-Initializing Glew: %s", glewGetErrorString(glew_init_error));
        return false;
    }

    if (!GLEW_VERSION_3_1)
    {
        LOG(LogType::LOG_ERROR, "-OpenGL 3.1 Not Supported!");
        return false;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.25, 0.25, .25, 1);

    LOG(LogType::LOG_OK, "-Initializing OpenGL");

    return true;
}

uint Window::GetDisplayRefreshRate()
{
    SDL_DisplayMode desktopDisplay;

    if (SDL_GetDesktopDisplayMode(0, &desktopDisplay) == 0)
        refreshRate = desktopDisplay.refresh_rate;
    else
        LOG(LogType::LOG_ERROR, "Getting display refresh rate: %s", SDL_GetError());

    return refreshRate;
}

int Window::GetDisplayMode()
{
    return this->displayMode;
}

void Window::SetDisplayMode(DisplayMode properties)
{
    switch (properties)
    {
    case DisplayMode::WINDOWED:
        if (SDL_SetWindowFullscreen(window, 0) != 0)
            LOG(LogType::LOG_ERROR, "Setting Display Mode to Windowed", SDL_GetError());

        //SDL_SetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
        break;

    case DisplayMode::FULLSCREEN:
        if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
            LOG(LogType::LOG_ERROR, "Setting Display Mode to Fullscreen Desktop", SDL_GetError());
        break;

    case DisplayMode::FULLSCREEN_DESKTOP:
        if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
            LOG(LogType::LOG_ERROR, "Setting Display Mode to Fullscreen", SDL_GetError());
        break;

    case DisplayMode::BORDERLESS:
        SDL_SetWindowBordered(window, (SDL_bool)!borderless);
        break;
    }
}

void Window::SetResolution(Resolution resolution)
{
    switch (resolution)
    {
    case Resolution::R_3840x2160:
        break;
    case Resolution::R_2560x1440:
        break;
    case Resolution::R_1920x1080:
        break;
    case Resolution::R_1280x720:
        break;
    case Resolution::R_854x480:
        break;
    case Resolution::R_640x360:
        break;
    case Resolution::R_426x240:
        break;
    case Resolution::R_NATIVE:
        break;
    }
}