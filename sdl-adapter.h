#ifndef MY_SDL_ADAPTER_H
#define MY_SDL_ADAPTER_H

#include <SDL3/SDL.h>

#include <vector>

#include "myvector.h"

void setRenderer(SDL_Renderer* renderer);

void setColor(Vector color);
void drawCircle(IntVec centre, int r, Vector color, bool fill);
void fillConvexPolygon(std::vector<IntVec> points, Vector color);

#endif // MY_SDL_ADAPTER_H