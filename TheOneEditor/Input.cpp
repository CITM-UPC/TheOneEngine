#include "App.h"

Input::Input(App* app) : Module(app) {}

Input::~Input(){}

bool Input::PreUpdate()
{
    while (processSDLEvents())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        SDL_GL_SwapWindow(app->window->GetWindow());
    }
    return true;
}

bool Input::processSDLEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT: return false;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: return false;
            }
            break;
        }
    }
    return true;
}