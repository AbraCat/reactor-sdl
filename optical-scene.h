#ifndef OPTICAL_SCENE_H
#define OPTICAL_SCENE_H

#include "plane.h"

class Ray;
class Material;
class Surface;
class Sphere;
class PlaneSurface;
class SphereSurface;
class OptScene;

bool intersectSphere(Sphere* s, Ray ray, double* t, bool* in);

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
    Material(double diffuse_c, double reflect_c, double refract_c, Vector color);

    double reflect_c, refract_c, diffuse_c;
    Vector color;
};

class Surface
{
public:
    Surface(Material m);
    Surface(Vector color);
    void setSource(bool is_source);

    virtual bool intersect(Ray ray, double* t, bool* in) = 0;
    virtual Vector normal(Vector p) = 0; // should face outside
    
    Ray reflect(Ray r, Vector p);
    Ray refract(Ray r, Vector p);
    Ray reflect_diffuse(Ray r, Vector p);

    Material m;
    bool is_source;
};

class Sphere
{
public:
    Sphere(Vector pos, double r);

    double radius;
    Vector pos;
};

class PlaneSurface : public Surface
{
public:
    PlaneSurface(double y_pos, Vector color);

    virtual bool intersect(Ray ray, double* t, bool* in) override;
    virtual Vector normal(Vector p) override;

    double y_pos;
};

class SphereSurface : public Sphere, public Surface
{
public:
    SphereSurface(Vector pos, double r, Vector color);
    
    virtual bool intersect(Ray ray, double* t, bool* in) override;
    virtual Vector normal(Vector p) override;
};

class OptScene : public Widget
{
public:
    OptScene(Widget* parent, Vector tl, Vector br);
    virtual void paint() override;

    void setV(Vector V);

    Vector traceRay(Ray ray, int depth);

    std::vector<Surface*>::iterator addSphere(Vector pos, Vector color, double r);
    std::vector<SphereSurface*>::iterator addSource(Vector pos, Vector color, double r);

private:
    Vector V;

    std::vector<Surface*> spheres;
    std::vector<SphereSurface*> sources;
};

#endif // OPTICAL_SCENE_H