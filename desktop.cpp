#include "desktop.h"
#include "plane.h"
#include "reactor.h"
#include "scroll.h"

IntVec stdtl(0, 0, 0), stdbr(1000, 1800, 0);
int nButtons = 6, wallStep = 10, nAddMols = 10;
double tempStep = 1;

const int r_size = 600, b_pad = 200, c_size = 200;

Desktop::Desktop() : Widget(stdtl, stdbr)
{
    reactor = new Reactor({b_pad, 0}, {b_pad + r_size, r_size, 0});
    addChild(reactor);

    energy_graph = new Graph(1, {{255, 255, 255}}, 0.05, 1e3, 
        {b_pad + r_size, 0}, {b_pad + r_size * 3 / 2, r_size / 2, 0});
    cnt_graph = new Graph(2, {{0, 0, 255}, {255, 0, 0}}, 1.5, 10, 
        {b_pad + r_size, r_size / 2}, {b_pad + r_size * 3 / 2, r_size, 0});
    addChild(energy_graph);
    addChild(cnt_graph);

    clock = new Clock({b_pad + r_size * 3 / 2, 0}, {b_pad + r_size * 3 / 2 + c_size, c_size}, 
        c_size * 2 / 5, 3.14 / 60, 1);
    addChild(clock);

    MoveWallButton* lftButton = new MoveWallButton(reactor, Vector(0, 0, 255), -wallStep, "Move left");
    MoveWallButton* rgtButton = new MoveWallButton(reactor, Vector(255, 0, 255), wallStep, "Move right");
    TemperatureButton* tempUpButton = new TemperatureButton(reactor, Vector(255, 0, 0), 
        tempStep, "Temp up");
    TemperatureButton* tempDownButton = new TemperatureButton(reactor, Vector(255, 127, 0), 
        -tempStep, "Temp down");
    AddMolButton* addMolButton = new AddMolButton(reactor, Vector(0, 255, 0), nAddMols, "Add mols");
    AddMolButton* removeMolButton = new AddMolButton(reactor, Vector(255, 255), -nAddMols, "Remove mols");

    button_container = new WContainer({0, 0}, {b_pad, r_size}, nButtons, 1);
    button_container->addWidget(lftButton);
    button_container->addWidget(rgtButton);
    button_container->addWidget(tempUpButton);
    button_container->addWidget(tempDownButton);
    button_container->addWidget(addMolButton);
    button_container->addWidget(removeMolButton);
    addChild(button_container);

    ScrollBar* scroll_bar = new ScrollBar({0, 700}, {50, 900});
    addChild(scroll_bar);
}

Desktop::~Desktop()
{
    //
}

void Desktop::paint()
{
    drawWidgetRect(1);
}

bool Desktop::onIdle(IdleEvent* e)
{
    energy_graph->addPoint({reactor->energy()});
    cnt_graph->addPoint(reactor->molCnt());

    return 0;
}