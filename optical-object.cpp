#include "optical-object.h"

#include "optical-scene.h"

#include <algorithm>
#include <random>
#include <sstream>

#include <limits>
#include <cmath>
#include <cassert>

const double intersect_eps = 1e-2;
const Vector sky_col = {0, 0.5, 0.75}, init_V = {0, 0, 10}, init_screen_tl = {-2, -1.15, 4};
extern const Material glass(0, 0, 1, 1.06), plastic(1, 1, 0, 1.3), std_material = plastic;



OptProperty::OptProperty(OptPropEnum prop, std::string name, double val)
    : prop(prop), name(name), val(val)
{
    //
}

void OptProperty::setVal(double val) { this->val = val; }
double OptProperty::getVal() { return val; }
std::string OptProperty::getName() { return name; }
void OptProperty::setName(std::string name) { this->name = name; }

OptObject::OptObject(std::string name, OptScene* scene, Vector color, Vector pos)
    : name(name), scene(scene), color(color), pos(pos)
{
    has_rect = 0;
    size = 0;
}

std::vector<OptProperty> OptObject::getProperties()
{
    std::vector<OptProperty> properties;

    properties.push_back(OptProperty(OPT_COLOR_R, "color_r", color.x));
    properties.push_back(OptProperty(OPT_COLOR_G, "color_g", color.y));
    properties.push_back(OptProperty(OPT_COLOR_B, "color_b", color.z));

    properties.push_back(OptProperty(OPT_POS_X, "pos_x", pos.x));
    properties.push_back(OptProperty(OPT_POS_Y, "pos_y", pos.y));
    properties.push_back(OptProperty(OPT_POS_Z, "pos_z", pos.z));

    return properties;
}

bool OptObject::setProperty(OptPropEnum prop, double val)
{
    switch (prop)
    {
        case OPT_COLOR_R: color.x = val; return 1;
        case OPT_COLOR_G: color.y = val; return 1;
        case OPT_COLOR_B: color.z = val; return 1;

        case OPT_POS_X: pos.x = val; return 1;
        case OPT_POS_Y: pos.y = val; return 1;
        case OPT_POS_Z: pos.z = val; return 1;
    }

    return 0;
}

void OptObject::movePos(Vector change) { pos += change; }

Vector randUnitVector()
{
    int x = rand(), y = rand(), z = rand();
    return !Vector(x, y, z);
}

Ray::Ray(Vector p, Vector a) : p(p), a(a)
{
    //
};

Vector Ray::eval(double t) { return p + a * t; }

Material::Material(double diffuse_c, double reflect_c, double refract_c, 
    double refract_k) :
    reflect_c(reflect_c), refract_c(refract_c), diffuse_c(diffuse_c),
    refract_k(refract_k)
{
    // assert(diffuse_c + reflect_c + refract_c <= 1);
}

Surface::Surface(Vector color, Vector pos, std::string name, OptScene* scene, Material m)
    : OptObject(name, scene, color, pos), m(m)
{
    //
}

std::vector<OptProperty> Surface::getProperties()
{
    std::vector<OptProperty> properties = OptObject::getProperties();

    properties.push_back(OptProperty(OPT_DIFFUSE_PORTION, "diffuse_c", m.diffuse_c));
    properties.push_back(OptProperty(OPT_SPECULAR_PORTION, "specular_c", m.reflect_c));
    properties.push_back(OptProperty(OPT_REFRACT_PORTION, "refract_c", m.refract_c));
    properties.push_back(OptProperty(OPT_REFRACT_COEFF, "refract_k", m.refract_k));

    return properties;
}

bool Surface::setProperty(OptPropEnum prop, double val)
{
    if (OptObject::setProperty(prop, val)) return 1;

    switch (prop)
    {
        case OPT_DIFFUSE_PORTION: m.diffuse_c = val; return 1;
        case OPT_SPECULAR_PORTION: m.reflect_c = val; return 1;
        case OPT_REFRACT_PORTION: m.refract_c = val; return 1;
        case OPT_REFRACT_COEFF: m.refract_k = val; return 1;
    }

    return 0;
}

bool Surface::rayGoesIn(Ray r, Vector intersection_p)
{
    return (r.a ^ normal(intersection_p)) < 0;
}

Ray Surface::reflect(Ray r, Vector p)
{
    Vector n = normal(p);
    Vector r_proj_n = proj(r.a, n);

    // 2 * r_proj_n = a - x
    return {p, r.a - r_proj_n * 2};
}

