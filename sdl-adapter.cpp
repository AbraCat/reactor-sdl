#include "sdl-adapter.h"

#include <cmath>

static SDL_Renderer* rend = nullptr;

void setRenderer(SDL_Renderer* renderer) { rend = renderer; }

void setColor(Vector color) { SDL_SetRenderDrawColor(rend, color.x, color.y, color.z, 255); }

void drawCircle(IntVec centre, int r, Vector color)
{
    setColor(color);

    for (int x = centre.x - r; x <= centre.x + r; ++x)
    {
        // (x - x0)^2 + (y - y0)^2 = r^2
        int yUp = centre.y + std::sqrt(r * r - (centre.x - x) * (centre.x - x));
        int yDown = centre.y - std::sqrt(r * r - (centre.x - x) * (centre.x - x));

        SDL_RenderPoint(rend, x, yUp);
        SDL_RenderPoint(rend, x, yDown);
    }
}

void fillConvexPolygon(std::vector<IntVec> points, Vector color)
{
    //
}