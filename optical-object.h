#ifndef OPTICAL_OBJECT_H
#define OPTICAL_OBJECT_H

#include "myvector.h"

#include <string>
#include <vector>

class OptScene;

class Material;

extern const Material glass, plastic, std_material;

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

class OptObject
{
public:
    OptObject(std::string name, OptScene* scene, Vector color, Vector pos);

    virtual std::vector<OptProperty> getProperties();
    virtual bool setProperty(OptPropEnum prop, double val);

    void movePos(Vector change);

// protected:
    std::string name;
    OptScene* scene;

    bool has_rect;
    double size;
    Vector pos, color;
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
    PlaneSurface(double y_pos, Vector color, std::string name, OptScene* scene, Material m = plastic);

    virtual bool intersect(Ray ray, double* t) override;
    virtual Vector normal(Vector p) override;
};

class SphereSurface : public Surface
{
public:
    SphereSurface(Vector pos, double r, Vector color, std::string name, OptScene* scene, Material m = plastic);

    virtual std::vector<OptProperty> getProperties();
    virtual bool setProperty(OptPropEnum prop, double val);
    
    virtual bool intersect(Ray ray, double* t) override;
    virtual Vector normal(Vector p) override;

    double r;
};

#endif // OPTICAL_OBJECT_H