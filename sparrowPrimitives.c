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

//#define FAST_BUT_UGLY

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
Uint32 spTextureXY = 0;
Sint32 spOne_over_x_look_up[1<<SP_PRIM_ACCURACY];

PREFIX Sint32* spGetOne_over_x_pointer()
{
  return spOne_over_x_look_up;
}

PREFIX void spInitPrimitives()
{
  int i;
  for (i = 1; i < (1<<SP_PRIM_ACCURACY); i++)
    spOne_over_x_look_up[i] = (1<<SP_PRIM_ACCURACY)/i;
  spOne_over_x_look_up[0] = 0;
}

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
  spTextureXY = spTextureX * spTextureY;
  spTexturePixel=(Uint16*)texture->pixels;
}

PREFIX void spClearTarget(Uint16 color)
{
  SDL_FillRect(spTarget,NULL,color);
}

inline Sint32 one_over_x(Sint32 x)
{
  if (x>0)
  {
    if (x<(1<<SP_PRIM_ACCURACY))
      return spOne_over_x_look_up[x];
    if (x==(1<<SP_PRIM_ACCURACY))
      return 1;
    return 0;
  }
  if (x>(-1<<SP_PRIM_ACCURACY))
    return -spOne_over_x_look_up[-x];
  if (x==(-1<<SP_PRIM_ACCURACY))
    return -1;
  return 0;
}

inline Sint32 z_div(Sint32 z,Sint32 d)
{
  #ifdef REALGP2X_TODO_FIX_ME
    return (z>>SP_HALF_PRIM_ACCURACY)*(one_over_x(d)>>SP_HALF_PRIM_ACCURACY);
  #else
    #ifndef ARMCPU
    if (d == 0)
      return 0;
    #endif
    return z/d;
  #endif
}

inline void draw_pixel_ztest_zset(Sint16 x,Sint16 y,Sint32 z,Uint16 color)
{
  if (z<0 && z > spZBuffer[x+y*spTargetX])
  {
    spTargetPixel[x+y*spTargetX] = color;
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel_ztest(Sint16 x,Sint16 y,Sint32 z,Uint16 color)
{
  if (z<0 && z > spZBuffer[x+y*spTargetX])
  {
    spTargetPixel[x+y*spTargetX] = color;
  }
}

inline void draw_pixel_zset(Sint16 x,Sint16 y,Sint32 z,Uint16 color)
{
  spTargetPixel[x+y*spTargetX] = color;
  spZBuffer[x+y*spTargetX] = z;
}

inline void draw_pixel(Sint16 x,Sint16 y,Uint16 color)
{
  spTargetPixel[x+y*spTargetX] = color;
}

inline void draw_line_ztest_zset(Sint16 x1,Sint32 z1,Sint16 x2,Sint32 z2,Sint16 y,Uint16 color,Sint32 sZ)
{
  if (x1 >= spTargetX)
    return;
  if (x2 < 0)
    return;
  Sint32 z = z1;
  if (x1 < 0)
  {
    z -= x1*sZ;
    x1 = 0;
  }
  if (x2 >= spTargetX)
    x2 = spTargetX-1;
  int x;
  for (x = x1; x<= x2; x++)
  {
    draw_pixel_ztest_zset(x,y,z,color);
    z += sZ;
  }
}

inline void draw_line_ztest(Sint16 x1,Sint32 z1,Sint16 x2,Sint32 z2,Sint16 y,Uint16 color,Sint32 sZ)
{
  if (x1 >= spTargetX)
    return;
  if (x2 < 0)
    return;
  Sint32 z = z1;
  if (x1 < 0)
  {
    z -= x1*sZ;
    x1 = 0;
  }
  if (x2 >= spTargetX)
    x2 = spTargetX-1;
  int x;
  for (x = x1; x<= x2; x++)
  {
    draw_pixel_ztest(x,y,z,color);
    z += sZ;
  }
}

inline void draw_line_zset(Sint16 x1,Sint32 z1,Sint16 x2,Sint32 z2,Sint16 y,Uint16 color,Sint32 sZ)
{
  if (x1 >= spTargetX)
    return;
  if (x2 < 0)
    return;
  Sint32 z = z1;
  if (x1 < 0)
  {
    z -= x1*sZ;
    x1 = 0;
  }
  if (x2 >= spTargetX)
    x2 = spTargetX-1;
  int x;
  for (x = x1; x<= x2; x++)
  {
    draw_pixel_zset(x,y,z,color);
    z += sZ;
  }
}

inline void draw_line(Sint16 x1,Sint16 x2,Sint16 y,Uint16 color)
{
  if (x1 >= spTargetX)
    return;
  if (x2 < 0)
    return;
  if (x1 < 0)
    x1 = 0;
  if (x2 >= spTargetX)
    x2 = spTargetX-1;
  //TODO: Replace with hline!
  int x;
  for (x = x1; x<= x2; x++)
  {
    draw_pixel(x,y,color);
  }
}

inline void sp_intern_Triangle_ztest_zset(Sint16 x1, Sint16 y1, Sint32 z1, Sint16 x2, Sint16 y2, Sint32 z2, Sint16 x3, Sint16 y3, Sint32 z3, Uint16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint32 z4 = z1;
  int div = y2-y1;
  if (div!=0)
  {
    int mul = y3-y1;
    Sint32 mul32 = mul*one_over_x(div);//(mul<<SP_PRIM_ACCURACY)/div;
    x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
    y4 = y3;
    //z4 = z1+((z2-z1)*mul32>>SP_PRIM_ACCURACY-SP_ACCURACY);
    z4 = z1+mul*z_div(z2-z1,div);
  }
  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;
  Sint32 sX_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = one_over_x(y1-y2);//(1<<SP_PRIM_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    //sZ_l = (z1-z2)*mul;
    sZ_l = z_div(z1-z2,y1-y2);
  }

  Sint32 xr = xl;
  Sint32 zr = zl;
  Sint32 sX_r = 0;
  Sint32 sZ_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = one_over_x(y1-y3);//(1<<SP_PRIM_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    //sZ_r = (z1-z3)*mul;
    sZ_r = z_div(z1-z3,y1-y3);
  }

  if (y3 < 0)
  {
    int diff = y3-y1;
    xl += sX_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    zr += sZ_r*diff;
  }
  else
  {
    if (y1 < 0)
    {
      int diff = -y1;
      y1 = 0;
      xl += sX_l*diff;
      zl += sZ_l*diff;
      xr += sX_r*diff;
      zr += sZ_r*diff;
    }
    if (y3 >= spTargetY)
      y3 = spTargetY-1;

    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
      //sZ = (z4-z3)*mul;
      sZ = z_div(z4-z3,x4-x3);
    }
    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      { 
        //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_ztest_zset(xl>>SP_PRIM_ACCURACY,zl,
                             xr>>SP_PRIM_ACCURACY,zr,y,color,sZ);
        xl += sX_l;
        zl += sZ_l;
        xr += sX_r;
        zr += sZ_r;
      }
    }
    else
    {
      Sint32 sZ = 0;
      if ((x4-x3) != 0)
      {
        //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
        //sZ = (z4-z3)*mul;
        sZ = z_div(z4-z3,x4-x3);
      }
      for (y = y1; y < y3; y++)
      {
        //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_ztest_zset(xr>>SP_PRIM_ACCURACY,zr,
                             xl>>SP_PRIM_ACCURACY,zl,y,color,sZ);
        xl += sX_l;
        zl += sZ_l;
        xr += sX_r;
        zr += sZ_r;
      }
    }
  }
  
  xr = x3 << SP_PRIM_ACCURACY;
  zr = z3;
  sX_r = 0;
  sZ_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = one_over_x(y2-y3);//(1<<SP_PRIM_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
    //sZ_r = (z2-z3)*mul;
    sZ_r = z_div(z2-z3,y2-y3);
  }

  if (y3 < 0)
  {
    int diff = -y3;
    y3 = 0;
    xl += sX_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    zr += sZ_r*diff;
  }
  if (y2 >= spTargetY)
    y2 = spTargetY-1;
  
  Sint32 sZ = 0;
  if ((x4-x3) != 0)
  {
    //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
    //sZ = (z4-z3)*mul;
    sZ = z_div(z4-z3,x4-x3);
  }
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {      
      //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
      //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_ztest_zset(xl>>SP_PRIM_ACCURACY,zl,
                           xr>>SP_PRIM_ACCURACY,zr,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
      //sZ = (z4-z3)*mul;
      sZ = z_div(z4-z3,x4-x3);
    }
    for (y = y3; y <= y2; y++)
    {
      //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
      //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_ztest_zset(xr>>SP_PRIM_ACCURACY,zr,
                           xl>>SP_PRIM_ACCURACY,zl,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  SDL_UnlockSurface(spTarget);  
}

