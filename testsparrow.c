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
  spTranslate(0,0,-5<<SP_ACCURACY);
  spRotate(1<<SP_ACCURACY-1,1<<SP_ACCURACY,1<<SP_ACCURACY-2,SDL_GetTicks()*64);
  spBindTexture(garfield);
  spTriangle3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY,0<<SP_ACCURACY,
                0<<SP_ACCURACY,-1<<SP_ACCURACY,0<<SP_ACCURACY,
                1<<SP_ACCURACY, 1<<SP_ACCURACY,0<<SP_ACCURACY,65535);
  spTriangle3D( 0<<SP_ACCURACY, 0<<SP_ACCURACY,-1<<SP_ACCURACY,
                0<<SP_ACCURACY,-1<<SP_ACCURACY,0<<SP_ACCURACY,
               -1<<SP_ACCURACY, 1<<SP_ACCURACY,0<<SP_ACCURACY,12345);
  spTriangle3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY,0<<SP_ACCURACY,
                1<<SP_ACCURACY, 1<<SP_ACCURACY,0<<SP_ACCURACY,
                0<<SP_ACCURACY, 0<<SP_ACCURACY,-1<<SP_ACCURACY,23456);
  spTriangle3D( 0<<SP_ACCURACY, 0<<SP_ACCURACY,-1<<SP_ACCURACY,
                1<<SP_ACCURACY, 1<<SP_ACCURACY,0<<SP_ACCURACY,
                0<<SP_ACCURACY,-1<<SP_ACCURACY,0<<SP_ACCURACY,34567);
  
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
  spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,0.1,100);
  spLoop(draw_test,calc_test,10,resize);
  SDL_FreeSurface(garfield);
  spQuitCore();
  return 0;
}
