#ifndef INPUT_H
#define INPUT_H

#include "Module.h"
#include "App.h"
#include <SDL2/SDL.h>


class Input : public Module
{
public:
    Input() {}

    virtual bool PreUpdate()
    {
        while (processSDLEvents())
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            
            SDL_GL_SwapWindow(app->window);
        }
        return true;
    }

    static bool processSDLEvents()
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
};

#endif // INPUT_H