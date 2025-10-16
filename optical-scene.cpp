#include "optical-scene.h"

#include <limits>
#include <cmath>
#include <cassert>

const double screen_z = 8, intersect_eps = 0.01;
const int screen_size = 2, max_depth = 2, n_diffuse_rays = 20, n_shadow_rays = 10;
const Vector screen_tl = {-screen_size, -screen_size}, screen_br = {screen_size, screen_size};
const Vector sky_col = {0, 0.5, 1}, stdV = {0, 0, 12};

bool d = 0;

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
    double refract_k, Vector color) :
    reflect_c(reflect_c), refract_c(refract_c), diffuse_c(diffuse_c),
    refract_k(refract_k), color(color)
{
    assert(diffuse_c + reflect_c + refract_c <= 1);
}

Surface::Surface(Material m) : m(m)
{
    //
}

Surface::Surface(Vector color) : m(0.5, 0.5, 0, 0, color)
{
    //
}

Ray Surface::reflect(Ray r, Vector p)
{
    Vector n = normal(p);
    Vector r_proj_n = proj(r.a, n);

    // 2 * r_proj_n = a - x
    return {p, r.a - r_proj_n * 2};
}

Ray Surface::refract(Ray r, Vector p)
{
    return {{}, {}};
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

Source::Source(Vector color, Vector pos) : color(color), pos(pos)
{
    //
}

Vector Source::getRandPoint()
{
    return pos;
}

SphereSource::SphereSource(Vector color, Vector pos, double r) : Source(color, pos), r(r)
{
    //
}

Vector SphereSource::getRandPoint()
{
    Vector tl = pos - Vector(r, r, r), br = pos + Vector(r, r, r);

    Vector rand_p;
    do
    {
        rand_p = Vector(randDouble(tl.x, br.x), randDouble(tl.y, br.y), randDouble(tl.z, br.z));
    } while (*(rand_p - pos) > r);

    return rand_p;
}

PlaneSurface::PlaneSurface(double y_pos, Vector color) : Surface(color), y_pos(y_pos)
{
    //
}

bool PlaneSurface::intersect(Ray ray, double* t_ptr, bool* in)
{
    // (p + at).y = y_pos
    // p.y + a.y * t = y_pos
    if (isZero(ray.a.y)) return 0;

    double t = (y_pos - ray.p.y) / ray.a.y;
    if (t < intersect_eps) return 0;

    if (in != nullptr) *in = (ray.a.y > 0);
    if (t_ptr != nullptr) *t_ptr = t;
    return 1;
}

Vector PlaneSurface::normal(Vector p)
{
    return Vector(0, -1, 0);
}


SphereSurface::SphereSurface(Vector pos, double r, Vector color) : 
    Surface(color), r(r), pos(pos)
{
    //
}
    
bool SphereSurface::intersect(Ray ray, double* t_ptr, bool* in)
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
        if (in != nullptr) *in = 1;
        return 1;
    }
    if (n_roots == 2 && t2 > intersect_eps)
    {
        if (t_ptr != nullptr) *t_ptr = t2;
        if (in != nullptr) *in = 0;
        return 1;
    }
    return 0;
}

Vector SphereSurface::normal(Vector p)
{
    return !(p - pos);
}

Vector getDiffuseColor(Surface* s, Source* l, Vector p_surface, Vector p_light)
{
    Vector L = p_light - p_surface;
    double cosA = angle(L, s->normal(p_surface));
    return s->m.color * l->color * cosA;
}


OptScene::OptScene(Widget* parent, Vector tl, Vector br) : Widget(tl, br, parent)
{
    V = stdV;

    spheres = std::vector<Surface*>();
    sources = std::vector<Source*>();
    spheres.reserve(10);
    sources.reserve(10);

    spheres.push_back(new PlaneSurface(3, gray_col));
    // addSource({0, 0, 0}, red_col, 0.5);
    // addSphere({-2, 0, 0}, gray_col, 1);
}

void OptScene::paint()
{
    for (int pixel_x = 0; pixel_x <= width; ++pixel_x)
    {
        double x = screen_tl.x + pixel_x * (screen_br.x - screen_tl.x) / width; 
        for (int pixel_y = 0; pixel_y <= height; ++pixel_y)
        {
            // if (pixel_x == 651 && pixel_y == 745) d = 1;

            double y = screen_tl.y + pixel_y * (screen_br.y - screen_tl.y) / height; 

            Ray ray(V, Vector(x, y, screen_z) - V);
            Vector color = traceRay(ray, 0);
            t->addRect({{pixel_x, pixel_y}, {pixel_x + 1, pixel_y + 1}}, color * 255, 1);

            // if (d) d = 0;
        }
    }
}

Vector OptScene::traceRay(Ray ray, int depth)
{
    if (depth >= max_depth) return blackV;

    Surface *s = nullptr;
    double t = std::numeric_limits<double>::infinity();
    bool in = 0;
    for (std::vector<Surface*>::iterator s1 = spheres.begin(); s1 != spheres.end(); ++s1)
    {
        double cur_t = 0;
        bool cur_in = 0;
        if ((*s1)->intersect(ray, &cur_t, &cur_in) && cur_t < t)
        {
            t = cur_t;
            in = cur_in;
            s = *s1;
        }
    }
    if (s == nullptr) return blackV;

    Vector p = ray.eval(t);

    Ray reflect_ray = s->reflect(ray, p);
    Vector reflect_color = s->m.color * traceRay(reflect_ray, depth + 1);

    Vector diffuse_color = blackV;
    for (Source* l: sources)
    {
        Vector source_diffuse_col = blackV;
        for (int n_ray = 0; n_ray < n_shadow_rays; ++n_ray)
        {
            Vector p_light = l->getRandPoint();
            Ray shadow_ray(p, p_light - p);

            bool shadow = 0;
            double intersect_t = 0;
            for (std::vector<Surface*>::iterator s1 = spheres.begin(); s1 != spheres.end(); ++s1)
            {
                if (*s1 == s) continue;
                if ((*s1)->intersect(shadow_ray, &intersect_t, nullptr) && intersect_t < 1)
                {
                    shadow = 1;
                    break;
                }
            }

            if (!shadow)
                source_diffuse_col += getDiffuseColor(s, l, p, p_light);
        }
        diffuse_color += source_diffuse_col / n_shadow_rays;
    }

    Vector refract_col = blackV;

    Vector color = diffuse_color * s->m.diffuse_c + 
                   reflect_color * s->m.reflect_c + 
                   refract_col * s->m.refract_c;
    return limitVector(color, 0, 1);
}

void OptScene::setV(Vector V) {this->V = V;}

std::vector<Surface*>::iterator OptScene::addSphere(Vector pos, Vector color, double r)
{
    spheres.push_back(new SphereSurface(pos, r, color));
    return spheres.end() - 1;
}

std::vector<Source*>::iterator OptScene::addSource(Vector pos, Vector color, double r)
{
    Source* new_sorce = new SphereSource(color, pos, r);
    sources.push_back(new_sorce);
    return sources.end() - 1;
}
