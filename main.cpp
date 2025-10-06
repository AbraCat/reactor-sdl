#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "sdl-adapter.h"
#include "desktop.h"
#include "plane.h"
#include "reactor.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static Desktop* desktop;

/*
TODO:

text rendering                         -
scroll bar mouse wheel                 -
rescale reactor                        -
only draw texture parts in widget rect -
*/

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    srand(1);

    if (!SDL_CreateWindowAndRenderer("Hello World", 1800, 1000, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    setRenderer(renderer);

    desktop = new Desktop();
    desktop->drawRec();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        MouseEvent e(MOUSE_MOVE, event->button.x, event->button.y);
        desktop->handleEvent(&e);
        return SDL_APP_CONTINUE;
    }

    if (event->button.button == SDL_BUTTON_LEFT)
    {
        MouseEnum type;
        switch (event->type)
        {
            case SDL_EVENT_MOUSE_BUTTON_DOWN: type = MOUSE_DOWN; break;
            case SDL_EVENT_MOUSE_BUTTON_UP: type = MOUSE_UP; break;
            default: return SDL_APP_CONTINUE;
        }
        
        MouseEvent e(type, event->button.x, event->button.y);
        desktop->handleEvent(&e);
        return SDL_APP_CONTINUE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    desktop->handleEvent(new IdleEvent());

    desktop->t->paintRec();
    SDL_RenderPresent(renderer);

    SDL_Delay(1000.0 / 30);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    delete desktop;
}
