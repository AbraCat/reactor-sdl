#include "button.h"

#include <cassert>

const double unpressColorCoeff = 0.7;

Button::Button(SDL_Renderer* renderer, IntVec TL, IntVec BR, Vector color, std::string text) : Widget(renderer, TL, BR)
{
    this->press_color = color;
    this->unpress_color = color * unpressColorCoeff;
    this->is_pressed = 0;

    this->text = text;
}

void Button::paint()
{
    if (is_pressed) drawRect(1, press_color);
    else drawRect(1, unpress_color);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, TL.x, TL.y, text.c_str());
}

bool Button::mousePressEvent(MouseEvent* e)
{
    if (!inRect(IntVec(e->x, e->y))) return 0;
    if (is_pressed) return 0;

    is_pressed = 1;
    action();
    return 1;
}

bool Button::mouseReleaseEvent(MouseEvent* e)
{
    is_pressed = 0;
    return 0;
}

void Button::unpress()
{
    is_pressed = 0;
}

MoveWallButton::MoveWallButton(SDL_Renderer* renderer, Reactor* reactor, Vector color, int step, std::string text)
    : Button(renderer, IntVec(), IntVec(), color, text)
{
    this->reactor = reactor;
    this->step = step;
}

void MoveWallButton::action()
{
    reactor->moveWall(step);
}

TemperatureButton::TemperatureButton(SDL_Renderer* renderer, Reactor* reactor, Vector color, double step, std::string text)
    : Button(renderer, IntVec(), IntVec(), color, text)
{
    this->reactor = reactor;
    this->step = step;
}

void TemperatureButton::action()
{
    reactor->increaseTemp(step);
}

AddMolButton::AddMolButton(SDL_Renderer* renderer, Reactor* reactor, Vector color, int nAddMols, std::string text)
    : Button(renderer, IntVec(), IntVec(), color, text)
{
    this->reactor = reactor;
    this->nAddMols = nAddMols;
}

void AddMolButton::action()
{
    reactor->addRandomMols(nAddMols);
}
