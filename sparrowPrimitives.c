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
#include "sparrowPrimitives.h"

SDL_Surface* spRenderTarget = NULL;
SDL_Surface* spTexture = NULL;
char spZTest = 1;
char spZSet = 1;
Sint32* spZBuffer = NULL;
Uint16 spTargetX = 0;
Uint16 spTargetY = 0;

PREFIX void spSelectRenderTarget(SDL_Surface* target)
{
  spRenderTarget = target;
  spTargetX = target->w;
  spTargetY = target->h;
  spReAllocateZBuffer();
}

PREFIX void spBindTexture(SDL_Surface* texture)
{
  spTexture = texture;
}

PREFIX void spSetZTest(char test)
{
  spZTest = test;
}

PREFIX void spSetZSet(char test)
{
  spZSet = test;
}

PREFIX void spClearTarget(Uint16 color)
{
  SDL_FillRect(spRenderTarget,NULL,color);
}

void draw_and_share_X(Uint16* pixel,Sint16 x1,Sint16 z1,Sint16 x2,Sint16 z2,Sint16 y,Uint16 color)
{
  if (x1 == x2)
    return;
  Sint16 nx = x1+x2>>1;
  if (nx == x1)
    return;
  if (nx == x2)
    return;
  Sint16 nz = z1+z2>>1;
  if (nx>=0 && y>=0 && nx<spTargetX && y<spTargetY && (!spZTest || (nz<0 && nz > spZBuffer[nx+y*spTargetX])))
  {
    pixel[nx+y*spTargetX] = color;
    if (spZSet)
     spZBuffer[nx+y*spTargetX] = nz;
  }
  draw_and_share_X(pixel,x1,z1,nx,nz,y,color);
  draw_and_share_X(pixel,nx,nz,x2,z2,y,color);
}


void draw_and_share(Uint16* pixel,Sint16 x1_l,Sint16 z1_l,Sint16 x1_r,Sint16 z1_r,Sint16 y1,Sint16 x2_l,Sint16 z2_l,Sint16 x2_r,Sint16 z2_r,Sint16 y2,Uint16 color)
{
  if (y1 == y2)
    return;
  int ny = y1+y2>>1;
  if (ny == y1)
    return;
  if (ny == y2)
    return;
  Sint16 nx_l = x1_l+x2_l>>1;
  Sint16 nx_r = x1_r+x2_r>>1;
  Sint16 nz_l = z1_l+z2_l>>1;
  Sint16 nz_r = z1_r+z2_r>>1;

  if (nx_l < nx_r)
    draw_and_share_X(pixel,nx_l-1,nz_l,nx_r+1,nz_r,ny,color);
  else
    draw_and_share_X(pixel,nx_r-1,nz_r,nx_l+1,nz_l,ny,color);

  draw_and_share(pixel,x1_l,z1_l,x1_r,z1_r,y1,nx_l,nz_l,nx_r,nz_r,ny,color);
  draw_and_share(pixel,nx_l,nz_l,nx_r,nz_r,ny,x2_l,z2_l,x2_r,z2_r,y2,color);
}

PREFIX void spTriangle(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 x3, Sint16 y3, Sint16 z3, Uint16 color)
{
  if (y1 > y2)
  {
      Sint16 temp = y1;
      y1 = y2;
      y2 = temp;
      temp = x1;
      x1 = x2;
      x2 = temp;
  }
  if (y1 > y3)
  {
      Sint16 temp = y1;
      y1 = y3;
      y3 = temp;
      temp = x1;
      x1 = x3;
      x3 = temp;
  }
  if (y2 < y3)
  {
      Sint16 temp = y2;
      y2 = y3;
      y3 = temp;
      temp = x2;
      x2 = x3;
      x3 = temp;
  }
  
  
  SDL_LockSurface(spRenderTarget);
  Uint16* pixel=(Uint16*)spRenderTarget->pixels;
  
  
  int div = y2-y1;
  if (div!=0)
  {
    if (div<0)
      div = -div;
    int mul = y3-y1;
    if (mul < 0)
      mul = -mul;
    Sint16 x4 = x1+(x2-x1)*mul/div;
    Sint16 z4 = z1+(z2-z1)*mul/div;
    if (x3 < x4)
      draw_and_share_X(pixel,x3-1,z3,x4+1,z4,y3,color);
    else
      draw_and_share_X(pixel,x4-1,z4,x3+1,z3,y3,color);
    
    draw_and_share(pixel,x1,z1,x1,z1,y1,x4,z4,x3,z3,y3,color);
    draw_and_share(pixel,x4,z4,x3,z3,y3,x2,z2,x2,z2,y2,color);
  }

  
  SDL_UnlockSurface(spRenderTarget);
}

PREFIX void spTriangle_alpha(Sint16 x1, Sint16 y1, Sint16 z1, Uint16 c1, Uint8 a1, Sint16 x2, Sint16 y2, Sint16 z2, Uint16 c2, Uint8 a2, Sint16 x3, Sint16 y3, Sint16 z3, Uint16 c3, Uint8 a3)
{

}

PREFIX void spTriangle_tex(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Uint16 c1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Uint16 c2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Uint16 c3)
{

}

PREFIX void spTriangle_tex_alpha(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Uint16 c1, Uint8 a1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Uint16 c2, Uint8 a2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Uint16 c3, Uint8 a3)
{

}

PREFIX void spQuad(Sint16 x1, Sint16 y1, Sint16 z1, Uint16 c1, Sint16 x2, Sint16 y2, Sint16 z2, Uint16 c2, Sint16 x3, Sint16 y3, Sint16 z3, Uint16 c3, Sint16 y4, Sint16 z4, Uint16 c4)
{

}

PREFIX void spQuad_alpha(Sint16 x1, Sint16 y1, Sint16 z1, Uint16 c1, Uint8 a1, Sint16 x2, Sint16 y2, Sint16 z2, Uint16 c2, Uint8 a2, Sint16 x3, Sint16 y3, Sint16 z3, Uint16 c3, Uint8 a3, Sint16 x4, Sint16 y4, Sint16 z4, Uint16 c4, Uint8 a4)
{

}

PREFIX void spQuad_tex(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Uint16 c1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Uint16 c2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Uint16 c3, Sint16 x4, Sint16 y4, Sint16 z4, Sint16 u4, Sint16 v4, Uint16 c4)
{

}

PREFIX void spQuad_tex_alpha(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Uint16 c1, Uint8 a1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Uint16 c2, Uint8 a2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Uint16 c3, Uint8 a3, Sint16 x4, Sint16 y4, Sint16 z4, Sint16 u4, Sint16 v4, Uint16 c4, Uint8 a4)
{

}


PREFIX void spReAllocateZBuffer()
{
  if (spZBuffer)
    free(spZBuffer);
  spZBuffer = (Sint32*)malloc(spTargetX*spTargetY*sizeof(Sint32));
}

PREFIX void spResetZBuffer()
{
  int i;
  if (spZBuffer)
    for (i = 0; i<spTargetX*spTargetY; i++)
      spZBuffer[i]=-32768;
}

PREFIX Sint32* spGetZBuffer()
{
  return spZBuffer;
}
