#ifndef OPTICAL_CONTROL_H
#define OPTICAL_CONTROL_H

#include "optical-scene.h"

class ObjControlPanel;
class OptPropWidget;

class OptPropField : public InputField
{
public:
    OptPropField(OptPropWidget* parent, Vector tl, Vector br, OptObject* obj, OptProperty prop);
    virtual void action() override;

private:
    OptObject* obj;
    OptProperty prop;
};

class OptPropWidget : public Widget
{
public:
    OptPropWidget(Widget* parent, Vector tl, Vector br, OptObject* obj, OptProperty prop);

// private:
    TextField *name_field;
    OptPropField *val_field;
};

class OptObjectButton : public ToggleButton
{
public:
    OptObjectButton(Widget* parent, Vector tl, Vector br, OptObject* obj, ObjControlPanel* panel);
    virtual void action() override;
    virtual void deactivate() override;

// private:
    OptObject* obj;
};

class MoveObjectButton : public Button
{
public:
    MoveObjectButton(Widget* parent, Vector tl, Vector br, OptObject* obj, Vector change, std::string text);
    virtual void action() override;

private:
    OptObject* obj;
    Vector change;
};

class DeleteObjectButton : public Button
{
public:
    DeleteObjectButton(Widget* parent, Vector tl, Vector br, OptObject* obj, std::string text);
    virtual void action() override;

private:
    OptObject* obj;
};

class ObjControlPanel : public Widget
{
public:
    ObjControlPanel(Widget* parent, Vector tl, Vector br);
    void setObject(OptObject* obj);

private:
    OptObject *obj;
    WContainer *prop_cont, *button_cont;
};



class OptController
{
public:
    OptController(Widget* parent);
    WList* makeObjectContainer(Vector tl, Vector br);

    void addObject(OptObject* obj);
    std::vector<Surface*>::iterator addSphere(Vector pos, Vector color, double r, Material m = plastic);
    std::vector<Source*>::iterator addSource(Vector pos, Vector color, double r);
    
    void select(OptObject* obj);
    void deselect(OptObject* obj);
    void selected_changed();

    void deleteObject(OptObject* obj);

    Widget* parent;

    OptScene* s;
    WContainer *cam_cont;
    WList *obj_cont;
    ListScrollBar *obj_scroll;
    ObjControlPanel* panel;
};

class MoveCameraButton : public Button
{
public:
    MoveCameraButton(Widget* parent, OptScene* scene, Vector change, Vector color, std::string text);
    virtual void action() override;

private:
    OptScene* scene;
    Vector change;
};

#endif // OPTICAL_CONTROL_H