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

SDL_Surface* screen;

void draw_test(void)
{

  spFlip();
}

int calc_test(Uint32 steps)
{
  int i;
  SDL_LockSurface(spGetWindowSurface());
  Uint16* pixel = (Uint16*)(spGetWindowSurface()->pixels);
  for (i = 0; i < steps; i++) 
    pixel[rand()%(spGetWindowSurface()->w*spGetWindowSurface()->h)] = rand()%65536;
  SDL_UnlockSurface(spGetWindowSurface());
  if (spGetInput()->button[SP_BUTTON_START])
    return 1;
  return 0; 
}

int main(int argc, char **argv)
{
  spInitCore();
  screen = spCreateWindow();
  spLoop(draw_test,calc_test,10);
  spQuitCore();
  return 0;
}
