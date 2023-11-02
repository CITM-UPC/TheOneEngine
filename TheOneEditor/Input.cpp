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

bool Input::Update(double dt)
{
    bool ret = true;

    CameraInput(dt);

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
    // Implement a debug camera with keys and mouse
    // Now we can make this movememnt frame rate independant!

    double speed = 10 * dt;
    if (GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
        speed = 30 * dt;

    vec3 normalizedVec = (speed * (glm::normalize(app->engine->camera.center - app->engine->camera.eye)));

    // Rotation matrix definition
    glm::mat3 rotationMat = glm::mat3(cos(glm::radians(90.0f)), 0, sin(glm::radians(90.0f)),
                                                             0, 1,                        0,
                                     -sin(glm::radians(90.0f)), 0, cos(glm::radians(90.0f)));

    // Normal vector between the rotation matrix and the normalized vector
    vec3 normalVec = rotationMat * normalizedVec;

    if (GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        /*if (GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
        {
            normalizedVec *= App->game_engine->camera.cameraSpeedMultiplier;
            normalVec *= App->game_engine->camera.cameraSpeedMultiplier;
        }*/


        if (GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
        {
            app->engine->camera.eye += normalizedVec;
            app->engine->camera.center += normalizedVec;
        }
        if (GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
        {
            app->engine->camera.eye -= normalizedVec;
            app->engine->camera.center -= normalizedVec;
        }
        if (GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
        {
            app->engine->camera.cameraRight = glm::normalize(glm::cross(app->engine->camera.center, app->engine->camera.up));
            app->engine->camera.cameraRight.y = 0;
            app->engine->camera.eye -= app->engine->camera.cameraRight * speed;
            //app->engine->camera.center -= normalizedVec;
        }
        if (GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
        {
            app->engine->camera.cameraRight = glm::normalize(glm::cross(app->engine->camera.center, app->engine->camera.up));
            app->engine->camera.cameraRight.y = 0;
            app->engine->camera.eye += app->engine->camera.cameraRight * speed;
            //app->engine->camera.center += normalizedVec;
        } 

        /* MOUSE CAMERA MOVEMENT */
        // Compute mouse input displacement
        float mouseSensitivity = 15.0f * dt;
        int deltaX = GetMouseXMotion();
        int deltaY = -GetMouseYMotion();

        app->engine->camera.yaw += deltaX * mouseSensitivity;
        app->engine->camera.pitch += deltaY * mouseSensitivity;

        // Limiting Camera Piitch to prevent flipping
        if (app->engine->camera.pitch > 89.0f)
            app->engine->camera.pitch = 89.0f;
        if (app->engine->camera.pitch < -89.0f)
            app->engine->camera.pitch = -89.0f;

        // Update Camera's Focus view point vector to be recomputed in the renderer with gluLookAt()
        glm::vec3 direction;
        direction.x = cos(glm::radians(app->engine->camera.yaw)) * cos(glm::radians(app->engine->camera.pitch));
        direction.y = sin(glm::radians(app->engine->camera.pitch));
        direction.z = sin(glm::radians(app->engine->camera.yaw)) * cos(glm::radians(app->engine->camera.pitch));
        app->engine->camera.center = glm::normalize(direction);
    }
}
