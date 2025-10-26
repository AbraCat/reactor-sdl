#include "button.h"
#include "sdl-adapter.h"

#include <cassert>

const double unpressColorCoeff = 0.7;
const int key_enter = 13;

TextField::TextField(Widget* parent, Vector tl, Vector br, std::string text, Vector color)
    : Widget(tl, br, parent), color(color), text(text)
{
    setTextureBorderVisible(1);
}

void TextField::paint()
{
    Widget::paint();
    drawWidgetRect(1, color);
    
    t->addText(text);
}

void TextField::SetFieldColor(Vector color) { this->color = color; paint(); }
void TextField::SetText(std::string text) { this->text = text; paint(); }
std::string TextField::getText() { return text; }

InputField::InputField(Widget* parent, Vector tl, Vector br, std::string text)
    : TextField(parent, tl, br, text)
{
    //
}

bool InputField::mousePressEvent(MouseEvent* e)
{
    if (inAbsRect({e->x, e->y}))
    {
        state->focused = this;
        SetFieldColor(gray_v);
        return 1;
    }

    if (state->focused == this)
    {
        action();
        return 1;
    }

    return 0;
}

bool InputField::keyboardEvent(KeyboardEvent* evt)
{
    if (evt->key == key_enter)
    {
        action();
        return 1;
    }

    std::string cur_text = getText();
    if (evt->key == SDLK_BACKSPACE)
        SetText(cur_text.substr(0, cur_text.size() - 1));
    else
        SetText(getText() + std::string(1, evt->key));

    return 1;
}

void InputField::action()
{
    state->focused = nullptr;
    SetFieldColor(blackV);
}

Button::Button(Widget* parent, Vector tl, Vector br, Vector color, std::string text)
    : TextField(parent, tl, br, text, color * unpressColorCoeff)
{
    this->press_color = color;
    this->unpress_color = color * unpressColorCoeff;
    this->is_pressed = 0;

    this->text = text;
}

bool Button::mousePressEvent(MouseEvent* e)
{
    Widget::mousePressEvent(e);

    if (!inAbsRect(Vector(e->x, e->y))) return 0;
    if (is_pressed) return 0;

    is_pressed = 1;
    SetFieldColor(press_color);
    paint();
    action();
    return 1;
}

bool Button::mouseReleaseEvent(MouseEvent* e)
{
    Widget::mouseReleaseEvent(e);

    is_pressed = 0;
    SetFieldColor(unpress_color);
    paint();
    return 0;
}

void Button::unpress()
{
    is_pressed = 0;
}




MoveWallButton::MoveWallButton(Widget* parent, Reactor* reactor, Vector color, int step, std::string text)
    : Button(parent, Vector(), Vector(), color, text)
{
    this->reactor = reactor;
    this->step = step;
}

void MoveWallButton::action()
{
    reactor->moveWall(step);
}

TemperatureButton::TemperatureButton(Widget* parent, Reactor* reactor, Vector color, double step, std::string text)
    : Button(parent, Vector(), Vector(), color, text)
{
    this->reactor = reactor;
    this->step = step;
}

void TemperatureButton::action()
{
    reactor->increaseTemp(step);
}

AddMolButton::AddMolButton(Widget* parent, Reactor* reactor, Vector color, int nAddMols, std::string text)
    : Button(parent, Vector(), Vector(), color, text)
{
    this->reactor = reactor;
    this->nAddMols = nAddMols;
}

void AddMolButton::action()
{
    reactor->addRandomMols(nAddMols);
}
