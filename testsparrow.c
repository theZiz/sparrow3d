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
#include "../sparrow3d/sparrow3d.h"
#include <SDL_image.h>

SDL_Surface* screen;
SDL_Surface* garfield;

void draw_test(void)
{
  spResetZBuffer();
  spClearTarget(0);
  spIdentity();
  spTranslate(spSin(SDL_GetTicks()*25),spSin(SDL_GetTicks()*16),(-5<<SP_ACCURACY));
  spRotate(1<<SP_ACCURACY-1,0,0,SDL_GetTicks()*32);
  spRotate(0,1<<SP_ACCURACY,0,SDL_GetTicks()*32);
  spRotate(0,0,1<<SP_ACCURACY-2,SDL_GetTicks()*32);
  
  spBindTexture(garfield);
  //Front / Back
  spTranslate(-3<<SP_ACCURACY,0,0);
  spQuadTex3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,0,garfield->h-1,
              -1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,0,0,
               1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,garfield->w-1,0,
               1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,garfield->w-1,garfield->h-1,12345 | 31727);
  spQuadTex3D( 1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,0,garfield->h-1,
               1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,0,0,
              -1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,garfield->w-1,0,
              -1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,garfield->w-1,garfield->h-1,23456 | 31727);
  //Left / Right
  spQuadTex3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,0,garfield->h-1,
              -1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,0,0,
              -1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,garfield->w-1,0,
              -1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,garfield->w-1,garfield->h-1,34567 | 31727);
  spQuadTex3D( 1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,0,garfield->h-1,
               1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,0,0,
               1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,garfield->w-1,0,
               1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,garfield->w-1,garfield->h-1,45678 | 31727);  
  //Up / Down
  spQuadTex3D( 1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,0,garfield->h-1,
               1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,0,0,
              -1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,garfield->w-1,0,
              -1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,garfield->w-1,garfield->h-1,56789 | 31727);
  spQuadTex3D(-1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,0,garfield->h-1,
              -1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,0,0,
               1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,garfield->w-1,0,
               1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,garfield->w-1,garfield->h-1,61234 | 31727);
  //Front / Back
  spTranslate( 3<<SP_ACCURACY,0,0);
  spQuad3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,
           -1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,
            1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,
            1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,12345 | 31727);
  spQuad3D( 1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,
            1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
           -1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
           -1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,23456 | 31727);
  //Left / Right
  spQuad3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,
           -1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,
           -1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
           -1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,34567 | 31727);
  spQuad3D( 1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,
            1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
            1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,
            1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,45678 | 31727);  
  //Up / Down
  spQuad3D( 1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,
            1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,
           -1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,
           -1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,56789 | 31727);
  spQuad3D(-1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,
           -1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
            1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
            1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,61234 | 31727);
               
  spFlip();
}

int calc_test(Uint32 steps)
{
  if (spGetInput()->button[SP_BUTTON_START])
    return 1;
  return 0; 
}

void resize(Uint16 w,Uint16 h)
{
  spSelectRenderTarget(spGetWindowSurface());
  spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,0.1,100);
}


int main(int argc, char **argv)
{
  spInitCore();
  spInitMath();
  screen = spCreateWindow();
  spSelectRenderTarget(screen);
  SDL_Surface* surface = IMG_Load("./data/garfield.png");
  garfield = SDL_DisplayFormat(surface);
  spBindTexture(garfield);
  spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,0.1,100);
  spLoop(draw_test,calc_test,10,resize);
  SDL_FreeSurface(garfield);
  spQuitCore();
  return 0;
}
