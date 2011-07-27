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

inline void draw_pixel_ztest_zset(Sint16 x,Sint16 y,Sint16 z,Uint16 color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY && (z<0 && z > spZBuffer[x+y*spTargetX]))
  {
    spTargetPixel[x+y*spTargetX] = color;
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel_ztest(Sint16 x,Sint16 y,Sint16 z,Uint16 color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY && (z<0 && z > spZBuffer[x+y*spTargetX]))
  {
    spTargetPixel[x+y*spTargetX] = color;
  }
}

inline void draw_pixel_zset(Sint16 x,Sint16 y,Sint16 z,Uint16 color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY)
  {
    spTargetPixel[x+y*spTargetX] = color;
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel(Sint16 x,Sint16 y,Uint16 color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY)
  {
    spTargetPixel[x+y*spTargetX] = color;
  }
}

inline void draw_line_ztest_zset(Sint16 x1,Sint16 z1,Sint16 x2,Sint16 z2,Sint16 y,Uint16 color,Sint32 sZ)
{
  Sint32 z = z1<<SP_ACCURACY;
  for (int x = x1; x<= x2; x++)
  {
    draw_pixel_ztest_zset(x,y,z>>SP_ACCURACY,color);
    z += sZ;
  }
}

inline void draw_line_ztest(Sint16 x1,Sint16 z1,Sint16 x2,Sint16 z2,Sint16 y,Uint16 color,Sint32 sZ)
{
  Sint32 z = z1<<SP_ACCURACY;
  for (int x = x1; x<= x2; x++)
  {
    draw_pixel_ztest(x,y,z>>SP_ACCURACY,color);
    z += sZ;
  }
}

inline void draw_line_zset(Sint16 x1,Sint16 z1,Sint16 x2,Sint16 z2,Sint16 y,Uint16 color,Sint32 sZ)
{
  Sint32 z = z1<<SP_ACCURACY;
  for (int x = x1; x<= x2; x++)
  {
    draw_pixel_zset(x,y,z>>SP_ACCURACY,color);
    z += sZ;
  }
}

inline void draw_line(Sint16 x1,Sint16 x2,Sint16 y,Uint16 color)
{
  //TODO: Replace with hline!
  for (int x = x1; x<= x2; x++)
  {
    draw_pixel(x,y,color);
  }
}

