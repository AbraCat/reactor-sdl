/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
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

sdl adapter
texture manager (texture is an array of pixel colors)
only redraw widgets if they change
*/

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    srand(1);

    if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    setRenderer(renderer);

    desktop = new Desktop();
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    if (event->button.button == SDL_BUTTON_LEFT)
    {
        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            MouseEvent e(1, event->button.x, event->button.y);
            desktop->handleEvent(&e);
        }
        else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            MouseEvent e(0, event->button.x, event->button.y);
            desktop->handleEvent(&e);
        }
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    desktop->handleEvent(new IdleEvent());
    desktop->put();

    SDL_RenderPresent(renderer);
    SDL_Delay(1000.0 / 60);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    delete desktop;
}
