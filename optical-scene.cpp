#include "optical-scene.h"

#include <algorithm>
#include <random>
#include <sstream>

#include <limits>
#include <cmath>
#include <cassert>

const double ratio = 16.0 / 9.0, screen_size = 4, obj_change = 1;
const int max_depth = 8, n_diffuse_rays = 1, n_shadow_rays = 1, n_move_buttons = 6, obj_button_h = 100,
    n_threads = 1, pix_per_frame = 5e4, select_rect_size = 75;

const Vector sky_col = {0, 0.5, 0.75}, init_V = {0, 0, 10}, init_screen_tl = {-2, -1.15, 4};

const double cam_change_x = 0.5, cam_change_y = 0.5, cam_change_z = 1;

struct RenderThreadData
{
    OptScene* scene;
    int thread_num;
    std::vector<IntVec>* thread_pix;
};

void OptScene::updateTexture()
{
    t->clear();
    for (OptObject* obj: selected)
    {
        if (!obj->has_rect) continue;
        FixedVec rect = getRect(obj);
        t->addRect(rect, red_v, 0);
    }

    if (!redraw_picture)
    {
        redraw_picture = 1;
        return;
    }

    pix_queue = std::vector<IntVec>(width * height);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            pix_queue[y * width + x] = IntVec(x, y);

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(pix_queue.begin(), pix_queue.end(), g);
}

int calcIdleThread(void* void_data)
{
    RenderThreadData* data = (RenderThreadData*)void_data;
    OptScene* s = data->scene;

    for (IntVec pix: *(data->thread_pix))
    {
        Vector p = s->pixels_to_screen(pix);

        Ray ray(s->V, p - s->V);
        Vector color = s->traceRay(ray, 0);

        s->pix_texture->setPix(pix.x, pix.y, color * 255);
    }

    return 0;
}

bool OptScene::onIdle(IdleEvent* evt)
{
    if (pix_queue.size() == 0) return 0;

    std::vector<std::vector<IntVec>> thread_pix(n_threads, std::vector<IntVec>());
    std::vector<SDL_Thread*> threads;
    RenderThreadData* thread_data = new RenderThreadData[n_threads];

    for (int thread_num = 0; thread_num < n_threads; ++thread_num)
    {
        if (pix_queue.size() == 0) break;

        for (int n_pix = 0; n_pix < pix_per_frame; ++n_pix)
        {
            thread_pix[thread_num].push_back(pix_queue.back());
            pix_queue.pop_back();
            if (pix_queue.size() == 0) break;
        }

        thread_data[thread_num].scene = this;
        thread_data[thread_num].thread_num = thread_num;
        thread_data[thread_num].thread_pix = &thread_pix[thread_num];

        threads.push_back(SDL_CreateThread(calcIdleThread,
            std::to_string(thread_num).c_str(), &thread_data[thread_num]));
    }

    for (int thread_num = 0; thread_num < n_threads; ++thread_num)
    {
        int status = 0;
        SDL_WaitThread(threads[thread_num], &status);
        assert(status == 0);
    }

    return 0;
}

Vector getDiffuseColor(Surface* s, Source* l, Vector p_surface, Vector p_light)
{
    Vector L = p_light - p_surface;
    double cosA = angle(L, s->normal(p_surface));
    return l->color * cosA;
}


OptScene::OptScene(Widget* parent, Vector tl, Vector br, OptController* control)
    : Widget(tl, br, parent), control(control)
{
    V = init_V;
    screen_tl = init_screen_tl;
    screen_w = {screen_size, 0, 0};
    screen_h = {0, screen_size / ratio, 0};

    surfaces = std::vector<Surface*>();
    sources = std::vector<Source*>();
    surfaces.reserve(10);
    sources.reserve(10);

    redraw_picture = 1;
    setPixelTexture(true);
    pix_texture = dynamic_cast<PixelTexture*>(t);
    assert(pix_texture != nullptr);

    surfaces.push_back(new PlaneSurface(2, white_col, "plane", this));
}

Vector OptScene::screen_to_pixels(Vector p)
{
    return Vector((p - screen_tl).x * width / screen_w.x, (p - screen_tl).y * height / screen_h.y);
}

Vector OptScene::pixels_to_screen(IntVec pix)
{
    return screen_tl + screen_w * (1.0 * pix.x / width) + screen_h * (1.0 * pix.y / height);
}

FixedVec OptScene::getRect(OptObject* obj)
{
    Ray ray(V, obj->pos - V);

    if (isZero(ray.a.z)) return {{}, {}};

    // (p + at).z == screen_tl.z
    double t = (screen_tl.z - ray.p.z) / ray.a.z;
    if (t <= 0) return {{}, {}};

    Vector p = ray.eval(t);
    Vector centre = screen_to_pixels(p);
    Vector rect_size_vec = Vector(select_rect_size, select_rect_size) * obj->size;
    return {centre - rect_size_vec, centre + rect_size_vec};
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
    for (std::vector<Surface*>::iterator s1_it = surfaces.begin(); s1_it != surfaces.end(); ++s1_it)
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

    for (std::vector<Surface*>::iterator s1 = surfaces.begin(); s1 != surfaces.end(); ++s1)
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
    return limitVector(s->color * color, 0, 1);
}

void OptScene::setV(Vector V) {this->V = V;}

void OptScene::moveCamera(Vector change)
{
    V += change;
    screen_tl += change;
    updateTexture();
}