inline void sp_intern_Triangle_ztest(Sint16 x1, Sint16 y1, Sint32 z1,   Sint16 x2, Sint16 y2, Sint32 z2,   Sint16 x3, Sint16 y3, Sint32 z3,   Uint16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint32 z4 = z1;
  int div = y2-y1;
  if (div!=0)
  {
    int mul = y3-y1;
    Sint32 mul32 = mul*one_over_x(div);//(mul<<SP_PRIM_ACCURACY)/div;
    x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
    y4 = y3;
    //z4 = z1+((z2-z1)*mul32>>SP_PRIM_ACCURACY-SP_ACCURACY);
    z4 = z1+mul*z_div(z2-z1,div);
  }
  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;
  Sint32 sX_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = one_over_x(y1-y2);//(1<<SP_PRIM_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    //sZ_l = (z1-z2)*mul;
    sZ_l = z_div(z1-z2,y1-y2);
  }

  Sint32 xr = xl;
  Sint32 zr = zl;
  Sint32 sX_r = 0;
  Sint32 sZ_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = one_over_x(y1-y3);//(1<<SP_PRIM_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    //sZ_r = (z1-z3)*mul;
    sZ_r = z_div(z1-z3,y1-y3);
  }

  if (y3 < 0)
  {
    int diff = y3-y1;
    xl += sX_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    zr += sZ_r*diff;
  }
  else
  {
    if (y1 < 0)
    {
      int diff = -y1;
      y1 = 0;
      xl += sX_l*diff;
      zl += sZ_l*diff;
      xr += sX_r*diff;
      zr += sZ_r*diff;
    }
    if (y3 >= spTargetY)
      y3 = spTargetY-1;

    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
      //sZ = (z4-z3)*mul;
      sZ = z_div(z4-z3,x4-x3);
    }
    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      { 
        //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_ztest(xl>>SP_PRIM_ACCURACY,zl,
                             xr>>SP_PRIM_ACCURACY,zr,y,color,sZ);
        xl += sX_l;
        zl += sZ_l;
        xr += sX_r;
        zr += sZ_r;
      }
    }
    else
    {
      Sint32 sZ = 0;
      if ((x4-x3) != 0)
      {
        //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
        //sZ = (z4-z3)*mul;
        sZ = z_div(z4-z3,x4-x3);
      }
      for (y = y1; y < y3; y++)
      {
        //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_ztest(xr>>SP_PRIM_ACCURACY,zr,
                             xl>>SP_PRIM_ACCURACY,zl,y,color,sZ);
        xl += sX_l;
        zl += sZ_l;
        xr += sX_r;
        zr += sZ_r;
      }
    }
  }
  
  xr = x3 << SP_PRIM_ACCURACY;
  zr = z3;
  sX_r = 0;
  sZ_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = one_over_x(y2-y3);//(1<<SP_PRIM_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
    //sZ_r = (z2-z3)*mul;
    sZ_r = z_div(z2-z3,y2-y3);
  }

  if (y3 < 0)
  {
    int diff = -y3;
    y3 = 0;
    xl += sX_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    zr += sZ_r*diff;
  }
  if (y2 >= spTargetY)
    y2 = spTargetY-1;
  
  Sint32 sZ = 0;
  if ((x4-x3) != 0)
  {
    //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
    //sZ = (z4-z3)*mul;
    sZ = z_div(z4-z3,x4-x3);
  }
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {      
      //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
      //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_ztest(xl>>SP_PRIM_ACCURACY,zl,
                           xr>>SP_PRIM_ACCURACY,zr,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
      //sZ = (z4-z3)*mul;
      sZ = z_div(z4-z3,x4-x3);
    }
    for (y = y3; y <= y2; y++)
    {
      //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
      //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_ztest(xr>>SP_PRIM_ACCURACY,zr,
                           xl>>SP_PRIM_ACCURACY,zl,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  SDL_UnlockSurface(spTarget);
}