inline void sp_intern_Triangle_ztest_zset(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 x3, Sint16 y3, Sint16 z3, Uint16 color)
{
  SDL_LockSurface(spTarget);
  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint16 z4 = z1;
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
  }
  Sint32 xl = x1<<SP_ACCURACY;
  Sint32 zl = z1<<SP_ACCURACY;
  Sint32 sX_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    sZ_l = (z1-z2)*mul;
  }

  Sint32 xr = xl;
  Sint32 zr = zl;
  Sint32 sX_r = 0;
  Sint32 sZ_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sZ_r = (z1-z3)*mul;
  }

  if (x2 < x3)
  {
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sZ = (z4-z3)*mul;
    }
    for (int y = y1; y < y3; y++)
    {      
      draw_line_ztest_zset(xl>>SP_ACCURACY,zl>>SP_ACCURACY,
                           xr>>SP_ACCURACY,zr>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sZ = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sZ = (z3-z4)*mul;
    }    
    for (int y = y1; y < y3; y++)
    {
      draw_line_ztest_zset(xr>>SP_ACCURACY,zr>>SP_ACCURACY,
                           xl>>SP_ACCURACY,zl>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  xr = x3 << SP_ACCURACY;
  sX_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
  }
  
  if (x2 < x3)
  {
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sZ = (z4-z3)*mul;
    }
    for (int y = y3; y <= y2; y++)
    {      
      draw_line_ztest_zset(xl>>SP_ACCURACY,zl>>SP_ACCURACY,
                           xr>>SP_ACCURACY,zr>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sZ = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sZ = (z3-z4)*mul;
    }    
    for (int y = y3; y <= y2; y++)
    {
      draw_line_ztest_zset(xr>>SP_ACCURACY,zr>>SP_ACCURACY,
                           xl>>SP_ACCURACY,zl>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  SDL_UnlockSurface(spTarget);  
}

inline void sp_intern_Triangle_ztest(Sint16 x1, Sint16 y1, Sint16 z1,   Sint16 x2, Sint16 y2, Sint16 z2,   Sint16 x3, Sint16 y3, Sint16 z3,   Uint16 color)
{
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint16 z4 = z1;
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
  }
  Sint32 xl = x1<<SP_ACCURACY;
  Sint32 zl = z1<<SP_ACCURACY;
  Sint32 sX_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    sZ_l = (z1-z2)*mul;
  }

  Sint32 xr = xl;
  Sint32 zr = zl;
  Sint32 sX_r = 0;
  Sint32 sZ_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sZ_r = (z1-z3)*mul;
  }

  if (x2 < x3)
  {
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sZ = (z4-z3)*mul;
    }
    for (int y = y1; y < y3; y++)
    {      
      draw_line_ztest(xl>>SP_ACCURACY,zl>>SP_ACCURACY,
                      xr>>SP_ACCURACY,zr>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sZ = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sZ = (z3-z4)*mul;
    }    
    for (int y = y1; y < y3; y++)
    {
      draw_line_ztest(xr>>SP_ACCURACY,zr>>SP_ACCURACY,
                      xl>>SP_ACCURACY,zl>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  xr = x3 << SP_ACCURACY;
  sX_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
  }
  
  if (x2 < x3)
  {
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sZ = (z4-z3)*mul;
    }
    for (int y = y3; y <= y2; y++)
    {      
      draw_line_ztest(xl>>SP_ACCURACY,zl>>SP_ACCURACY,
                      xr>>SP_ACCURACY,zr>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sZ = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sZ = (z3-z4)*mul;
    }    
    for (int y = y3; y <= y2; y++)
    {
      draw_line_ztest(xr>>SP_ACCURACY,zr>>SP_ACCURACY,
                      xl>>SP_ACCURACY,zl>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  SDL_UnlockSurface(spTarget);  
}

inline void sp_intern_Triangle_zset(Sint16 x1, Sint16 y1, Sint16 z1,   Sint16 x2, Sint16 y2, Sint16 z2,   Sint16 x3, Sint16 y3, Sint16 z3,   Uint16 color)
{
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint16 z4 = z1;
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
  }
  Sint32 xl = x1<<SP_ACCURACY;
  Sint32 zl = z1<<SP_ACCURACY;
  Sint32 sX_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    sZ_l = (z1-z2)*mul;
  }

  Sint32 xr = xl;
  Sint32 zr = zl;
  Sint32 sX_r = 0;
  Sint32 sZ_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sZ_r = (z1-z3)*mul;
  }

  if (x2 < x3)
  {
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sZ = (z4-z3)*mul;
    }
    for (int y = y1; y < y3; y++)
    {      
      draw_line_zset(xl>>SP_ACCURACY,zl>>SP_ACCURACY,
                     xr>>SP_ACCURACY,zr>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sZ = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sZ = (z3-z4)*mul;
    }    
    for (int y = y1; y < y3; y++)
    {
      draw_line_zset(xr>>SP_ACCURACY,zr>>SP_ACCURACY,
                     xl>>SP_ACCURACY,zl>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  xr = x3 << SP_ACCURACY;
  sX_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
  }
  
  if (x2 < x3)
  {
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sZ = (z4-z3)*mul;
    }
    for (int y = y3; y <= y2; y++)
    {      
      draw_line_zset(xl>>SP_ACCURACY,zl>>SP_ACCURACY,
                     xr>>SP_ACCURACY,zr>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  else
  {
    Sint32 sZ = 0;
    if ((x3-x4) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sZ = (z3-z4)*mul;
    }    
    for (int y = y3; y <= y2; y++)
    {
      draw_line_zset(xr>>SP_ACCURACY,zr>>SP_ACCURACY,
                     xl>>SP_ACCURACY,zl>>SP_ACCURACY,y,color,sZ);
      xl += sX_l;
      zl += sZ_l;
      xr += sX_r;
      zr += sZ_r;
    }
  }
  SDL_UnlockSurface(spTarget);  
}

inline void sp_intern_Triangle(Sint16 x1, Sint16 y1, Sint16 z1,   Sint16 x2, Sint16 y2, Sint16 z2,   Sint16 x3, Sint16 y3, Sint16 z3,   Uint16 color)
{
  SDL_LockSurface(spTarget);

  Sint16 x4 = x1;
  Sint16 y4 = y1;
  Sint16 z4 = z1;
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
  }
  Sint32 xl = x1<<SP_ACCURACY;
  Sint32 sX_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
  }

  Sint32 xr = xl;
  Sint32 sX_r = 0;
  if ((y1-y3) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
  }

  if (x2 < x3)
  {
    for (int y = y1; y < y3; y++)
    {      
      draw_line(xl>>SP_ACCURACY,xr>>SP_ACCURACY,y,color);
      xl += sX_l;
      xr += sX_r;
    }
  }
  else
  {
    for (int y = y1; y < y3; y++)
    {
      draw_line(xr>>SP_ACCURACY,xl>>SP_ACCURACY,y,color);
      xl += sX_l;
      xr += sX_r;
    }
  }
  xr = x3 << SP_ACCURACY;
  sX_r = 0;
  if ((y2-y3) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y2-y3);
    sX_r = (x2-x3)*mul;
  }
  
  if (x2 < x3)
  {
    for (int y = y3; y <= y2; y++)
    {
      draw_line(xl>>SP_ACCURACY,xr>>SP_ACCURACY,y,color);
      xl += sX_l;
      xr += sX_r;
    }
  }
  else
  {
    for (int y = y3; y <= y2; y++)
    {
      draw_line(xr>>SP_ACCURACY,xl>>SP_ACCURACY,y,color);
      xl += sX_l;
      xr += sX_r;
    }
  }
  SDL_UnlockSurface(spTarget);  
}

PREFIX void spTriangle(Sint16 x1, Sint16 y1, Sint16 z1,   Sint16 x2, Sint16 y2, Sint16 z2,   Sint16 x3, Sint16 y3, Sint16 z3,   Uint16 color)
{
  if (spZSet)
  {
    if (spZTest)
    {
      if (y1 < y3)
      {
        if (y3 < y2) // y1 < y3 < y2
          sp_intern_Triangle_ztest_zset(x1,y1,z1,x2,y2,z2,x3,y3,z3,color);
        else
        if (y1 < y2) // y1 < y2 < y3
          sp_intern_Triangle_ztest_zset(x1,y1,z1,x3,y3,z3,x2,y2,z2,color);
        else // y2 < y1 < y3
          sp_intern_Triangle_ztest_zset(x2,y2,z2,x3,y3,z3,x1,y1,z1,color);
      }
      else
      {
        if (y1 < y2) //  y3 < y1 < y2
          sp_intern_Triangle_ztest_zset(x3,y3,z3,x2,y2,z2,x1,y1,z1,color);
        else
        if (y2 < y3) // y2 < y3 < y1
          sp_intern_Triangle_ztest_zset(x2,y2,z2,x1,y1,z1,x3,y3,z3,color);
        else // y3 < y2 < y1
          sp_intern_Triangle_ztest_zset(x3,y3,z3,x1,y1,z1,x2,y2,z2,color);
      }      
    } 
    else 
    {
      if (y1 < y3)
      {
        if (y3 < y2) // y1 < y3 < y2
          sp_intern_Triangle_zset(x1,y1,z1,x2,y2,z2,x3,y3,z3,color);
        else
        if (y1 < y2) // y1 < y2 < y3
          sp_intern_Triangle_zset(x1,y1,z1,x3,y3,z3,x2,y2,z2,color);
        else // y2 < y1 < y3
          sp_intern_Triangle_zset(x2,y2,z2,x3,y3,z3,x1,y1,z1,color);
      }
      else
      {
        if (y1 < y2) //  y3 < y1 < y2
          sp_intern_Triangle_zset(x3,y3,z3,x2,y2,z2,x1,y1,z1,color);
        else
        if (y2 < y3) // y2 < y3 < y1
          sp_intern_Triangle_zset(x2,y2,z2,x1,y1,z1,x3,y3,z3,color);
        else // y3 < y2 < y1
          sp_intern_Triangle_zset(x3,y3,z3,x1,y1,z1,x2,y2,z2,color);
      }      
    }  
  }
  else
  {
    if (spZTest)
    {
      if (y1 < y3)
      {
        if (y3 < y2) // y1 < y3 < y2
          sp_intern_Triangle_ztest(x1,y1,z1,x2,y2,z2,x3,y3,z3,color);
        else
        if (y1 < y2) // y1 < y2 < y3
          sp_intern_Triangle_ztest(x1,y1,z1,x3,y3,z3,x2,y2,z2,color);
        else // y2 < y1 < y3
          sp_intern_Triangle_ztest(x2,y2,z2,x3,y3,z3,x1,y1,z1,color);
      }
      else
      {
        if (y1 < y2) //  y3 < y1 < y2
          sp_intern_Triangle_ztest(x3,y3,z3,x2,y2,z2,x1,y1,z1,color);
        else
        if (y2 < y3) // y2 < y3 < y1
          sp_intern_Triangle_ztest(x2,y2,z2,x1,y1,z1,x3,y3,z3,color);
        else // y3 < y2 < y1
          sp_intern_Triangle_ztest(x3,y3,z3,x1,y1,z1,x2,y2,z2,color);
      }  
    }
    else
    {
      if (y1 < y3)
      {
        if (y3 < y2) // y1 < y3 < y2
          sp_intern_Triangle(x1,y1,z1,x2,y2,z2,x3,y3,z3,color);
        else
        if (y1 < y2) // y1 < y2 < y3
          sp_intern_Triangle(x1,y1,z1,x3,y3,z3,x2,y2,z2,color);
        else // y2 < y1 < y3
          sp_intern_Triangle(x2,y2,z2,x3,y3,z3,x1,y1,z1,color);
      }
      else
      {
        if (y1 < y2) //  y3 < y1 < y2
          sp_intern_Triangle(x3,y3,z3,x2,y2,z2,x1,y1,z1,color);
        else
        if (y2 < y3) // y2 < y3 < y1
          sp_intern_Triangle(x2,y2,z2,x1,y1,z1,x3,y3,z3,color);
        else // y3 < y2 < y1
          sp_intern_Triangle(x3,y3,z3,x1,y1,z1,x2,y2,z2,color);
      }  
    }
  }
}

inline void draw_pixel_tex_ztest_zset(Sint16 x,Sint16 y,Sint16 z,Sint16 u,Sint16 v,Uint16 color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY && (z<0 && z > spZBuffer[x+y*spTargetX]))
  {
    Uint16 r = ((spTexturePixel[u+v*spTextureX] >> 11) * (color >> 11)) >> 5;
    Uint16 g = (((spTexturePixel[u+v*spTextureX] & 2016) >> 5) * ((color & 2016) >> 5)) >> 6;
    Uint16 b = ((spTexturePixel[u+v*spTextureX] & 31) * (color & 31)) >> 5;
    spTargetPixel[x+y*spTargetX] = (r<<11) + (g<<5) + b;
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel_tex_ztest(Sint16 x,Sint16 y,Sint16 z,Sint16 u,Sint16 v,Uint16 color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY && (z<0 && z > spZBuffer[x+y*spTargetX]))
  {
    spTargetPixel[x+y*spTargetX] = spTexturePixel[u+v*spTextureX];//color;
  }
}

inline void draw_pixel_tex_zset(Sint16 x,Sint16 y,Sint16 z,Sint16 u,Sint16 v,Uint16 color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY)
  {
    spTargetPixel[x+y*spTargetX] = spTexturePixel[u+v*spTextureX];//color;
    spZBuffer[x+y*spTargetX] = z;
  }
}

inline void draw_pixel_tex(Sint16 x,Sint16 y,Sint16 u,Sint16 v,Uint16 color)
{
  if (x>=0 && y>=0 && x<spTargetX && y<spTargetY)
  {
    spTargetPixel[x+y*spTargetX] = spTexturePixel[u+v*spTextureX];//color;
  }
}

inline void draw_line_tex_ztest_zset(Sint16 x1,Sint16 z1,Sint16 u1,Sint16 v1,Sint16 x2,Sint16 z2,Sint16 u2,Sint16 v2,Sint16 y,Uint16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
{
  Sint32 u = u1<<SP_ACCURACY;
  Sint32 v = v1<<SP_ACCURACY;
  Sint32 z = z1<<SP_ACCURACY;
  for (int x = x1; x<= x2; x++)
  {
    draw_pixel_tex_ztest_zset(x,y,z>>SP_ACCURACY,u>>SP_ACCURACY,v>>SP_ACCURACY,color);
    u += sU;
    v += sV;
    z += sZ;
  }
}

inline void draw_line_tex_ztest(Sint16 x1,Sint16 z1,Sint16 u1,Sint16 v1,Sint16 x2,Sint16 z2,Sint16 u2,Sint16 v2,Sint16 y,Uint16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
{
  Sint32 u = u1<<SP_ACCURACY;
  Sint32 v = v1<<SP_ACCURACY;
  Sint32 z = z1<<SP_ACCURACY;
  for (int x = x1; x<= x2; x++)
  {
    draw_pixel_tex_ztest(x,y,z>>SP_ACCURACY,u>>SP_ACCURACY,v>>SP_ACCURACY,color);
    u += sU;
    v += sV;
    z += sZ;
  }
}

inline void draw_line_tex_zset(Sint16 x1,Sint16 z1,Sint16 u1,Sint16 v1,Sint16 x2,Sint16 z2,Sint16 u2,Sint16 v2,Sint16 y,Uint16 color,Sint32 sU,Sint32 sV,Sint32 sZ)
{
  Sint32 u = u1<<SP_ACCURACY;
  Sint32 v = v1<<SP_ACCURACY;
  Sint32 z = z1<<SP_ACCURACY;
  for (int x = x1; x<= x2; x++)
  {
    draw_pixel_tex_zset(x,y,z>>SP_ACCURACY,u>>SP_ACCURACY,v>>SP_ACCURACY,color);
    u += sU;
    v += sV;
    z += sZ;
  }
}

inline void draw_line_tex(Sint16 x1,Sint16 u1,Sint16 v1,Sint16 x2,Sint16 u2,Sint16 v2,Sint16 y,Uint16 color,Sint32 sU,Sint32 sV)
{
  Sint32 u = u1<<SP_ACCURACY;
  Sint32 v = v1<<SP_ACCURACY;
  for (int x = x1; x<= x2; x++)
  {
    draw_pixel_tex(x,y,u>>SP_ACCURACY,v>>SP_ACCURACY,color);
    u += sU;
    v += sV;
  }
}

inline void sp_intern_Triangle_tex_ztest_zset(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
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
  Sint32 zl = z1<<SP_ACCURACY;
  Sint32 sX_l = 0;
  Sint32 sU_l = 0;
  Sint32 sV_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    sU_l = (u1-u2)*mul;
    sV_l = (v1-v2)*mul;
    sZ_l = (z1-z2)*mul;
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
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sU_r = (u1-u3)*mul;
    sV_r = (v1-v3)*mul;
    sZ_r = (z1-z3)*mul;
  }

  if (x2 < x3)
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
      sZ = (z4-z3)*mul;
    }
    for (int y = y1; y < y3; y++)
    {      
      draw_line_tex_ztest_zset(xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,
                         xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,y,color,sU,sV,sZ);
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
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
      sZ = (z3-z4)*mul;
    }    
    for (int y = y1; y < y3; y++)
    {
      draw_line_tex_ztest_zset(xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,
                         xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,y,color,sU,sV,sZ);
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
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
      sZ = (z4-z3)*mul;
    }
    for (int y = y3; y <= y2; y++)
    {      
      draw_line_tex_ztest_zset(xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,
                         xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,y,color,sU,sV,sZ);
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
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
      sZ = (z3-z4)*mul;
    }    
    for (int y = y3; y <= y2; y++)
    {
      draw_line_tex_ztest_zset(xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,
                         xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,y,color,sU,sV,sZ);
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

inline void sp_intern_Triangle_tex_ztest(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
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
  Sint32 zl = z1<<SP_ACCURACY;
  Sint32 sX_l = 0;
  Sint32 sU_l = 0;
  Sint32 sV_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    sU_l = (u1-u2)*mul;
    sV_l = (v1-v2)*mul;
    sZ_l = (z1-z2)*mul;
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
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sU_r = (u1-u3)*mul;
    sV_r = (v1-v3)*mul;
    sZ_r = (z1-z3)*mul;
  }

  if (x2 < x3)
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
      sZ = (z4-z3)*mul;
    }
    for (int y = y1; y < y3; y++)
    {      
      draw_line_tex_ztest(xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,
                         xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,y,color,sU,sV,sZ);
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
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
      sZ = (z3-z4)*mul;
    }    
    for (int y = y1; y < y3; y++)
    {
      draw_line_tex_ztest(xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,
                         xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,y,color,sU,sV,sZ);
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
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
      sZ = (z4-z3)*mul;
    }
    for (int y = y3; y <= y2; y++)
    {      
      draw_line_tex_ztest(xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,
                         xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,y,color,sU,sV,sZ);
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
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
      sZ = (z3-z4)*mul;
    }    
    for (int y = y3; y <= y2; y++)
    {
      draw_line_tex_ztest(xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,
                         xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,y,color,sU,sV,sZ);
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

inline void sp_intern_Triangle_tex_zset(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
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
  Sint32 zl = z1<<SP_ACCURACY;
  Sint32 sX_l = 0;
  Sint32 sU_l = 0;
  Sint32 sV_l = 0;
  Sint32 sZ_l = 0;
  if ((y1-y2) != 0)
  {
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y2);
    sX_l = (x1-x2)*mul;
    sU_l = (u1-u2)*mul;
    sV_l = (v1-v2)*mul;
    sZ_l = (z1-z2)*mul;
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
    Sint32 mul = (1<<SP_ACCURACY)/(y1-y3);
    sX_r = (x1-x3)*mul;
    sU_r = (u1-u3)*mul;
    sV_r = (v1-v3)*mul;
    sZ_r = (z1-z3)*mul;
  }

  if (x2 < x3)
  {
    Sint32 sU = 0;
    Sint32 sV = 0;
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
      sZ = (z4-z3)*mul;
    }
    for (int y = y1; y < y3; y++)
    {      
      draw_line_tex_zset(xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,
                         xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,y,color,sU,sV,sZ);
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
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
      sZ = (z3-z4)*mul;
    }    
    for (int y = y1; y < y3; y++)
    {
      draw_line_tex_zset(xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,
                         xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,y,color,sU,sV,sZ);
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
    Sint32 sZ = 0;
    if ((x4-x3) != 0)
    {
      Sint32 mul = (1<<SP_ACCURACY)/(x4-x3);
      sU = (u4-u3)*mul;
      sV = (v4-v3)*mul;
      sZ = (z4-z3)*mul;
    }
    for (int y = y3; y <= y2; y++)
    {      
      draw_line_tex_zset(xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,
                         xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,y,color,sU,sV,sZ);
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
      Sint32 mul = (1<<SP_ACCURACY)/(x3-x4);
      sU = (u3-u4)*mul;
      sV = (v3-v4)*mul;
      sZ = (z3-z4)*mul;
    }    
    for (int y = y3; y <= y2; y++)
    {
      draw_line_tex_zset(xr>>SP_ACCURACY,zr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,
                         xl>>SP_ACCURACY,zl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,y,color,sU,sV,sZ);
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

inline void sp_intern_Triangle_tex(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
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
      draw_line_tex(xl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,
                xr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,y,color,sU,sV);
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
      draw_line_tex(xr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,
                xl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,y,color,sU,sV);
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
      draw_line_tex(xl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,
                xr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,y,color,sU,sV);
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
      draw_line_tex(xr>>SP_ACCURACY,ur>>SP_ACCURACY,vr>>SP_ACCURACY,
                xl>>SP_ACCURACY,ul>>SP_ACCURACY,vl>>SP_ACCURACY,y,color,sU,sV);
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

PREFIX void spTriangle_tex(Sint16 x1, Sint16 y1, Sint16 z1, Sint16 u1, Sint16 v1, Sint16 x2, Sint16 y2, Sint16 z2, Sint16 u2, Sint16 v2, Sint16 x3, Sint16 y3, Sint16 z3, Sint16 u3, Sint16 v3, Uint16 color)
{
  if (spZSet)
  {
    if (spZTest)
    {
      if (y1 < y3)
      {
        if (y3 < y2) // y1 < y3 < y2
          sp_intern_Triangle_tex_ztest_zset(x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
        else
        if (y1 < y2) // y1 < y2 < y3
          sp_intern_Triangle_tex_ztest_zset(x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
        else // y2 < y1 < y3
          sp_intern_Triangle_tex_ztest_zset(x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,x1,y1,z1,u1,v1,color);
      }
      else
      {
        if (y1 < y2) //  y3 < y1 < y2
          sp_intern_Triangle_tex_ztest_zset(x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,x1,y1,z1,u1,v1,color);
        else
        if (y2 < y3) // y2 < y3 < y1
          sp_intern_Triangle_tex_ztest_zset(x2,y2,z2,u2,v2,x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,color);
        else // y3 < y2 < y1
          sp_intern_Triangle_tex_ztest_zset(x3,y3,z3,u3,v3,x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,color);
      }      
    } 
    else 
    {
      if (y1 < y3)
      {
        if (y3 < y2) // y1 < y3 < y2
          sp_intern_Triangle_tex_zset(x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
        else
        if (y1 < y2) // y1 < y2 < y3
          sp_intern_Triangle_tex_zset(x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
        else // y2 < y1 < y3
          sp_intern_Triangle_tex_zset(x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,x1,y1,z1,u1,v1,color);
      }
      else
      {
        if (y1 < y2) //  y3 < y1 < y2
          sp_intern_Triangle_tex_zset(x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,x1,y1,z1,u1,v1,color);
        else
        if (y2 < y3) // y2 < y3 < y1
          sp_intern_Triangle_tex_zset(x2,y2,z2,u2,v2,x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,color);
        else // y3 < y2 < y1
          sp_intern_Triangle_tex_zset(x3,y3,z3,u3,v3,x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,color);
      }      
    }  
  }
  else
  {
    if (spZTest)
    {
      if (y1 < y3)
      {
        if (y3 < y2) // y1 < y3 < y2
          sp_intern_Triangle_tex_ztest(x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
        else
        if (y1 < y2) // y1 < y2 < y3
          sp_intern_Triangle_tex_ztest(x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
        else // y2 < y1 < y3
          sp_intern_Triangle_tex_ztest(x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,x1,y1,z1,u1,v1,color);
      }
      else
      {
        if (y1 < y2) //  y3 < y1 < y2
          sp_intern_Triangle_tex_ztest(x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,x1,y1,z1,u1,v1,color);
        else
        if (y2 < y3) // y2 < y3 < y1
          sp_intern_Triangle_tex_ztest(x2,y2,z2,u2,v2,x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,color);
        else // y3 < y2 < y1
          sp_intern_Triangle_tex_ztest(x3,y3,z3,u3,v3,x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,color);
      }  
    }
    else
    {
      if (y1 < y3)
      {
        if (y3 < y2) // y1 < y3 < y2
          sp_intern_Triangle_tex(x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
        else
        if (y1 < y2) // y1 < y2 < y3
          sp_intern_Triangle_tex(x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,color);
        else // y2 < y1 < y3
          sp_intern_Triangle_tex(x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,x1,y1,z1,u1,v1,color);
      }
      else
      {
        if (y1 < y2) //  y3 < y1 < y2
          sp_intern_Triangle_tex(x3,y3,z3,u3,v3,x2,y2,z2,u2,v2,x1,y1,z1,u1,v1,color);
        else
        if (y2 < y3) // y2 < y3 < y1
          sp_intern_Triangle_tex(x2,y2,z2,u2,v2,x1,y1,z1,u1,v1,x3,y3,z3,u3,v3,color);
        else // y3 < y2 < y1
          sp_intern_Triangle_tex(x3,y3,z3,u3,v3,x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,color);
      }  
    }
  }
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
