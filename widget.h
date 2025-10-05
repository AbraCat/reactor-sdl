#ifndef WIDGET_H
#define WIDGET_H

#include <SDL3/SDL.h>

#include <vector>

#include "myvector.h"

class Widget;
class Event;
class MouseEvent;
class IdleEvent;

class Widget
{
public:
    Widget(IntVec tl, IntVec br, Widget* parent = nullptr);
    ~Widget();

    void setBorderVisible(bool visible);
    void setFillRect(bool fill, Vector color = Vector(0, 0, 0));

    void drawWidgetRect(bool fill, Vector color = Vector(0, 0, 0));
    bool inRect(IntVec point);
    virtual void resize(IntVec newtl, IntVec newbr);
    virtual void movePos(IntVec newtl);

    void put();
    virtual void paint();

    void addChild(Widget* widget);
    bool handleEvent(Event* e);

    virtual bool onIdle(IdleEvent* e);
    virtual bool mousePressEvent(MouseEvent* e);
    virtual bool mouseReleaseEvent(MouseEvent* e);
protected:
    IntVec tl, br;
    int width, height;

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

class MouseEvent : public Event
{
public:
    MouseEvent(bool down, int x, int y);
    virtual bool dispatch(Widget* w) override;

    int x, y;
    bool down;
};

class IdleEvent : public Event
{
    virtual bool dispatch(Widget* w) override;
};

#endif // WIDGET_H