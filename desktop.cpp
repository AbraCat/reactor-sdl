#include "desktop.h"
#include "plane.h"
#include "reactor.h"
#include "scroll.h"
#include "optical-scene.h"

const Vector stdtl(0, 0, 0), stdbr(1800, 1000, 0);
const double tempStep = 1, scale_reactor_amplitude = 2;
const int nButtons = 6, wallStep = 10, nAddMols = 10, r_size = 600, b_pad = 200, c_size = 200, 
    scroll_w = 100, scroll_len = 400, move_reactor_amplitude = 200;

Desktop::Desktop() : Widget(stdtl, stdbr)
{
    setFillRect(1);
    // reactor = new Reactor({b_pad, 0}, {b_pad + r_size, r_size}, this);

    // energy_graph = new Graph(this, 1, {{255, 255, 255}}, 0.05, 1e3, 
    //     {b_pad + r_size, 0}, {b_pad + r_size * 3 / 2, r_size / 2, 0});
    // cnt_graph = new Graph(this, 2, {{0, 0, 255}, {255, 0, 0}}, 1.5, 10, 
    //     {b_pad + r_size, r_size / 2}, {b_pad + r_size * 3 / 2, r_size, 0});

    // clock = new Clock(this, {b_pad + r_size * 3 / 2, 0}, {b_pad + r_size * 3 / 2 + c_size, c_size}, 
    //     c_size * 2 / 5, 3.14 / 60, 1);

    // button_cont = new WContainer(this, {0, 0}, {b_pad, r_size}, nButtons, 1);
    // new MoveWallButton(button_cont, reactor, Vector(0, 0, 255), -wallStep, "Move left");
    // new MoveWallButton(button_cont, reactor, Vector(255, 0, 255), wallStep, "Move right");
    // new TemperatureButton(button_cont, reactor, Vector(255, 0, 0), tempStep, "Temp up");
    // new TemperatureButton(button_cont, reactor, Vector(255, 127, 0), -tempStep, "Temp down");
    // new AddMolButton(button_cont, reactor, Vector(0, 255, 0), nAddMols, "Add mols");
    // new AddMolButton(button_cont, reactor, Vector(255, 255), -nAddMols, "Remove mols");

    OptScene* scene = new OptScene(this, {0, 0}, {1000, 1000});
    scene->addSphere({0, 0, 0}, {0.6, 0.3, 0.9}, 100);
    scene->addSource({100, 0, 100}, {0.9, 0.9, 0.3});

    WContainer* scroll_cont = new WContainer(this, {b_pad + r_size * 3 / 2, c_size}, 
        {b_pad + r_size * 3 / 2 + c_size,  c_size + scroll_len}, 3, 0);
    new MoveScrollBar(scroll_cont, {}, {}, scene, move_reactor_amplitude, 1);
    new MoveScrollBar(scroll_cont, {}, {}, scene, move_reactor_amplitude, 0);
    new ScaleScrollBar(scroll_cont, {}, {}, scene, 2);
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
    // energy_graph->addPoint({reactor->energy()});
    // cnt_graph->addPoint(reactor->molCnt());

    return 0;
}