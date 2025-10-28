#include "sdl-adapter.h"

#include <cmath>
#include <cassert>

#include <cstdio>

const double Pi = 3.1415926;
static SDL_Renderer* rend = nullptr;

void setRenderer(SDL_Renderer* renderer) { rend = renderer; }
void setColor(Vector color) { SDL_SetRenderDrawColor(rend, color.x, color.y, color.z, 255); }

void drawPoint(Vector p)
{
    SDL_RenderPoint(rend, p.x, p.y);
}

void drawLine(Vector p1, Vector p2)
{
    SDL_RenderLine(rend, p1.x, p1.y, p2.x, p2.y);
}

void drawRect(Vector tl, Vector br, bool fill)
{
    SDL_FRect rect;
    rect.x = tl.x;
    rect.y = tl.y;
    rect.w = br.x - tl.x;
    rect.h = br.y - tl.y;

    if (fill) SDL_RenderFillRect(rend, &rect);
    else SDL_RenderRect(rend, &rect);
}

void putText(std::string text, Vector tl, Vector br)
{
    double scale = 2, lft_pad = 5, text_h = 5;

    SDL_SetRenderScale(rend, scale, scale);
    SDL_RenderDebugText(rend, (tl.x + lft_pad) / scale, ((tl.y + br.y) / 2 - text_h) / scale, text.c_str());
    SDL_SetRenderScale(rend, 1, 1);
}

void drawCircle(Vector centre, int r, bool fill)
{
    if (fill)
    {
        for (int x = centre.x - r; x <= centre.x + r; ++x)
        {
            for (int y = centre.y - r; y <= centre.y + r; ++y)
            {
                if ((x - centre.x) * (x - centre.x) + (y - centre.y) * (y - centre.y) < r * r)
                    SDL_RenderPoint(rend, x, y);
            }
        }

        return;
    }

    double angleStep = 0.01;
    int nSteps = 3.1415 * 2 / angleStep;
    FixedVec v{Vector(centre.x, centre.y), Vector(centre.x, centre.y) + Vector(r, 0)};

    while (nSteps--)
    {
        SDL_RenderPoint(rend, v.p2.x, v.p2.y);
        v = rotateV(v, angleStep);
    }
}

void fillConvexPolygon(std::vector<Vector> points)
{
    // assume points go in clockwise order
    int nPts = points.size();
    assert(nPts >= 3);

    Vector tl = points[0], br = points[0];
    for (int i = 0; i < nPts; ++i)
    {
        tl.x = std::min(tl.x, points[i].x);
        tl.y = std::min(tl.y, points[i].y);
        br.x = std::max(br.x, points[i].x);
        br.y = std::max(br.y, points[i].y);
    }

    for (int x = tl.x; x <= br.x; ++x)
    {
        for (int y = tl.y; y <= br.y; ++y)
        {
            bool inPolygon = 1;
            for (int i = 0; i < nPts; ++i)
            {
                Vector pt1 = points[i], pt2 = points[(i + 1) % nPts];
                Vector v1 = pt2 - pt1, v2 = Vector(x, y) - pt2;

                double arg1 = std::atan2(-v1.y, v1.x), arg2 = std::atan2(-v2.y, v2.x);
                double angle = arg1 - arg2; // in [-2pi, 2pi]
                if (-Pi <= angle && angle <= 0 || Pi <= angle && angle <= 2 * Pi)
                {
                    inPolygon = 0;
                    break;
                }
            }

            if (inPolygon)
                SDL_RenderPoint(rend, x, y);
        }
    }

}