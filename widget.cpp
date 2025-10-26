#include "widget.h"
#include "sdl-adapter.h"

#include <iostream>
#include <cassert>

State* state = nullptr;

State::State() : focused(nullptr)
{
    //
}

Widget::Widget(Vector tl, Vector br, Widget* parent)
{
    this->tl = tl;
    this->br = br;
    width = br.x - tl.x;
    height = br.y - tl.y;
    this->wh = Vector(width, height);

    w_border_visible = 0;
    t_border_visible = 0;
    fill_rect = 0;
    fill_rect_color = Vector(0, 0, 0);

    this->draggable = 0;
    this->dragged = 0;
    this->dragTL = this->dragBR = {};

    this->t = new Texture(this);

    this->parent = parent;
    if (parent != nullptr)
    {
        this->absTL = parent->absTL + tl;
        parent->addWidget(this);
    }
    else
        this->absTL = Vector(0, 0);
}

Widget::~Widget()
{
    for (Widget* w: children)
        delete w;
    delete t;
}

void Widget::paint()
{
    t->clear();

    if (fill_rect) t->addRect({{0, 0}, wh}, {0, 0, 0}, 1);
    if (t_border_visible) t->addRect({{0, 0}, wh}, {255, 255, 255}, 0);
}

Vector Widget::propagatedAbsTL() { return absTL; }

void Widget::propagateAbsPos()
{
    for (Widget *w: children)
    {
        // w->absTL = this->absTL + w->tl;
        w->absTL = this->propagatedAbsTL() + w->tl;
        w->propagateAbsPos();
    }
}

void Widget::setDraggable(Vector dragTL, Vector dragBR)
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

void Widget::setPixelTexture(bool pixel_texture)
{
    if (t != nullptr) delete t;

    if (pixel_texture)
    {
        t = new PixelTexture(this);
        return;
    }

    t = new Texture(this);
}

void Widget::setWidgetBorderVisible(bool visible) { w_border_visible = visible; }
void Widget::setTextureBorderVisible(bool visible) { t_border_visible = visible; }

void Widget::setFillRect(bool fill, Vector color)
{
    fill_rect = fill;
    fill_rect_color = color;
}

Vector Widget::getAbsTL() { return this->absTL; }

bool Widget::inRect(Vector point)
{
    return point.x >= tl.x && point.x <= br.x && point.y >= tl.y && point.y <= br.y;
}

bool Widget::inAbsRect(Vector point)
{
    return point.x >= absTL.x && point.x <= absTL.x + wh.x && point.y >= absTL.y && point.y <= absTL.y + wh.y;
}

void Widget::resize(Vector newtl, Vector newbr)
{
    Vector TLchange = newtl - tl;
    this->absTL = newtl - tl + absTL;

    this->tl = newtl;
    this->br = newbr;

    width = br.x - tl.x;
    height = br.y - tl.y;
    wh = Vector(width, height);

    propagateAbsPos();
}

void Widget::movePos(Vector newtl)
{
    resize(newtl, br + newtl - tl);
}

void Widget::drawWidgetRect(bool fill, Vector color)
{
    t->addRect({{0, 0}, wh}, color, fill);
}

void Widget::drawRec()
{
    this->paint();
    for (Widget* w: children)
        w->drawRec();
}

void Widget::addWidget(Widget* child)
{
    assert(child->parent == this);
    children.push_back(child);
}

int Widget::removeChild(Widget* child)
{
    for (int n_child = 0; n_child < children.size(); ++n_child)
    {
        if (children[n_child] == child)
        {
            children.erase(children.begin() + n_child);
            return n_child;
        }
    }

    return -1;
}

int Widget::removeChildByPredicate(std::function<bool(Widget*)> predicate)
{
    for (int n_child = 0; n_child < children.size(); ++n_child)
    {
        if (predicate(children[n_child]))
        {
            children.erase(children.begin() + n_child);
            return n_child;
        }
    }

    return -1;
}

void Widget::clearChildren()
{
    for (Widget* w: children)
        delete w;

    children.clear();
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
        dragMouse = Vector(e->x, e->y) - parent->absTL;
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
        Vector newDragMouse = Vector(e->x, e->y) - parent->absTL;
        Vector change = newDragMouse - dragMouse;

        Vector newTL = tl + change, newBR = br + change;
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

bool Widget::keyboardEvent(KeyboardEvent* e)
{
    return 0;
}

bool Widget::onIdle(IdleEvent* e)
{
    return 0;
}
