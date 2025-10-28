#ifndef OPTICAL_SCENE_H
#define OPTICAL_SCENE_H

#include <set>

#include "optical-object.h"

#include "wcontainer.h"
#include "plane.h"
#include "button.h"
#include "scroll.h"

/*
multithreading segfault
better text rendering
3 views
propagate texture transformation
*/

class OptController;

using SurfaceIt = std::vector<Surface*>::iterator;
using SourceIt = std::vector<Source*>::iterator;

extern const double ratio;



Vector getDiffuseColor(Surface* s, Source* l, Vector p_surface, Vector p_light);

class OptScene : public Widget
{
public:
    OptScene(Widget* parent, Vector tl, Vector br, OptController* control);
    virtual void updateTexture() override;
    virtual bool onIdle(IdleEvent* evt) override;

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

    OptController* control;
    std::vector<IntVec> pix_queue;
    PixelTexture *pix_texture;
};

#endif // OPTICAL_SCENE_H