inline void sp_intern_Triangle_zset(Sint16 x1, Sint16 y1, Sint32 z1,   Sint16 x2, Sint16 y2, Sint32 z2,   Sint16 x3, Sint16 y3, Sint32 z3,   Uint16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint32 z4 = z1;
  int div = y2-y1;
  if (div!=0)
  {
    int mul = y3-y1;
    Sint32 mul32 = mul*one_over_x(div);//(mul<<SP_PRIM_ACCURACY)/div;
    x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
    y4 = y3;
    //z4 = z1+((z2-z1)*mul32>>SP_PRIM_ACCURACY-SP_ACCURACY);
    z4 = z1+mul*z_div(z2-z1,div);
  }
  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;
  Sint32 sX_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = one_over_x(y1-y2);//(1<<SP_PRIM_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    //sZ_l = (z1-z2)*mul;
    sZ_l = z_div(z1-z2,y1-y2);
  }

  Sint32 xr = xl;
  Sint32 zr = zl;
  Sint32 sX_r = 0;
  Sint32 sZ_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = one_over_x(y1-y3);//(1<<SP_PRIM_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    //sZ_r = (z1-z3)*mul;
    sZ_r = z_div(z1-z3,y1-y3);
  }

  if (y3 < 0)
  {
    int diff = y3-y1;
    xl += sX_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    zr += sZ_r*diff;
  }
  else
  {
    if (y1 < 0)
    {
      int diff = -y1;
      y1 = 0;
      xl += sX_l*diff;
      zl += sZ_l*diff;
      xr += sX_r*diff;
      zr += sZ_r*diff;
    }
    if (y3 >= spTargetY)
      y3 = spTargetY-1;

    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
      //sZ = (z4-z3)*mul;
      sZ = z_div(z4-z3,x4-x3);
    }
    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      { 
        //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_zset(xl>>SP_PRIM_ACCURACY,zl,
                             xr>>SP_PRIM_ACCURACY,zr,y,color,sZ);
        xl += sX_l;
        zl += sZ_l;
        xr += sX_r;
        zr += sZ_r;
      }
    }
    else
    {
      Sint32 sZ = 0;
      if ((x4-x3) != 0)
      {
        //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
        //sZ = (z4-z3)*mul;
        sZ = z_div(z4-z3,x4-x3);
      }
      for (y = y1; y < y3; y++)
      {
        //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_zset(xr>>SP_PRIM_ACCURACY,zr,
                             xl>>SP_PRIM_ACCURACY,zl,y,color,sZ);
        xl += sX_l;
        zl += sZ_l;
        xr += sX_r;
        zr += sZ_r;
      }
    }
  }
  
  xr = x3 << SP_PRIM_ACCURACY;
  zr = z3;
  sX_r = 0;
  sZ_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = one_over_x(y2-y3);//(1<<SP_PRIM_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
    //sZ_r = (z2-z3)*mul;
    sZ_r = z_div(z2-z3,y2-y3);
  }

  if (y3 < 0)
  {
    int diff = -y3;
    y3 = 0;
    xl += sX_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    zr += sZ_r*diff;
  }
  if (y2 >= spTargetY)
    y2 = spTargetY-1;
  
  Sint32 sZ = 0;
  if ((x4-x3) != 0)
  {
    //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
    //sZ = (z4-z3)*mul;
    sZ = z_div(z4-z3,x4-x3);
  }
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {      
      //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
      //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_zset(xl>>SP_PRIM_ACCURACY,zl,
                           xr>>SP_PRIM_ACCURACY,zr,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      //Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
      //sZ = (z4-z3)*mul;
      sZ = z_div(z4-z3,x4-x3);
    }
    for (y = y3; y <= y2; y++)
    {
      //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
      //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_zset(xr>>SP_PRIM_ACCURACY,zr,
                           xl>>SP_PRIM_ACCURACY,zl,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  SDL_UnlockSurface(spTarget);
}

inline void sp_intern_Triangle(Sint16 x1, Sint16 y1, Sint32 z1,   Sint16 x2, Sint16 y2, Sint32 z2,   Sint16 x3, Sint16 y3, Sint32 z3,   Uint16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  int div = y2-y1;
  if (div!=0)
  {
    if (div<0)
      div = -div;
    int mul = y3-y1;
    if (mul < 0)
      mul = -mul;
    Sint32 mul32 = mul*one_over_x(div);//(mul<<SP_PRIM_ACCURACY)/div;
    x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
    y4 = y3;
  }
  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 sX_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = one_over_x(y1-y2);//(1<<SP_PRIM_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
  }

  Sint32 xr = xl;
  Sint32 sX_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = one_over_x(y1-y3);//(1<<SP_PRIM_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
  }

  if (y3 < 0)
  {
    int diff = y3-y1;
    xl += sX_l*diff;
    xr += sX_r*diff;
  }
  else
  {
    if (y1 < 0)
    {
      int diff = -y1;
      y1 = 0;
      xl += sX_l*diff;
      xr += sX_r*diff;
    }
    if (y3 >= spTargetY)
      y3 = spTargetY-1;

    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      {      
        draw_line(xl>>SP_PRIM_ACCURACY,xr>>SP_PRIM_ACCURACY,y,color);
        xl += sX_l;
        xr += sX_r;
      }
    }
    else
    {
      for (y = y1; y < y3; y++)
      {
        draw_line(xr>>SP_PRIM_ACCURACY,xl>>SP_PRIM_ACCURACY,y,color);
        xl += sX_l;
        xr += sX_r;
      }
    }
  }
  xr = x3 << SP_PRIM_ACCURACY;
  sX_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = one_over_x(y2-y3);//(1<<SP_PRIM_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
  }
  if (y3 < 0)
  {
    int diff = -y3;
    y3 = 0;
    xl += sX_l*diff;
    xr += sX_r*diff;
  }
  if (y2 >= spTargetY)
    y2 = spTargetY-1;
  
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {
      draw_line(xl>>SP_PRIM_ACCURACY,xr>>SP_PRIM_ACCURACY,y,color);
      xl += sX_l;
      xr += sX_r;
    }
  }
  else
  {
    for (y = y3; y <= y2; y++)
    {
      draw_line(xr>>SP_PRIM_ACCURACY,xl>>SP_PRIM_ACCURACY,y,color);
      xl += sX_l;
      xr += sX_r;
    }
  }
  SDL_UnlockSurface(spTarget);  
}

PREFIX void spTriangle(Sint16 x1, Sint16 y1, Sint32 z1,   Sint16 x2, Sint16 y2, Sint32 z2,   Sint16 x3, Sint16 y3, Sint32 z3,   Uint16 color)
{
  if (y1 > y2)
  {
    Sint32 temp = x1;
    x1 = x2;
    x2 = temp;
    temp = y1;
    y1 = y2;
    y2 = temp;
    temp = z1;
    z1 = z2;
    z2 = temp;
  }
  if (y1 > y3)
  {
    Sint32 temp = x1;
    x1 = x3;
    x3 = temp;
    temp = y1;
    y1 = y3;
    y3 = temp;
    temp = z1;
    z1 = z3;
    z3 = temp;
  }
  if (y2 < y3)
  {
    Sint32 temp = x2;
    x2 = x3;
    x3 = temp;
    temp = y2;
    y2 = y3;
    y3 = temp;
    temp = z2;
    z2 = z3;
    z3 = temp;
  }
  if (spZSet)
  {
    if (spZTest)
      sp_intern_Triangle_ztest_zset(x1,y1,z1,x2,y2,z2,x3,y3,z3,color);
    else
      sp_intern_Triangle_zset      (x1,y1,z1,x2,y2,z2,x3,y3,z3,color);
  }
  else
  {
    if (spZTest)
      sp_intern_Triangle_ztest     (x1,y1,z1,x2,y2,z2,x3,y3,z3,color);
    else
      sp_intern_Triangle           (x1,y1,z1,x2,y2,z2,x3,y3,z3,color);
  }
}

