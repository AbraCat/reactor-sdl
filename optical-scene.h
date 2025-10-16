#ifndef OPTICAL_SCENE_H
#define OPTICAL_SCENE_H

#include "plane.h"

class Ray;
class Material;
class Surface;
class Source;
class PlaneSurface;
class SphereSurface;
class OptScene;

using SurfaceIt = std::vector<Surface*>::iterator;
using SourceIt = std::vector<Source*>::iterator;

Vector getDiffuseColor(Surface* s, Source* l, Vector p);

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

class Surface
{
public:
    Surface(Material m);
    Surface(Vector color);

    virtual bool intersect(Ray ray, double* t, bool* in) = 0;
    virtual Vector normal(Vector p) = 0; // should face outside
    
    Ray reflect(Ray r, Vector p);
    Ray refract(Ray r, Vector p);
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

class PlaneSurface : public Surface
{
public:
    PlaneSurface(double y_pos, Vector color);

    virtual bool intersect(Ray ray, double* t, bool* in) override;
    virtual Vector normal(Vector p) override;

    double y_pos;
};

class SphereSurface : public Surface
{
public:
    SphereSurface(Vector pos, double r, Vector color);
    
    virtual bool intersect(Ray ray, double* t, bool* in) override;
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

    Vector traceRay(Ray ray, int depth);

    std::vector<Surface*>::iterator addSphere(Vector pos, Vector color, double r);
    std::vector<Source*>::iterator addSource(Vector pos, Vector color, double r);

private:
    Vector V;

    std::vector<Surface*> spheres;
    std::vector<Source*> sources;
};

#endif // OPTICAL_SCENE_H