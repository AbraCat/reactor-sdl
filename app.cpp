#include "app.h"
#include "planeitem.h"
#include "reactor.h"

IntVec stdTL(0, 0, 0), stdBR(500, 500, 0);
int nButtons = 2;

App::App(SDL_Renderer* renderer) : Widget(renderer, stdTL, stdBR)
{
    reactor = new Reactor(renderer, {0, 0, 0}, {600, 600, 0});
    energy_graph = new PlaneItem(renderer, 1, {{255, 255, 255}}, 0.03, 1e3, {700, 10, 0}, {1000, 310, 0});
    cnt_graph = new PlaneItem(renderer, 2, {{0, 0, 255}, {255, 0, 0}}, 1, 10, {700, 320, 0}, {1000, 620, 0});
    addChild(reactor);
    addChild(energy_graph);
    addChild(cnt_graph);

    MoveWallButton* lftButton = new MoveWallButton(renderer, reactor, Vector(255, 0, 0), -10, "move left");
    MoveWallButton* rgtButton = new MoveWallButton(renderer, reactor, Vector(255, 0, 0), 10, "move right");

    button_container = new WContainer(renderer, IntVec(0, 610), IntVec(200, 710), nButtons);
    button_container->addWidget(lftButton);
    button_container->addWidget(rgtButton);
    addChild(button_container);
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