inline void draw_pixel_tex_ztest_zset(Sint16 x,Sint16 y,Sint32 z,Sint16 u,Sint16 v,Uint16 color)
{
  if (z<0 && z > spZBuffer[x+y*spTargetX])
  {
    #ifdef FAST_BUT_UGLY
    if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
      spTargetPixel[x+y*spTargetX] = color;
    else
    #else
    if (u < 0)
      u = 0;
    if (v < 0)
      v = 0;
    if (u >= spTextureX)
      u = spTextureX-1;
    if (v >= spTextureY)
      v = spTextureY-1;
    #endif
    {
      Uint16 r = ((spTexturePixel[u+v*spTextureX] >> 11) * (color >> 11)) >> 5;
      Uint16 g = (((spTexturePixel[u+v*spTextureX] & 2016) >> 5) * ((color & 2016) >> 5)) >> 6;
      Uint16 b = ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31)) >> 5;
      spTargetPixel[x+y*spTargetX] = (r<<11) + (g<<5) + b;
    }
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel_tex_ztest(Sint16 x,Sint16 y,Sint32 z,Sint16 u,Sint16 v,Uint16 color)
{
  if (z<0 && z > spZBuffer[x+y*spTargetX])
  {
    #ifdef FAST_BUT_UGLY
    if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
      spTargetPixel[x+y*spTargetX] = color;
    else
    #else
    if (u < 0)
      u = 0;
    if (v < 0)
      v = 0;
    if (u >= spTextureX)
      u = spTextureX-1;
    if (v >= spTextureY)
      v = spTextureY-1;
    #endif
    {
      Uint16 r = ((spTexturePixel[u+v*spTextureX] >> 11) * (color >> 11)) >> 5;
      Uint16 g = (((spTexturePixel[u+v*spTextureX] & 2016) >> 5) * ((color & 2016) >> 5)) >> 6;
      Uint16 b = ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31)) >> 5;
      spTargetPixel[x+y*spTargetX] = (r<<11) + (g<<5) + b;
    }
  }
}

inline void draw_pixel_tex_zset(Sint16 x,Sint16 y,Sint32 z,Sint16 u,Sint16 v,Uint16 color)
{
  #ifdef FAST_BUT_UGLY
  if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
    spTargetPixel[x+y*spTargetX] = color;
  else
  #else
  if (u < 0)
    u = 0;
  if (v < 0)
    v = 0;
  if (u >= spTextureX)
    u = spTextureX-1;
  if (v >= spTextureY)
    v = spTextureY-1;
  #endif
  {
    Uint16 r = ((spTexturePixel[u+v*spTextureX] >> 11) * (color >> 11)) >> 5;
    Uint16 g = (((spTexturePixel[u+v*spTextureX] & 2016) >> 5) * ((color & 2016) >> 5)) >> 6;
    Uint16 b = ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31)) >> 5;
    spTargetPixel[x+y*spTargetX] = (r<<11) + (g<<5) + b;
  }
  spZBuffer[x+y*spTargetX] = z;
}

inline void draw_pixel_tex(Sint16 x,Sint16 y,Sint16 u,Sint16 v,Uint16 color)
{
  #ifdef FAST_BUT_UGLY
  if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
    spTargetPixel[x+y*spTargetX] = color;
  else
  #else
  if (u < 0)
    u = 0;
  if (v < 0)
    v = 0;
  if (u >= spTextureX)
    u = spTextureX-1;
  if (v >= spTextureY)
    v = spTextureY-1;
  #endif
  {
    Uint16 r = ((spTexturePixel[u+v*spTextureX] >> 11) * (color >> 11)) >> 5;
    Uint16 g = (((spTexturePixel[u+v*spTextureX] & 2016) >> 5) * ((color & 2016) >> 5)) >> 6;
    Uint16 b = ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31)) >> 5;
    spTargetPixel[x+y*spTargetX] = (r<<11) + (g<<5) + b;
  }
}

