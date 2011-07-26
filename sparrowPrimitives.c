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
#include <stdlib.h>

SDL_Surface* spTarget = NULL;
Uint16* spTargetPixel = NULL;
SDL_Surface* spTexture = NULL;
Uint16* spTexturePixel = NULL;
char spZTest = 1;
char spZSet = 1;
Sint32* spZBuffer = NULL;
Uint16 spTargetX = 0;
Uint16 spTargetY = 0;
Uint16 spTextureX = 0;
Uint16 spTextureY = 0;

PREFIX void spSelectRenderTarget(SDL_Surface* target)
{
  spTarget = target;
  spTargetX = target->w;
  spTargetY = target->h;
  spTargetPixel=(Uint16*)target->pixels;
  spReAllocateZBuffer();
}

PREFIX void spBindTexture(SDL_Surface* texture)
{
  spTexture = texture;
  spTextureX = texture->w;
  spTextureY = texture->h;
  spTexturePixel=(Uint16*)texture->pixels;
}

PREFIX void spClearTarget(Uint16 color)
{
  SDL_FillRect(spTarget,NULL,color);
}

void draw_and_share_X(Sint16 x1,Sint16 z1,Sint16 x2,Sint16 z2,Sint16 y,Uint16 color)
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
    spTargetPixel[nx+y*spTargetX] = color;
    if (spZSet)
     spZBuffer[nx+y*spTargetX] = nz;
  }
  draw_and_share_X(x1,z1,nx,nz,y,color);
  draw_and_share_X(nx,nz,x2,z2,y,color);
}


void draw_and_share(Sint16 x1_l,Sint16 z1_l,Sint16 x1_r,Sint16 z1_r,Sint16 y1,Sint16 x2_l,Sint16 z2_l,Sint16 x2_r,Sint16 z2_r,Sint16 y2,Uint16 color)
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
    draw_and_share_X(nx_l-1,nz_l,nx_r+1,nz_r,ny,color);
  else
    draw_and_share_X(nx_r-1,nz_r,nx_l+1,nz_l,ny,color);

  draw_and_share(x1_l,z1_l,x1_r,z1_r,y1,nx_l,nz_l,nx_r,nz_r,ny,color);
  draw_and_share(nx_l,nz_l,nx_r,nz_r,ny,x2_l,z2_l,x2_r,z2_r,y2,color);
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
  
  
  SDL_LockSurface(spTarget);

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
      draw_and_share_X(x3-1,z3,x4+1,z4,y3,color);
    else
      draw_and_share_X(x4-1,z4,x3+1,z3,y3,color);
    
    draw_and_share(x1,z1,x1,z1,y1,x4,z4,x3,z3,y3,color);
    draw_and_share(x4,z4,x3,z3,y3,x2,z2,x2,z2,y2,color);
  }

  
  SDL_UnlockSurface(spTarget);
}

inline void draw_pixel_tex_ztest_zset(Sint16 const &x,Sint16 const &y,Sint16 const &z,Sint16 const &u,Sint16 const &v,Uint16 const &color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY && (z<0 && z > spZBuffer[x+y*spTargetX]))
  {
    spTargetPixel[x+y*spTargetX] = spTexturePixel[u+v*spTextureX];//color;
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel_tex_ztest(Sint16 const &x,Sint16 const &y,Sint16 const &z,Sint16 const &u,Sint16 const &v,Uint16 const &color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY && (z<0 && z > spZBuffer[x+y*spTargetX]))
  {
    spTargetPixel[x+y*spTargetX] = spTexturePixel[u+v*spTextureX];//color;
  }
}

inline void draw_pixel_tex_zset(Sint16 const &x,Sint16 const &y,Sint16 const &z,Sint16 const &u,Sint16 const &v,Uint16 const &color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY)
  {
    spTargetPixel[x+y*spTargetX] = spTexturePixel[u+v*spTextureX];//color;
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel_tex(Sint16 const &x,Sint16 const &y,Sint16 const &z,Sint16 const &u,Sint16 const &v,Uint16 const &color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY)
  {
    spTargetPixel[x+y*spTargetX] = spTexturePixel[u+v*spTextureX];//color;
  }
}

//void (*draw_pixel_tex_func)(Sint16 const &x,Sint16 const &y,Sint16 const &z,Sint16 const &u,Sint16 const &v,Uint16 const &color) = draw_pixel_tex_ztest_zset;

inline void draw_line(Sint16 const &x1,Sint16 const &z1,Sint16 const &u1,Sint16 const &v1,Sint16 const &x2,Sint16 const &z2,Sint16 const &u2,Sint16 const &v2,Sint16 const &y,Uint16 const &color,Sint32 const &sU,Sint32 const &sV)
{
  Sint32 u = u1<<SP_ACCURACY;
  Sint32 v = v1<<SP_ACCURACY;
  for (int x = x1; x<= x2; x++)
  {
    draw_pixel_tex(x,y,0,u>>SP_ACCURACY,v>>SP_ACCURACY,color);
    u += sU;
    v += sV;
  }
}

