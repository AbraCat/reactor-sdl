#include "optical-control.h"


#include "optical-scene.h"

#include <algorithm>
#include <random>
#include <sstream>

#include <limits>
#include <cmath>
#include <cassert>

const Vector sky_col = {0, 0.5, 0.75}, init_V = {0, 0, 10}, init_screen_tl = {-2, -1.15, 4};

const double cam_change_x = 0.5, cam_change_y = 0.5, cam_change_z = 1, obj_change = 1;

const int scene_w = 1200, scene_h = scene_w / ratio, button_h = 50, obj_button_h = 120, 
    obj_list_w = 150, properties_h = 500, obj_scroll_w = 50, properties_w = 400,
    properties_left = scene_w + obj_list_w + obj_scroll_w,
    n_camera_buttons = 6, n_move_buttons = 6, max_n_objects = 10;


std::string doubleToStr(double val)
{
    std::ostringstream out;
    out.precision(2);
    out << std::fixed << val;
    return std::move(out).str();
}

ObjControlPanel::ObjControlPanel(Widget* parent, Vector tl, Vector br)
    : Widget(tl, br, parent)
{
    prop_cont = new WContainer(this, {0, 0}, {width, properties_h}, OPT_TOTAL, 1);
    button_cont = new WContainer(this, {0, properties_h}, wh, n_move_buttons + 1, 1);
}

void ObjControlPanel::setObject(OptObject* obj)
{
    prop_cont->clearChildren();
    button_cont->clearChildren();

    if (obj != nullptr)
    {
        for (OptProperty prop: obj->getProperties())
            new OptPropWidget(prop_cont, {}, {}, obj, prop);

        new MoveObjectButton(button_cont, {}, {}, obj, {0, 0, -obj_change}, "forward", this);
        new MoveObjectButton(button_cont, {}, {}, obj, {0, 0, obj_change}, "back", this);
        new MoveObjectButton(button_cont, {}, {}, obj, {-obj_change, 0, 0}, "left", this);
        new MoveObjectButton(button_cont, {}, {}, obj, {obj_change, 0, 0}, "right", this);
        new MoveObjectButton(button_cont, {}, {}, obj, {0, -obj_change, 0}, "up", this);
        new MoveObjectButton(button_cont, {}, {}, obj, {0, obj_change, 0}, "down", this);

        new DeleteObjectButton(button_cont, {}, {}, obj, "delete");
    }

    prop_cont->drawRec();
    button_cont->drawRec();
}

void ObjControlPanel::setDisplayedVal(OptPropEnum prop, double val)
{
    for (Widget* w: prop_cont->children)
    {
        OptPropWidget* prop_w = dynamic_cast<OptPropWidget*>(w);
        assert(prop_w != nullptr);

        if (prop_w->val_field->prop.prop == prop)
        {
            prop_w->val_field->SetText(doubleToStr(val));
            break;
        }
    }
}



OptPropWidget::OptPropWidget(Widget* parent, Vector tl, Vector br, OptObject* obj, OptProperty prop)
    : Widget(tl, br, parent)
{
    name_field = new TextField(this, {0, 0}, {width / 2, height}, prop.getName());
    val_field = new OptPropField(this, {width / 2, 0}, {width, height}, obj, prop);
}

OptPropField::OptPropField(OptPropWidget* parent, Vector tl, Vector br, OptObject* obj, OptProperty prop)
    : InputField(parent, tl, br, ""), obj(obj), prop(prop)
{
    SetText(doubleToStr(prop.val));
}

void OptPropField::action()
{
    obj->setProperty(prop.prop, std::stod(getText()));
    obj->scene->paint();
}



OptObjectButton::OptObjectButton(Widget* parent, Vector tl, Vector br, OptObject* obj, ObjControlPanel* panel)
    : ToggleButton(parent, tl, br, gray_v, obj->name), obj(obj)
{
    //
}

void OptObjectButton::action() { obj->scene->control->select(obj); }
void OptObjectButton::deactivate() { obj->scene->control->deselect(obj); }


MoveObjectButton::MoveObjectButton(Widget* parent, Vector tl, Vector br, OptObject* obj,
    Vector change, std::string text, ObjControlPanel* panel)
    : Button(parent, tl, br, gray_v, text), obj(obj), change(change), panel(panel)
{
    //
}

void MoveObjectButton::action()
{
    obj->movePos(change);

    panel->setDisplayedVal(OPT_POS_X, obj->pos.x);
    panel->setDisplayedVal(OPT_POS_Y, obj->pos.y);
    panel->setDisplayedVal(OPT_POS_Z, obj->pos.z);

    obj->scene->paint();
}

