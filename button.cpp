#include "button.h"
#include "sdl-adapter.h"

#include <cassert>

const double unpressColorCoeff = 0.7;

Button::Button(IntVec TL, IntVec BR, Vector color, std::string text) : Widget(TL, BR)
{
    this->press_color = color;
    this->unpress_color = color * unpressColorCoeff;
    this->is_pressed = 0;

    this->text = text;
}

void Button::paint()
{
    if (is_pressed) drawWidgetRect(1, press_color);
    else drawWidgetRect(1, unpress_color);

    setColor({255, 255, 255});
    putText(text, TL, BR);
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

MoveWallButton::MoveWallButton(Reactor* reactor, Vector color, int step, std::string text)
    : Button(IntVec(), IntVec(), color, text)
{
    this->reactor = reactor;
    this->step = step;
}

void MoveWallButton::action()
{
    reactor->moveWall(step);
}

TemperatureButton::TemperatureButton(Reactor* reactor, Vector color, double step, std::string text)
    : Button(IntVec(), IntVec(), color, text)
{
    this->reactor = reactor;
    this->step = step;
}

void TemperatureButton::action()
{
    reactor->increaseTemp(step);
}

AddMolButton::AddMolButton(Reactor* reactor, Vector color, int nAddMols, std::string text)
    : Button(IntVec(), IntVec(), color, text)
{
    this->reactor = reactor;
    this->nAddMols = nAddMols;
}

void AddMolButton::action()
{
    reactor->addRandomMols(nAddMols);
}
