#ifndef OPTICAL_SCENE_H
#define OPTICAL_SCENE_H

#include "plane.h"
#include "button.h"

/*
список объектов с возм. выбора                                         -
выделение одного.нескольких объектов                                   -
прямоугольник вокруг выделенного объекта                               -
список свойств при выделении одного объекта (общие и частные свойства) -
изменение свойств                                                      -
кнопки движения объектов                                               -
кнопки движения камеры                                                 -
проблема медленного рендера                                            -
3 ракурса                                                              -

панель выбора объекта (прокручиваемый список объектов)                 -
панель управления объектом (движение, удаление)                        -

trace random ray that hasn't been traced yet (stochastic rendering)
queue of pixels to render

keyboard events
global state
focused widget
input field
changing object properties

scrollable list texture visibility
better text rendering
partial scene rendering

deleting objects
don't show irrelevant properties
*/

class Ray;
class Material;

class OptObject;
class Surface;
class Source;

class PlaneSurface;
class SphereSurface;
class OptScene;

class OptPropField;

using SurfaceIt = std::vector<Surface*>::iterator;
using SourceIt = std::vector<Source*>::iterator;

using VecMtx1 = std::vector<Vector>;
using VecMtx2 = std::vector<std::vector<Vector>>;
using VecMtx3 = std::vector<VecMtx2>;

extern const Material water, wood, std_material;

Vector getDiffuseColor(Surface* s, Source* l, Vector p_surface, Vector p_light);

enum OptPropEnum
{
    OPT_DIFFUSE_PORTION,
    OPT_SPECULAR_PORTION,
    OPT_REFRACT_PORTION,
    OPT_REFRACT_COEFF,

    OPT_POS_X,
    OPT_POS_Y,
    OPT_POS_Z,

    OPT_RADIUS,

    OPT_COLOR_R,
    OPT_COLOR_G,
    OPT_COLOR_B,

    OPT_TOTAL
};

class OptProperty
{
public:
    OptProperty(OptPropEnum prop, std::string name, double val);

    void setVal(double val);
    double getVal();
    std::string getName();
    void setName(std::string name);

// private:
    OptPropEnum prop;
    std::string name;
    double val;
};

class OptPropWidget : public Widget
{
public:
    OptPropWidget(Widget* parent, Vector tl, Vector br, OptObject* obj, OptProperty prop);

// private:
    TextField *name_field;
    OptPropField *val_field;
};

class OptPropField : public InputField
{
public:
    OptPropField(OptPropWidget* parent, Vector tl, Vector br, OptObject* obj, OptProperty prop);
    virtual void action() override;

private:
    OptObject* obj;
    OptProperty prop;
};

class OptObject
{
public:
    OptObject(std::string name, OptScene* scene, Vector color, Vector pos);

    virtual std::vector<OptProperty> getProperties();
    virtual bool setProperty(OptPropEnum prop, double val);

// protected:
    std::string name;
    OptScene* scene;

    Vector pos, color;
};

class OptObjectButton : public Button
{
public:
    OptObjectButton(Widget* parent, Vector tl, Vector br, OptObject* obj, WContainer* prop_cont);
    virtual void action() override;

private:
    OptObject* obj;
    WContainer* prop_cont;
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

class Ray
{
public:
    Ray(Vector p, Vector a);
    Vector eval(double t);

    Vector p, a;
};

class Material
{
public:
    Material(double diffuse_c, double reflect_c, double refract_c, double refract_k);

    double reflect_c, refract_c, diffuse_c;
    double refract_k;
};

class Surface : public OptObject
{
public:
    Surface(Vector color, Vector pos, std::string name, OptScene* scene, Material m = std_material);

    virtual std::vector<OptProperty> getProperties();
    virtual bool setProperty(OptPropEnum prop, double val);

    virtual bool intersect(Ray ray, double* t) = 0;
    virtual Vector normal(Vector p) = 0; // should face outside
    bool rayGoesIn(Ray r, Vector intersection_p);
    
    Ray reflect(Ray r, Vector p);
    Ray refract(Ray r, Vector p, bool* success);
    Ray reflect_diffuse(Ray r, Vector p);

    Material m;
};

class Source : public OptObject
{
public:
    Source(Vector color, Vector pos, std::string name, OptScene* scene);
    virtual Vector getRandPoint();
};

class SphereSource : public Source
{
public:
    SphereSource(Vector color, Vector pos, double r, std::string name, OptScene* scene);
    virtual Vector getRandPoint() override;

    virtual std::vector<OptProperty> getProperties();
    virtual bool setProperty(OptPropEnum prop, double val);

    double r;
};

class PlaneSurface : public Surface
{
public:
    PlaneSurface(double y_pos, Vector color, std::string name, OptScene* scene, Material m = wood);

    virtual bool intersect(Ray ray, double* t) override;
    virtual Vector normal(Vector p) override;

    double y_pos;
};

class SphereSurface : public Surface
{
public:
    SphereSurface(Vector pos, double r, Vector color, std::string name, OptScene* scene, Material m = wood);

    virtual std::vector<OptProperty> getProperties();
    virtual bool setProperty(OptPropEnum prop, double val);
    
    virtual bool intersect(Ray ray, double* t) override;
    virtual Vector normal(Vector p) override;

    double r;
};



class OptScene : public Widget
{
public:
    OptScene(Widget* parent, Vector tl, Vector br, WContainer* prop_cont);
    virtual void paint() override;

    void setV(Vector V);
    void moveCamera(Vector change);

    Vector traceRay(Ray ray, int depth);
    Surface* getIntersectedSurface(Ray ray, double *t_ptr);
    Vector castShadowRay(Surface* s, Source* l, Vector p);
    Vector traceDiffuse(Surface* s, Vector p);
    Vector traceRefract(Surface* s, Ray ray, Vector p, int depth);

    void calculateThread(int thread_num, VecMtx1* colors);

    std::vector<Surface*>::iterator addSphere(Vector pos, Vector color, double r);
    std::vector<Source*>::iterator addSource(Vector pos, Vector color, double r);

    WContainer* makeObjectContainer(Widget* parent, Vector tl, Vector br);

// private:
    Vector V, screen_tl, screen_w, screen_h;

    std::vector<Surface*> surfaces;
    std::vector<Source*> sources;
    WContainer *obj_cont, *prop_cont;
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

#endif // OPTICAL_SCENE_H