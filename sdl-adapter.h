#ifndef MY_SDL_ADAPTER_H
#define MY_SDL_ADAPTER_H

#include "myvector.h"

#include <SDL3/SDL.h>
#include <vector>
#include <string>

void setRenderer(SDL_Renderer* renderer);
void setColor(Vector color);

void drawPoint(Vector p);
void drawLine(Vector p1, Vector p2);
void drawRect(Vector tl, Vector br, bool fill);

void drawCircle(Vector centre, int r, bool fill);
void fillConvexPolygon(std::vector<Vector> points);

void putText(std::string text, Vector tl, Vector br);

#endif // MY_SDL_ADAPTER_H