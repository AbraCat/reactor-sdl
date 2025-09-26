#include "widget.h"

#include <iostream>

Widget::Widget(SDL_Renderer* renderer, IntVec TL, IntVec BR)
{
    this->renderer = renderer;
    this->TL = TL;
    this->BR = BR;

    width = BR.x - TL.x;
    height = BR.y - TL.y;
}

void Widget::paint()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_FRect rect;
    rect.x = TL.x;
    rect.y = TL.y;
    rect.w = width;
    rect.h = height;
    SDL_RenderFillRect(renderer, &rect);
}

bool Widget::inRect(IntVec point)
{
    return point.x > TL.x && point.x < BR.x && point.y > TL.y && point.y < BR.y;
}