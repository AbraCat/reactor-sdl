#ifndef MY_BUTTON_H
#define MY_BUTTON_H

#include "widget.h"
#include "reactor.h"

#include "widget.hpp"

#include <cstdio>
#include <string>

class TextField : public hui::Widget
{
public:
    TextField(hui::State *state, dr4::Vec2f pos, dr4::Vec2f size);
    // virtual void updateTexture() override;
    virtual void Redraw() override;

    void SetFieldColor(Vector color);
    void SetText(std::string text);
    std::string getText();

private:
    Vector color;
    std::string text;
};

// class InputField : public TextField
// {
// public:
//     InputField(Widget* parent, Vector tl, Vector br, std::string text);

//     virtual bool mousePressEvent(MouseEvent* e) override;
//     virtual bool keyboardEvent(KeyboardEvent* evt) override;

//     void update_text();
//     virtual void action() = 0;

//     void setValidator(std::function<bool(std::string)> text_valid);

//     bool focused;
//     std::string init_text;
//     std::function<bool(std::string)> text_valid;
// };

// class Button : public TextField
// {
// public:
//     Button(Widget* parent, Vector tl, Vector br, Vector color, std::string text);

//     virtual bool mousePressEvent(MouseEvent* e) override;
//     virtual bool mouseReleaseEvent(MouseEvent* e) override;

//     virtual void action() = 0;
//     virtual void unpress();

//     bool is_pressed;
//     Vector press_color, unpress_color;
//     std::string text;
// };

// class ToggleButton : public Button
// {
// public:
//     ToggleButton(Widget* parent, Vector tl, Vector br, Vector color, std::string text);

//     virtual bool mousePressEvent(MouseEvent* e) override;
//     virtual bool mouseReleaseEvent(MouseEvent* e) override;

//     virtual void deactivate() = 0;
// };

#endif // MY_BUTTON_H