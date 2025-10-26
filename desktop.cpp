#include "desktop.h"
#include "scroll.h"

#include <cmath>

const Vector stdtl(0, 0, 0), stdbr(1800, 1000, 0);
const double tempStep = 1, scale_reactor_amplitude = 2;
const int nButtons = 6, wallStep = 10, nAddMols = 10, r_size = 600, b_pad = 200, c_size = 200, 
    scroll_w = 100, scroll_len = 400, move_reactor_amplitude = 200;

const double h = 1.3, r = 2, src_size = 0.3, ratio = 16.0 / 9.0,
    cam_change_x = 0.5, cam_change_y = 0.5, cam_change_z = 1;

const int scene_w = 1000, scene_h = scene_w / ratio, button_h = 50, obj_list_w = 150, 
    scene_scroll_w = 30, properties_w = 600, properties_left = scene_w + obj_list_w + scene_scroll_w,
    n_camera_buttons = 6, max_n_opt_objects = 2;

Desktop::Desktop() : Widget(stdtl, stdbr)
{
    setFillRect(1);

    ObjControlPanel* panel = new ObjControlPanel(this, {properties_left, 0},
        {properties_left + properties_w, scene_h + button_h}, scene_h);

    scene = new OptScene(this, {0, 0}, {scene_w, scene_h}, panel);

    SurfaceIt it1 = scene->addSphere({-1, 0, 0}, gray_col, 0.5);
    scene->addSphere({1, -0.5, 0}, gray_col, 0.5);
    scene->addSphere({0.3, 1, 0}, gray_col, 0.5);
    scene->addSphere({0, 0, -12}, gray_col, 5);
    scene->addSphere({0, 0, -2}, purple_col, 0.3);
    scene->addSource({0, -1, 4}, green_col * 0.5, src_size);

    scene->addSphere({0, 0, 3}, white_col, 1, glass);

    WContainer* cam_cont = new WContainer(this, {0, scene_h}, {scene_w, scene_h + button_h}, n_camera_buttons, 0);
    new MoveCameraButton(cam_cont, scene, {0, 0, -cam_change_z}, gray_v, "forward");
    new MoveCameraButton(cam_cont, scene, {0, 0, cam_change_z}, gray_v, "back");
    new MoveCameraButton(cam_cont, scene, {-cam_change_x, 0, 0}, gray_v, "left");
    new MoveCameraButton(cam_cont, scene, {cam_change_x, 0, 0}, gray_v, "right");
    new MoveCameraButton(cam_cont, scene, {0, -cam_change_y, 0}, gray_v, "up");
    new MoveCameraButton(cam_cont, scene, {0, cam_change_y, 0}, gray_v, "down");

    WContainer* obj_cont = scene->makeObjectContainer(this, {scene_w, 0}, {scene_w + obj_list_w, scene_h});
    new ListScrollBar(this, {scene_w + obj_list_w, 0}, {scene_w + obj_list_w + scene_scroll_w, scene_h}, obj_cont);

    // new ToggleButton(this, {0, 700}, {50, 750}, gray_v, "aboba");
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
    return 0;
}