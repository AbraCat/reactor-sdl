#ifndef OPTICAL_SCENE_H
#define OPTICAL_SCENE_H

#include "plane.h"

class Sphere
{
public:
    Sphere();
    Sphere(Vector Im, Vector pos, double r);

    Vector Imaterial, centre;
    double radius;
};

class Source
{
public:
    Source();
    Source(Vector Is, Vector p);

    Vector Isource, pos;
};

class OptScene : public CoordWidget
{
public:
    OptScene(Widget* parent, Vector tl, Vector br);
    virtual void paint() override;

    void setV(Vector V);
    void setBG(Vector bgColor);
    void setIambient(Vector Ia);

    Vector traceRay(FixedVec ray);

    std::vector<Sphere>::iterator addSphere(Vector pos, Vector color, double radius);
    std::vector<Source>::iterator addSource(Vector pos, Vector color);

private:
    int specularPow;
    double shadowSmooth;

    Vector bgColor, Iambient, V;

    std::vector<Sphere> spheres;
    std::vector<Source> sources;
};

#endif // OPTICAL_SCENE_H