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
  //int thetime = SDL_GetTicks();
  spResetZBuffer();
  spClearTarget(0);
  //printf("                                       clear time: %i ms\n",SDL_GetTicks()-thetime);
  int i;
  /*SDL_LockSurface(spGetWindowSurface());
  Uint16* pixel = (Uint16*)(spGetWindowSurface()->pixels);
  for (i = 0; i < 1000; i++) 
    pixel[rand()%(spGetWindowSurface()->w*spGetWindowSurface()->h)] = rand()%65536;
  SDL_UnlockSurface(spGetWindowSurface());*/
  //thetime = SDL_GetTicks();
  spBindTexture(garfield);
  spTriangle_tex(100,100,-100,0,0,300,50,-100,garfield->w-1,0,200,200,-100,0,garfield->h-1,12345);
  spTriangle_tex(300,50,-100,garfield->w-1,0,200,200,-100,0,garfield->h-1,300,200,-100,garfield->w-1,garfield->h-1,12345);
  //printf("                  render time: %i ms\n",SDL_GetTicks()-thetime);
  //spTriangle(120,50,-150,270,100,-70,150,220,20,54321);
  //thetime = SDL_GetTicks();
  spFlip();
  //printf("flip time: %i ms\n",SDL_GetTicks()-thetime);
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
}


int main(int argc, char **argv)
{
  spInitCore();
  screen = spCreateWindow();
  spSelectRenderTarget(screen);
  SDL_Surface* surface = IMG_Load("./data/garfield.png");
  garfield = SDL_DisplayFormat(surface);
  spLoop(draw_test,calc_test,10,resize);
  SDL_FreeSurface(garfield);
  spQuitCore();
  return 0;
}
