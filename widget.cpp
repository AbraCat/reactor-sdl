#include "widget.h"
#include "sdl-adapter.h"

#include <iostream>
#include <cassert>

Widget::Widget(IntVec tl, IntVec br, Widget* parent)
{
    this->tl = tl;
    this->br = br;
    width = br.x - tl.x;
    height = br.y - tl.y;

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
        drawRect(tl, br, 1);
    }
    if (border_visible)
    {
        setColor({255, 255, 255});
        drawRect(tl, br, 0);
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
    return point.x > tl.x && point.x < br.x && point.y > tl.y && point.y < br.y;
}

void Widget::resize(IntVec newtl, IntVec newbr)
{
    this->tl = newtl;
    this->br = newbr;

    width = br.x - tl.x;
    height = br.y - tl.y;
}

void Widget::movePos(IntVec newtl)
{
    IntVec change = newtl - tl;
    tl = newtl;
    br += change;
}

void Widget::drawWidgetRect(bool fill, Vector color)
{
    setColor(color);
    drawRect(tl, br, fill);
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


WContainer::WContainer(IntVec tl, IntVec br, int nChildren, bool vertical) : Widget(tl, br)
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

    if (vertical) w->resize(IntVec(tl.x + padding, tl.y + padding * (nChild + 1) + childHeight * nChild), 
        IntVec(br.x - padding, tl.y + (padding + childHeight) * (nChild + 1)));
    else w->resize(IntVec(tl.x + padding * (nChild + 1) + childWidth * nChild, tl.y + padding), 
        IntVec(tl.x + (padding + childWidth) * (nChild + 1), br.y - padding));

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