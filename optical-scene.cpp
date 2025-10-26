#include "optical-scene.h"

#include <algorithm>
#include <random>
#include <sstream>

#include <limits>
#include <cmath>
#include <cassert>

const double intersect_eps = 0.01, ratio = 16.0 / 9.0, screen_size = 4, obj_change = 1;
const int max_depth = 10, n_diffuse_rays = 1, n_shadow_rays = 1, n_move_buttons = 6, obj_button_h = 100,
    n_threads = 1, pix_per_frame = 5e4, select_rect_size = 75;

const Vector sky_col = {0, 0.5, 0.75}, init_V = {0, 0, 10}, init_screen_tl = {-2, -1.15, 4};
extern const Material glass(0, 0, 1, 1.06), plastic(1, 1, 0, 1.3), std_material = plastic;

bool d = 0;

struct RenderThreadData
{
    OptScene* scene;
    int thread_num;
    std::vector<IntVec>* thread_pix;
};

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



ObjControlPanel::ObjControlPanel(Widget* parent, Vector tl, Vector br, double properties_h)
    : Widget(tl, br, parent), properties_h(properties_h)
{
    prop_cont = new WContainer(this, {0, 0}, {width, properties_h}, OPT_TOTAL, 1);
    button_cont = new WContainer(this, {0, properties_h}, wh, n_move_buttons, 0);
}

void ObjControlPanel::setObject(OptObject* obj)
{
    prop_cont->clearChildren();
    button_cont->clearChildren();

    if (obj != nullptr)
    {
        for (OptProperty prop: obj->getProperties())
            new OptPropWidget(prop_cont, {}, {}, obj, prop);

        new MoveObjectButton(button_cont, {}, {}, obj, {0, 0, -obj_change}, "forward");
        new MoveObjectButton(button_cont, {}, {}, obj, {0, 0, obj_change}, "back");
        new MoveObjectButton(button_cont, {}, {}, obj, {-obj_change, 0, 0}, "left");
        new MoveObjectButton(button_cont, {}, {}, obj, {obj_change, 0, 0}, "right");
        new MoveObjectButton(button_cont, {}, {}, obj, {0, -obj_change, 0}, "up");
        new MoveObjectButton(button_cont, {}, {}, obj, {0, obj_change, 0}, "down");
    }

    prop_cont->drawRec();
    button_cont->drawRec();
}



OptPropWidget::OptPropWidget(Widget* parent, Vector tl, Vector br, OptObject* obj, OptProperty prop)
    : Widget(tl, br, parent)
{
    name_field = new TextField(this, {0, 0}, {width / 2, height}, prop.getName());
    val_field = new OptPropField(this, {width / 2, 0}, {width, height}, obj, prop);
}

OptPropField::OptPropField(OptPropWidget* parent, Vector tl, Vector br, OptObject* obj, OptProperty prop)
    : InputField(parent, tl, br, ""), obj(obj), prop(prop)
{
    std::ostringstream out;
    out.precision(2);
    out << std::fixed << prop.val;
    SetText(std::move(out).str());
}

void OptPropField::action()
{
    obj->setProperty(prop.prop, std::stod(getText()));
    obj->scene->paint();
}



OptObjectButton::OptObjectButton(Widget* parent, Vector tl, Vector br, OptObject* obj, ObjControlPanel* panel)
    : ToggleButton(parent, tl, br, gray_v, obj->name), obj(obj), panel(panel)
{
    //
}

void OptObjectButton::action() { obj->scene->select(obj); }
void OptObjectButton::deactivate() { obj->scene->deselect(obj); }


MoveObjectButton::MoveObjectButton(Widget* parent, Vector tl, Vector br, OptObject* obj,
    Vector change, std::string text)
    : Button(parent, tl, br, gray_v, text), obj(obj), change(change)
{
    //
}

void MoveObjectButton::action()
{
    obj->movePos(change);
    obj->scene->paint();
}

void OptScene::paint()
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

    // redraw_picture = 1;
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

Vector getDiffuseColor(Surface* s, Source* l, Vector p_surface, Vector p_light)
{
    Vector L = p_light - p_surface;
    double cosA = angle(L, s->normal(p_surface));
    return l->color * cosA;
}


OptScene::OptScene(Widget* parent, Vector tl, Vector br, ObjControlPanel* panel) : Widget(tl, br, parent)
{
    V = init_V;
    screen_tl = init_screen_tl;
    screen_w = {screen_size, 0, 0};
    screen_h = {0, screen_size / ratio, 0};

    surfaces = std::vector<Surface*>();
    sources = std::vector<Source*>();
    surfaces.reserve(10);
    sources.reserve(10);

    this->obj_cont = nullptr;
    this->panel = panel;

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

WContainer* OptScene::makeObjectContainer(Widget* parent, Vector tl, Vector br)
{
    int n_objects = surfaces.size() + sources.size();

    this->obj_cont = new WContainer(parent, tl, br, n_objects, 1, obj_button_h * n_objects);

    for (OptObject* obj: surfaces)
        new OptObjectButton(obj_cont, {}, {}, obj, panel);
    for (OptObject* obj: sources)
        new OptObjectButton(obj_cont, {}, {}, obj, panel);

    obj_cont->blockChildrenMouseDown(true);
    return obj_cont;
}

void OptScene::select(OptObject* obj)
{
    selected.insert(obj);
    selected_changed();
}

void OptScene::deselect(OptObject* obj)
{
    selected.erase(obj);
    selected_changed();
}

void OptScene::selected_changed()
{
    if (selected.size() == 1)
        panel->setObject(*selected.begin());
    else
        panel->setObject(nullptr);

    redraw_picture = 0;
    paint();
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

std::vector<Surface*>::iterator OptScene::addSphere(Vector pos, Vector color, double r, Material m)
{
    std::string name = "sphere " + std::to_string(surfaces.size());

    surfaces.push_back(new SphereSurface(pos, r, color, name, this, m));
    return surfaces.end() - 1;
}

std::vector<Source*>::iterator OptScene::addSource(Vector pos, Vector color, double r)
{
    std::string name = "source " + std::to_string(sources.size());
    Source* new_sorce = new SphereSource(color, pos, r, name, this);

    sources.push_back(new_sorce);
    return sources.end() - 1;
}

void OptScene::moveCamera(Vector change)
{
    V += change;
    screen_tl += change;
    paint();
}



MoveCameraButton::MoveCameraButton(Widget* parent, OptScene* scene, Vector change, Vector color, std::string text)
    : Button(parent, Vector(), Vector(), color, text), scene(scene), change(change)
{
    
}

void MoveCameraButton::action()
{
    scene->moveCamera(change);
}
