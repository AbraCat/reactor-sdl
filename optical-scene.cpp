#include "optical-scene.h"

#include <limits>
#include <cmath>

const double screen_z = 3;
const int screen_size = 2, max_depth = 2;
const Vector screen_tl = {-screen_size, -screen_size}, screen_br = {screen_size, screen_size};

bool d = 0;

OptScene::OptScene(Widget* parent, Vector tl, Vector br) : Widget(tl, br, parent)
{
    specularPow = 15;
    shadowSmooth = 3;
    V = {0, 0, 6};
    bgColor = {0, 0, 0};

    spheres = std::vector<Sphere>();
    sources = std::vector<Source>();
    spheres.reserve(10);
    sources.reserve(10);
}

void OptScene::paint()
{
    for (int pixel_x = 0; pixel_x <= width; ++pixel_x)
    {
        double x = screen_tl.x + pixel_x * (screen_br.x - screen_tl.x) / width; 
        for (int pixel_y = 0; pixel_y <= height; ++pixel_y)
        {
            double y = screen_tl.y + pixel_y * (screen_br.y - screen_tl.y) / height; 

            FixedVec ray = {V, Vector(x, y, screen_z) - V};
            Vector color = traceRay(ray, 0);

            t->addRect({{pixel_x, pixel_y}, {pixel_x + 1, pixel_y + 1}}, color * 255, 1);
        }
    }
}

bool intersectSphere(Sphere s, FixedVec ray, double* t)
{
    /*
    p = p1 - c
    |p1 + t * p2 - c| = r
    |p + t * p2| = r
    (p.x + t * p2.x) ^ 2 + (p.y + t * p2.y) ^ 2 = r ^ 2
    t^2(p2.x^2 + p2.y^2) + 2t * (p.x * p2.x + p.y * p2.y) + p.x^2 + p.y^2 - r^2 = 0
    */

    Vector p = ray.p1 - s.centre, p2 = ray.p2;
    double r = s.radius;
    double t1 = 0, t2 = 0;
    int n_roots = -1;
    solveQuadratic(p2.x * p2.x + p2.y * p2.y + p2.z * p2.z, 2 * (p.x * p2.x + p.y * p2.y + p.z * p2.z),
        p.x * p.x + p.y * p.y + p.z * p.z - r * r, &t1, &t2, &n_roots);

    const double eps = 1e-3;
    if (n_roots >= 1 && t1 > eps)
    {
        if (t != nullptr) *t = t1;
        return 1;
    }
    return 0;
}

FixedVec getReflectedRay(Sphere s, Vector p, FixedVec ray)
{
    Vector n = p - s.centre;
    Vector r_proj_n = proj(ray.p2, n);

    // 2 * r_proj_n = p2 - x
    return {p, ray.p2 - r_proj_n * 2};
}

Vector getDiffuseColor(Sphere s, Source l, Vector p)
{
    Vector L = l.pos - p;
    double cosA = angle(L, p - s.centre);
    return s.Imaterial * l.Isource * cosA;
}

Vector OptScene::traceRay(FixedVec ray, int depth)
{
    if (depth >= max_depth) return {0, 0, 0};

    Sphere *s = nullptr;
    double t = std::numeric_limits<double>::infinity();
    for (std::vector<Sphere>::iterator s1 = spheres.begin(); s1 != spheres.end(); ++s1)
    {
        double t1 = 0;
        if (intersectSphere(*s1, ray, &t1))
        {
            if (t1 < t)
            {
                t = t1;
                s = &*s1;
            }
        }
    }
    if (s == nullptr)
    {
        // if (ray.p2.z < 0)
            // return {63, 63, 63};
        // else
            return {0, 0, 0};
    }

    Vector p = ray.p1 + ray.p2 * t;
    Vector reflect_color = s->Imaterial * traceRay(getReflectedRay(*s, p, ray), depth + 1);

    Vector diffuse_color = {0, 0, 0};
    for (Source l: sources)
    {
        FixedVec shadow_ray = {p, l.pos - p};
        if (d) print(shadow_ray);

        bool shadow = 0;
        for (std::vector<Sphere>::iterator s1 = spheres.begin(); s1 != spheres.end(); ++s1)
        {
            if (&*s1 == s) continue;
            if (intersectSphere(*s1, shadow_ray, nullptr))
            {
                shadow = 1;
                break;
            }
        }

        if (!shadow) diffuse_color += getDiffuseColor(*s, l, p);
    }

    Vector color = diffuse_color + reflect_color;
    return limitVector(color, 0, 1);
}

void OptScene::setV(Vector V) {this->V = V;}

void OptScene::setBG(Vector bgColor) {this->bgColor = bgColor;}
void OptScene::setIambient(Vector Ia) {this->Iambient = Ia;}

Sphere::Sphere()
{
    Imaterial = centre = {0, 0, 0};
    radius = 1;
}

Sphere::Sphere(Vector Im, Vector pos, double r)
{
    this->Imaterial = Im;
    this->centre = pos;
    this->radius = r;
}

Source::Source()
{
    Isource = {0, 0, 0};
    pos = {0, 0, 0};
}

Source::Source(Vector Is, Vector p)
{
    this->Isource = Is;
    this->pos = p;
}

std::vector<Sphere>::iterator OptScene::addSphere(Vector pos, Vector color, double radius)
{
    spheres.push_back(Sphere(color, pos, radius));
    return spheres.end() - 1;
}

std::vector<Source>::iterator OptScene::addSource(Vector pos, Vector color)
{
    sources.push_back(Source(color, pos));
    return sources.end() - 1;
}
