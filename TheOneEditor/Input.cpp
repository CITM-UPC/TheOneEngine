#include "App.h"
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
    LOG("Init SDL input event system");
    bool ret = true;
    SDL_Init(0);

    if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
    {
        LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
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
                /*if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    App->renderer3D->OnResize(e.window.data1, e.window.data2);*/
            }
        }
        
    }
    return true;
}

void Input::CameraInput(double dt)
{
    //// Implement a debug camera with keys and mouse
    //// Now we can make this movememnt frame rate independant!

    //vec3 newPos(0, 0, 0);
    //double speed = 3.0f * dt;
    //if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
    //    speed = 8.0f * dt;

    //if (app->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
    //if (app->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

    //if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= app->engine->camera.center * speed;
    //if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += app->engine->camera.center * speed;


    //if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= app->engine->camera.eye * speed;
    //if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += app->engine->camera.eye * speed;

    ///*app->engine->camera.eye += newPos;
    //app->engine->camera.center += newPos;*/

    //// Mouse motion ----------------

    //if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    //{
    //    int dx = -app->input->GetMouseXMotion();
    //    int dy = -app->input->GetMouseYMotion();

    //    float Sensitivity = 0.25f;

    //    app->engine->camera.eye -= app->engine->camera.center;

    //    if (dx != 0)
    //    {
    //        float DeltaX = (float)dx * Sensitivity;

    //        /*X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
    //        Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
    //        Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));*/
    //    }

    //    if (dy != 0)
    //    {
    //        float DeltaY = (float)dy * Sensitivity;

    //        Y = rotate(Y, DeltaY, X);
    //        Z = rotate(Z, DeltaY, X);

    //        if (Y.y < 0.0f)
    //        {
    //            Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
    //            Y = cross(Z, X);
    //        }
    //    }

    //    Position = Reference + Z * length(Position);
    //}
}
