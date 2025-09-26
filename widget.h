#ifndef WIDGET_H
#define WIDGET_H

#include <SDL3/SDL.h>

#include "myvector.h"

class Widget
{
public:
    Widget(SDL_Renderer* renderer, IntVec TL, IntVec BR);
    virtual void paint();
    bool inRect(IntVec point);

    IntVec TL, BR;
    int width, height;
    SDL_Renderer* renderer;
};

#endif // WIDGET_H