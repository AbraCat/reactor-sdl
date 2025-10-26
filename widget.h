#ifndef WIDGET_H
#define WIDGET_H

#include <SDL3/SDL.h>

#include <vector>
#include <string>
#include <functional>

#include "myvector.h"
#include "event.h"
#include "texture.h"

class Widget;
class Event;
class MouseEvent;
class IdleEvent;
class KeyboardEvent;
class State;

extern State* state;

class State
{
public:
    State();

    Widget* focused;
};

/*
coordinate systems:

absTL - absolute
tl, br - parent
everything else - this (if not said otherwise)
*/

class Widget
{
public:
    Widget(Vector tl, Vector br, Widget* parent = nullptr);
    ~Widget();

    virtual void addWidget(Widget* child);
    virtual int removeChild(Widget* child);
    virtual int removeChildByPredicate(std::function<bool(Widget*)> predicate);
    void clearChildren();

    void setPixelTexture(bool pixel_texture);
    void setWidgetBorderVisible(bool visible);
    void setTextureBorderVisible(bool visible);
    void setFillRect(bool fill, Vector color = Vector(0, 0, 0));
    void setDraggable(Vector dragTL = Vector(), Vector dragBR = Vector());

    virtual Vector getAbsTL();
    virtual Vector propagatedAbsTL();
    void propagateAbsPos();

    void drawWidgetRect(bool fill, Vector color = Vector(0, 0, 0));
    bool inRect(Vector point);
    bool inAbsRect(Vector point);

    virtual void resize(Vector newtl, Vector newbr);
    virtual void movePos(Vector newtl);

    void drawRec();
    virtual void paint();

    virtual bool handleEvent(Event* e);
    virtual bool onIdle(IdleEvent* e);
    virtual bool mousePressEvent(MouseEvent* e);
    virtual bool mouseMoveEvent(MouseEvent* e);
    virtual bool mouseReleaseEvent(MouseEvent* e);
    virtual bool keyboardEvent(KeyboardEvent* e);
// protected:
    Texture *t;

    Vector tl, br, wh, absTL;
    int width, height;

    bool draggable, dragged;
    Vector dragTL, dragBR, dragMouse; // relative to parent

    Vector fill_rect_color;
    bool w_border_visible, t_border_visible, fill_rect, pixel_texture;

    Widget *parent;
    std::vector<Widget*> children;
};

#endif // WIDGET_H