inline void draw_line_tex_ztest_zset(Sint16 x1,Sint32 z1,Sint32 u1,Sint32 v1,Sint16 x2,Sint32 z2,Sint32 u2,Sint32 v2,Sint16 y,Uint16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
{
  if (x1 >= spTargetX)
    return;
  if (x2 < 0)
    return;
  Sint32 u = u1;
  Sint32 v = v1;
  Sint32 z = z1;
  if (x1 < 0)
  {
    z -= x1*sZ;
    u -= x1*sU;
    v -= x1*sV;
    x1 = 0;
  }
  if (x2 >= spTargetX)
    x2 = spTargetX-1;
  int x;
  for (x = x1; x<= x2; x++)
  {
    draw_pixel_tex_ztest_zset(x,y,z,u>>SP_PRIM_ACCURACY,v>>SP_PRIM_ACCURACY,color);
    u += sU;
    v += sV;
    z += sZ;
  }
}

inline void draw_line_tex_ztest(Sint16 x1,Sint32 z1,Sint32 u1,Sint32 v1,Sint16 x2,Sint32 z2,Sint32 u2,Sint32 v2,Sint16 y,Uint16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
{
  if (x1 >= spTargetX)
    return;
  if (x2 < 0)
    return;
  Sint32 u = u1;
  Sint32 v = v1;
  Sint32 z = z1;
  if (x1 < 0)
  {
    z -= x1*sZ;
    u -= x1*sU;
    v -= x1*sV;
    x1 = 0;
  }
  if (x2 >= spTargetX)
    x2 = spTargetX-1;
  int x;
  for (x = x1; x<= x2; x++)
  {
    draw_pixel_tex_ztest(x,y,z,u>>SP_PRIM_ACCURACY,v>>SP_PRIM_ACCURACY,color);
    u += sU;
    v += sV;
    z += sZ;
  }
}

inline void draw_line_tex_zset(Sint16 x1,Sint32 z1,Sint32 u1,Sint32 v1,Sint16 x2,Sint32 z2,Sint32 u2,Sint32 v2,Sint16 y,Uint16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
{
  if (x1 >= spTargetX)
    return;
  if (x2 < 0)
    return;
  Sint32 u = u1;
  Sint32 v = v1;
  Sint32 z = z1;
  if (x1 < 0)
  {
    z -= x1*sZ;
    u -= x1*sU;
    v -= x1*sV;
    x1 = 0;
  }
  if (x2 >= spTargetX)
    x2 = spTargetX-1;
  int x;
  for (x = x1; x<= x2; x++)
  {
    draw_pixel_tex_zset(x,y,z,u>>SP_PRIM_ACCURACY,v>>SP_PRIM_ACCURACY,color);
    u += sU;
    v += sV;
    z += sZ;
  }
}

inline void draw_line_tex(Sint16 x1,Sint32 u1,Sint32 v1,Sint16 x2,Sint32 u2,Sint32 v2,Sint16 y,Uint16 color,Sint32 sU,Sint32 sV)
{
  if (x1 >= spTargetX)
    return;
  if (x2 < 0)
    return;
  Sint32 u = u1;
  Sint32 v = v1;
  if (x1 < 0)
  {
    u -= x1*sU;
    v -= x1*sV;
    x1 = 0;
  }
  if (x2 >= spTargetX)
    x2 = spTargetX-1;
  int x;
  for (x = x1; x<= x2; x++)
  {
    draw_pixel_tex(x,y,u>>SP_PRIM_ACCURACY,v>>SP_PRIM_ACCURACY,color);
    u += sU;
    v += sV;
  }
}

inline void sp_intern_Triangle_tex_ztest_zset(Sint16 x1, Sint16 y1, Sint32 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint32 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint32 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint32 z4 = z1;
  Sint16 u4 = u1;
  Sint16 v4 = v1;  
  int div = y2-y1;
  if (div!=0)
  {
    int mul = y3-y1;
    Sint32 mul32 = mul*one_over_x(div);//(mul<<SP_PRIM_ACCURACY)/div;
    x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
    y4 = y3;
    //z4 = z1+((z2-z1)*mul32>>SP_PRIM_ACCURACY-SP_ACCURACY);
    z4 = z1+mul*z_div(z2-z1,div);
    u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
    v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);
  }
  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;
  Sint32 sX_l = 0;
  Sint32 sU_l = 0;
  Sint32 sV_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = one_over_x(y1-y2);//(1<<SP_PRIM_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    sU_l = (u1-u2)*mul;
    sV_l = (v1-v2)*mul;
    //sZ_l = (z1-z2)*mul;
    sZ_l = z_div(z1-z2,y1-y2);
  }

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 zr = zl;
  Sint32 sX_r = 0;
  Sint32 sU_r = 0;
  Sint32 sV_r = 0;
  Sint32 sZ_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = one_over_x(y1-y3);//(1<<SP_PRIM_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sU_r = (u1-u3)*mul;
    sV_r = (v1-v3)*mul;
    //sZ_r = (z1-z3)*mul;
    sZ_r = z_div(z1-z3,y1-y3);
  }

  if (y3 < 0)
  {
    int diff = y3-y1;
    xl += sX_l*diff;
    ul += sU_l*diff;
    vl += sV_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    ur += sU_r*diff;
    vr += sV_r*diff;
    zr += sZ_r*diff;
  }
  else
  {
    if (y1 < 0)
    {
      int diff = -y1;
      y1 = 0;
      xl += sX_l*diff;
      ul += sU_l*diff;
      vl += sV_l*diff;
      zl += sZ_l*diff;
      xr += sX_r*diff;
      ur += sU_r*diff;
      vr += sV_r*diff;
      zr += sZ_r*diff;
    }
    if (y3 >= spTargetY)
      y3 = spTargetY-1;

    if (x4 < x3)
    {
      Sint32 sU = 0;
      Sint32 sV = 0;
      Sint32 sZ = 0;
      if ((x4-x3) != 0)
      {
        Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
        sU = (u4-u3)*mul;
        sV = (v4-v3)*mul;
        //sZ = (z4-z3)*mul;
        sZ = z_div(z4-z3,x4-x3);
      }
      for (y = y1; y < y3; y++)
      { 
        //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_tex_ztest_zset(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
                                 xr>>SP_PRIM_ACCURACY,zr,ur,vr,y,color,sU,sV,sZ);
        xl += sX_l;
        ul += sU_l;
        vl += sV_l;
        zl += sZ_l;
        xr += sX_r;
        ur += sU_r;
        vr += sV_r;
        zr += sZ_r;
      }
    }
    else
    {
      Sint32 sU = 0;
      Sint32 sV = 0;
      Sint32 sZ = 0;
      if ((x3-x4) != 0)
      {
        Sint32 mul = one_over_x(x3-x4);//(1<<SP_PRIM_ACCURACY)/(x3-x4);
        sU = (u3-u4)*mul;
        sV = (v3-v4)*mul;
        //sZ = (z3-z4)*mul;
        sZ = z_div(z3-z4,x3-x4);
      }    
      for (y = y1; y < y3; y++)
      {
        //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_tex_ztest_zset(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
                                 xl>>SP_PRIM_ACCURACY,zl,ul,vl,y,color,sU,sV,sZ);
        xl += sX_l;
        ul += sU_l;
        vl += sV_l;
        zl += sZ_l;
        xr += sX_r;
        ur += sU_r;
        vr += sV_r;
        zr += sZ_r;
      }
    }
  }
  
  xr = x3 << SP_PRIM_ACCURACY;
  ur = u3 << SP_PRIM_ACCURACY;
  vr = v3 << SP_PRIM_ACCURACY;
  zr = z3;
  sX_r = 0;
  sU_r = 0;
  sV_r = 0;
  sZ_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = one_over_x(y2-y3);//(1<<SP_PRIM_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
    sU_r = (u2-u3)*mul;
    sV_r = (v2-v3)*mul;
    //sZ_r = (z2-z3)*mul;
    sZ_r = z_div(z2-z3,y2-y3);
  }

  if (y3 < 0)
  {
    int diff = -y3;
    y3 = 0;
    xl += sX_l*diff;
    ul += sU_l*diff;
    vl += sV_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    ur += sU_r*diff;
    vr += sV_r*diff;
    zr += sZ_r*diff;
  }
  if (y2 >= spTargetY)
    y2 = spTargetY-1;
  
  if (x4 < x3)
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
      //sZ = (z4-z3)*mul;
      sZ = z_div(z4-z3,x4-x3);
    }
    for (y = y3; y <= y2; y++)
    {      
      //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
      //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_tex_ztest_zset(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
                               xr>>SP_PRIM_ACCURACY,zr,ur,vr,y,color,sU,sV,sZ);
      xl += sX_l;
      ul += sU_l;
      vl += sV_l;
      zl += sZ_l;
      xr += sX_r;
      ur += sU_r;
      vr += sV_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    Sint32 sZ = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = one_over_x(x3-x4);//(1<<SP_PRIM_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
      //sZ = (z3-z4)*mul;
      sZ = z_div(z3-z4,x3-x4);
    }    
    for (y = y3; y <= y2; y++)
    {
      //if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
      //  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_tex_ztest_zset(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
                               xl>>SP_PRIM_ACCURACY,zl,ul,vl,y,color,sU,sV,sZ);
      xl += sX_l;
      ul += sU_l;
      vl += sV_l;
      zl += sZ_l;
      xr += sX_r;
      ur += sU_r;
      vr += sV_r;
      zr += sZ_r;
    }
  }
  SDL_UnlockSurface(spTarget);  
}

