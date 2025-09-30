#ifndef MY_APP_H
#define MY_APP_H

#include <SDL3/SDL.h>

#include "widget.h"
#include "planeitem.h"
#include "reactor.h"

class App : public Widget
{
public:
    App(SDL_Renderer* renderer);
    ~App();

    void advance();

private:
    Reactor *reactor;
    PlaneItem *energy_graph, *cnt_graph;
};

#endif // MY_APP_H