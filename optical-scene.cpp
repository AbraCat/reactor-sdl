#include "optical-scene.h"

#include <limits>
#include <cmath>

const double screen_z = 3, intersect_eps = 0.01;
const int screen_size = 2, max_depth = 2, n_diffuse_rays = 20;
const Vector screen_tl = {-screen_size, -screen_size}, screen_br = {screen_size, screen_size};
const Vector sky_col = {0, 0.5, 1};

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

Material::Material(double diffuse_c, double reflect_c, double refract_c, Vector color) :
    reflect_c(reflect_c), refract_c(refract_c), diffuse_c(diffuse_c), color(color)
{
    //
}

Surface::Surface(Material m) : m(m), is_source(0)
{
    //
}

Surface::Surface(Vector color) : m(0.5, 0.5, 0, color), is_source(0)
{
    //
}

void Surface::setSource(bool is_source)
{
    this->is_source = is_source;
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

Sphere::Sphere(Vector pos, double r) : radius(r), pos(pos)
{
    //
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

    if (in != nullptr) *in = (ray.a.y < 0);
    if (t_ptr != nullptr) *t_ptr = t;
    return 1;
}

Vector PlaneSurface::normal(Vector p)
{
    return Vector(0, 1, 0);
}


SphereSurface::SphereSurface(Vector pos, double r, Vector color) : Sphere(pos, r), Surface(color)
{
    //
}
    
bool SphereSurface::intersect(Ray ray, double* t_ptr, bool* in)
{
    return intersectSphere(this, ray, t_ptr, in);
}

Vector SphereSurface::normal(Vector p)
{
    return !(p - pos);
}

bool intersectSphere(Sphere* s, Ray ray, double* t, bool* in)
{
    /*
    p = p1 - c
    |p1 + t * p2 - c| = r
    |p + t * p2| = r
    (p.x + t * p2.x) ^ 2 + (p.y + t * p2.y) ^ 2 = r ^ 2
    t^2(p2.x^2 + p2.y^2) + 2t * (p.x * p2.x + p.y * p2.y) + p.x^2 + p.y^2 - r^2 = 0
    */

    Vector p = ray.p - s->pos, p2 = ray.a;
    double r = s->radius;

    double t1 = 0, t2 = 0;
    int n_roots = -1;
    solveQuadratic(p2.x * p2.x + p2.y * p2.y + p2.z * p2.z, 2 * (p.x * p2.x + p.y * p2.y + p.z * p2.z),
        p.x * p.x + p.y * p.y + p.z * p.z - r * r, &t1, &t2, &n_roots);
    if (n_roots == 2 && t1 > t2) std::swap(t1, t2);

    if (n_roots >= 1 && t1 > intersect_eps)
    {
        if (t != nullptr) *t = t1;
        if (in != nullptr) *in = 1;
        return 1;
    }
    if (n_roots == 2 && t2 > intersect_eps)
    {
        if (t != nullptr) *t = t2;
        if (in != nullptr) *in = 0;
        return 1;
    }
    return 0;
}

Vector getDiffuseColor(Surface* s, SphereSurface* l, Vector p)
{
    Vector L = l->pos - p;
    double cosA = angle(L, s->normal(p));
    return s->m.color * l->m.color * cosA;
}


OptScene::OptScene(Widget* parent, Vector tl, Vector br) : Widget(tl, br, parent)
{
    V = {0, 0, 6};

    spheres = std::vector<Surface*>();
    sources = std::vector<SphereSurface*>();
    spheres.reserve(10);
    sources.reserve(10);

    // spheres.push_back(new PlaneSurface(1, gray_col));
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
            double y = screen_tl.y + pixel_y * (screen_br.y - screen_tl.y) / height; 

            Ray ray(V, Vector(x, y, screen_z) - V);
            Vector color = traceRay(ray, 0);

            t->addRect({{pixel_x, pixel_y}, {pixel_x + 1, pixel_y + 1}}, color * 255, 1);
        }
    }
}

Vector OptScene::traceRay(Ray ray, int depth)
{
    if (depth >= max_depth) return {0, 0, 0};

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
    // for (std::vector<SphereSurface*>::iterator s1 = sources.begin(); s1 != sources.end(); ++s1)
    // {
    //     double cur_t = 0;
    //     bool cur_in = 0;
    //     if ((*s1)->intersect(ray, &cur_t, &cur_in) && cur_t < t)
    //     {
    //         t = cur_t;
    //         in = cur_in;
    //         s = *s1;
    //     }
    // }

    if (s == nullptr) return blackV; //sky_col;
    // if (s->is_source) return s->m.color;

    Vector p = ray.eval(t);

    // Vector diffuse_color(0, 0, 0);
    // for (int i = 0; i < n_diffuse_rays; ++i) {
    //     Ray diffuse_ray = s->reflect_diffuse(ray, p);
    //     diffuse_color += traceRay(diffuse_ray, depth + 1);
    // }
    // diffuse_color = (diffuse_color / n_diffuse_rays) * s->m.color;

    // Ray diffuse_ray = s->reflect_diffuse(ray, p);
    // Vector diffuse_color = s->m.color * traceRay(diffuse_ray, depth + 1);

    // Vector color = diffuse_color;

    Ray reflect_ray = s->reflect(ray, p);
    Vector reflect_color = s->m.color * traceRay(reflect_ray, depth + 1);

    Vector diffuse_color = {0, 0, 0};
    for (SphereSurface* l: sources)
    {
        Ray shadow_ray(p, l->pos - p);

        bool shadow = 0;
        for (std::vector<Surface*>::iterator s1 = spheres.begin(); s1 != spheres.end(); ++s1)
        {
            if (*s1 == s) continue;
            if ((*s1)->intersect(shadow_ray, nullptr, nullptr))
            {
                shadow = 1;
                break;
            }
        }

        if (!shadow)
            diffuse_color += getDiffuseColor(s, l, p);
    }

    Vector color = diffuse_color * s->m.diffuse_c + reflect_color * s->m.reflect_c;

    return limitVector(color, 0, 1);
}

void OptScene::setV(Vector V) {this->V = V;}

std::vector<Surface*>::iterator OptScene::addSphere(Vector pos, Vector color, double r)
{
    spheres.push_back(new SphereSurface(pos, r, color));
    return spheres.end() - 1;
}

std::vector<SphereSurface*>::iterator OptScene::addSource(Vector pos, Vector color, double r)
{
    SphereSurface* new_sorce = new SphereSurface(pos, r, color);
    new_sorce->setSource(1);
    sources.push_back(new_sorce);
    return sources.end() - 1;
}
