#ifndef _GRAPHICSTUFF_H
#define _GRAPHICSTUFF_H
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include <string.h>

/*#define ACCURACY 10
#define HALF_ACCURACY 5
#define ACCURACY_FACTOR 1024.0
#define MY_PI 3217*/

#define ACCURACY 16
#define HALF_ACCURACY 8
#define ACCURACY_FACTOR 65536.0
#define MY_PI 3217*64
#define COS_ACCURACY 4

Sint32* engineGetProjectionMatrix();

Sint32* engineGetModellViewMatrix();

void engineSetWindowX(int x);

int engineGetWindowX();

void engineSetWindowY(int y);

int engineGetWindowY();

void initMath();

Sint32 mycos(Sint32 w);

Sint32 mysin(Sint32 w);

Sint32 fpsqrt (Sint32 n);

void setModellViewMatrixIdentity();

void drawtext(SDL_Surface *screen,Sint32 x,Sint32 y,char* text,SDL_Surface *keymap);

void drawtextMX(SDL_Surface *screen,Sint32 x,Sint32 y,char* text,SDL_Surface *keymap);

void drawtextMY(SDL_Surface *screen,Sint32 x,Sint32 y,char* text,SDL_Surface *keymap);

void drawtextMXMY(SDL_Surface *screen,Uint16 x,Uint16 y,char* text,SDL_Surface *keymap);

void setFrustumf2(Sint32 *matrix, Sint32 left, Sint32 right, Sint32 bottom, Sint32 top,
                  Sint32 znear, Sint32 zfar);
                  
void setPerspective(float fovyInDegrees, float aspectRatio,
                      float znear, float zfar);
    
void hline(SDL_Surface* screen,Sint32 x,Sint32 y,Sint32 l_,Uint16 color_,char check);

void triangle(SDL_Surface* screen,Sint16 x1,Sint16 y1,Sint16 x2,Sint16 y2,Sint16 x3,Sint16 y3,Sint16 color);

void quad(SDL_Surface* screen,Sint16 x1,Sint16 y1,Sint16 x2,Sint16 y2,Sint16 x3,Sint16 y3,Sint16 x4,Sint16 y4,Uint16 color);

void clearScreen(SDL_Surface* screen,Uint16 color);

void ellipse(SDL_Surface* screen,Sint16 mx,Sint16 my,Sint16 rx,Sint16 ry,Uint16 color);

void engineSetWindowXasm(int x);

void engineSetWindowYasm(int y);

#endif
