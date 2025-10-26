#include "desktop.h"
#include "scroll.h"

#include <cmath>

const Vector stdtl(0, 0, 0), stdbr(1800, 1000, 0);
const double tempStep = 1, scale_reactor_amplitude = 2;
const int nButtons = 6, wallStep = 10, nAddMols = 10, r_size = 600, b_pad = 200, c_size = 200, 
    scroll_w = 100, scroll_len = 400, move_reactor_amplitude = 200;

const double h = 1.3, r = 2, src_size = 0.3, ratio = 16.0 / 9.0, cam_change = 1;

const int scene_w = 500, scene_h = scene_w / ratio, button_h = 50, obj_list_w = 150, 
    scene_scroll_w = 30, properties_w = 300, n_camera_buttons = 6, max_n_opt_objects = 2;

const int properties_left = scene_w + obj_list_w + scene_scroll_w;

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

    // WContainer* scroll_cont = new WContainer(this, {b_pad + r_size * 3 / 2, c_size}, 
    //     {b_pad + r_size * 3 / 2 + c_size,  c_size + scroll_len}, 3, 0);
    // new MoveScrollBar(scroll_cont, {}, {}, scene, move_reactor_amplitude, 1);
    // new MoveScrollBar(scroll_cont, {}, {}, scene, move_reactor_amplitude, 0);
    // new ScaleScrollBar(scroll_cont, {}, {}, scene, 2);



    WContainer* prop_cont = new WContainer(this, {properties_left, 0},
        {properties_left + properties_w, scene_h}, OPT_TOTAL, 1);

    scene = new OptScene(this, {0, 0}, {scene_w, scene_h}, prop_cont);

    SurfaceIt it1 = scene->addSphere({-1, 0, 0}, gray_col, 0.5);
    // scene->addSphere({1, -0.5, 0}, gray_col, 0.5);
    // scene->addSphere({0.3, 1, 0}, gray_col, 0.5);
    // scene->addSphere({0, 0, -12}, gray_col, 5);
    // scene->addSphere({0, 0, -2}, purple_col, 0.3);
    scene->addSource({0, -1, 4}, green_col * 0.5, src_size);

    WContainer* cam_cont = new WContainer(this, {0, scene_h}, {scene_w, scene_h + button_h}, n_camera_buttons, 0);
    new MoveCameraButton(cam_cont, scene, {0, 0, -cam_change}, gray_v, "forward");
    new MoveCameraButton(cam_cont, scene, {0, 0, cam_change}, gray_v, "back");
    new MoveCameraButton(cam_cont, scene, {-cam_change, 0, 0}, gray_v, "left");
    new MoveCameraButton(cam_cont, scene, {cam_change, 0, 0}, gray_v, "right");
    new MoveCameraButton(cam_cont, scene, {0, -cam_change, 0}, gray_v, "up");
    new MoveCameraButton(cam_cont, scene, {0, cam_change, 0}, gray_v, "down");

    WContainer* obj_cont = scene->makeObjectContainer(this, {scene_w, 0}, {scene_w + obj_list_w, scene_h});
    new ListScrollBar(this, {scene_w + obj_list_w, 0}, {scene_w + obj_list_w + scene_scroll_w, scene_h}, obj_cont);

    // InputField* inp_f = new InputField(this, {0, 0}, {100, 50}, "aboba");
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

    // static double angle = 0;
    // const double r = 2;
    // angle += 0.1;
    // source->pos = {r * std::cos(angle), r * std::sin(angle)};
    // scene->paint();

    return 0;
}