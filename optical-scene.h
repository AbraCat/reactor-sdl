#ifndef OPTICAL_SCENE_H
#define OPTICAL_SCENE_H

#include "plane.h"
#include "button.h"

class Ray;
class Material;
class Surface;
class Source;
class PlaneSurface;
class SphereSurface;
class OptScene;

using SurfaceIt = std::vector<Surface*>::iterator;
using SourceIt = std::vector<Source*>::iterator;

using VecMtx1 = std::vector<Vector>;
using VecMtx2 = std::vector<std::vector<Vector>>;
using VecMtx3 = std::vector<VecMtx2>;

extern const Material water;

Vector getDiffuseColor(Surface* s, Source* l, Vector p_surface, Vector p_light);

enum OptPropEnum
{
    OPT_DIFFUSE_PORTION,
    OPT_SPECULAR_PORTION,
    OPT_DEFRACT_PORTION,
    OPT_DEFRACT_COEFF,

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

private:
    OptPropEnum prop;
    std::string name;
    double val;
};

class OptObject
{
public:
    OptObject(std::string name);

    void setProperty(OptPropEnum prop, double val);
    double getProperty(OptPropEnum prop);

    void setOptColor(Vector color);
    Vector getOptColor();
    void setOptPos(Vector pos);
    Vector getOptPos();

    virtual void updateProperties(); // updates fields according to properties vector

// private:
    std::string name;

private:
    Vector pos, color;
    std::vector<OptProperty> properties;
};

class OptObjectButton : public Button
{
public:
    OptObjectButton(Widget* parent, Vector tl, Vector br, OptObject* obj);
    virtual void action() override;

private:
    OptObject* obj;
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
    Material(double diffuse_c, double reflect_c, double refract_c, double refract_k, Vector color);

    double reflect_c, refract_c, diffuse_c;
    double refract_k;
    Vector color;
};

class Surface : public OptObject
{
public:
    Surface(Material m);
    Surface(Vector color);
    virtual void updateProperties() override;

    virtual bool intersect(Ray ray, double* t) = 0;
    virtual Vector normal(Vector p) = 0; // should face outside
    bool rayGoesIn(Ray r, Vector intersection_p);
    
    Ray reflect(Ray r, Vector p);
    Ray refract(Ray r, Vector p, bool* success);
    Ray reflect_diffuse(Ray r, Vector p);

    Material m;
};

class Source
{
public:
    Source(Vector color, Vector pos);
    virtual Vector getRandPoint();

    Vector color, pos;
};

class SphereSource : public Source
{
public:
    SphereSource(Vector color, Vector pos, double r);
    virtual Vector getRandPoint() override;

    double r;
};

class PlaneSurface : public Surface
{
public:
    PlaneSurface(double y_pos, Vector color);
    PlaneSurface(double y_pos, Material m);

    virtual bool intersect(Ray ray, double* t) override;
    virtual Vector normal(Vector p) override;

    double y_pos;
};

class SphereSurface : public Surface
{
public:
    SphereSurface(Vector pos, double r, Vector color);
    SphereSurface(Vector pos, double r, Material m);
    
    virtual bool intersect(Ray ray, double* t) override;
    virtual Vector normal(Vector p) override;

    double r;
    Vector pos;
};

class OptScene : public Widget
{
public:
    OptScene(Widget* parent, Vector tl, Vector br);
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

// private:
    Vector V;

    std::vector<Surface*> spheres;
    std::vector<Source*> sources;
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