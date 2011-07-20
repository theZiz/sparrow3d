/*
 The contents of this file are subject to the Mozilla Public License        
 Version 1.1 (the "License"); you may not use this file except in           
 compliance with the License. You may obtain a copy of the License at       
 http://www.mozilla.org/MPL/                                                
                                                                            
 Software distributed under the License is distributed on an "AS IS"        
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the    
 License for the specific language governing rights and limitations         
 under the License.                                                         
                                                                            
 Alternatively, the contents of this file may be used under the terms       
 of the GNU Lesser General Public license (the  "LGPL License"), in which case the  
 provisions of LGPL License are applicable instead of those                  
 above.                                                                     
                                                                            
 For feedback and questions about my Files and Projects please mail me,     
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com                         
*/
#ifndef _GRAPHICSTUFF_H
#define _GRAPHICSTUFF_H
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
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

void setLetterSpacing(int* point0r);

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
    
void hline(Uint16* pixel,Sint32 x,Sint32 y,Sint32 l_,Uint16 color_,char check);

void triangle(SDL_Surface* screen,Sint16 x1,Sint16 y1,Sint16 x2,Sint16 y2,Sint16 x3,Sint16 y3,Uint16 color);

void quad(SDL_Surface* screen,Sint16 x1,Sint16 y1,Sint16 x2,Sint16 y2,Sint16 x3,Sint16 y3,Sint16 x4,Sint16 y4,Uint16 color);

void clearScreen(SDL_Surface* screen,Uint16 color);

void ellipse(SDL_Surface* screen,Sint16 mx,Sint16 my,Sint16 rx,Sint16 ry,Uint16 color);

void engineSetWindowXasm(int x);

void engineSetWindowYasm(int y);

int fpdiv(int numerator,int denominator);
#endif
