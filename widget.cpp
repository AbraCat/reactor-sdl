#include "widget.h"
#include "sdl-adapter.h"

#include <iostream>
#include <cassert>

Widget::Widget(IntVec TL, IntVec BR, Widget* parent)
{
    this->TL = TL;
    this->BR = BR;
    width = BR.x - TL.x;
    height = BR.y - TL.y;

    this->parent = parent;
    border_visible = 0;
    fill_rect = 0;
    fill_rect_color = Vector(0, 0, 0);
}

Widget::~Widget()
{
    for (Widget* w: children)
        delete w;
}

void Widget::paint()
{
    if (fill_rect)
    {
        setColor(fill_rect_color);
        drawRect(TL, BR, 1);
    }
    if (border_visible)
    {
        setColor({255, 255, 255});
        drawRect(TL, BR, 0);
    }
}

void Widget::setBorderVisible(bool visible)
{
    border_visible = visible;
}

void Widget::setFillRect(bool fill, Vector color)
{
    fill_rect = fill;
    fill_rect_color = color;
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

void Widget::drawWidgetRect(bool fill, Vector color)
{
    setColor(color);
    drawRect(TL, BR, fill);
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


WContainer::WContainer(IntVec TL, IntVec BR, int nChildren, bool vertical) : Widget(TL, BR)
{
    this->vertical = vertical;
    this->nChildren = nChildren;
    this->padding = 10;

    if (vertical)
    {
        this->childHeight = (height - padding * (nChildren + 1)) / nChildren;
        this->childWidth = width - 2 * padding;
    }
    else
    {
        this->childWidth = (width - padding * (nChildren + 1)) / nChildren;
        this->childHeight = height - 2 * padding;
    }
}

void WContainer::addWidget(Widget* w)
{
    assert(children.size() < nChildren);

    int nChild = children.size();

    if (vertical) w->resize(IntVec(TL.x + padding, TL.y + padding * (nChild + 1) + childHeight * nChild), 
        IntVec(BR.x - padding, TL.y + (padding + childHeight) * (nChild + 1)));
    else w->resize(IntVec(TL.x + padding * (nChild + 1) + childWidth * nChild, TL.y + padding), 
        IntVec(TL.x + (padding + childWidth) * (nChild + 1), BR.y - padding));

    addChild(w);
}

void WContainer::paint()
{
    drawWidgetRect(0, Vector(255, 255, 255));
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