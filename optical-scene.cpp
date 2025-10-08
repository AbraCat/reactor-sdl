#include "optical-scene.h"

#include <limits>
#include <cmath>

const double screen_z = 200;
const int screen_size = 200;
const Vector screen_tl = {-screen_size, -screen_size}, screen_br = {screen_size, screen_size};

bool d = 0;

OptScene::OptScene(Widget* parent, Vector tl, Vector br) : 
    CoordWidget(parent, tl, br, (br - tl) / 2, 1, 1, 1, 1)
{
    setAxisVisible(0);

    specularPow = 15;
    shadowSmooth = 3;
    V = {0, 0, 300};
    bgColor = {0, 0, 0};

    spheres = std::vector<Sphere>();
    sources = std::vector<Source>();
    spheres.reserve(10);
    sources.reserve(10);
}

void OptScene::paint()
{
    for (int x = screen_tl.x; x <= screen_br.x; ++x)
    {
        for (int y = screen_tl.y; y <= screen_br.y; ++y)
        {
            if (x == 0 && y == 0) d = 1;

            FixedVec ray = {V, Vector(x, y, screen_z) - V};
            // if (d)
            // {
            //     print(ray.p1);
            //     print(ray.p2);
            // }
            Vector color = traceRay(ray);
            // if (d) print(color);
            t->addRect({{x, y}, {x + 1, y + 1}}, color * 255, 1);

            d = 0;
        }
    }
}

void intersectSphere(Sphere s, FixedVec ray, int* n_roots, double* t1, double* t2)
{
    /*
    p = p1 - c
    |p1 + t * p2 - c| = r
    |p + t * p2| = r
    (p.x + t * p2.x) ^ 2 + (p.y + t * p2.y) ^ 2 = r ^ 2
    t^2(p2.x^2 + p2.y^2) + 2t * (p.x * p2.x + p.y * p2.y) + p.x^2 + p.y^2 - r^2 = 0
    */
    if (d)
    {
        // printf("aaa\n");
    }
    Vector p = ray.p1 - s.centre, p2 = ray.p2;
    double r = s.radius;
    solveQuadratic(p2.x * p2.x + p2.y * p2.y + p2.z * p2.z, 2 * (p.x * p2.x + p.y * p2.y + p.z * p2.z),
        p.x * p.x + p.y * p.y + p.z * p.z - r * r, t1, t2, n_roots);
}

FixedVec getReflectedRay(Sphere s, Vector p, FixedVec ray)
{
    Vector n = p - s.centre;
    Vector r_proj_n  = proj(ray.p2, n);
    // x - p2 = 2n
    return {p, n * 2 + ray.p2};
}

Vector getDiffuseColor(Sphere s, Source l, Vector p)
{
    Vector L = l.pos - p, p_rel = p - s.centre;
    // double cosA = angle(L, p - s.centre);
    double cosA = (!L) ^ (!p_rel);
    if (d)
    {
        print(p);
        print(l.pos);
        print(s.centre);
        print(L);
        print(p_rel);
        printf("%lf\n", cosA);
    }
    return s.Imaterial * l.Isource * cosA;
}

Vector OptScene::traceRay(FixedVec ray)
{
    // ray: p1 + t * p2
    Sphere *s = nullptr;
    double t = std::numeric_limits<double>::infinity();
    for (std::vector<Sphere>::iterator s1 = spheres.begin(); s1 != spheres.end(); ++s1)
    {
        int n_roots = 0;
        double t1 = 0, t2 = 0;
        intersectSphere(*s1, ray, &n_roots, &t1, &t2);
        // if (d) printf("%d %lf %lf\n", n_roots, t1, t2);

        if (n_roots >= 1 && t1 > 0 && t1 < t)
        {
            t = t1;
            s = &*s1;
        }
    }
    if (s == nullptr) return {0, 0, 0};

    Vector color = {0, 0, 0}, p = ray.p1 + ray.p2 * t;
    // if (d)
    // {
    //     print(p);
    //     print(s->centre);
    //     printf("%lf\n", s->radius);
    // }
    for (Source l: sources)
    {
        // check for shadow

        // if no shadow:
        // reflection, refraction
        color += getDiffuseColor(*s, l, p);
        // if (d) print(color);
    }

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
