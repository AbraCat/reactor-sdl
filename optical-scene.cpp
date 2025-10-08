#include "optical-scene.h"

#include <limits>
#include <cmath>

OptScene::OptScene(Widget* parent, Vector tl, Vector br) : 
    CoordWidget(parent, tl, br, (br - tl) / 2, 1, 1, 1, 1)
{
    setAxisVisible(0);

    specularPow = 15;
    shadowSmooth = 3;
    V = {0, 0, 3};
    bgColor = {0, 0, 0};

    spheres = std::vector<Sphere>();
    sources = std::vector<Source>();
    spheres.reserve(10);
    sources.reserve(10);
}

void OptScene::paint()
{
    for (std::vector<Sphere>::iterator sphere = spheres.begin(); sphere != spheres.end(); ++sphere)
    {
        Vector topLeft = sphere->centre - Vector(sphere->radius, sphere->radius);

        for (int i = topLeft.x; i < topLeft.x + 2 * sphere->radius; ++i)
        {
            for (int j = topLeft.y; j < topLeft.y + 2 * sphere->radius; ++j)
            {
                Vector color = bgColor;

                Vector centre2d = sphere->centre;
                centre2d.z = 0;
                Vector p2d = Vector(i, j, 0) - centre2d;
                double distToCentre = *p2d;

                if (distToCentre < sphere->radius)
                {
                    double Pz = std::sqrt(sphere->radius * sphere->radius - distToCentre * distToCentre);
                    Vector p = p2d;
                    p.z = Pz;
                    color = sphere->Imaterial * Iambient;

                    for (Source source: sources)
                    {
                        Vector L = source.pos - sphere->centre;
                        Vector L1 = L - p;
                        double cosA = (!L1) ^ (!p);
                        if (cosA < 0) continue;

                        // double shadowCoeff = 1;
                        // for (std::vector<Sphere>::iterator s2 = spheres.begin(); s2 != spheres.end(); ++s2)
                        // {
                        //     if (s2 == sphere) continue;

                        //     Vector PC2 = s2->centre - (p + sphere->centre);
                        //     if (((!L1) ^ (!PC2)) <= 0) continue;

                        //     double shadowDist = dist(s2->centre, sphere->centre + p, L - p);
                        //     if (shadowDist > s2->radius) continue;
                        //     shadowCoeff *= std::atan(shadowSmooth * shadowDist) / (3.1415926 / 2);
                        //     d = 1;
                        // }

                        if (cosA >= 0) color += sphere->Imaterial * source.Isource * cosA; //shadowCoeff;

                        Vector ort = ortog(L1, p);
                        Vector L2 = L1 - ort * 2, PV = (V - sphere->centre) - p;
                        double cosB = ((!PV) ^ (!L2));
                        double specCoeff = std::pow(cosB, specularPow);
                        if (cosB >= 0) color += source.Isource * specCoeff;
                    }
                }

                color = limitVector(color, 0, 1);
                color *= 255;
                t->addRect({{i, j}, {i + 1, j + 1}}, color, 1);
            }
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
   Vector p = ray.p1 - s.centre, p2 = ray.p2;
   double r = s.radius;
   solveQuadratic(p2.x * p2.x + p2.y * p2.y, 2 * (p.x * p2.x + p.y * p2.y), p.x * p.x + p.y * p.y - r * r, 
    t1, t2, n_roots);
}

Vector getReflectedRay(Sphere c, Vector p, FixedVec ray)
{
    //
}

Vector OptScene::traceRay(FixedVec ray)
{
    // ray: p1 + t * p2
    Sphere *s = nullptr;
    double t = std::numeric_limits<double>::infinity();
    for (Sphere s1: spheres)
    {
        int n_roots = 0;
        double t1 = 0, t2 = 0;
        intersectSphere(s1, ray, &n_roots, &t1, &t2);
        if (n_roots >= 1 && t1 > 0 && t1 < t)
        {
            t = t1;
            *s = s1;
        }
    }
    if (s == nullptr) return {0, 0, 0};

    for (Source l: sources)
    {
        // check for shadow

        // if no shadow:
        //
    }
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
