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

void Widget::resize(IntVec newTL, IntVec newBR)
{
    this->TL = newTL;
    this->BR = newBR;

    width = BR.x - TL.x;
    height = BR.y - TL.y;
}

void Widget::drawRect(bool fill, Vector color)
{
    SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);
    SDL_FRect rect;
    rect.x = TL.x;
    rect.y = TL.y;
    rect.w = width;
    rect.h = height;
    
    if (fill) SDL_RenderFillRect(renderer, &rect);
    else SDL_RenderRect(renderer, &rect);
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
        if (w->handleEvent(e)) return 1;
    return e->dispatch(this);
}

bool Widget::mousePressEvent(MouseEvent* e) { return 0; }
bool Widget::mouseReleaseEvent(MouseEvent* e) { return 0; }
bool Widget::onIdle(IdleEvent* e) { return 0; }


WContainer::WContainer(SDL_Renderer* renderer, IntVec TL, IntVec BR, int nChildren) : Widget(renderer, TL, BR)
{
    this->nChildren = nChildren;
    this->padding = 10;

    this->childWidth = (width - padding * (nChildren + 1)) / nChildren;
    this->childHeight = height - 2 * padding;
}

void WContainer::addWidget(Widget* w)
{
    assert(children.size() < nChildren);

    int nChild = children.size();
    w->resize(IntVec(TL.x + padding * (nChild + 1) + childWidth * nChild, TL.y + padding), 
        IntVec(TL.x + (padding + childWidth) * (nChild + 1), BR.y - padding));

    addChild(w);
}

void WContainer::paint()
{
    drawRect(0, Vector(255, 255, 255));
}



MouseEvent::MouseEvent(bool down, int x, int y)
{
    this->down = down;
    this->x = x;
    this->y = y;
}

bool MouseEvent::dispatch(Widget* w)
{
    if (down) return w->mousePressEvent(this);
    else return w->mouseReleaseEvent(this);
}

bool IdleEvent::dispatch(Widget* w)
{
    return w->onIdle(this);
}