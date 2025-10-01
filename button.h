#ifndef MY_BUTTON_H
#define MY_BUTTON_H

#include "widget.h"
#include "reactor.h"

#include <SDL3/SDL.h>

#include <cstdio>

class Button : public Widget
{
public:
    Button(SDL_Renderer* renderer, IntVec TL, IntVec BR, Vector color);
    virtual void paint() override;

    virtual bool mousePressEvent(MouseEvent* e) override;

    virtual void action() = 0;
    void unpress();

    Vector press_color, unpress_color;
    bool is_pressed;
};

class MoveWallButton : public Button
{
public:
    MoveWallButton(SDL_Renderer* renderer, Reactor* reactor, IntVec TL, IntVec BR, Vector color);
    virtual void action() override;

private:
    Reactor* reactor;
};

#endif // MY_BUTTON_H