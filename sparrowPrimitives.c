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
Uint32 spZTest = 1;
Uint32 spZSet = 1;
Uint32 spAlphaTest = 1;
Sint32* spZBuffer = NULL;
SP_SingedInt16 spTargetX = 0;
SP_SingedInt16 spTargetY = 0;
SP_SingedInt16 spTextureX = 0;
SP_SingedInt16 spTextureY = 0;
Sint32 spTextureXY = 0;
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

PREFIX void spClearTarget(SP_UnsingedInt16 color)
{
  SDL_FillRect(spTarget,NULL,color);
  /*SDL_LockSurface(spTarget);  
  spHorizentalLine(spTargetPixel,0,0,spTargetX*spTargetY,color,0,0,0);
  SDL_UnlockSurface(spTarget);*/
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
    if (d == 0)
      return 0;
    return z/d;
  #endif
}

inline void draw_pixel_ztest_zset(SP_SingedInt16 x,SP_SingedInt16 y,Sint32 z,SP_UnsingedInt16 color)
{
  if (z<0 && z > spZBuffer[x+y*spTargetX])
  {
    spTargetPixel[x+y*spTargetX] = color;
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel_ztest(SP_SingedInt16 x,SP_SingedInt16 y,Sint32 z,SP_UnsingedInt16 color)
{
  if (z<0 && z > spZBuffer[x+y*spTargetX])
  {
    spTargetPixel[x+y*spTargetX] = color;
  }
}

inline void draw_pixel_zset(SP_SingedInt16 x,SP_SingedInt16 y,Sint32 z,SP_UnsingedInt16 color)
{
  spTargetPixel[x+y*spTargetX] = color;
  spZBuffer[x+y*spTargetX] = z;
}

inline void draw_pixel(SP_SingedInt16 x,SP_SingedInt16 y,SP_UnsingedInt16 color)
{
  spTargetPixel[x+y*spTargetX] = color;
}

inline void draw_line_ztest_zset(SP_SingedInt16 x1,Sint32 z1,SP_SingedInt16 x2,Sint32 z2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sZ)
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

inline void draw_line_ztest(SP_SingedInt16 x1,Sint32 z1,SP_SingedInt16 x2,Sint32 z2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sZ)
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

inline void draw_line_zset(SP_SingedInt16 x1,Sint32 z1,SP_SingedInt16 x2,Sint32 z2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sZ)
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

inline void draw_line(SP_SingedInt16 x1,SP_SingedInt16 x2,SP_SingedInt16 y,SP_UnsingedInt16 color)
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
  /*int x;
  for (x = x1; x<= x2; x++)
  {
    draw_pixel(x,y,color);
  }*/
  spHorizentalLine(spTargetPixel,x1,y,x2-x1,color,1,spTargetX,spTargetY);
}

inline void sp_intern_Triangle_ztest_zset(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  SP_SingedInt16 x4 = x1;
  SP_SingedInt16 y4 = y1;
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

inline void sp_intern_Triangle_ztest(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1,   SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2,   SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3,   SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  SP_SingedInt16 x4 = x1;
  SP_SingedInt16 y4 = y1;
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

inline void sp_intern_Triangle_zset(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1,   SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2,   SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3,   SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  SP_SingedInt16 x4 = x1;
  SP_SingedInt16 y4 = y1;
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

inline void sp_intern_Triangle(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1,   SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2,   SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3,   SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  SP_SingedInt16 x4 = x1;
  SP_SingedInt16 y4 = y1;
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

PREFIX void spTriangle(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1,   SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2,   SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3,   SP_UnsingedInt16 color)
{
  if (spAlphaTest && color==SP_ALPHA_COLOR)
    return;
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

inline void draw_pixel_tex_ztest_zset(SP_SingedInt16 x,SP_SingedInt16 y,Sint32 z,SP_SingedInt16 u,SP_SingedInt16 v,SP_UnsingedInt16 color)
{
  if (z<0 && z > spZBuffer[x+y*spTargetX])
  {
    #ifdef FAST_BUT_UGLY
    //if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
    //  spTargetPixel[x+y*spTargetX] = color; 
    //else
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
      spTargetPixel[x+y*spTargetX] = ((spTexturePixel[u+v*spTextureX] * color >> 16) & 63488)
                                   + (((spTexturePixel[u+v*spTextureX] & 2047) * (color & 2047) >> 11) & 2016)
                                   + ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31) >> 5);
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel_tex_ztest(SP_SingedInt16 x,SP_SingedInt16 y,Sint32 z,SP_SingedInt16 u,SP_SingedInt16 v,SP_UnsingedInt16 color)
{
  if (z<0 && z > spZBuffer[x+y*spTargetX])
  {
    #ifdef FAST_BUT_UGLY
    //if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
    //  spTargetPixel[x+y*spTargetX] = color; 
    //else
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
      spTargetPixel[x+y*spTargetX] = ((spTexturePixel[u+v*spTextureX] * color >> 16) & 63488)
                                   + (((spTexturePixel[u+v*spTextureX] & 2047) * (color & 2047) >> 11) & 2016)
                                   + ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31) >> 5);
  }
}

inline void draw_pixel_tex_zset(SP_SingedInt16 x,SP_SingedInt16 y,Sint32 z,SP_SingedInt16 u,SP_SingedInt16 v,SP_UnsingedInt16 color)
{
  #ifdef FAST_BUT_UGLY
  //if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
  //  spTargetPixel[x+y*spTargetX] = color; 
  //else
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
    spTargetPixel[x+y*spTargetX] = ((spTexturePixel[u+v*spTextureX] * color >> 16) & 63488)
                                 + (((spTexturePixel[u+v*spTextureX] & 2047) * (color & 2047) >> 11) & 2016)
                                 + ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31) >> 5);
  spZBuffer[x+y*spTargetX] = z;
}

inline void draw_pixel_tex(SP_SingedInt16 x,SP_SingedInt16 y,SP_SingedInt16 u,SP_SingedInt16 v,SP_UnsingedInt16 color)
{
  #ifdef FAST_BUT_UGLY
  //if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
  //  spTargetPixel[x+y*spTargetX] = color; 
  //else
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
    spTargetPixel[x+y*spTargetX] = ((spTexturePixel[u+v*spTextureX] * color >> 16) & 63488)
                                 + (((spTexturePixel[u+v*spTextureX] & 2047) * (color & 2047) >> 11) & 2016)
                                 + ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31) >> 5);
}

