#ifndef MY_SDL_ADAPTER_H
#define MY_SDL_ADAPTER_H

#include "myvector.h"

#include <SDL3/SDL.h>
#include <vector>
#include <string>

void setRenderer(SDL_Renderer* renderer);
void setColor(Vector color);

void drawPoint(IntVec p);
void drawLine(IntVec p1, IntVec p2);
void drawRect(IntVec tl, IntVec br, bool fill);

void drawCircle(IntVec centre, int r, bool fill);
void fillConvexPolygon(std::vector<IntVec> points);

void putText(std::string text, IntVec tl, IntVec br);

#endif // MY_SDL_ADAPTER_H