#include "widget.h"
#include "sdl-adapter.h"

#include <iostream>
#include <cassert>

Widget::Widget(IntVec tl, IntVec br, Widget* parent)
{
    this->parent = parent;
    if (parent != nullptr)
    {
        parent->children.push_back(this);
        this->absTL = parent->absTL + tl;
    }
    else
        this->absTL = IntVec(0, 0);

    this->tl = tl;
    this->br = br;
    width = br.x - tl.x;
    height = br.y - tl.y;
    this->wh = IntVec(width, height);

    border_visible = 0;
    fill_rect = 0;
    fill_rect_color = Vector(0, 0, 0);

    this->draggable = 0;
    this->dragged = 0;
    this->dragTL = this->dragBR = {};
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
        drawRect(absTL, absTL + wh, 1);
    }
    if (border_visible)
    {
        setColor({255, 255, 255});
        drawRect(absTL, absTL + wh, 0);
    }
}

void Widget::setDraggable(IntVec dragTL, IntVec dragBR)
{
    assert(parent != nullptr);
    this->draggable = 1;

    if ((dragBR - dragTL).x == 0 && (dragBR - dragTL).y == 0)
    {
        this->dragTL = {0, 0};
        this->dragBR = parent->wh;
    }
    else
    {
        this->dragTL = dragTL;
        this->dragBR = dragBR;
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

IntVec Widget::getAbsTL() { return this->absTL; }

bool Widget::inRect(IntVec point)
{
    return point.x > tl.x && point.x < br.x && point.y > tl.y && point.y < br.y;
}

bool Widget::inAbsRect(IntVec point)
{
    return point.x > absTL.x && point.x < absTL.x + wh.x && point.y > absTL.y && point.y < absTL.y + wh.y;
}

void Widget::resize(IntVec newtl, IntVec newbr)
{
    this->absTL = newtl - tl + absTL;
    this->tl = newtl;
    this->br = newbr;

    width = br.x - tl.x;
    height = br.y - tl.y;
    wh = IntVec(width, height);
}

void Widget::movePos(IntVec newtl)
{
    IntVec change = newtl - tl;
    tl = newtl;
    br += change;
    absTL += change;
}

void Widget::drawWidgetRect(bool fill, Vector color)
{
    setColor(color);
    drawRect(absTL, absTL + wh, 1);
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
    // widget->setParent(this);
}

bool Widget::handleEvent(Event* e)
{
    for (Widget* w: children)
        if (w->handleEvent(e)) return 1;
    return e->dispatch(this);
}

bool Widget::mousePressEvent(MouseEvent* e)
{
    if (!inAbsRect({e->x, e->y})) return 0;
    if (!draggable) return 0;
    assert(parent != nullptr);

    if (!dragged)
    {
        dragged = 1;
        dragMouse = IntVec(e->x, e->y) - parent->absTL;
        return 1;
    }

    return 0;
}

bool Widget::mouseMoveEvent(MouseEvent* e)
{
    if (!draggable) return 0;
    assert(parent != nullptr);

    if (dragged)
    {
        IntVec newDragMouse = IntVec(e->x, e->y) - parent->absTL;
        IntVec change = newDragMouse - dragMouse;

        IntVec newTL = tl + change, newBR = br + change;
        if (newTL.x < dragTL.x || newBR.x > dragBR.x) change.x = 0;
        if (newTL.y < dragTL.y || newBR.y > dragBR.y) change.y = 0;

        movePos(tl + change);
        dragMouse = newDragMouse;
        return 1;
    }

    return 0;
}

bool Widget::mouseReleaseEvent(MouseEvent* e)
{
    if (!draggable) return 0;

    if (dragged)
    {
        dragged = 0;
        return 1;
    }
    return 0;
}

bool Widget::onIdle(IdleEvent* e)
{
    return 0;
}


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



MouseEvent::MouseEvent(MouseEnum type, int x, int y)
{
    this->type = type;
    this->x = x;
    this->y = y;
}

bool MouseEvent::dispatch(Widget* w)
{
    switch (type)
    {
        case MOUSE_DOWN: return w->mousePressEvent(this);
        case MOUSE_UP: return w->mouseReleaseEvent(this);
        case MOUSE_MOVE: return w->mouseMoveEvent(this);
    }
    return 0;
}

bool IdleEvent::dispatch(Widget* w)
{
    return w->onIdle(this);
}