PREFIX void spTriangle_tex(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
  if (y1 > y2)
  {
      Sint16 temp = y1;
      y1 = y2;
      y2 = temp;
      temp = x1;
      x1 = x2;
      x2 = temp;
      temp = u1;
      u1 = u2;
      u2 = temp;
      temp = v1;
      v1 = v2;
      v2 = temp;
  }
  if (y1 > y3)
  {
      Sint16 temp = y1;
      y1 = y3;
      y3 = temp;
      temp = x1;
      x1 = x3;
      x3 = temp;
      temp = u1;
      u1 = u3;
      u3 = temp;
      temp = v1;
      v1 = v3;
      v3 = temp;
  }
  if (y2 < y3)
  {
      Sint16 temp = y2;
      y2 = y3;
      y3 = temp;
      temp = x2;
      x2 = x3;
      x3 = temp;
      temp = u2;
      u2 = u3;
      u3 = temp;
      temp = v2;
      v2 = v3;
      v3 = temp;
  }
  
  
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint16 z4 = z1;
  Sint16 u4 = u1;
  Sint16 v4 = v1;  
  int div = y2-y1;
  if (div!=0)
  {
    if (div<0)
      div = -div;
    int mul = y3-y1;
    if (mul < 0)
      mul = -mul;
    Sint32 mul32 = (mul<<SP_ACCURACY)/div;
    x4 = x1+((x2-x1)*mul32>>SP_ACCURACY);
    y4 = y3;
    z4 = z1+((z2-z1)*mul32>>SP_ACCURACY);
    u4 = u1+((u2-u1)*mul32>>SP_ACCURACY);
    v4 = v1+((v2-v1)*mul32>>SP_ACCURACY);
  }
  Sint32 xl = x1<<SP_ACCURACY;
  Sint32 ul = u1<<SP_ACCURACY;
  Sint32 vl = v1<<SP_ACCURACY;
  Sint32 sX_l = 0;
  Sint32 sU_l = 0;
  Sint32 sV_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    sU_l = (u1-u2)*mul;
    sV_l = (v1-v2)*mul;
  }

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 sX_r = 0;
  Sint32 sU_r = 0;
  Sint32 sV_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sU_r = (u1-u3)*mul;
    sV_r = (v1-v3)*mul;
  }

  if (x2 < x3)
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
    }
    for (int y = y1; y < y3; y++)
    {      
      draw_line(xl>>SP_ACCURACY,0>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,
                xr>>SP_ACCURACY,0>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,y,color,sU,sV);
      xl += sX_l;
      ul += sU_l;
      vl += sV_l;
      xr += sX_r;
      ur += sU_r;
      vr += sV_r;
    }
  }
  else
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
    }    
    for (int y = y1; y < y3; y++)
    {
      draw_line(xr>>SP_ACCURACY,0>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,
                xl>>SP_ACCURACY,0>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,y,color,sU,sV);
      xl += sX_l;
      ul += sU_l;
      vl += sV_l;
      xr += sX_r;
      ur += sU_r;
      vr += sV_r;
    }
  }
  xr = x3 << SP_ACCURACY;
  ur = u3 << SP_ACCURACY;
  vr = v3 << SP_ACCURACY;
  sX_r = 0;
  sU_r = 0;
  sV_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
    sU_r = (u2-u3)*mul;
    sV_r = (v2-v3)*mul;
  }
  
  if (x2 < x3)
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
    }
    for (int y = y3; y <= y2; y++)
    {
      draw_line(xl>>SP_ACCURACY,0>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,
                xr>>SP_ACCURACY,0>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,y,color,sU,sV);
      xl += sX_l;
      ul += sU_l;
      vl += sV_l;
      xr += sX_r;
      ur += sU_r;
      vr += sV_r;
    }
  }
  else
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
    }
    for (int y = y3; y <= y2; y++)
    {
      draw_line(xr>>SP_ACCURACY,0>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,
                xl>>SP_ACCURACY,0>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,y,color,sU,sV);
      xl += sX_l;
      ul += sU_l;
      vl += sV_l;
      xr += sX_r;
      ur += sU_r;
      vr += sV_r;
    }
  }
  SDL_UnlockSurface(spTarget);
}

PREFIX void spQuad(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 y4, Sint16 z4, Uint16 color)
{

}

PREFIX void spQuad_tex(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Sint16 x4, Sint16 y4, Sint16 z4, Sint16 u4, Sint16 v4, Uint16 color)
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

void reset_draw_pixel_func()
{
/*  if (spZTest)
  {
    if (spZSet)
      draw_pixel_tex_func = draw_pixel_tex_ztest_zset;
    else
      draw_pixel_tex_func = draw_pixel_tex_ztest;
  }
  else
  {
    if (spZSet)
      draw_pixel_tex_func = draw_pixel_tex_zset;
    else
      draw_pixel_tex_func = draw_pixel_tex;
  }*/  
}

PREFIX void spSetZTest(char test)
{
  spZTest = test;
  reset_draw_pixel_func();
}

PREFIX void spSetZSet(char test)
{
  spZSet = test;
  reset_draw_pixel_func();
}