DeleteObjectButton::DeleteObjectButton(Widget* parent, Vector tl, Vector br, OptObject* obj, std::string text)
    : Button(parent, tl, br, red_v, text), obj(obj)
{
    //
}

void DeleteObjectButton::action()
{
    obj->scene->control->deleteObject(obj);
}

MoveCameraButton::MoveCameraButton(Widget* parent, OptScene* scene, Vector change, Vector color, std::string text)
    : Button(parent, Vector(), Vector(), color, text), scene(scene), change(change)
{
    
}

void MoveCameraButton::action()
{
    scene->moveCamera(change);
}




OptController::OptController(Widget* parent) : parent(parent)
{
    s = new OptScene(parent, {0, 0}, {scene_w, scene_h}, this);

    panel = new ObjControlPanel(parent, {properties_left, 0},
        {properties_left + properties_w, scene_h + button_h});

    cam_cont = new WContainer(parent, {0, scene_h}, {scene_w, scene_h + button_h}, n_camera_buttons, 0);
    new MoveCameraButton(cam_cont, s, {0, 0, -cam_change_z}, gray_v, "forward");
    new MoveCameraButton(cam_cont, s, {0, 0, cam_change_z}, gray_v, "back");
    new MoveCameraButton(cam_cont, s, {-cam_change_x, 0, 0}, gray_v, "left");
    new MoveCameraButton(cam_cont, s, {cam_change_x, 0, 0}, gray_v, "right");
    new MoveCameraButton(cam_cont, s, {0, -cam_change_y, 0}, gray_v, "up");
    new MoveCameraButton(cam_cont, s, {0, cam_change_y, 0}, gray_v, "down");

    obj_cont = makeObjectContainer({scene_w, 0}, {scene_w + obj_list_w, scene_h + button_h});
    obj_scroll = new ListScrollBar(parent, {scene_w + obj_list_w, 0},
        {scene_w + obj_list_w + obj_scroll_w, scene_h + button_h}, obj_cont);
}

WList* OptController::makeObjectContainer(Vector tl, Vector br)
{
    int n_objects = s->surfaces.size() + s->sources.size();

    this->obj_cont = new WList(parent, tl, br, 1, obj_button_h);
    
    for (OptObject* obj: s->surfaces)
        new OptObjectButton(obj_cont, {}, {}, obj, panel);
    for (OptObject* obj: s->sources)
        new OptObjectButton(obj_cont, {}, {}, obj, panel);
    return obj_cont;

    return nullptr;
}

void OptController::select(OptObject* obj)
{
    s->selected.insert(obj);
    selected_changed();
}

void OptController::deselect(OptObject* obj)
{
    s->selected.erase(obj);
    selected_changed();
}

void OptController::selected_changed()
{
    if (s->selected.size() == 1)
        panel->setObject(*(s->selected).begin());
    else
        panel->setObject(nullptr);

    s->redraw_picture = 0;
    s->paint();
}

void OptController::deleteObject(OptObject* obj)
{
    for (std::vector<Surface*>::iterator it = s->surfaces.begin(); it != s->surfaces.end(); ++it)
    {
        if (*it == obj)
        {
            s->surfaces.erase(it);
            break;
        }
    }

    for (std::vector<Source*>::iterator it = s->sources.begin(); it != s->sources.end(); ++it)
    {
        if (*it == obj)
        {
            s->sources.erase(it);
            break;
        }
    }

    s->selected.erase(obj);
    panel->setObject(nullptr);

    obj_cont->removeChildByPredicate([obj](Widget* w){
        OptObjectButton* button = dynamic_cast<OptObjectButton*>(w);
        assert(button != nullptr);
        return button->obj == obj;
    });
    obj_scroll->moveThumb(0);

    delete obj;
    obj_cont->drawRec();
    s->paint();
}

void OptController::addObject(OptObject* obj)
{
    new OptObjectButton(obj_cont, {}, {}, obj, panel);
}

std::vector<Surface*>::iterator OptController::addSphere(Vector pos, Vector color, double r, Material m)
{
    std::string name = "sphere " + std::to_string(s->surfaces.size());
    SphereSurface* new_sphere = new SphereSurface(pos, r, color, name, s, m);

    s->surfaces.push_back(new_sphere);
    addObject(new_sphere);
    return s->surfaces.end() - 1;
}

std::vector<Source*>::iterator OptController::addSource(Vector pos, Vector color, double r)
{
    std::string name = "source " + std::to_string(s->sources.size());
    Source* new_source = new SphereSource(color, pos, r, name, s);

    s->sources.push_back(new_source);
    addObject(new_source);
    return s->sources.end() - 1;
}