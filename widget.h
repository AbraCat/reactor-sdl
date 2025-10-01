#ifndef WIDGET_H
#define WIDGET_H

#include <SDL3/SDL.h>

#include <vector>

#include "myvector.h"

class Widget;
class Event;
class MouseEvent;

class Widget
{
public:
    Widget(SDL_Renderer* renderer, IntVec TL, IntVec BR, Widget* parent = nullptr);
    ~Widget();

    void drawRect(bool fill, Vector color = Vector(0, 0, 0));
    bool inRect(IntVec point);
    virtual void resize(IntVec newTL, IntVec newBR);
    void put();
    virtual void paint();

    void addChild(Widget* widget);

    virtual bool mousePressEvent(MouseEvent* e);
    virtual bool mouseReleaseEvent(MouseEvent* e);
    bool handleEvent(Event* e);

    IntVec TL, BR;
    int width, height;
    SDL_Renderer* renderer;

    Widget *parent;
    std::vector<Widget*> children;
};

class WContainer : public Widget
{
public:
    WContainer(SDL_Renderer* renderer, IntVec TL, IntVec BR, int nChildren);
    virtual void paint() override;

    void addWidget(Widget* widget);

private:
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

#endif // WIDGET_H