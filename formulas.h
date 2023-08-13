#ifndef FORMULAS_H_INCLUDED
#define FORMULAS_H_INCLUDED
#include "math.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL2_gfxPrimitives.h"


double d2r(double degree);

double r2d(double radians);

int checkOverlap( int object1X, int object1width,
                    int object1Y, int object1height,
                    int object2X, int object2width,
                    int object2Y, int object2height);

Uint32 getpixel(SDL_Surface* surface, int x, int y);

double matrixRotate(double x, double y, double angle, int is_X);

int isOpaque(SDL_Surface* surface, int x, int y);

double normal(SDL_Surface* surface, double x, double y, double angle);
void drawAngleOfIncidence(SDL_Renderer* renderer, double x, double y, double angle, double normal, int draw_tangent, int draw_reflection);

#endif //FORMULAS_H_INCLUDED
