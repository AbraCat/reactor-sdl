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

#include "app.h"
#include "planeitem.h"
#include "reactor.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

// static Reactor* reactor;
// static PlaneItem* plane1;
static App* app;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    srand(1);

    if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // plane1 = new PlaneItem(renderer, 2, std::vector{Vector(255, 0, 0), Vector(0, 0, 255)}, 10, 1, {0, 0, 0}, {200, 200, 0});
    app = new App(renderer);
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    // plane1->addPoint({-5, -4});
    // plane1->paint();

    // reactor->advance();
    // reactor->put();

    app->advance();

    SDL_RenderPresent(renderer);
    SDL_Delay(1000.0 / 30);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // delete plane1;
    // delete reactor;
    delete app;
}
