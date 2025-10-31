#include "desktop.h"

#include <cmath>

const Vector stdtl(0, 0, 0), stdbr(1800, 1000, 0);
const double tempStep = 1, scale_reactor_amplitude = 2;
const int nButtons = 6, wallStep = 10, nAddMols = 10, r_size = 600, b_pad = 200, c_size = 200, 
    scroll_w = 100, scroll_len = 400, move_reactor_amplitude = 200;


const double src_size = 0.3;

const int scene_w = 1000;

Desktop::Desktop(hui::State* state, dr4::Vec2f size) : hui::MyContainerWidget(state, dr4::Vec2f(0, 0), size)
{
    // setFillRect(1);
    // control = new OptController(this);

    // control->addSource({0, -1, 4}, green_col * 0.5, src_size);

    // control->addSphere({-1, 0, 0}, gray_col, 0.5);
    // control->addSphere({1, -0.5, 0}, gray_col, 0.5);
    // control->addSphere({0.3, 1, 0}, gray_col, 0.5);
    // control->addSphere({0, 0, -12}, gray_col, 5);
    // control->addSphere({0, 0, -2}, purple_col, 0.3);
    // control->addSphere({0, 0, 3}, white_col, 1, glass);


    OptScene* scene = new OptScene(state, nullptr, dr4::Vec2f(100, 50), dr4::Vec2f(scene_w, scene_w / ratio));
    addChild(scene);
}

Desktop::~Desktop()
{
    //
}

void Desktop::Redraw()
{
    //
}

// void Desktop::updateTexture()
// {
//     Widget::updateTexture();
// }

// bool Desktop::onIdle(IdleEvent* e)
// {
//     return 0;
// }