#ifndef MY_BUTTON_H
#define MY_BUTTON_H

#include "widget.h"
#include "reactor.h"

#include <cstdio>
#include <string>

class TextField : public Widget
{
public:
    TextField(Widget* parent, Vector tl, Vector br, std::string text, Vector color = blackV);
    virtual void updateTexture() override;

    void SetFieldColor(Vector color);
    void SetText(std::string text);
    std::string getText();

private:
    Vector color;
    std::string text;
};

class InputField : public TextField
{
public:
    InputField(Widget* parent, Vector tl, Vector br, std::string text);

    virtual bool mousePressEvent(MouseEvent* e) override;
    virtual bool keyboardEvent(KeyboardEvent* evt) override;

    void update_text();
    virtual void action() = 0;

    bool focused;
    std::string init_text;
};

class Button : public TextField
{
public:
    Button(Widget* parent, Vector tl, Vector br, Vector color, std::string text);

    virtual bool mousePressEvent(MouseEvent* e) override;
    virtual bool mouseReleaseEvent(MouseEvent* e) override;

    virtual void action() = 0;
    virtual void unpress();

    bool is_pressed;
    Vector press_color, unpress_color;
    std::string text;
};

class ToggleButton : public Button
{
public:
    ToggleButton(Widget* parent, Vector tl, Vector br, Vector color, std::string text);

    virtual bool mousePressEvent(MouseEvent* e) override;
    virtual bool mouseReleaseEvent(MouseEvent* e) override;

    virtual void deactivate() = 0;
};




class MoveWallButton : public Button
{
public:
    MoveWallButton(Widget* parent, Reactor* reactor, Vector color, int step, std::string text);
    virtual void action() override;

private:
    Reactor* reactor;
    int step;
};

class TemperatureButton : public Button
{
public:
    TemperatureButton(Widget* parent, Reactor* reactor, Vector color, double step, std::string text);
    virtual void action() override;

private:
    Reactor* reactor;
    double step;
};

class AddMolButton : public Button
{
public:
    AddMolButton(Widget* parent, Reactor* reactor, Vector color, int nAddMols, std::string text);
    virtual void action() override;

private:
    Reactor* reactor;
    int nAddMols;
};

#endif // MY_BUTTON_H