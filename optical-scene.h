#ifndef OPTICAL_SCENE_H
#define OPTICAL_SCENE_H

#include <set>

#include "optical-object.h"
// #include "wcontainer.h"
// #include "plane.h"
// #include "button.h"
// #include "scroll.h"

#include "widget.hpp"

// class OptController;

using SurfaceIt = std::vector<Surface*>::iterator;
using SourceIt = std::vector<Source*>::iterator;

extern const double ratio;



Vector getDiffuseColor(Surface* s, Source* l, Vector p_surface, Vector p_light);

class OptScene : public hui::Widget
{
public:
    // OptScene(Widget* parent, Vector tl, Vector br, OptController* control);
    OptScene(hui::State* state, hui::Widget* parent, dr4::Vec2f pos, dr4::Vec2f size);
    void updateTexture();
    // virtual bool onIdle(IdleEvent* evt) override;

    virtual void Redraw() override;

    Vector screen_to_pixels(Vector p);
    Vector pixels_to_screen(IntVec pix);

    FixedVec getRect(OptObject* obj);

    void setV(Vector V);
    void moveCamera(Vector change);

    Vector traceRay(Ray ray, int depth);
    Surface* getIntersectedSurface(Ray ray, double *t_ptr);
    Vector castShadowRay(Surface* s, Source* l, Vector p);
    Vector traceDiffuse(Surface* s, Vector p);
    Vector traceRefract(Surface* s, Ray ray, Vector p, int depth);

// private:
    bool redraw_picture;
    Vector V, screen_tl, screen_w, screen_h;

    std::vector<Surface*> surfaces;
    std::vector<Source*> sources;
    std::set<OptObject*> selected;

    // OptController* control;
    // std::vector<IntVec> pix_queue;
    // PixelTexture *pix_texture;
};

#endif // OPTICAL_SCENE_H