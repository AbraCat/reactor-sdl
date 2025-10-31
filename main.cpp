#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "sdl-adapter.h"
#include "window.hpp"
#include "texture.hpp"

static dr4::MyWindow* window = NULL;
static dr4::Texture* root_texture = NULL;

const double fps = 30;
const int begin_ticks = 1000, width = 1920, height = 1000;
bool first_frame = 1;

/*
virtual functions:
copying object
saving and reading scene configuration to/from file

multithreading segfault
better text rendering
3 views
propagate texture transformation
*/

SDL_Texture* te = NULL;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    srand(1);

    window = new dr4::MyWindow(dr4::Vec2f(width, height), "Optical Constructor");
    window->Open();

    // root_texture = new dr4::MyTexture(dr4::Vec2f(width / 2, height / 2));

    // state = new State();
    // desktop = new Desktop();

    // te = SDL_CreateTexture()
    // window->Display();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) return SDL_APP_SUCCESS;

    // if (event->type == SDL_EVENT_KEY_DOWN)
    // {
    //     if (event->key.key == SDLK_RSHIFT) return SDL_APP_SUCCESS;
    //     if (event->key.repeat == true) return SDL_APP_CONTINUE;

    //     if (state->focused != nullptr)
    //     {
    //         KeyboardEvent evt(event->key.key);
    //         state->focused->keyboardEvent(&evt);
    //     }

    //     return SDL_APP_CONTINUE;
    // }

    // if (event->type == SDL_EVENT_MOUSE_MOTION)
    // {
    //     MouseEvent e(MOUSE_MOVE, event->button.x, event->button.y);
    //     desktop->handleEvent(&e);
    //     return SDL_APP_CONTINUE;
    // }

    // if (event->button.button == SDL_BUTTON_LEFT)
    // {
    //     MouseEnum type;
    //     switch (event->type)
    //     {
    //         case SDL_EVENT_MOUSE_BUTTON_DOWN: type = MOUSE_DOWN; break;
    //         case SDL_EVENT_MOUSE_BUTTON_UP: type = MOUSE_UP; break;
    //         default: return SDL_APP_CONTINUE;
    //     }
        
    //     MouseEvent e(type, event->button.x, event->button.y);
    //     desktop->handleEvent(&e);
    //     return SDL_APP_CONTINUE;
    // }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    // if (first_frame)
    // {
    //     desktop->updateTextureRec();
    //     first_frame = 0;
    // }

    // if (SDL_GetTicks() < begin_ticks)
    //     desktop->t->renderRec();

    // IdleEvent evt;
    // desktop->handleEvent(&evt);

    // // desktop->t->renderRec();
    // desktop->t->renderIfUpdatedRec();
    // SDL_RenderPresent(renderer);

    // SDL_Delay(1000.0 / fps);

    // SDL_FRect r;
    // r.x = 100;
    // r.y = 100;
    // r.w = 100;
    // r.h = 100;

    // SDL_SetRenderTarget(getRenderer(), NULL);
    // SDL_SetRenderDrawColor(getRenderer(), 255, 0, 0, 255);
    // SDL_RenderFillRect(getRenderer(), &r);

    // dr4::Rectangle rect(dr4::Rect2f(dr4::Vec2f(100, 100),
    //     dr4::Vec2f(100, 100)), dr4::Color(255, 0, 0, 255), dr4::Color(0, 0, 0, 0));
    // root_texture->Draw(rect);

    // SDL_FRect r;
    // r.x = 100;
    // r.y = 100;
    // r.w = 100;
    // r.h = 100;

    // printf("%lf %lf %lf %lf %p\n", r.x, r.y, r.w, r.h, getRenderer());

    // print(Vector(rect.fill.r, rect.fill.g, rect.fill.b));
    
    // setColor(Vector(255, 0, 0));
    // SDL_SetRenderTarget(getRenderer(), window->root_widget->texture->t);
    // SDL_RenderFillRect(getRenderer(), &r);

    // SDL_SetRenderTarget(getRenderer(), NULL);
    // SDL_RenderTexture(getRenderer(), window->root_widget->texture->t, NULL, NULL);

    // SDL_RenderPresent(getRenderer());

    // window->Draw(*root_texture, dr4::Vec2f(0, 0));
    // window->Draw()
    window->Display();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // delete desktop;
    // delete state;

    window->Close();
    delete window;
}
