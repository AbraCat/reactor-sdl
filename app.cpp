#include "app.h"
#include "planeitem.h"
#include "reactor.h"

IntVec stdTL(0, 0, 0), stdBR(500, 500, 0);
int nButtons = 6, wallStep = 10, nAddMols = 10;
double tempStep = 1;

App::App(SDL_Renderer* renderer) : Widget(renderer, stdTL, stdBR)
{
    reactor = new Reactor(renderer, {0, 0, 0}, {600, 600, 0});
    energy_graph = new PlaneItem(renderer, 1, {{255, 255, 255}}, 0.03, 1e3, {700, 10, 0}, {1000, 310, 0});
    cnt_graph = new PlaneItem(renderer, 2, {{0, 0, 255}, {255, 0, 0}}, 1, 10, {700, 320, 0}, {1000, 620, 0});
    addChild(reactor);
    addChild(energy_graph);
    addChild(cnt_graph);

    MoveWallButton* lftButton = new MoveWallButton(renderer, reactor, Vector(0, 0, 255), -wallStep, "move left");
    MoveWallButton* rgtButton = new MoveWallButton(renderer, reactor, Vector(255, 0, 255), wallStep, "move right");
    TemperatureButton* tempUpButton = new TemperatureButton(renderer, reactor, Vector(255, 0, 0), 
        tempStep, "temp up");
    TemperatureButton* tempDownButton = new TemperatureButton(renderer, reactor, Vector(255, 127, 0), 
        -tempStep, "temp down");
    AddMolButton* addMolButton = new AddMolButton(renderer, reactor, Vector(0, 255, 0), nAddMols, "add mols");
    AddMolButton* removeMolButton = new AddMolButton(renderer, reactor, Vector(255, 255), -nAddMols, "remove mols");

    button_container = new WContainer(renderer, IntVec(0, 610), IntVec(600, 710), nButtons);
    button_container->addWidget(lftButton);
    button_container->addWidget(rgtButton);
    button_container->addWidget(tempUpButton);
    button_container->addWidget(tempDownButton);
    button_container->addWidget(addMolButton);
    button_container->addWidget(removeMolButton);
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