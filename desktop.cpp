#include "desktop.h"
#include "plane.h"
#include "reactor.h"
#include "scroll.h"

IntVec stdtl(0, 0, 0), stdbr(1800, 1000, 0);
int nButtons = 6, wallStep = 10, nAddMols = 10;
double tempStep = 1;

const int r_size = 600, b_pad = 200, c_size = 200;

Desktop::Desktop() : Widget(stdtl, stdbr)
{
    setFillRect(1);

    reactor = new Reactor({b_pad, 0}, {b_pad + r_size, r_size}, this);
    // reactor->setDraggable();

    button_cont = new WContainer(this, {50, 50}, {b_pad, r_size}, nButtons, 1);
    MoveWallButton* lftButton = new MoveWallButton(button_cont, reactor, Vector(0, 0, 255), -wallStep, "Move left");
    MoveWallButton* rgtButton = new MoveWallButton(button_cont, reactor, Vector(255, 0, 255), wallStep, "Move right");
    TemperatureButton* tempUpButton = new TemperatureButton(button_cont, reactor, Vector(255, 0, 0), 
        tempStep, "Temp up");
    TemperatureButton* tempDownButton = new TemperatureButton(button_cont, reactor, Vector(255, 127, 0), 
        -tempStep, "Temp down");
    AddMolButton* addMolButton = new AddMolButton(button_cont, reactor, Vector(0, 255, 0), nAddMols, "Add mols");
    AddMolButton* removeMolButton = new AddMolButton(button_cont, reactor, Vector(255, 255), -nAddMols, "Remove mols");

    ScrollBar* scroll_bar = new ScrollBar(this, {0, 700}, {50, 900});

    energy_graph = new Graph(this, 1, {{255, 255, 255}}, 0.05, 1e3, 
        {b_pad + r_size, 0}, {b_pad + r_size * 3 / 2, r_size / 2, 0});
    cnt_graph = new Graph(this, 2, {{0, 0, 255}, {255, 0, 0}}, 1.5, 10, 
        {b_pad + r_size, r_size / 2}, {b_pad + r_size * 3 / 2, r_size, 0});

    clock = new Clock(this, {b_pad + r_size * 3 / 2, 0}, {b_pad + r_size * 3 / 2 + c_size, c_size}, 
        c_size * 2 / 5, 3.14 / 60, 1);
}

Desktop::~Desktop()
{
    //
}

void Desktop::paint()
{
    Widget::paint();
}

bool Desktop::onIdle(IdleEvent* e)
{
    energy_graph->addPoint({reactor->energy()});
    cnt_graph->addPoint(reactor->molCnt());

    return 0;
}