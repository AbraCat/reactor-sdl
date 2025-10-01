#ifndef MY_BUTTON_H
#define MY_BUTTON_H

#include "widget.h"
#include "reactor.h"

#include <SDL3/SDL.h>

#include <cstdio>
#include <string>

class Button : public Widget
{
public:
    Button(SDL_Renderer* renderer, IntVec TL, IntVec BR, Vector color, std::string text);
    virtual void paint() override;

    virtual bool mousePressEvent(MouseEvent* e) override;
    virtual bool mouseReleaseEvent(MouseEvent* e) override;

    virtual void action() = 0;
    void unpress();

    bool is_pressed;
    Vector press_color, unpress_color;
    std::string text;
};

class MoveWallButton : public Button
{
public:
    MoveWallButton(SDL_Renderer* renderer, Reactor* reactor, Vector color, int step, std::string text);
    virtual void action() override;

private:
    Reactor* reactor;
    int step;
};

class TemperatureButton : public Button
{
public:
    TemperatureButton(SDL_Renderer* renderer, Reactor* reactor, Vector color, double step, std::string text);
    virtual void action() override;

private:
    Reactor* reactor;
    double step;
};

class AddMolButton : public Button
{
public:
    AddMolButton(SDL_Renderer* renderer, Reactor* reactor, Vector color, int nAddMols, std::string text);
    virtual void action() override;

private:
    Reactor* reactor;
    int nAddMols;
};

#endif // MY_BUTTON_H