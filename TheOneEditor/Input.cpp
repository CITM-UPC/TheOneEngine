#include "App.h"
#include "Input.h"
#include "Gui.h"
#include "Window.h"

#include "Log.h"


#define MAX_KEYS 300
#define SCREEN_SIZE 1


Input::Input(App* app) : Module(app) 
{
    keyboard = new KEY_STATE[MAX_KEYS];
    memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
    memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

Input::~Input()
{
    delete[] keyboard;
}

bool Input::Awake()
{
    LOG(LogType::LOG_INFO, "Init SDL input event system");
    bool ret = true;
    SDL_Init(0);

    if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
    {
        LOG(LogType::LOG_ERROR,"SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
        ret = false;
    }

    return ret;
}

bool Input::PreUpdate()
{
    bool ret = true;

    ret = processSDLEvents();

    return ret;
}

bool Input::Update(double dt)
{
    bool ret = true;

    return ret;
}

bool Input::processSDLEvents()
{
    SDL_PumpEvents();

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (keys[i] == 1)
        {
            if (keyboard[i] == KEY_IDLE)
                keyboard[i] = KEY_DOWN;
            else
                keyboard[i] = KEY_REPEAT;
        }
        else
        {
            if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
                keyboard[i] = KEY_UP;
            else
                keyboard[i] = KEY_IDLE;
        }
    }

    Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

    mouse_x /= SCREEN_SIZE;
    mouse_y /= SCREEN_SIZE;
    mouse_z = 0;

    for (int i = 0; i < 5; ++i)
    {
        if (buttons & SDL_BUTTON(i))
        {
            if (mouse_buttons[i] == KEY_IDLE)
                mouse_buttons[i] = KEY_DOWN;
            else
                mouse_buttons[i] = KEY_REPEAT;
        }
        else
        {
            if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
                mouse_buttons[i] = KEY_UP;
            else
                mouse_buttons[i] = KEY_IDLE;
        }
    }

    mouse_x_motion = mouse_y_motion = 0;

    static SDL_Event event;

    while (SDL_PollEvent(&event) != 0)
    {
        app->gui->HandleInput(&event);
        switch (event.type)
        {
            case SDL_MOUSEWHEEL:
                mouse_z = event.wheel.y;
                break;

            case SDL_MOUSEMOTION:
                mouse_x = event.motion.x / SCREEN_SIZE;
                mouse_y = event.motion.y / SCREEN_SIZE;

                mouse_x_motion = event.motion.xrel / SCREEN_SIZE;
                mouse_y_motion = event.motion.yrel / SCREEN_SIZE;
                break;

            case SDL_QUIT: return false;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE: return false;
                }
                break;

            case SDL_WINDOWEVENT:
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    app->window->OnResizeWindow(event.window.data1, event.window.data2);
                }
            }
            case (SDL_DROPFILE):
            {      // In case if dropped file
                dropped_filedir = event.drop.file;

                /*if (dropped_filedir.ends_with(".fbx")) {
                    std::filesystem::copy(dropped_filedir, "Assets");
                }
                else if (dropped_filedir.ends_with(".png") || dropped_filedir.ends_with(".dds")) {
                    std::filesystem::copy(dropped_filedir, "Assets");
                }*/
            }
        }
    }
    return true;
}