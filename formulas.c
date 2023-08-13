#include "formulas.h"

double d2r(double degree){return degree * (M_PI / 180.0);}

double r2d(double radians){return radians * (180.0 / M_PI);}

int checkOverlap( int object1X, int object1width,
                    int object1Y, int object1height,
                    int object2X, int object2width,
                    int object2Y, int object2height)
{
    int xOverlap, yOverlap;
    int min1st, max1st, min2nd, max2nd;

    min1st = object1X;
    max1st = object1X+object1width;
    min2nd = object2X;
    max2nd = object2X+object2width;
    xOverlap = ((min2nd <= max1st) && (max1st <= max2nd))
                || ((min1st <= min2nd) && (min2nd <= max1st))
                || ((min2nd <= min1st) && (min1st <= max2nd))
                || ((min1st <= max2nd) && (max2nd <= max1st));


    min1st = object1Y;
    max1st = object1Y+object1height;
    min2nd = object2Y;
    max2nd = object2Y+object2height;
    yOverlap = ((min2nd <= max1st) && (max1st <= max2nd))
                || ((min1st <= min2nd) && (min2nd <= max1st))
                || ((min2nd <= min1st) && (min1st <= max2nd))
                || ((min1st <= max2nd) && (max2nd <= max1st));

    return (xOverlap && yOverlap);

}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

switch (bpp)
{
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
      }
}

int isOpaque(SDL_Surface* surface, int x, int y){
    SDL_Color rgba;
    Uint32 data = getpixel(surface, x, y);
    SDL_GetRGBA(data, surface->format, &rgba.r, &rgba.g, &rgba.b, &rgba.a);

    return rgba.a != 0;
}

//returns the normal angle
//angle in degrees, as always
double normal(SDL_Surface* surface, double x, double y, double angle){
    #define DELTA (10.0)
    #define STEP (10)
    //angle = fmod(angle + 270, 360);
    double tempX, tempY, r;
    //clockwise, first point
    double x1, y1;
    for (int i = 0; i < 180; i += STEP){
        r = d2r(fmod((angle + i + 360), 360.0));
        tempX = x + cos(r) * DELTA;
        tempY = y + sin(r) * DELTA;
        
        if (isOpaque(surface, round(tempX), round(tempY))){
            x1 = tempX, y1 = tempY;
            break;
        }
    }
    //anticlockwise, same thing repeated code yay
    double x2, y2;
    for (int i = 0; i < 180; i += STEP){
        r = d2r(fmod((angle - i + 360), 360.0));
        tempX = x + cos(r) * DELTA;
        tempY = y + sin(r) * DELTA;
        if (isOpaque(surface, round(tempX), round(tempY))){
            x2 = tempX, y2 = tempY;
            break;
        }
    }
    double result = r2d(atan2(y1 - y2, x1 - x2));
    return fmod((result + 270), 360.0);
}

//again, given in degrees
void drawAngleOfIncidence(SDL_Renderer* renderer, double x, double y, double angle, double normal, int draw_tangent, int draw_reflection){
    #define TANGENT_RADIUS (3.0)
    #define LINE_RADIUS (5.0)
    if (draw_tangent){
        
        double r = d2r(fmod(normal + 90, 360));
        for (int i = 0; i < 5; i++){
            SDL_SetRenderDrawColor(renderer, 68, 255, 112, 50 + 20 * i);
            //SDL_SetRenderDrawColor(renderer, 106, 199, 201, 50 + 20 * i);
            double xDir = cos(r) * TANGENT_RADIUS * (5 - i);
            double yDir = sin(r) * TANGENT_RADIUS * (5 - i);
            SDL_RenderDrawLine(renderer, round(x - xDir), round(y - yDir), round(x + xDir), round(y + yDir));
        }
    }

    //reflected is always smaller than 90 due to maths
    if (draw_reflection){
        
        double diff = fmod(angle - normal + 360, 360);
        double r = d2r(fmod(normal - diff + 360, 360));
        double xDir = cos(r) * LINE_RADIUS;
        double yDir = sin(r) * LINE_RADIUS;
        for (int i = 0; i < 6; i++){
            SDL_SetRenderDrawColor(renderer, 255, 120, 48, 150 - 20 * i);
            SDL_RenderDrawLine(renderer, round(x), round(y), round(x + xDir), round(y + yDir));
            x += xDir, y += yDir;
        }
    }
}

double matrixRotate(double x, double y, double angle, int is_x){
    double r = d2r(angle);
    if (is_x) return x * cos(r) - y * sin(r);
    else return x * sin(r) + y * cos(r);
}
