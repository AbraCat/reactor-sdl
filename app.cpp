#include "app.h"
#include "planeitem.h"
#include "reactor.h"

IntVec stdTL(0, 0, 0), stdBR(500, 500, 0);

App::App(SDL_Renderer* renderer) : Widget(renderer, stdTL, stdBR)
{
    reactor = new Reactor(renderer, {0, 0, 0}, {600, 600, 0});
    energy_graph = new PlaneItem(renderer, 1, {{255, 255, 255}}, 0.03, 1e3, {700, 10, 0}, {1000, 310, 0});
    cnt_graph = new PlaneItem(renderer, 2, {{0, 0, 255}, {255, 0, 0}}, 1, 10, {700, 320, 0}, {1000, 620, 0});

    addChild(reactor);
    addChild(energy_graph);
    addChild(cnt_graph);
}

App::~App()
{
    //
}

void App::advance()
{
    reactor->advance();

    energy_graph->addPoint({reactor->energy()});
    cnt_graph->addPoint(reactor->molCnt());

    this->put();
}