inline void draw_line_tex_ztest_zset(SP_SingedInt16 x1,Sint32 z1,Sint32 u1,Sint32 v1,SP_SingedInt16 x2,Sint32 z2,Sint32 u2,Sint32 v2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
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

inline void draw_line_tex_ztest(SP_SingedInt16 x1,Sint32 z1,Sint32 u1,Sint32 v1,SP_SingedInt16 x2,Sint32 z2,Sint32 u2,Sint32 v2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
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

inline void draw_line_tex_zset(SP_SingedInt16 x1,Sint32 z1,Sint32 u1,Sint32 v1,SP_SingedInt16 x2,Sint32 z2,Sint32 u2,Sint32 v2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
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

inline void draw_line_tex(SP_SingedInt16 x1,Sint32 u1,Sint32 v1,SP_SingedInt16 x2,Sint32 u2,Sint32 v2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sU,Sint32 sV)
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

inline void sp_intern_Triangle_tex_ztest_zset(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  int div = y2-y1;
  Sint32 mul = y3-y1;
  Sint32 mul32 = mul*one_over_x(div);
  SP_SingedInt16 x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
  Sint32 z4 = z1+mul*z_div(z2-z1,div);
  SP_SingedInt16 u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);

  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;

  mul = one_over_x(y1-y2);
  Sint32 sX_l = (x1-x2)*mul;
  Sint32 sU_l = (u1-u2)*mul;
  Sint32 sV_l = (v1-v2)*mul;
  Sint32 sZ_l = z_div(z1-z2,y1-y2);

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 zr = zl;
  mul = one_over_x(y1-y3);
  Sint32 sX_r = (x1-x3)*mul;
  Sint32 sU_r = (u1-u3)*mul;
  Sint32 sV_r = (v1-v3)*mul;
  Sint32 sZ_r = z_div(z1-z3,y1-y3);

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

    Sint32 mul = one_over_x(x4-x3);
    Sint32 sU = (u4-u3)*mul;
    Sint32 sV = (v4-v3)*mul;
    Sint32 sZ = z_div(z4-z3,x4-x3);
    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      { 
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
      for (y = y1; y < y3; y++)
      {
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
  mul = one_over_x(y2-y3);
  sX_r = (x2-x3)*mul;
  sU_r = (u2-u3)*mul;
  sV_r = (v2-v3)*mul;
  sZ_r = z_div(z2-z3,y2-y3);

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
  
  mul = one_over_x(x4-x3);
  Sint32 sU = (u4-u3)*mul;
  Sint32 sV = (v4-v3)*mul;
  Sint32 sZ = z_div(z4-z3,x4-x3);
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {      
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
    for (y = y3; y <= y2; y++)
    {
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

inline void sp_intern_Triangle_tex_ztest(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  int div = y2-y1;
  Sint32 mul = y3-y1;
  Sint32 mul32 = mul*one_over_x(div);
  SP_SingedInt16 x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
  Sint32 z4 = z1+mul*z_div(z2-z1,div);
  SP_SingedInt16 u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);

  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;

  mul = one_over_x(y1-y2);
  Sint32 sX_l = (x1-x2)*mul;
  Sint32 sU_l = (u1-u2)*mul;
  Sint32 sV_l = (v1-v2)*mul;
  Sint32 sZ_l = z_div(z1-z2,y1-y2);

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 zr = zl;
  mul = one_over_x(y1-y3);
  Sint32 sX_r = (x1-x3)*mul;
  Sint32 sU_r = (u1-u3)*mul;
  Sint32 sV_r = (v1-v3)*mul;
  Sint32 sZ_r = z_div(z1-z3,y1-y3);

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

    Sint32 mul = one_over_x(x4-x3);
    Sint32 sU = (u4-u3)*mul;
    Sint32 sV = (v4-v3)*mul;
    Sint32 sZ = z_div(z4-z3,x4-x3);
    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      { 
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
      for (y = y1; y < y3; y++)
      {
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
  mul = one_over_x(y2-y3);
  sX_r = (x2-x3)*mul;
  sU_r = (u2-u3)*mul;
  sV_r = (v2-v3)*mul;
  sZ_r = z_div(z2-z3,y2-y3);

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
  
  mul = one_over_x(x4-x3);
  Sint32 sU = (u4-u3)*mul;
  Sint32 sV = (v4-v3)*mul;
  Sint32 sZ = z_div(z4-z3,x4-x3);
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {      
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
    for (y = y3; y <= y2; y++)
    {
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

inline void sp_intern_Triangle_tex_zset(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  int div = y2-y1;
  Sint32 mul = y3-y1;
  Sint32 mul32 = mul*one_over_x(div);
  SP_SingedInt16 x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
  Sint32 z4 = z1+mul*z_div(z2-z1,div);
  SP_SingedInt16 u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);

  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;

  mul = one_over_x(y1-y2);
  Sint32 sX_l = (x1-x2)*mul;
  Sint32 sU_l = (u1-u2)*mul;
  Sint32 sV_l = (v1-v2)*mul;
  Sint32 sZ_l = z_div(z1-z2,y1-y2);

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 zr = zl;
  mul = one_over_x(y1-y3);
  Sint32 sX_r = (x1-x3)*mul;
  Sint32 sU_r = (u1-u3)*mul;
  Sint32 sV_r = (v1-v3)*mul;
  Sint32 sZ_r = z_div(z1-z3,y1-y3);

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

    Sint32 mul = one_over_x(x4-x3);
    Sint32 sU = (u4-u3)*mul;
    Sint32 sV = (v4-v3)*mul;
    Sint32 sZ = z_div(z4-z3,x4-x3);
    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      { 
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
      for (y = y1; y < y3; y++)
      {
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
  mul = one_over_x(y2-y3);
  sX_r = (x2-x3)*mul;
  sU_r = (u2-u3)*mul;
  sV_r = (v2-v3)*mul;
  sZ_r = z_div(z2-z3,y2-y3);

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
  
  mul = one_over_x(x4-x3);
  Sint32 sU = (u4-u3)*mul;
  Sint32 sV = (v4-v3)*mul;
  Sint32 sZ = z_div(z4-z3,x4-x3);
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {      
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
    for (y = y3; y <= y2; y++)
    {
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

inline void sp_intern_Triangle_tex(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  int div = y2-y1;
  Sint32 mul = y3-y1;
  Sint32 mul32 = mul*one_over_x(div);
  SP_SingedInt16 x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);

  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;

  mul = one_over_x(y1-y2);
  Sint32 sX_l = (x1-x2)*mul;
  Sint32 sU_l = (u1-u2)*mul;
  Sint32 sV_l = (v1-v2)*mul;

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  mul = one_over_x(y1-y3);
  Sint32 sX_r = (x1-x3)*mul;
  Sint32 sU_r = (u1-u3)*mul;
  Sint32 sV_r = (v1-v3)*mul;

  if (y3 < 0)
  {
    int diff = y3-y1;
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

    Sint32 mul = one_over_x(x4-x3);
    Sint32 sU = (u4-u3)*mul;
    Sint32 sV = (v4-v3)*mul;
    if (x4 < x3)
    {
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
  mul = one_over_x(y2-y3);
  sX_r = (x2-x3)*mul;
  sU_r = (u2-u3)*mul;
  sV_r = (v2-v3)*mul;

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
  
  mul = one_over_x(x4-x3);
  Sint32 sU = (u4-u3)*mul;
  Sint32 sV = (v4-v3)*mul;
  if (x4 < x3)
  {
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

inline void draw_pixel_tex_ztest_zset_alpha(SP_SingedInt16 x,SP_SingedInt16 y,Sint32 z,SP_SingedInt16 u,SP_SingedInt16 v,SP_UnsingedInt16 color)
{
  if (z<0 && z>spZBuffer[x+y*spTargetX])
  {
    #ifdef FAST_BUT_UGLY
    //if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
    //  spTargetPixel[x+y*spTargetX] = color; 
    //else
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
    if (spTexturePixel[u+v*spTextureX]!=SP_ALPHA_COLOR)
    {
      spTargetPixel[x+y*spTargetX] = ((spTexturePixel[u+v*spTextureX] * color >> 16) & 63488)
                                   + (((spTexturePixel[u+v*spTextureX] & 2047) * (color & 2047) >> 11) & 2016)
                                   + ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31) >> 5);
      spZBuffer[x+y*spTargetX] = z;
    }
  }
}

inline void draw_pixel_tex_ztest_alpha(SP_SingedInt16 x,SP_SingedInt16 y,Sint32 z,SP_SingedInt16 u,SP_SingedInt16 v,SP_UnsingedInt16 color)
{
  if (z<0 && z > spZBuffer[x+y*spTargetX])
  {
    #ifdef FAST_BUT_UGLY
    //if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
    //  spTargetPixel[x+y*spTargetX] = color; 
    //else
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
    if (spTexturePixel[u+v*spTextureX]!=SP_ALPHA_COLOR)
      spTargetPixel[x+y*spTargetX] = ((spTexturePixel[u+v*spTextureX] * color >> 16) & 63488)
                                   + (((spTexturePixel[u+v*spTextureX] & 2047) * (color & 2047) >> 11) & 2016)
                                   + ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31) >> 5);
  }
}

inline void draw_pixel_tex_zset_alpha(SP_SingedInt16 x,SP_SingedInt16 y,Sint32 z,SP_SingedInt16 u,SP_SingedInt16 v,SP_UnsingedInt16 color)
{
  #ifdef FAST_BUT_UGLY
  //if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
  //  spTargetPixel[x+y*spTargetX] = color; 
  //else
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
  if (spTexturePixel[u+v*spTextureX]!=SP_ALPHA_COLOR)
  {
    spTargetPixel[x+y*spTargetX] = ((spTexturePixel[u+v*spTextureX] * color >> 16) & 63488)
                                 + (((spTexturePixel[u+v*spTextureX] & 2047) * (color & 2047) >> 11) & 2016)
                                 + ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31) >> 5);
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel_tex_alpha(SP_SingedInt16 x,SP_SingedInt16 y,SP_SingedInt16 u,SP_SingedInt16 v,SP_UnsingedInt16 color)
{
  #ifdef FAST_BUT_UGLY
  //if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
  //  spTargetPixel[x+y*spTargetX] = color; 
  //else
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
  if (spTexturePixel[u+v*spTextureX]!=SP_ALPHA_COLOR)
  {  
    spTargetPixel[x+y*spTargetX] = ((spTexturePixel[u+v*spTextureX] * color >> 16) & 63488)
                                 + (((spTexturePixel[u+v*spTextureX] & 2047) * (color & 2047) >> 11) & 2016)
                                 + ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31) >> 5);
  }
}

inline void draw_line_tex_ztest_zset_alpha(SP_SingedInt16 x1,Sint32 z1,Sint32 u1,Sint32 v1,SP_SingedInt16 x2,Sint32 z2,Sint32 u2,Sint32 v2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
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
    draw_pixel_tex_ztest_zset_alpha(x,y,z,u>>SP_PRIM_ACCURACY,v>>SP_PRIM_ACCURACY,color);
    u += sU;
    v += sV;
    z += sZ;
  }
}

inline void draw_line_tex_ztest_alpha(SP_SingedInt16 x1,Sint32 z1,Sint32 u1,Sint32 v1,SP_SingedInt16 x2,Sint32 z2,Sint32 u2,Sint32 v2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
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
    draw_pixel_tex_ztest_alpha(x,y,z,u>>SP_PRIM_ACCURACY,v>>SP_PRIM_ACCURACY,color);
    u += sU;
    v += sV;
    z += sZ;
  }
}

inline void draw_line_tex_zset_alpha(SP_SingedInt16 x1,Sint32 z1,Sint32 u1,Sint32 v1,SP_SingedInt16 x2,Sint32 z2,Sint32 u2,Sint32 v2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
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
    draw_pixel_tex_zset_alpha(x,y,z,u>>SP_PRIM_ACCURACY,v>>SP_PRIM_ACCURACY,color);
    u += sU;
    v += sV;
    z += sZ;
  }
}

inline void draw_line_tex_alpha(SP_SingedInt16 x1,Sint32 u1,Sint32 v1,SP_SingedInt16 x2,Sint32 u2,Sint32 v2,SP_SingedInt16 y,SP_UnsingedInt16 color,Sint32 sU,Sint32 sV)
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
    draw_pixel_tex_alpha(x,y,u>>SP_PRIM_ACCURACY,v>>SP_PRIM_ACCURACY,color);
    u += sU;
    v += sV;
  }
}

/*inline void horz_tex_zset_zset_alpha(SP_SingedInt16 xl,SP_SingedInt16 xr,SP_SingedInt16 y,
                              SP_SingedInt16 zl,SP_SingedInt16 zr,
                              SP_SingedInt16 ul,SP_SingedInt16 ur,
                              SP_SingedInt16 vl,SP_SingedInt16 vr,SP_UnsingedInt16 color)
{
  if (xl+1 >= xr)
    return;
  Sint32 xm = xl+xr>>1;
  Sint32 zm = zl+zr>>1;
  Sint32 um = ul+ur>>1;
  Sint32 vm = vl+vr>>1;
  draw_pixel_tex_ztest_zset_alpha(xm,y,zm,um,vm,color);
  horz_tex_zset_zset_alpha(xl,xm,y,zl,zm,ul,um,vl,vm,color);
  horz_tex_zset_zset_alpha(xm,xr,y,zm,zr,um,ur,vm,vr,color);
}

inline void vert_tex_ztest_zset_alpha(SP_SingedInt16 xtl,SP_SingedInt16 xtr,SP_SingedInt16 yt,
                                       Sint32 ztl,        Sint32 ztr,
                               SP_SingedInt16 utl,SP_SingedInt16 utr,
                               SP_SingedInt16 vtl,SP_SingedInt16 vtr,
                               SP_SingedInt16 xbl,SP_SingedInt16 xbr,SP_SingedInt16 yb,
                                       Sint32 zbl,        Sint32 zbr,
                               SP_SingedInt16 ubl,SP_SingedInt16 ubr,
                               SP_SingedInt16 vbl,SP_SingedInt16 vbr,SP_UnsingedInt16 color)
{
  if (yt+1 >= yb)
    return;
  Sint32 xml = xtl + xbl >>1;
  Sint32 xmr = xtr + xbr >>1;
  Sint32 ym  =  yt +  yb >>1;
  Sint32 zml = ztl + zbl >>1;
  Sint32 zmr = ztr + zbr >>1;
  Sint32 uml = utl + ubl >>1;
  Sint32 umr = utr + ubr >>1;
  Sint32 vml = vtl + vbl >>1;
  Sint32 vmr = vtr + vbr >>1;
  vert_tex_ztest_zset_alpha(xtl,xtr,yt,ztl,ztr,utl,utr,vtl,vtr,
                            xml,xmr,ym,zml,zmr,uml,umr,vml,vmr,color);
  draw_pixel_tex_ztest_zset_alpha(xml,ym,zml,uml,vml,color);
  if (xml < xmr)
    horz_tex_zset_zset_alpha(xml,xmr,ym,zml,zmr,uml,umr,vml,vmr,color);
  else
    horz_tex_zset_zset_alpha(xmr,xml,ym,zmr,zml,umr,uml,vmr,vml,color);
  draw_pixel_tex_ztest_zset_alpha(xmr,ym,zmr,umr,vmr,color);
  vert_tex_ztest_zset_alpha(xml,xmr,ym,zml,zmr,uml,umr,vml,vmr,
                            xbl,xbr,yb,zbl,zbr,ubl,ubr,vbl,vbr,color);
}*/

inline void sp_intern_Triangle_tex_ztest_zset_alpha(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  int div = y2-y1;
  Sint32 mul = y3-y1;
  Sint32 mul32 = mul*one_over_x(div);
  SP_SingedInt16 x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
  Sint32 z4 = z1+mul*z_div(z2-z1,div);
  SP_SingedInt16 u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);
  
  /*draw_pixel_tex_ztest_zset_alpha(x1,y1,z1,u1,v1,color);
  vert_tex_ztest_zset_alpha(x1,x1,y1,z1,z1,u1,u1,v1,v1,
                            x3,x4,y3,z3,z4,u3,u4,v3,v4,color);
  if (x3 < x4)
    horz_tex_zset_zset_alpha(x3,x4,y3,z3,z4,u3,u4,v3,v4,color);
  else
    horz_tex_zset_zset_alpha(x4,x3,y3,z4,z3,u4,u3,v4,v3,color);
  vert_tex_ztest_zset_alpha(x3,x4,y3,z3,z4,u3,u4,v3,v4,
                            x2,x2,y2,z2,z2,u2,u2,v2,v2,color);
  draw_pixel_tex_ztest_zset_alpha(x2,y2,z2,u2,v2,color);
  */
  
  
  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;

  mul = one_over_x(y1-y2);
  Sint32 sX_l = (x1-x2)*mul;
  Sint32 sU_l = (u1-u2)*mul;
  Sint32 sV_l = (v1-v2)*mul;
  Sint32 sZ_l = z_div(z1-z2,y1-y2);

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 zr = zl;
  mul = one_over_x(y1-y3);
  Sint32 sX_r = (x1-x3)*mul;
  Sint32 sU_r = (u1-u3)*mul;
  Sint32 sV_r = (v1-v3)*mul;
  Sint32 sZ_r = z_div(z1-z3,y1-y3);

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

    Sint32 mul = one_over_x(x4-x3);
    Sint32 sU = (u4-u3)*mul;
    Sint32 sV = (v4-v3)*mul;
    Sint32 sZ = z_div(z4-z3,x4-x3);
    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      { 
        draw_line_tex_ztest_zset_alpha(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
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
      for (y = y1; y < y3; y++)
      {
        draw_line_tex_ztest_zset_alpha(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
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
  mul = one_over_x(y2-y3);
  sX_r = (x2-x3)*mul;
  sU_r = (u2-u3)*mul;
  sV_r = (v2-v3)*mul;
  sZ_r = z_div(z2-z3,y2-y3);

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
  
  mul = one_over_x(x4-x3);
  Sint32 sU = (u4-u3)*mul;
  Sint32 sV = (v4-v3)*mul;
  Sint32 sZ = z_div(z4-z3,x4-x3);
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {      
      draw_line_tex_ztest_zset_alpha(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
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
    for (y = y3; y <= y2; y++)
    {
      draw_line_tex_ztest_zset_alpha(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
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

inline void sp_intern_Triangle_tex_ztest_alpha(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  int div = y2-y1;
  Sint32 mul = y3-y1;
  Sint32 mul32 = mul*one_over_x(div);
  SP_SingedInt16 x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
  Sint32 z4 = z1+mul*z_div(z2-z1,div);
  SP_SingedInt16 u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);

  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;

  mul = one_over_x(y1-y2);
  Sint32 sX_l = (x1-x2)*mul;
  Sint32 sU_l = (u1-u2)*mul;
  Sint32 sV_l = (v1-v2)*mul;
  Sint32 sZ_l = z_div(z1-z2,y1-y2);

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 zr = zl;
  mul = one_over_x(y1-y3);
  Sint32 sX_r = (x1-x3)*mul;
  Sint32 sU_r = (u1-u3)*mul;
  Sint32 sV_r = (v1-v3)*mul;
  Sint32 sZ_r = z_div(z1-z3,y1-y3);

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

    Sint32 mul = one_over_x(x4-x3);
    Sint32 sU = (u4-u3)*mul;
    Sint32 sV = (v4-v3)*mul;
    Sint32 sZ = z_div(z4-z3,x4-x3);
    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      { 
        draw_line_tex_ztest_alpha(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
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
      for (y = y1; y < y3; y++)
      {
        draw_line_tex_ztest_alpha(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
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
  mul = one_over_x(y2-y3);
  sX_r = (x2-x3)*mul;
  sU_r = (u2-u3)*mul;
  sV_r = (v2-v3)*mul;
  sZ_r = z_div(z2-z3,y2-y3);

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
  
  mul = one_over_x(x4-x3);
  Sint32 sU = (u4-u3)*mul;
  Sint32 sV = (v4-v3)*mul;
  Sint32 sZ = z_div(z4-z3,x4-x3);
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {      
      draw_line_tex_ztest_alpha(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
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
    for (y = y3; y <= y2; y++)
    {
      draw_line_tex_ztest_alpha(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
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

inline void sp_intern_Triangle_tex_zset_alpha(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  int div = y2-y1;
  Sint32 mul = y3-y1;
  Sint32 mul32 = mul*one_over_x(div);
  SP_SingedInt16 x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
  Sint32 z4 = z1+mul*z_div(z2-z1,div);
  SP_SingedInt16 u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);

  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;

  mul = one_over_x(y1-y2);
  Sint32 sX_l = (x1-x2)*mul;
  Sint32 sU_l = (u1-u2)*mul;
  Sint32 sV_l = (v1-v2)*mul;
  Sint32 sZ_l = z_div(z1-z2,y1-y2);

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 zr = zl;
  mul = one_over_x(y1-y3);
  Sint32 sX_r = (x1-x3)*mul;
  Sint32 sU_r = (u1-u3)*mul;
  Sint32 sV_r = (v1-v3)*mul;
  Sint32 sZ_r = z_div(z1-z3,y1-y3);

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

    Sint32 mul = one_over_x(x4-x3);
    Sint32 sU = (u4-u3)*mul;
    Sint32 sV = (v4-v3)*mul;
    Sint32 sZ = z_div(z4-z3,x4-x3);
    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      { 
        draw_line_tex_zset_alpha(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
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
      for (y = y1; y < y3; y++)
      {
        draw_line_tex_zset_alpha(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
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
  mul = one_over_x(y2-y3);
  sX_r = (x2-x3)*mul;
  sU_r = (u2-u3)*mul;
  sV_r = (v2-v3)*mul;
  sZ_r = z_div(z2-z3,y2-y3);

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
  
  mul = one_over_x(x4-x3);
  Sint32 sU = (u4-u3)*mul;
  Sint32 sV = (v4-v3)*mul;
  Sint32 sZ = z_div(z4-z3,x4-x3);
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {      
      draw_line_tex_zset_alpha(xl>>SP_PRIM_ACCURACY,zl,ul,vl,
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
    for (y = y3; y <= y2; y++)
    {
      draw_line_tex_zset_alpha(xr>>SP_PRIM_ACCURACY,zr,ur,vr,
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

inline void sp_intern_Triangle_tex_alpha(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_UnsingedInt16 color)
{
  int y;
  if (y2 < 0)
    return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  int div = y2-y1;
  Sint32 mul = y3-y1;
  Sint32 mul32 = mul*one_over_x(div);
  SP_SingedInt16 x4 = x1+((x2-x1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 u4 = u1+((u2-u1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 v4 = v1+((v2-v1)*mul32>>SP_PRIM_ACCURACY);

  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;

  mul = one_over_x(y1-y2);
  Sint32 sX_l = (x1-x2)*mul;
  Sint32 sU_l = (u1-u2)*mul;
  Sint32 sV_l = (v1-v2)*mul;

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  mul = one_over_x(y1-y3);
  Sint32 sX_r = (x1-x3)*mul;
  Sint32 sU_r = (u1-u3)*mul;
  Sint32 sV_r = (v1-v3)*mul;

  if (y3 < 0)
  {
    int diff = y3-y1;
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

    Sint32 mul = one_over_x(x4-x3);
    Sint32 sU = (u4-u3)*mul;
    Sint32 sV = (v4-v3)*mul;
    if (x4 < x3)
    {
      for (y = y1; y < y3; y++)
      { 
        draw_line_tex_alpha(xl>>SP_PRIM_ACCURACY,ul,vl,
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
      for (y = y1; y < y3; y++)
      {
        draw_line_tex_alpha(xr>>SP_PRIM_ACCURACY,ur,vr,
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
  mul = one_over_x(y2-y3);
  sX_r = (x2-x3)*mul;
  sU_r = (u2-u3)*mul;
  sV_r = (v2-v3)*mul;

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
  
  mul = one_over_x(x4-x3);
  Sint32 sU = (u4-u3)*mul;
  Sint32 sV = (v4-v3)*mul;
  if (x4 < x3)
  {
    for (y = y3; y <= y2; y++)
    {      
      draw_line_tex_alpha(xl>>SP_PRIM_ACCURACY,ul,vl,
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
    for (y = y3; y <= y2; y++)
    {
      draw_line_tex_alpha(xr>>SP_PRIM_ACCURACY,ur,vr,
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

PREFIX void spTriangle_tex(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_UnsingedInt16 color)
{
  if (spAlphaTest && color==SP_ALPHA_COLOR)
    return;
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
  if (spAlphaTest)
  {
    if (spZSet)
    {
      if (spZTest)
        sp_intern_Triangle_tex_ztest_zset_alpha(x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
      else
        sp_intern_Triangle_tex_zset_alpha      (x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
    }
    else
    {
      if (spZTest)
        sp_intern_Triangle_tex_ztest_alpha     (x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
      else
        sp_intern_Triangle_tex_alpha           (x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
    }
  }
  else
  {
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
}


PREFIX void spQuad(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 x4,SP_SingedInt16 y4, Sint32 z4, SP_UnsingedInt16 color)
{
  spTriangle(x1,y1,z1,x2,y2,z2,x3,y3,z3,color);
  spTriangle(x1,y1,z1,x3,y3,z3,x4,y4,z4,color);
  /*SP_SingedInt16 mx = x1+x2+x3+x4>>2;
  SP_SingedInt16 my = y1+y2+y3+y4>>2;
  Sint32 mz = (z1>>2)+(z2>>2)+(z3>>2)+(z4>>2);
  spTriangle(mx,my,mz,x1,y1,z1,x2,y2,z2,color);
  spTriangle(mx,my,mz,x2,y2,z2,x3,y3,z3,color);
  spTriangle(mx,my,mz,x3,y3,z3,x4,y4,z4,color);
  spTriangle(mx,my,mz,x4,y4,z4,x1,y1,z1,color);*/
}

inline void sp_intern_Quad_tex_ztest_zset(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_SingedInt16 x4, SP_SingedInt16 y4, Sint32 z4, SP_SingedInt16 u4, SP_SingedInt16 v4, SP_UnsingedInt16 color)
{
  int y;
  if (y3>y4)
  {
    if (y3 < 0)
      return;
  }
  else
    if (y4 < 0)
      return;
  if (y1 >= spTargetY)
    return;
  SDL_LockSurface(spTarget);

  Sint32 div = y4-y1;
  Sint32 mul = y2-y1;
  Sint32 mul32 = mul*one_over_x(div);
  SP_SingedInt16 xt = x1+((x4-x1)*mul32>>SP_PRIM_ACCURACY);
  Sint32 zt = z1+mul*z_div(z4-z1,div);
  SP_SingedInt16 ut = u1+((u4-u1)*mul32>>SP_PRIM_ACCURACY);
  SP_SingedInt16 vt = v1+((v4-v1)*mul32>>SP_PRIM_ACCURACY);

  Sint32 xl = x1<<SP_PRIM_ACCURACY;
  Sint32 ul = u1<<SP_PRIM_ACCURACY;
  Sint32 vl = v1<<SP_PRIM_ACCURACY;
  Sint32 zl = z1;

  mul = one_over_x(y1-y4);
  Sint32 sX_l = (x1-x4)*mul;
  Sint32 sU_l = (u1-u4)*mul;
  Sint32 sV_l = (v1-v4)*mul;
  Sint32 sZ_l = z_div(z1-z4,y1-y4);

  Sint32 xr = xl;
  Sint32 ur = ul;
  Sint32 vr = vl;
  Sint32 zr = zl;
  mul = one_over_x(y1-y2);
  Sint32 sX_r = (x1-x2)*mul;
  Sint32 sU_r = (u1-u2)*mul;
  Sint32 sV_r = (v1-v2)*mul;
  Sint32 sZ_r = z_div(z1-z2,y1-y2);

  if (y2 < 0)
  {
    int diff = y2-y1;
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
    if (y2 >= spTargetY)
      y2 = spTargetY-1;

    Sint32 mul = one_over_x(xt-x2);
    Sint32 sU = (ut-u2)*mul;
    Sint32 sV = (vt-v2)*mul;
    Sint32 sZ = z_div(zt-z2,xt-x2);
    if (xt < x2)
    {
      for (y = y1; y < y2; y++)
      { 
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
      for (y = y1; y < y2; y++)
      {
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

  xr = x2 << SP_PRIM_ACCURACY;
  ur = u2 << SP_PRIM_ACCURACY;
  vr = v2 << SP_PRIM_ACCURACY;
  zr = z2;
  mul = one_over_x(y3-y2);
  sX_r = (x3-x2)*mul;
  sU_r = (u3-u2)*mul;
  sV_r = (v3-v2)*mul;
  sZ_r = z_div(z3-z2,y3-y2);
  
  if (y4 < y3)
  {      
    if (y4 < 0)
    {
      int diff = -y4;
      y4 = 0;
      xl += sX_l*diff;
      ul += sU_l*diff;
      vl += sV_l*diff;
      zl += sZ_l*diff;
      xr += sX_r*diff;
      ur += sU_r*diff;
      vr += sV_r*diff;
      zr += sZ_r*diff;
    }
      
    if (y4 >= spTargetY)
      y4 = spTargetY-1;
      
    mul = y4-y2;
    div = y3-y2;
    mul32 = mul*one_over_x(div);
    xt = x2+((x3-x2)*mul32>>SP_PRIM_ACCURACY);
    zt = z2+mul*z_div(z3-z2,div);
    ut = u2+((u3-u2)*mul32>>SP_PRIM_ACCURACY);
    vt = v2+((v3-v2)*mul32>>SP_PRIM_ACCURACY);
      
    if (xt > x4)
    {
      for (y = y2; y <= y4; y++)
      {      
        mul = one_over_x(xl-xr>>SP_PRIM_ACCURACY);
        Sint32 sU = (ul-ur>>SP_PRIM_ACCURACY)*mul;
        Sint32 sV = (vl-vr>>SP_PRIM_ACCURACY)*mul;
        Sint32 sZ = z_div(zl-zr>>SP_PRIM_ACCURACY,xl-xr>>SP_PRIM_ACCURACY);
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
      for (y = y2; y <= y4; y++)
      {
        mul = one_over_x(xl-xr>>SP_PRIM_ACCURACY);
        Sint32 sU = (ul-ur>>SP_PRIM_ACCURACY)*mul;
        Sint32 sV = (vl-vr>>SP_PRIM_ACCURACY)*mul;
        Sint32 sZ = z_div(zl-zr>>SP_PRIM_ACCURACY,xl-xr>>SP_PRIM_ACCURACY);
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
          
    if (y3 >= spTargetY)
      y3 = spTargetY-1;


    xl = x4 << SP_PRIM_ACCURACY;
    ul = u4 << SP_PRIM_ACCURACY;
    vl = v4 << SP_PRIM_ACCURACY;
    zl = z4;
    mul = one_over_x(y3-y4);
    sX_l = (x3-x4)*mul;
    sU_l = (u3-u4)*mul;
    sV_l = (v3-v4)*mul;
    sZ_l = z_div(z3-z4,y3-y4);
            
    /*mul = one_over_x(xt-x4);
    Sint32 sU = (ut-u4)*mul;
    Sint32 sV = (vt-v4)*mul;
    Sint32 sZ = z_div(zt-z4,xt-x4);*/
    if (xt > x4)
    {
      for (y = y4; y <= y3; y++)
      {      
        mul = one_over_x(xl-xr>>SP_PRIM_ACCURACY);
        Sint32 sU = (ul-ur>>SP_PRIM_ACCURACY)*mul;
        Sint32 sV = (vl-vr>>SP_PRIM_ACCURACY)*mul;
        Sint32 sZ = z_div(zl-zr>>SP_PRIM_ACCURACY,xl-xr>>SP_PRIM_ACCURACY);
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
      for (y = y4; y <= y3; y++)
      {
        mul = one_over_x(xl-xr>>SP_PRIM_ACCURACY);
        Sint32 sU = (ul-ur>>SP_PRIM_ACCURACY)*mul;
        Sint32 sV = (vl-vr>>SP_PRIM_ACCURACY)*mul;
        Sint32 sZ = z_div(zl-zr>>SP_PRIM_ACCURACY,xl-xr>>SP_PRIM_ACCURACY);
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
  SDL_UnlockSurface(spTarget);  
}


PREFIX void spQuad_tex(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_SingedInt16 x4, SP_SingedInt16 y4, Sint32 z4, SP_SingedInt16 u4, SP_SingedInt16 v4, SP_UnsingedInt16 color)
{
  spTriangle_tex(x1,y1,z1,u1,v1,
                 x2,y2,z2,u2,v2,
                 x3,y3,z3,u3,v3,color);
  spTriangle_tex(x1,y1,z1,u1,v1,
                 x3,y3,z3,u3,v3,
                 x4,y4,z4,u4,v4,color);
  
  /*int smallest = 1; //y1
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
      sp_intern_Quad_tex_ztest_zset(x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,x4,y4,z4,u4,v4,color);
    else
      sp_intern_Triangle_tex_zset      (x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
  }
  else
  {
    if (spZTest)
      sp_intern_Triangle_tex_ztest     (x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
    else
      sp_intern_Triangle_tex           (x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
  }*/
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

PREFIX void spSetZTest(Uint32 test)
{
  spZTest = test;
}

PREFIX void spSetZSet(Uint32 test)
{
  spZSet = test;
}

PREFIX void spSetAlphaTest(Uint32 test)
{
  spAlphaTest = test;
}

#ifndef ARMCPU
#ifndef X86CPU   
PREFIX void spHorizentalLine(Uint16* pixel,Sint32 x,Sint32 y,Sint32 l_,SP_UnsingedInt16 color_,Uint32 check,Sint32 engineWindowX,Sint32 engineWindowY)
{
  //l_++;
  if (check)
  { 
    if (y<0)
      return;
    if (y>=engineWindowY) //Their is a FUCKING Bug if I write in the last line on the gp2x. Dont know why...
      return;
    if (x<0)
    {
      l_=x+l_;
      x=0;
    }
    if (x+l_>=engineWindowX)
      l_=engineWindowX-x;
    if (l_<=0)
      return;
  }
  Uint32 pos=(x+y*engineWindowX);
  int a;
  for (a=pos;a<pos+l_;a++)
    pixel[a]=color_;
}    
#endif
#endif

#ifdef X86CPU
PREFIX void spHorizentalLine(Uint16* pixel,Sint32 x,Sint32 y,Sint32 l_,SP_UnsingedInt16 color_,Uint32 check,Sint32 engineWindowX,Sint32 engineWindowY)
{
  //l_++;
  if (check)
  { 
    if (y<0)
      return;
    if (y>=engineWindowY) //Their is a FUCKING Bug if I write in the last line on the gp2x. Dont know why...
      return;
    if (x<0)
    {
      l_=x+l_;
      x=0;
    }
    if (x+l_>=engineWindowX)
      l_=engineWindowX-x;
    if (l_<=0)
      return;
  }
  Uint32 pos=(x+y*engineWindowX);
  if ((int)pixel+(pos<<1) & 2)
  {
    pixel[pos]=color_;
    pos++;
    l_--;
  }
  Uint32 color=color_ | (color_<<16);
  int a;
  for (a=pos>>1;a<(pos+l_>>1);a++)
    ((Uint32*)pixel)[a]=color;
  pixel[pos+l_-1]=color_;
}    
#endif

PREFIX void spBlitSurface(Sint32 x,Sint32 y,Sint32 z,SDL_Surface* surface)
{
  if (z>=0)
    return;
  if (spAlphaTest)
  {
    if (spZSet)
    {
      if (spZTest)
      {
        SDL_LockSurface(spTarget);
        Uint16 *pixel;
        pixel = (Uint16*)(surface->pixels);
        int u = -surface->w/2;
        if (x+u < 0)
          u = -x;
        int maxu = surface->w/2;
        if (x+maxu > spTargetX)
          maxu = spTargetX - x;
        int minv = -surface->h/2;
        if (y+minv < 0)
          minv = -y;
        int maxv = surface->h/2;
        if (y+maxv > spTargetY)
          maxv = spTargetY - y;
        int v;
        for (;u < maxu;u++)
          for (v = minv;v < maxv;v++)
            if (pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w]!=SP_ALPHA_COLOR && z > spZBuffer[(x+u)+(y+v)*spTargetX])
            {
              spTargetPixel[(x+u)+(y+v)*spTargetX] = pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w];
              spZBuffer[(x+u)+(y+v)*spTargetX] = z;
            }
        SDL_UnlockSurface(spTarget);
      }
      else
      {
        int u = x-surface->w/2;
        if (u < 0)
          u = 0;
        if (u >= spTargetX)
          return;
        int v = y-surface->h/2;
        if (v < 0)
          v = 0;
        if (v >= spTargetY)
          return;
        int umax = x+surface->w/2;
        if (umax > spTargetX)
          umax = spTargetX;
        int vmax = y+surface->h/2;
        if (vmax > spTargetY)
          vmax = spTargetY;
        for (;u<umax;u++)
          for (;v<vmax;v++)
            spZBuffer[u+v*spTargetX] = z;
        SDL_Rect dest;
        dest.x = x-surface->w/2;
        dest.y = y-surface->h/2;
        dest.w = surface->w;
        dest.h = surface->h;
        SDL_SetColorKey(surface, SDL_SRCCOLORKEY, SP_ALPHA_COLOR);
        SDL_BlitSurface(surface,NULL,spTarget,&dest);
      }
    }
    else
    {
      if (spZTest)
      {
        SDL_LockSurface(spTarget);
        Uint16 *pixel;
        pixel = (Uint16*)(surface->pixels);
        int u = -surface->w/2;
        if (x+u < 0)
          u = -x;
        int maxu = surface->w/2;
        if (x+maxu > spTargetX)
          maxu = spTargetX - x;
        int minv = -surface->h/2;
        if (y+minv < 0)
          minv = -y;
        int maxv = surface->h/2;
        if (y+maxv > spTargetY)
          maxv = spTargetY - y;
        int v;
        for (;u < maxu;u++)
          for (v = minv;v < maxv;v++)
            if (pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w]!= SP_ALPHA_COLOR && z > spZBuffer[(x+u)+(y+v)*spTargetX])
              spTargetPixel[(x+u)+(y+v)*spTargetX] = pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w];
        SDL_UnlockSurface(spTarget);
      }
      else
      {
        SDL_Rect dest;
        dest.w = surface->w;
        dest.h = surface->h;
        dest.x = x-surface->w/2;
        dest.y = y-surface->h/2;
        SDL_SetColorKey(surface, SDL_SRCCOLORKEY, SP_ALPHA_COLOR);
        SDL_BlitSurface(surface,NULL,spTarget,&dest);
      }
    }
  }
  else
  {
    if (spZSet)
    {
      if (spZTest)
      {
        SDL_LockSurface(spTarget);
        Sint16 *pixel;
        pixel = (Sint16*)(surface->pixels);
        int u = -surface->w/2;
        if (x+u < 0)
          u = -x;
        int maxu = surface->w/2;
        if (x+maxu > spTargetX)
          maxu = spTargetX - x;
        int minv = -surface->h/2;
        if (y+minv < 0)
          minv = -y;
        int maxv = surface->h/2;
        if (y+maxv > spTargetY)
          maxv = spTargetY - y;
        int v;
        for (;u < maxu;u++)
          for (v = minv;v < maxv;v++)
            if (z > spZBuffer[(x+u)+(y+v)*spTargetX])
            {
              spTargetPixel[(x+u)+(y+v)*spTargetX] = pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w];
              spZBuffer[(x+u)+(y+v)*spTargetX] = z;
            }
        SDL_UnlockSurface(spTarget);
      }
      else
      {
        int u = x-surface->w/2;
        if (u < 0)
          u = 0;
        if (u >= spTargetX)
          return;
        int v = y-surface->h/2;
        if (v < 0)
          v = 0;
        if (v >= spTargetY)
          return;
        int umax = x+surface->w/2;
        if (umax > spTargetX)
          umax = spTargetX;
        int vmax = y+surface->h/2;
        if (vmax > spTargetY)
          vmax = spTargetY;
        for (;u<umax;u++)
          for (;v<vmax;v++)
            spZBuffer[u+v*spTargetX] = z;
        SDL_Rect dest;
        dest.x = x-surface->w/2;
        dest.y = y-surface->h/2;
        dest.w = surface->w;
        dest.h = surface->h;
        SDL_SetColorKey(surface, 0, 0);
        SDL_BlitSurface(surface,NULL,spTarget,&dest);
      }
    }
    else
    {
      if (spZTest)
      {
        SDL_LockSurface(spTarget);
        Sint16 *pixel;
        pixel = (Sint16*)(surface->pixels);
        int u = -surface->w/2;
        if (x+u < 0)
          u = -x;
        int maxu = surface->w/2;
        if (x+maxu > spTargetX)
          maxu = spTargetX - x;
        int minv = -surface->h/2;
        if (y+minv < 0)
          minv = -y;
        int maxv = surface->h/2;
        if (y+maxv > spTargetY)
          maxv = spTargetY - y;
        int v;
        for (;u < maxu;u++)
          for (v = minv;v < maxv;v++)
            {
              spTargetPixel[(x+u)+(y+v)*spTargetX] = pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w];
              spZBuffer[(x+u)+(y+v)*spTargetX] = z;
            }
        SDL_UnlockSurface(spTarget);
      }
      else
      {
        SDL_Rect dest;
        dest.x = x-surface->w/2;
        dest.y = y-surface->h/2;
        dest.w = surface->w;
        dest.h = surface->h;
        SDL_SetColorKey(surface, 0, 0);
        SDL_BlitSurface(surface,NULL,spTarget,&dest);
      }
    }
  }
}

PREFIX void spBlitSurfacePart(Sint32 x,Sint32 y,Sint32 z,SDL_Surface* surface,Sint32 sx,Sint32 sy,Sint32 w,Sint32 h)
{
  if (z>=0)
    return;
  if (spAlphaTest)
  {
    if (spZSet)
    {
      if (spZTest)
      {
        SDL_LockSurface(spTarget);
        Uint16 *pixel;
        pixel = (Uint16*)(surface->pixels);
        int u = sx-surface->w/2;
        if (x+u < 0)
          u = -x;
        int maxu = sx-surface->w/2+w;
        if (x+maxu > spTargetX)
          maxu = spTargetX - x;
        int minv = sy-surface->h/2;
        if (y+minv < 0)
          minv = -y;
        int maxv = sy-surface->h/2+h;
        if (y+maxv > spTargetY)
          maxv = spTargetY - y;
        int v;
        for (;u < maxu;u++)
          for (v = minv;v < maxv;v++)
            if (pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w]!=SP_ALPHA_COLOR && z > spZBuffer[(x+u)+(y+v)*spTargetX])
            {
              spTargetPixel[(x+u)+(y+v)*spTargetX] = pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w];
              spZBuffer[(x+u)+(y+v)*spTargetX] = z;
            }
        SDL_UnlockSurface(spTarget);
      }
      else
      {
        int u = sx-surface->w/2;
        if (x+u < 0)
          u = -x;
        int maxu = sx-surface->w/2+w;
        if (x+maxu > spTargetX)
          maxu = spTargetX - x;
        int minv = sy-surface->h/2;
        if (y+minv < 0)
          minv = -y;
        int maxv = sy-surface->h/2+h;
        if (y+maxv > spTargetY)
          maxv = spTargetY - y;
        int v;
        for (;u < maxu;u++)
          for (v = minv;v < maxv;v++)
            spZBuffer[u+v*spTargetX] = z;
        SDL_Rect dest;
        dest.x = x-surface->w/2;
        dest.y = y-surface->h/2;
        dest.w = surface->w;
        dest.h = surface->h;
        SDL_SetColorKey(surface, SDL_SRCCOLORKEY, SP_ALPHA_COLOR);
        SDL_BlitSurface(surface,NULL,spTarget,&dest);
      }
    }
    else
    {
      if (spZTest)
      {
        SDL_LockSurface(spTarget);
        Uint16 *pixel;
        pixel = (Uint16*)(surface->pixels);
        int u = sx-surface->w/2;
        if (x+u < 0)
          u = -x;
        int maxu = sx-surface->w/2+w;
        if (x+maxu > spTargetX)
          maxu = spTargetX - x;
        int minv = sy-surface->h/2;
        if (y+minv < 0)
          minv = -y;
        int maxv = sy-surface->h/2+h;
        if (y+maxv > spTargetY)
          maxv = spTargetY - y;
        int v;
        for (;u < maxu;u++)
          for (v = minv;v < maxv;v++)
            if (pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w]!= SP_ALPHA_COLOR && z > spZBuffer[(x+u)+(y+v)*spTargetX])
              spTargetPixel[(x+u)+(y+v)*spTargetX] = pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w];
        SDL_UnlockSurface(spTarget);
      }
      else
      {
        SDL_Rect dest;
        dest.w = w;
        dest.h = h;
        dest.x = x-surface->w/2;
        dest.y = y-surface->h/2;
        SDL_Rect src;
        src.x = sx;
        src.y = sy;
        src.w = w;
        src.h = h;
        SDL_SetColorKey(surface, SDL_SRCCOLORKEY, SP_ALPHA_COLOR);
        SDL_BlitSurface(surface,&src,spTarget,&dest);
      }
    }
  }
  else
  {
    if (spZSet)
    {
      if (spZTest)
      {
        SDL_LockSurface(spTarget);
        Uint16 *pixel;
        pixel = (Uint16*)(surface->pixels);
        int u = sx-surface->w/2;
        if (x+u < 0)
          u = -x;
        int maxu = sx-surface->w/2+w;
        if (x+maxu > spTargetX)
          maxu = spTargetX - x;
        int minv = sy-surface->h/2;
        if (y+minv < 0)
          minv = -y;
        int maxv = sy-surface->h/2+h;
        if (y+maxv > spTargetY)
          maxv = spTargetY - y;
        int v;
        for (;u < maxu;u++)
          for (v = minv;v < maxv;v++)
            if (z > spZBuffer[(x+u)+(y+v)*spTargetX])
            {
              spTargetPixel[(x+u)+(y+v)*spTargetX] = pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w];
              spZBuffer[(x+u)+(y+v)*spTargetX] = z;
            }
        SDL_UnlockSurface(spTarget);
      }
      else
      {
        int u = sx-surface->w/2;
        if (x+u < 0)
          u = -x;
        int maxu = sx-surface->w/2+w;
        if (x+maxu > spTargetX)
          maxu = spTargetX - x;
        int minv = sy-surface->h/2;
        if (y+minv < 0)
          minv = -y;
        int maxv = sy-surface->h/2+h;
        if (y+maxv > spTargetY)
          maxv = spTargetY - y;
        int v;
        for (;u < maxu;u++)
          for (v = minv;v < maxv;v++)
            spZBuffer[u+v*spTargetX] = z;
        SDL_Rect dest;
        dest.x = x-surface->w/2;
        dest.y = y-surface->h/2;
        dest.w = surface->w;
        dest.h = surface->h;
        SDL_SetColorKey(surface, 0, 0);
        SDL_BlitSurface(surface,NULL,spTarget,&dest);
      }
    }
    else
    {
      if (spZTest)
      {
        SDL_LockSurface(spTarget);
        Uint16 *pixel;
        pixel = (Uint16*)(surface->pixels);
        int u = sx-surface->w/2;
        if (x+u < 0)
          u = -x;
        int maxu = sx-surface->w/2+w;
        if (x+maxu > spTargetX)
          maxu = spTargetX - x;
        int minv = sy-surface->h/2;
        if (y+minv < 0)
          minv = -y;
        int maxv = sy-surface->h/2+h;
        if (y+maxv > spTargetY)
          maxv = spTargetY - y;
        int v;
        for (;u < maxu;u++)
          for (v = minv;v < maxv;v++)
            if (z > spZBuffer[(x+u)+(y+v)*spTargetX])
              spTargetPixel[(x+u)+(y+v)*spTargetX] = pixel[(u+surface->w/2)+(v+surface->h/2)*surface->w];
        SDL_UnlockSurface(spTarget);
      }
      else
      {
        SDL_Rect dest;
        dest.x = x-surface->w/2;
        dest.y = y-surface->h/2;
        dest.w = surface->w;
        dest.h = surface->h;
        SDL_Rect src;
        src.x = sx;
        src.y = sy;
        src.x = w;
        src.y = h;
        SDL_SetColorKey(surface, 0, 0);
        SDL_BlitSurface(surface,&src,spTarget,&dest);
      }
    }
  }
}
