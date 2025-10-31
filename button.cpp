#include "button.h"
#include "sdl-adapter.h"

#include <cassert>

const double unpressColorCoeff = 0.7;
const int key_enter = 13;

TextField::TextField(hui::State *state, dr4::Vec2f pos, dr4::Vec2f size)
    : Widget(state, pos, size), color(color), text(text)
{
    // setTextureBorderVisible(1);
}

void TextField::Redraw()
{
    // Widget::Redraw();
    // drawWidgetRect(1, color);
    
    // t->addText(text);

    //
}

void TextField::SetFieldColor(Vector color) { this->color = color; Redraw(); }
void TextField::SetText(std::string text) { this->text = text; Redraw(); }
std::string TextField::getText() { return text; }

#if 0

InputField::InputField(Widget* parent, Vector tl, Vector br, std::string text)
    : TextField(parent, tl, br, text)
{
    focused = 0;
    this->text_valid = std::function([](std::string s){ return true; });
}

bool InputField::mousePressEvent(MouseEvent* e)
{
    bool in_abs_rect = inAbsRect({e->x, e->y});

    if (in_abs_rect && !focused)
    {
        state->focused = this;
        focused = true;

        SetFieldColor(gray_v);
        init_text = getText();
        return 0;
    }

    if (!in_abs_rect && focused)
        update_text();

    return 0;
}

bool InputField::keyboardEvent(KeyboardEvent* evt)
{
    if (evt->key == key_enter)
    {
        update_text();
        return 1;
    }

    std::string cur_text = getText();
    if (evt->key == SDLK_BACKSPACE)
        SetText(cur_text.substr(0, cur_text.size() - 1));
    else
        SetText(getText() + std::string(1, evt->key));

    return 1;
}

void InputField::update_text()
{
    SetFieldColor(blackV);
    focused = 0;
    if (state->focused == this) state->focused = nullptr;

    std::string new_text = getText();
    if (getText() != init_text)
    {
        if (text_valid(new_text))
        {
            action();
        }
        else
        {
            SetText(init_text);
        }

    }
}

void InputField::setValidator(std::function<bool(std::string)> text_valid)
{
    this->text_valid = text_valid;
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
    updateTexture();
    action();
    return 0;
}

bool Button::mouseReleaseEvent(MouseEvent* e)
{
    Widget::mouseReleaseEvent(e);

    is_pressed = 0;
    SetFieldColor(unpress_color);
    updateTexture();
    return 0;
}

void Button::unpress()
{
    is_pressed = 0;
}



ToggleButton::ToggleButton(Widget* parent, Vector tl, Vector br, Vector color, std::string text)
    : Button(parent, tl, br, color, text)
{
    //
}

bool ToggleButton::mousePressEvent(MouseEvent* e)
{
    Widget::mousePressEvent(e);

    if (!inAbsRect({e->x, e->y})) return 0;

    is_pressed = !is_pressed;
    SetFieldColor(is_pressed ? press_color : unpress_color);

    if (is_pressed) action();
    else deactivate();
    return 0;
}

bool ToggleButton::mouseReleaseEvent(MouseEvent* e)
{
    Widget::mouseReleaseEvent(e);
    return 0;
}
#endif // 0