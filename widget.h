#ifndef WIDGET_H
#define WIDGET_H

#include <SDL3/SDL.h>

#include <vector>

#include "myvector.h"

class Widget
{
public:
    Widget(SDL_Renderer* renderer, IntVec TL, IntVec BR, Widget* parent = nullptr);
    ~Widget();

    void fill(Vector color = Vector(0, 0, 0));
    bool inRect(IntVec point);
    void put();
    virtual void paint();

    void addChild(Widget* widget);

    IntVec TL, BR;
    int width, height;
    SDL_Renderer* renderer;

    Widget *parent;
    std::vector<Widget*> children;
};

#endif // WIDGET_H