Ray Surface::refract(Ray r, Vector p, bool *success)
{
    // if (d) printf("aaa\n");

    bool in = rayGoesIn(r, p);
    double n1 = 1, n2 = this->m.refract_k;
    if (!in) std::swap(n1, n2);

    Vector n = normal(p);
    double sin1 = sin_angle(r.a, n);
    double sin2 = n1 * sin1 / n2;

    if (sin2 >= 1)
    {
        success = 0;
        return Ray(Vector(), Vector());
    }

    Vector proj12 = proj(r.a, n), ort1 = ortog(r.a, n);
    // |ort2| / |proj12| = tan2
    double tan2 = std::tan(std::asin(sin2));
    Vector ort2 = (!ort1) * (*proj12) * tan2;

    *success = 1;
    return Ray(p, proj12 + ort2);
}

Ray Surface::reflect_diffuse(Ray r, Vector p)
{
    Vector norm = !normal(p);
    Vector random_vec;

    do
    {
        random_vec = randUnitVector();
    } while (*(norm + random_vec) < 0.01);

    return Ray(p, norm + random_vec);
}

Source::Source(Vector color, Vector pos, std::string name, OptScene* scene)
    : OptObject(name, scene, color, pos)
{
    //
}

Vector Source::getRandPoint()
{
    return pos;
}

SphereSource::SphereSource(Vector color, Vector pos, double r, std::string name, OptScene* scene)
    : Source(color, pos, name, scene), r(r)
{
    size = r;
}

Vector SphereSource::getRandPoint()
{
    return pos;

    Vector tl = pos - Vector(r, r, r), br = pos + Vector(r, r, r);

    Vector rand_p;
    do
    {
        rand_p = Vector(randDouble(tl.x, br.x), randDouble(tl.y, br.y), randDouble(tl.z, br.z));
    } while (*(rand_p - pos) > r);

    return rand_p;
}

std::vector<OptProperty> SphereSource::getProperties()
{
    std::vector<OptProperty> properties = Source::getProperties();
    properties.push_back(OptProperty(OPT_RADIUS, "radius", r));
    return properties;
}

bool SphereSource::setProperty(OptPropEnum prop, double val)
{
    if (Source::setProperty(prop, val)) return 1;

    if (prop == OPT_RADIUS)
    {
        r = val;
        return 1;
    }

    return 0;
}

PlaneSurface::PlaneSurface(double y_pos, Vector color, std::string name, OptScene* scene, Material m)
    : Surface(color, {0, y_pos, 0}, name, scene, m)
{
    //
}

bool PlaneSurface::intersect(Ray ray, double* t_ptr)
{
    // (p + at).y = y_pos
    // p.y + a.y * t = y_pos
    if (isZero(ray.a.y)) return 0;

    double t = (pos.y - ray.p.y) / ray.a.y;
    if (t < intersect_eps) return 0;

    if (t_ptr != nullptr) *t_ptr = t;
    return 1;
}

Vector PlaneSurface::normal(Vector p)
{
    // if (d) printf("plane\n");
    return Vector(0, -1, 0);
}


SphereSurface::SphereSurface(Vector pos, double r, Vector color, std::string name, OptScene* scene, Material m)
    : Surface(color, pos, name, scene, m), r(r)
{
    has_rect = 1;
    size = r;
}

std::vector<OptProperty> SphereSurface::getProperties()
{
    std::vector<OptProperty> properties = Surface::getProperties();
    properties.push_back(OptProperty(OPT_RADIUS, "radius", r));
    return properties;
}

bool SphereSurface::setProperty(OptPropEnum prop, double val)
{
    if (Surface::setProperty(prop, val)) return 1;

    if (prop == OPT_RADIUS)
    {
        r = val;
        return 1;
    }

    return 0;
}
    
bool SphereSurface::intersect(Ray ray, double* t_ptr)
{
    /*
    p = p1 - c
    |p1 + t * p2 - c| = r
    |p + t * p2| = r
    (p.x + t * p2.x) ^ 2 + (p.y + t * p2.y) ^ 2 = r ^ 2
    t^2(p2.x^2 + p2.y^2) + 2t * (p.x * p2.x + p.y * p2.y) + p.x^2 + p.y^2 - r^2 = 0
    */

    Vector p = ray.p - pos, p2 = ray.a;

    double t1 = 0, t2 = 0;
    int n_roots = -1;
    solveQuadratic(p2.x * p2.x + p2.y * p2.y + p2.z * p2.z, 2 * (p.x * p2.x + p.y * p2.y + p.z * p2.z),
        p.x * p.x + p.y * p.y + p.z * p.z - r * r, &t1, &t2, &n_roots);
    if (n_roots == 2 && t1 > t2) std::swap(t1, t2);

    if (n_roots >= 1 && t1 > intersect_eps)
    {
        if (t_ptr != nullptr) *t_ptr = t1;
        return 1;
    }
    if (n_roots == 2 && t2 > intersect_eps)
    {
        if (t_ptr != nullptr) *t_ptr = t2;
        return 1;
    }
    return 0;
}

Vector SphereSurface::normal(Vector p)
{
    // if (d) printf("sphere\n");
    return !(p - pos);
}