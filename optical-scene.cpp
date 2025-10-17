#include "optical-scene.h"

#include <limits>
#include <cmath>
#include <cassert>

#include <thread>

const double screen_z = 6, intersect_eps = 0.01, ratio = 1800.0 / 1000.0;
const int screen_size = 2, max_depth = 5, n_diffuse_rays = 20, n_shadow_rays = 1, n_threads = 4;
const Vector screen_tl = {-screen_size * ratio, -screen_size}, screen_br = {screen_size * ratio, screen_size};
const Vector sky_col = {0, 0.4, 0.8}, stdV = {0, 0, 12};

// 1: 9.74
// 2: 13.02
// 4: 14.5

const Material water(0.4, 0, 1, 1.06, white_col);

bool d = 0;

/*
trace random ray that hasn't been traced yet (stochastic rendering)
*/

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
    // assert(diffuse_c + reflect_c + refract_c <= 1);
}

Surface::Surface(Material m) : m(m)
{
    //
}

Surface::Surface(Vector color) : m(1, 1, 0, 1.3, color)
{
    //
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

PlaneSurface::PlaneSurface(double y_pos, Material m) : Surface(m), y_pos(y_pos)
{
    //
}

bool PlaneSurface::intersect(Ray ray, double* t_ptr)
{
    // (p + at).y = y_pos
    // p.y + a.y * t = y_pos
    if (isZero(ray.a.y)) return 0;

    double t = (y_pos - ray.p.y) / ray.a.y;
    if (t < intersect_eps) return 0;

    if (t_ptr != nullptr) *t_ptr = t;
    return 1;
}

Vector PlaneSurface::normal(Vector p)
{
    // if (d) printf("plane\n");
    return Vector(0, -1, 0);
}


SphereSurface::SphereSurface(Vector pos, double r, Vector color) : 
    Surface(color), r(r), pos(pos)
{
    //
}

SphereSurface::SphereSurface(Vector pos, double r, Material m) :
    Surface(m), r(r), pos(pos)
{

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

Vector getDiffuseColor(Surface* s, Source* l, Vector p_surface, Vector p_light)
{
    Vector L = p_light - p_surface;
    double cosA = angle(L, s->normal(p_surface));
    return l->color * cosA;
}


OptScene::OptScene(Widget* parent, Vector tl, Vector br) : Widget(tl, br, parent)
{
    V = stdV;

    spheres = std::vector<Surface*>();
    sources = std::vector<Source*>();
    spheres.reserve(10);
    sources.reserve(10);

    spheres.push_back(new PlaneSurface(2, white_col));
    spheres.push_back(new SphereSurface({0, 0, 2}, 1, water));
}

void OptScene::calculateThread(int thread_num, VecMtx2* colors)
{
    for (int pixel_x = 0; pixel_x <= width; ++pixel_x)
    {
        double x = screen_tl.x + pixel_x * (screen_br.x - screen_tl.x) / width; 
        for (int pixel_y = thread_num; pixel_y <= height; pixel_y += n_threads)
        {
            double y = screen_tl.y + pixel_y * (screen_br.y - screen_tl.y) / height; 

            Ray ray(V, Vector(x, y, screen_z) - V);
            Vector color = traceRay(ray, 0);

            (*colors)[pixel_x][pixel_y] = color;
        }
    }
}

void OptScene::paint()
{
    std::clock_t start = std::clock();

    std::vector<std::thread> threads;
    VecMtx2 colors(width + 1, VecMtx1(height + 1));

    for (int thread_num = 0; thread_num < n_threads; ++thread_num)
    {
        threads.push_back(std::move(std::thread(&OptScene::calculateThread, this, thread_num, &colors)));
    }

    for (int thread_num = 0; thread_num < n_threads; ++thread_num)
        threads[thread_num].join();

    for (int pixel_x = 0; pixel_x <= width; ++pixel_x)
    {
        for (int pixel_y = 0; pixel_y <= height; ++pixel_y)
        {
            Vector color = colors[pixel_x][pixel_y];
            t->addRect({{pixel_x, pixel_y}, {pixel_x + 1, pixel_y + 1}}, color * 255, 1);
        }
    }

    printf("%lf\n", 1.0 * (std::clock() - start));
}

Vector OptScene::traceDiffuse(Surface* s, Vector p)
{
    Vector diffuse_color = blackV;

    for (Source* l: sources)
    {
        Vector source_diffuse_col = blackV;

        for (int n_ray = 0; n_ray < n_shadow_rays; ++n_ray)
            source_diffuse_col += castShadowRay(s, l, p);

        diffuse_color += source_diffuse_col / n_shadow_rays;
    }

    return diffuse_color;
}

Vector OptScene::castShadowRay(Surface* s, Source* l, Vector p)
{
    Vector p_light = l->getRandPoint();
    Ray shadow_ray(p, p_light - p);

    double intersect_t = 0, intensity = 1;
    for (std::vector<Surface*>::iterator s1_it = spheres.begin(); s1_it != spheres.end(); ++s1_it)
    {
        Surface* s1 = *s1_it;
        if (s1 == s) continue;
        if (s1->intersect(shadow_ray, &intersect_t) && intersect_t < 1)
        {
            intensity *= s1->m.refract_c;
            if (intensity == 0) break;
        }
    }

    if (intensity != 0)
        return getDiffuseColor(s, l, p, p_light) * intensity;
    return blackV;
}

Vector OptScene::traceRefract(Surface* s, Ray ray, Vector p, int depth)
{
    bool refracted = 0;
    Ray refract_ray = s->refract(ray, p, &refracted);

    if (refracted)
        return traceRay(refract_ray, depth + 1);
    else
    {
        Ray reflect_ray = s->reflect(ray, p);
        return traceRay(reflect_ray, depth + 1);
    }
}

Surface* OptScene::getIntersectedSurface(Ray ray, double *t_ptr)
{
    Surface *s = nullptr;
    double t = std::numeric_limits<double>::infinity();

    for (std::vector<Surface*>::iterator s1 = spheres.begin(); s1 != spheres.end(); ++s1)
    {
        double cur_t = 0;
        if ((*s1)->intersect(ray, &cur_t) && cur_t < t)
        {
            t = cur_t;
            s = *s1;
        }
    }

    *t_ptr = t;
    return s;
}

Vector OptScene::traceRay(Ray ray, int depth)
{
    if (depth >= max_depth) return blackV;

    double t = 0;
    Surface* s = getIntersectedSurface(ray, &t);
    if (s == nullptr) return sky_col;
    Vector p = ray.eval(t);

    Vector reflect_col = blackV;
    if (s->m.reflect_c != 0)
    {
        Ray reflect_ray = s->reflect(ray, p);
        reflect_col = traceRay(reflect_ray, depth + 1);
    }

    Vector diffuse_col = blackV;
    if (s->m.diffuse_c != 0) diffuse_col = traceDiffuse(s, p);

    Vector refract_col = blackV;
    if (s->m.refract_c != 0) refract_col = traceRefract(s, ray, p, depth);

    Vector color = diffuse_col * s->m.diffuse_c + 
                   reflect_col * s->m.reflect_c + 
                   refract_col * s->m.refract_c;
    return limitVector(s->m.color * color, 0, 1);
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