inline void sp_intern_Triangle_tex_ztest(Sint16 x1, Sint16 y1, Sint32 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint32 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint32 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint32 z4 = z1;
  Sint16 u4 = u1;
  Sint16 v4 = v1;  
  int div = y2-y1;
  if (div!=0)
  {
    int mul = y3-y1;
    Sint32 mul32 = mul*one_over_x(div);//(mul<<SP_PRIM_ACCURACY)/div;
    x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
    y4 = y3;
    //z4 = z1+((z2-z1)*mul32>>SP_PRIM_ACCURACY-SP_ACCURACY);
    z4 = z1+mul*z_div(z2-z1,div);
    u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
    v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);
  }
  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;
  Sint32 sX_l = 0;
  Sint32 sU_l = 0;
  Sint32 sV_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = one_over_x(y1-y2);//(1<<SP_PRIM_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    sU_l = (u1-u2)*mul;
    sV_l = (v1-v2)*mul;
    //sZ_l = (z1-z2)*mul;
    sZ_l = z_div(z1-z2,y1-y2);
  }

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 zr = zl;
  Sint32 sX_r = 0;
  Sint32 sU_r = 0;
  Sint32 sV_r = 0;
  Sint32 sZ_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = one_over_x(y1-y3);//(1<<SP_PRIM_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sU_r = (u1-u3)*mul;
    sV_r = (v1-v3)*mul;
    //sZ_r = (z1-z3)*mul;
    sZ_r = z_div(z1-z3,y1-y3);
  }

  if (y3 < 0)
  {
    int diff = y3-y1;
    xl += sX_l*diff;
    ul += sU_l*diff;
    vl += sV_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    ur += sU_r*diff;
    vr += sV_r*diff;
    zr += sZ_r*diff;
  }
  else
  {
    if (y1 < 0)
    {
      int diff = -y1;
      y1 = 0;
      xl += sX_l*diff;
      ul += sU_l*diff;
      vl += sV_l*diff;
      zl += sZ_l*diff;
      xr += sX_r*diff;
      ur += sU_r*diff;
      vr += sV_r*diff;
      zr += sZ_r*diff;
    }
    if (y3 >= spTargetY)
      y3 = spTargetY-1;

    if (x4 < x3)
    {
      Sint32 sU = 0;
      Sint32 sV = 0;
      Sint32 sZ = 0;
      if ((x4-x3) != 0)
      {
        Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
        sU = (u4-u3)*mul;
        sV = (v4-v3)*mul;
        //sZ = (z4-z3)*mul;
        //sZ = z_div(z4-z3,x4-x3);
      }
      for (y = y1; y < y3; y++)
      { 
        if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
          sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_tex_ztest(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
                                 xr>>SP_PRIM_ACCURACY,zr,ur,vr,y,color,sU,sV,sZ);
        xl += sX_l;
        ul += sU_l;
        vl += sV_l;
        zl += sZ_l;
        xr += sX_r;
        ur += sU_r;
        vr += sV_r;
        zr += sZ_r;
      }
    }
    else
    {
      Sint32 sU = 0;
      Sint32 sV = 0;
      Sint32 sZ = 0;
      if ((x3-x4) != 0)
      {
        Sint32 mul = one_over_x(x3-x4);//(1<<SP_PRIM_ACCURACY)/(x3-x4);
        sU = (u3-u4)*mul;
        sV = (v3-v4)*mul;
        //sZ = (z3-z4)*mul;
        //sZ = z_div(z3-z4,x3-x4);
      }    
      for (y = y1; y < y3; y++)
      {
        if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
          sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_tex_ztest(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
                                 xl>>SP_PRIM_ACCURACY,zl,ul,vl,y,color,sU,sV,sZ);
        xl += sX_l;
        ul += sU_l;
        vl += sV_l;
        zl += sZ_l;
        xr += sX_r;
        ur += sU_r;
        vr += sV_r;
        zr += sZ_r;
      }
    }
  }
  
  xr = x3 << SP_PRIM_ACCURACY;
  ur = u3 << SP_PRIM_ACCURACY;
  vr = v3 << SP_PRIM_ACCURACY;
  zr = z3;
  sX_r = 0;
  sU_r = 0;
  sV_r = 0;
  sZ_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = one_over_x(y2-y3);//(1<<SP_PRIM_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
    sU_r = (u2-u3)*mul;
    sV_r = (v2-v3)*mul;
    //sZ_r = (z2-z3)*mul;
    sZ_r = z_div(z2-z3,y2-y3);
  }

  if (y3 < 0)
  {
    int diff = -y3;
    y3 = 0;
    xl += sX_l*diff;
    ul += sU_l*diff;
    vl += sV_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    ur += sU_r*diff;
    vr += sV_r*diff;
    zr += sZ_r*diff;
  }
  if (y2 >= spTargetY)
    y2 = spTargetY-1;
  
  if (x4 < x3)
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
      //sZ = (z4-z3)*mul;
      //sZ = z_div(z4-z3,x4-x3);
    }
    for (y = y3; y <= y2; y++)
    {      
      if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_tex_ztest(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
                               xr>>SP_PRIM_ACCURACY,zr,ur,vr,y,color,sU,sV,sZ);
      xl += sX_l;
      ul += sU_l;
      vl += sV_l;
      zl += sZ_l;
      xr += sX_r;
      ur += sU_r;
      vr += sV_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    Sint32 sZ = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = one_over_x(x3-x4);//(1<<SP_PRIM_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
      //sZ = (z3-z4)*mul;
      //sZ = z_div(z3-z4,x3-x4);
    }    
    for (y = y3; y <= y2; y++)
    {
      if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_tex_ztest(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
                               xl>>SP_PRIM_ACCURACY,zl,ul,vl,y,color,sU,sV,sZ);
      xl += sX_l;
      ul += sU_l;
      vl += sV_l;
      zl += sZ_l;
      xr += sX_r;
      ur += sU_r;
      vr += sV_r;
      zr += sZ_r;
    }
  }
  SDL_UnlockSurface(spTarget);  
}

