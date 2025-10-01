#include "button.h"

const double unpressColorCoeff = 0.7;

Button::Button(SDL_Renderer* renderer, IntVec TL, IntVec BR, Vector color) : Widget(renderer, TL, BR)
{
    this->press_color = color;
    this->unpress_color = color * unpressColorCoeff;
    this->is_pressed = 0;
}

void Button::paint()
{
    if (is_pressed) fill(press_color);
    else fill(unpress_color);
}

bool Button::mousePressEvent(MouseEvent* e)
{
    if (!inRect(IntVec(e->x, e->y))) return 0;
    if (is_pressed) return 1;

    is_pressed = 1;
    action();
    return 1;
}

void Button::unpress()
{
    is_pressed = 0;
}

MoveWallButton::MoveWallButton(SDL_Renderer* renderer, Reactor* reactor, IntVec TL, IntVec BR, 
    Vector color) : Button(renderer, TL, BR, color)
{
    this->reactor = reactor;
}

void MoveWallButton::action()
{
    reactor->moveWall(10);
}