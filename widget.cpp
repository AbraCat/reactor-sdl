#include "widget.h"

#include <iostream>

#include <cassert>

Widget::Widget(SDL_Renderer* renderer, IntVec TL, IntVec BR, Widget* parent)
{
    this->renderer = renderer;
    this->TL = TL;
    this->BR = BR;

    this->parent = parent;

    width = BR.x - TL.x;
    height = BR.y - TL.y;
}

Widget::~Widget()
{
    for (Widget* w: children)
        delete w;
}

void Widget::paint()
{
    //
}

bool Widget::inRect(IntVec point)
{
    return point.x > TL.x && point.x < BR.x && point.y > TL.y && point.y < BR.y;
}

void Widget::fill(Vector color)
{
    SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);
    SDL_FRect rect;
    rect.x = TL.x;
    rect.y = TL.y;
    rect.w = width;
    rect.h = height;
    SDL_RenderFillRect(renderer, &rect);
}

void Widget::put()
{
    this->paint();
    for (Widget* w: children)
        w->put();
}

void Widget::addChild(Widget* widget)
{
    assert(widget->parent == nullptr);

    children.push_back(widget);
    widget->parent = this;
}

bool Widget::handleEvent(Event* e)
{
    for (Widget* w: children)
    {
        if (e->dispatch(w)) return 1;
    }
    return e->dispatch(this);
}

bool Widget::mousePressEvent(MouseEvent* e)
{
    return 0;
}

MouseEvent::MouseEvent(bool down, int x, int y)
{
    this->down = down;
    this->x = x;
    this->y = y;
}

bool MouseEvent::dispatch(Widget* w)
{
    return w->mousePressEvent(this);
}