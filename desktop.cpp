#include "desktop.h"
#include "plane.h"
#include "reactor.h"

IntVec stdTL(0, 0, 0), stdBR(500, 500, 0);
int nButtons = 6, wallStep = 10, nAddMols = 10;
double tempStep = 1;

Desktop::Desktop(SDL_Renderer* renderer) : Widget(renderer, stdTL, stdBR)
{
    reactor = new Reactor(renderer, {0, 0, 0}, {600, 600, 0});
    addChild(reactor);

    energy_graph = new Graph(renderer, 1, {{255, 255, 255}}, 0.03, 1e3, {700, 10, 0}, {1000, 310, 0});
    cnt_graph = new Graph(renderer, 2, {{0, 0, 255}, {255, 0, 0}}, 1, 10, {700, 320, 0}, {1000, 620, 0});
    addChild(energy_graph);
    addChild(cnt_graph);

    // Plane* plane = new Plane(renderer, {700, 700}, {1000, 1000}, {850, 850}, 10, 1, 10, 1);
    // plane->addPoint({5, 2}, {255, 0, 0});
    // addChild(plane);

    clock = new Clock(renderer, {700, 700}, {1000, 1000}, 100, 3.14 / 60, 1);
    addChild(clock);

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

Desktop::~Desktop()
{
    //
}

bool Desktop::onIdle(IdleEvent* e)
{
    energy_graph->addPoint({reactor->energy()});
    cnt_graph->addPoint(reactor->molCnt());

    return 0;
}