inline void sp_intern_Triangle_tex_zset(Sint16 x1, Sint16 y1, Sint32 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint32 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint32 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint32 z4 = z1;
  Sint16 u4 = u1;
  Sint16 v4 = v1;  
  int div = y2-y1;
  if (div!=0)
  {
    int mul = y3-y1;
    Sint32 mul32 = mul*one_over_x(div);//(mul<<SP_PRIM_ACCURACY)/div;
    x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
    y4 = y3;
    //z4 = z1+((z2-z1)*mul32>>SP_PRIM_ACCURACY-SP_ACCURACY);
    z4 = z1+mul*z_div(z2-z1,div);
    u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
    v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);
  }
  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;
  Sint32 sX_l = 0;
  Sint32 sU_l = 0;
  Sint32 sV_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = one_over_x(y1-y2);//(1<<SP_PRIM_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    sU_l = (u1-u2)*mul;
    sV_l = (v1-v2)*mul;
    //sZ_l = (z1-z2)*mul;
    sZ_l = z_div(z1-z2,y1-y2);
  }

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 zr = zl;
  Sint32 sX_r = 0;
  Sint32 sU_r = 0;
  Sint32 sV_r = 0;
  Sint32 sZ_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = one_over_x(y1-y3);//(1<<SP_PRIM_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sU_r = (u1-u3)*mul;
    sV_r = (v1-v3)*mul;
    //sZ_r = (z1-z3)*mul;
    sZ_r = z_div(z1-z3,y1-y3);
  }

  if (y3 < 0)
  {
    int diff = y3-y1;
    xl += sX_l*diff;
    ul += sU_l*diff;
    vl += sV_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    ur += sU_r*diff;
    vr += sV_r*diff;
    zr += sZ_r*diff;
  }
  else
  {
    if (y1 < 0)
    {
      int diff = -y1;
      y1 = 0;
      xl += sX_l*diff;
      ul += sU_l*diff;
      vl += sV_l*diff;
      zl += sZ_l*diff;
      xr += sX_r*diff;
      ur += sU_r*diff;
      vr += sV_r*diff;
      zr += sZ_r*diff;
    }
    if (y3 >= spTargetY)
      y3 = spTargetY-1;

    if (x4 < x3)
    {
      Sint32 sU = 0;
      Sint32 sV = 0;
      Sint32 sZ = 0;
      if ((x4-x3) != 0)
      {
        Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
        sU = (u4-u3)*mul;
        sV = (v4-v3)*mul;
        //sZ = (z4-z3)*mul;
        //sZ = z_div(z4-z3,x4-x3);
      }
      for (y = y1; y < y3; y++)
      { 
        if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
          sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_tex_zset(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
                                 xr>>SP_PRIM_ACCURACY,zr,ur,vr,y,color,sU,sV,sZ);
        xl += sX_l;
        ul += sU_l;
        vl += sV_l;
        zl += sZ_l;
        xr += sX_r;
        ur += sU_r;
        vr += sV_r;
        zr += sZ_r;
      }
    }
    else
    {
      Sint32 sU = 0;
      Sint32 sV = 0;
      Sint32 sZ = 0;
      if ((x3-x4) != 0)
      {
        Sint32 mul = one_over_x(x3-x4);//(1<<SP_PRIM_ACCURACY)/(x3-x4);
        sU = (u3-u4)*mul;
        sV = (v3-v4)*mul;
        //sZ = (z3-z4)*mul;
        //sZ = z_div(z3-z4,x3-x4);
      }    
      for (y = y1; y < y3; y++)
      {
        if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
          sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
        draw_line_tex_zset(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
                                 xl>>SP_PRIM_ACCURACY,zl,ul,vl,y,color,sU,sV,sZ);
        xl += sX_l;
        ul += sU_l;
        vl += sV_l;
        zl += sZ_l;
        xr += sX_r;
        ur += sU_r;
        vr += sV_r;
        zr += sZ_r;
      }
    }
  }
  
  xr = x3 << SP_PRIM_ACCURACY;
  ur = u3 << SP_PRIM_ACCURACY;
  vr = v3 << SP_PRIM_ACCURACY;
  zr = z3;
  sX_r = 0;
  sU_r = 0;
  sV_r = 0;
  sZ_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = one_over_x(y2-y3);//(1<<SP_PRIM_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
    sU_r = (u2-u3)*mul;
    sV_r = (v2-v3)*mul;
    //sZ_r = (z2-z3)*mul;
    sZ_r = z_div(z2-z3,y2-y3);
  }

  if (y3 < 0)
  {
    int diff = -y3;
    y3 = 0;
    xl += sX_l*diff;
    ul += sU_l*diff;
    vl += sV_l*diff;
    zl += sZ_l*diff;
    xr += sX_r*diff;
    ur += sU_r*diff;
    vr += sV_r*diff;
    zr += sZ_r*diff;
  }
  if (y2 >= spTargetY)
    y2 = spTargetY-1;
  
  if (x4 < x3)
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
      //sZ = (z4-z3)*mul;
      //sZ = z_div(z4-z3,x4-x3);
    }
    for (y = y3; y <= y2; y++)
    {      
      if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_tex_zset(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
                               xr>>SP_PRIM_ACCURACY,zr,ur,vr,y,color,sU,sV,sZ);
      xl += sX_l;
      ul += sU_l;
      vl += sV_l;
      zl += sZ_l;
      xr += sX_r;
      ur += sU_r;
      vr += sV_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    Sint32 sZ = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = one_over_x(x3-x4);//(1<<SP_PRIM_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
      //sZ = (z3-z4)*mul;
      //sZ = z_div(z3-z4,x3-x4);
    }    
    for (y = y3; y <= y2; y++)
    {
      if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
        sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
      draw_line_tex_zset(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
                               xl>>SP_PRIM_ACCURACY,zl,ul,vl,y,color,sU,sV,sZ);
      xl += sX_l;
      ul += sU_l;
      vl += sV_l;
      zl += sZ_l;
      xr += sX_r;
      ur += sU_r;
      vr += sV_r;
      zr += sZ_r;
    }
  }
  SDL_UnlockSurface(spTarget);  
}

