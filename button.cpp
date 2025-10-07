#include "button.h"
#include "sdl-adapter.h"

#include <cassert>

const double unpressColorCoeff = 0.7;

Button::Button(Widget* parent, IntVec tl, IntVec br, Vector color, std::string text) : Widget(tl, br, parent)
{
    setTextureBorderVisible(1);
    
    this->press_color = color;
    this->unpress_color = color * unpressColorCoeff;
    this->is_pressed = 0;

    this->text = text;
}

void Button::paint()
{
    Widget::paint();
    if (is_pressed) drawWidgetRect(1, press_color);
    else drawWidgetRect(1, unpress_color);
    
    t->addText(text);
}

bool Button::mousePressEvent(MouseEvent* e)
{
    Widget::mousePressEvent(e);

    if (!inAbsRect(IntVec(e->x, e->y))) return 0;
    if (is_pressed) return 0;

    is_pressed = 1;
    paint();
    action();
    return 1;
}

bool Button::mouseReleaseEvent(MouseEvent* e)
{
    Widget::mouseReleaseEvent(e);

    is_pressed = 0;
    paint();
    return 0;
}

void Button::unpress()
{
    is_pressed = 0;
}




MoveWallButton::MoveWallButton(Widget* parent, Reactor* reactor, Vector color, int step, std::string text)
    : Button(parent, IntVec(), IntVec(), color, text)
{
    this->reactor = reactor;
    this->step = step;
}

void MoveWallButton::action()
{
    reactor->moveWall(step);
}

TemperatureButton::TemperatureButton(Widget* parent, Reactor* reactor, Vector color, double step, std::string text)
    : Button(parent, IntVec(), IntVec(), color, text)
{
    this->reactor = reactor;
    this->step = step;
}

void TemperatureButton::action()
{
    reactor->increaseTemp(step);
}

AddMolButton::AddMolButton(Widget* parent, Reactor* reactor, Vector color, int nAddMols, std::string text)
    : Button(parent, IntVec(), IntVec(), color, text)
{
    this->reactor = reactor;
    this->nAddMols = nAddMols;
}

void AddMolButton::action()
{
    reactor->addRandomMols(nAddMols);
}
