#ifndef WIDGET_H
#define WIDGET_H

#include <SDL3/SDL.h>

#include <vector>

#include "myvector.h"

class Widget;
class Event;
class MouseEvent;
class IdleEvent;

/*
coordinate systems:

absTL - absolute
tl, br - parent
everything else - this (if not said otherwise)
*/
class Widget
{
public:
    Widget(IntVec tl, IntVec br, Widget* parent = nullptr);
    ~Widget();

    void setBorderVisible(bool visible);
    void setFillRect(bool fill, Vector color = Vector(0, 0, 0));
    IntVec getAbsTL();
    // void propagateAbsChange();

    void drawWidgetRect(bool fill, Vector color = Vector(0, 0, 0));
    bool inRect(IntVec point);
    bool inAbsRect(IntVec point);
    virtual void resize(IntVec newtl, IntVec newbr);
    virtual void movePos(IntVec newtl);

    void put();
    virtual void paint();

    void addChild(Widget* widget);
    bool handleEvent(Event* e);

    virtual bool onIdle(IdleEvent* e);
    virtual bool mousePressEvent(MouseEvent* e);
    virtual bool mouseMoveEvent(MouseEvent* e);
    virtual bool mouseReleaseEvent(MouseEvent* e);

    void setDraggable(IntVec dragTL = IntVec(), IntVec dragBR = IntVec());

protected:
    IntVec tl, br, wh, absTL;
    int width, height;

    bool draggable, dragged;
    IntVec dragTL, dragBR, dragMouse; // relative to parent

    Vector fill_rect_color;
    bool border_visible, fill_rect;

    Widget *parent;
    std::vector<Widget*> children;
};

class WContainer : public Widget
{
public:
    WContainer(IntVec tl, IntVec br, int nChildren, bool vertical);
    virtual void paint() override;

    void addWidget(Widget* widget);

private:
    bool vertical;
    int nChildren, padding, childWidth, childHeight;
};



class Event
{
public:
    virtual bool dispatch(Widget* w) = 0;
};

enum MouseEnum
{
    MOUSE_DOWN,
    MOUSE_UP,
    MOUSE_MOVE
};

class MouseEvent : public Event
{
public:
    MouseEvent(MouseEnum type, int x, int y);
    virtual bool dispatch(Widget* w) override;

    int x, y;
    MouseEnum type;
};

class IdleEvent : public Event
{
    virtual bool dispatch(Widget* w) override;
};

#endif // WIDGET_H