inline void sp_intern_Triangle_tex(Sint16 x1, Sint16 y1, Sint32 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint32 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint32 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint32 z4 = z1;
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
    Sint32 mul32 = mul*one_over_x(div);//(mul<<SP_PRIM_ACCURACY)/div;
    x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
    y4 = y3;
    z4 = z1+((z2-z1)*mul32>>SP_PRIM_ACCURACY);
    u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
    v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);
  }
  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 sX_l = 0;
  Sint32 sU_l = 0;
  Sint32 sV_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = one_over_x(y1-y2);//(1<<SP_PRIM_ACCURACY)/(y1-y2);
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
    Sint32 mul = one_over_x(y1-y3);//(1<<SP_PRIM_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sU_r = (u1-u3)*mul;
    sV_r = (v1-v3)*mul;
  }

  if (y3 < 0)
  {
    int diff = y3-y1;
    y3 = 0;
    xl += sX_l*diff;
    ul += sU_l*diff;
    vl += sV_l*diff;
    xr += sX_r*diff;
    ur += sU_r*diff;
    vr += sV_r*diff;
  }
  else
  {
    if (y1 < 0)
    {
      int diff = -y1;
      y1 = 0;
      xl += sX_l*diff;
      ul += sU_l*diff;
      vl += sV_l*diff;
      xr += sX_r*diff;
      ur += sU_r*diff;
      vr += sV_r*diff;
    }
    if (y3 >= spTargetY)
      y3 = spTargetY-1;

    if (x4 < x3)
    {
      Sint32 sU = 0;
      Sint32 sV = 0;
      if ((x4-x3) != 0)
      {
        Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
        sU = (u4-u3)*mul;
        sV = (v4-v3)*mul;
      }
      for (y = y1; y < y3; y++)
      {      
        draw_line_tex(xl>>SP_PRIM_ACCURACY,ul,vl,
                      xr>>SP_PRIM_ACCURACY,ur,vr,y,color,sU,sV);
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
        Sint32 mul = one_over_x(x3-x4);//(1<<SP_PRIM_ACCURACY)/(x3-x4);
        sU = (u3-u4)*mul;
        sV = (v3-v4)*mul;
      }    
      for (y = y1; y < y3; y++)
      {
        draw_line_tex(xr>>SP_PRIM_ACCURACY,ur,vr,
                      xl>>SP_PRIM_ACCURACY,ul,vl,y,color,sU,sV);
        xl += sX_l;
        ul += sU_l;
        vl += sV_l;
        xr += sX_r;
        ur += sU_r;
        vr += sV_r;
      }
    }
  }
  xr = x3 << SP_PRIM_ACCURACY;
  ur = u3 << SP_PRIM_ACCURACY;
  vr = v3 << SP_PRIM_ACCURACY;
  sX_r = 0;
  sU_r = 0;
  sV_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = one_over_x(y2-y3);//(1<<SP_PRIM_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
    sU_r = (u2-u3)*mul;
    sV_r = (v2-v3)*mul;
  }
  if (y3 < 0)
  {
    int diff = -y3;
    y3 = 0;
    xl += sX_l*diff;
    ul += sU_l*diff;
    vl += sV_l*diff;
    xr += sX_r*diff;
    ur += sU_r*diff;
    vr += sV_r*diff;
  }
  if (y2 >= spTargetY)
    y2 = spTargetY-1;
  
  if (x4 < x3)
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
    }
    for (y = y3; y <= y2; y++)
    {
      draw_line_tex(xl>>SP_PRIM_ACCURACY,ul,vl,
                    xr>>SP_PRIM_ACCURACY,ur,vr,y,color,sU,sV);
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
      Sint32 mul = one_over_x(x3-x4);//(1<<SP_PRIM_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
    }
    for (y = y3; y <= y2; y++)
    {
      draw_line_tex(xr>>SP_PRIM_ACCURACY,ur,vr,
                    xl>>SP_PRIM_ACCURACY,ul,vl,y,color,sU,sV);
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

PREFIX void spTriangle_tex(Sint16 x1, Sint16 y1, Sint32 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint32 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint32 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
  if (y1 > y2)
  {
    Sint32 temp = x1;
    x1 = x2;
    x2 = temp;
    temp = y1;
    y1 = y2;
    y2 = temp;
    temp = z1;
    z1 = z2;
    z2 = temp;
    temp = u1;
    u1 = u2;
    u2 = temp;
    temp = v1;
    v1 = v2;
    v2 = temp;
  }
  if (y1 > y3)
  {
    Sint32 temp = x1;
    x1 = x3;
    x3 = temp;
    temp = y1;
    y1 = y3;
    y3 = temp;
    temp = z1;
    z1 = z3;
    z3 = temp;
    temp = u1;
    u1 = u3;
    u3 = temp;
    temp = v1;
    v1 = v3;
    v3 = temp;
  }
  if (y2 < y3)
  {
    Sint32 temp = x2;
    x2 = x3;
    x3 = temp;
    temp = y2;
    y2 = y3;
    y3 = temp;
    temp = z2;
    z2 = z3;
    z3 = temp;
    temp = u2;
    u2 = u3;
    u3 = temp;
    temp = v2;
    v2 = v3;
    v3 = temp;
  }
  if (spZSet)
  {
    if (spZTest)
      sp_intern_Triangle_tex_ztest_zset(x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
    else
      sp_intern_Triangle_tex_zset      (x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
  }
  else
  {
    if (spZTest)
      sp_intern_Triangle_tex_ztest     (x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
    else
      sp_intern_Triangle_tex           (x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
  }
}


PREFIX void spQuad(Sint16 x1, Sint16 y1, Sint32 z1, Sint16 x2, Sint16 y2, Sint32 z2, Sint16 x3, Sint16 y3, Sint32 z3, Sint16 x4,Sint16 y4, Sint32 z4, Uint16 color)
{
  //spTriangle(x1,y1,z1,x2,y2,z2,x3,y3,z3,color);
  //spTriangle(x1,y1,z1,x3,y3,z3,x4,y4,z4,color);
  Sint16 mx = x1+x2+x3+x4>>2;
  Sint16 my = y1+y2+y3+y4>>2;
  Sint32 mz = (z1>>2)+(z2>>2)+(z3>>2)+(z4>>2);
  spTriangle(mx,my,mz,x1,y1,z1,x2,y2,z2,color);
  spTriangle(mx,my,mz,x2,y2,z2,x3,y3,z3,color);
  spTriangle(mx,my,mz,x3,y3,z3,x4,y4,z4,color);
  spTriangle(mx,my,mz,x4,y4,z4,x1,y1,z1,color);
}

PREFIX void spQuad_tex(Sint16 x1, Sint16 y1, Sint32 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint32 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint32 z3, Sint16 u3, Sint16 v3, Sint16 x4, Sint16 y4, Sint32 z4, Sint16 u4, Sint16 v4, Uint16 color)
{
  int smallest = 1; //y1
  if (y2 < y1)
  {
    if (y2 < y3)
    {
      if (y2 < y4)
        smallest = 2;
      else
        smallest = 4;
    }
    else
    {
      if (y3 < y4)
        smallest = 3;
      else
        smallest = 4;
    }
  } 
  else
  {
    if (y1 < y3)
    {
      if (y1 < y4)
        smallest = 1;
      else
        smallest = 4;
    }
    else
    {
      if (y3 < y4)
        smallest = 3;
      else
        smallest = 4;
    }
  }
  Sint32 temp;
  switch (smallest)
  {
    case 2: //shift to the left
      temp = x1;
      x1 = x2;
      x2 = x3;
      x3 = x4;
      x4 = temp;
      temp = y1;
      y1 = y2;
      y2 = y3;
      y3 = y4;
      y4 = temp;
      temp = z1;
      z1 = z2;
      z2 = z3;
      z3 = z4;
      z4 = temp;
      temp = u1;
      u1 = u2;
      u2 = u3;
      u3 = u4;
      u4 = temp;
      temp = v1;
      v1 = v2;
      v2 = v3;
      v3 = v4;
      v4 = temp;
    break;
    case 3: //Mirror (switch of 3 and 1)
      temp = x1;
      x1 = x3;
      x3 = temp;
      temp = y1;
      y1 = y3;
      y3 = temp;
      temp = z1;
      z1 = z3;
      z3 = temp;
      temp = u1;
      u1 = u3;
      u3 = temp;
      temp = v1;
      v1 = v3;
      v3 = temp;
    break;
    case 4: //shift to the right
      temp = x4;
      x4 = x3;
      x3 = x2;
      x2 = x1;
      x1 = temp;
      temp = y4;
      y4 = y3;
      y3 = y2;
      y2 = y1;
      y1 = temp;
      temp = z4;
      z4 = z3;
      z3 = z2;
      z2 = z1;
      z1 = temp;
      temp = u4;
      u4 = u3;
      u3 = u2;
      u2 = u1;
      u1 = temp;
      temp = v4;
      v4 = v3;
      v3 = v2;
      v2 = v1;
      v1 = temp;
    break;
  }
  //we say, that all quads are convex! So p2 or p4 is the nearest point to p1
  //Furthermore we define, that it should be p2, e.g.
  // p1        p1
  // | \       | \
  // p2 \      p2 \
  //  \  \      \  p4
  //   p3 \      \/
  //    '~.p4    p3
  if (y2 > y4) //Mirror p2 and p4
  {
    temp = x2;
    x2 = x4;
    x4 = temp;
    temp = y2;
    y2 = y4;
    y4 = temp;
    temp = z2;
    z2 = z4;
    z4 = temp;
    temp = u2;
    u2 = u4;
    u4 = temp;
    temp = v2;
    v2 = v4;
    v4 = temp;
  }
  
  if (spZSet)
  {
    if (spZTest)
    {
      sp_intern_Triangle_tex_ztest_zset(x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
      if (y3 > y4)
        sp_intern_Triangle_tex_ztest_zset(x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x4,y4,z4,u4,v4,color);
      else
        sp_intern_Triangle_tex_ztest_zset(x1,y1,z1,u1,v1,x4,y4,z4,u4,v4,x3,y3,z3,u3,v3,color);
    }
    else
      sp_intern_Triangle_tex_zset      (x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
  }
  else
  {
    if (spZTest)
      sp_intern_Triangle_tex_ztest     (x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
    else
      sp_intern_Triangle_tex           (x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
  }
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
      spZBuffer[i]=SP_MAX_NEGATIVE;
}

PREFIX Sint32* spGetZBuffer()
{
  return spZBuffer;
}

PREFIX void spSetZTest(char test)
{
  spZTest = test;
}

PREFIX void spSetZSet(char test)
{
  spZSet = test;
}
