#include "graphicstuff.h"

Sint32 projectionMatrix[16];
Sint32 modellViewMatrix[16];
Sint32 cosvalue[(2*MY_PI)>>COS_ACCURACY];
int engineWindowX;
int engineWindowY;

Sint32* engineGetProjectionMatrix()
{
  return projectionMatrix;
}

Sint32* engineGetModellViewMatrix()
{
  return modellViewMatrix;
}

void engineSetWindowX(int x)
{
  engineWindowX=x;
  engineSetWindowXasm(x);
}

int engineGetWindowX()
{
  return engineWindowX;
}

void engineSetWindowY(int y)
{
  engineSetWindowYasm(y);
  engineWindowY=y;
}

int engineGetWindowY()
{
  return engineWindowY;
}

void initMath()
{
  int a;
  for (a=0;a<((MY_PI<<1)>>COS_ACCURACY);a++)
    cosvalue[a]=(Sint32)(cos((float)(a<<COS_ACCURACY)/ACCURACY_FACTOR)*ACCURACY_FACTOR);
}

Sint32 mycos(Sint32 w)
{
  return cosvalue[(abs(w)%(MY_PI<<1))>>COS_ACCURACY];
  //return (Sint32)(cos((float)w/ACCURACY_FACTOR)*ACCURACY_FACTOR);
}

Sint32 mysin(Sint32 w)
{
  return mycos(w+((3*MY_PI)>>1));
}

//senquack - credit for this fast sqrt goes to Wilco Dijkstra http://www.finesse.demon.co.uk/steven/sqrt.html
#define iter1(N) \
    try = root + (1 << (N)); \
    if (n >= try << (N))   \
    {   n -= try << (N);   \
        root |= 2 << (N); \
    }

Sint32 fpsqrt (Sint32 n)
{
    Sint32 root = 0, try;
    iter1 (15);    iter1 (14);    iter1 (13);    iter1 (12);
    iter1 (11);    iter1 (10);    iter1 ( 9);    iter1 ( 8);
    iter1 ( 7);    iter1 ( 6);    iter1 ( 5);    iter1 ( 4);
    iter1 ( 3);    iter1 ( 2);    iter1 ( 1);    iter1 ( 0);
    return root << (HALF_ACCURACY-1);
}

void setModellViewMatrixIdentity()
{
  modellViewMatrix[ 0] = 1<<ACCURACY;
  modellViewMatrix[ 1] = 0<<ACCURACY;
  modellViewMatrix[ 2] = 0<<ACCURACY;
  modellViewMatrix[ 3] = 0<<ACCURACY;
  modellViewMatrix[ 4] = 0<<ACCURACY;
  modellViewMatrix[ 5] = 1<<ACCURACY;
  modellViewMatrix[ 6] = 0<<ACCURACY;
  modellViewMatrix[ 7] = 0<<ACCURACY;
  modellViewMatrix[ 8] = 0<<ACCURACY;
  modellViewMatrix[ 9] = 0<<ACCURACY;
  modellViewMatrix[10] = 1<<ACCURACY;
  modellViewMatrix[11] = 0<<ACCURACY;
  modellViewMatrix[12] = 0<<ACCURACY;
  modellViewMatrix[13] = 0<<ACCURACY;
  modellViewMatrix[14] = 0<<ACCURACY;
  modellViewMatrix[15] = 1<<ACCURACY;
}

Uint16 getRGB(Uint8 r,Uint8 g,Uint8 b)
{
  return ((r>>3)<<11)+((g>>2)<<5)+(b>>3);
}

Uint16 getHSV(Sint32 h,Uint8 s,Uint8 v)
{
  h=h%(2*MY_PI);
  Uint8 hi = ((h<<HALF_ACCURACY)/(MY_PI/3))>>HALF_ACCURACY;
  Sint32 f = (((h<<HALF_ACCURACY)/(MY_PI/3))<<HALF_ACCURACY)-(hi<<ACCURACY);
  Sint32 p = (v<<(HALF_ACCURACY-8))*((1<<HALF_ACCURACY)-(s<<(HALF_ACCURACY-8)));
  Sint32 q = (v<<(HALF_ACCURACY-8))*((1<<HALF_ACCURACY)-(((f>>HALF_ACCURACY)*(s<<(HALF_ACCURACY-8)))>>HALF_ACCURACY));
  Sint32 t = (v<<(HALF_ACCURACY-8))*((1<<HALF_ACCURACY)-((((1<<HALF_ACCURACY)-(f>>HALF_ACCURACY))*(s<<(HALF_ACCURACY-8)))>>HALF_ACCURACY));
  Uint8 r;
  Uint8 g;
  Uint8 b;
  switch (hi)
  {
    case 1:
      r=q>>(ACCURACY-8);
      g=v; 
      b=p>>(ACCURACY-8);
      break;
    case 2:
      r=p>>(ACCURACY-8);
      g=v; 
      b=t>>(ACCURACY-8);
      break;
    case 3:
      r=p>>(ACCURACY-8);
      g=q>>(ACCURACY-8); 
      b=v;
      break;
    case 4:
      r=t>>(ACCURACY-8);
      g=p>>(ACCURACY-8); 
      b=v;
      break;
    case 5:
      r=v;
      g=p>>(ACCURACY-8); 
      b=q>>(ACCURACY-8);
      break;
    default:
      r=v;
      g=t>>(ACCURACY-8); 
      b=p>>(ACCURACY-8);
      break;
  }
  return ((r>>3)<<11)+((g>>2)<<5)+(b>>3);
}

void drawtext(SDL_Surface *screen,Sint32 x,Sint32 y,char* text,SDL_Surface *keymap)
{
  int p=0;
  while (text[p]!=0 /*&& x<engineWindowX*/)
  {
    unsigned char c = text[p]-32;
    SDL_Rect srcrect;
    srcrect.x=(c % 16)*(keymap->w >> 4);
    srcrect.y=(c >> 4)*(keymap->h >> 3);
    srcrect.w=keymap->w >> 4;
    srcrect.h=keymap->h >> 3;
    SDL_Rect dstrect;
    dstrect.x=x;
    dstrect.y=y;
    dstrect.w=keymap->w >> 4;
    dstrect.h=keymap->h >> 3;
    
    SDL_BlitSurface(keymap, &srcrect, screen, &dstrect);
    x+=keymap->w >> 4;
    p++;
  }
}

void drawtextMX(SDL_Surface *screen,Sint32 x,Sint32 y,char* text,SDL_Surface *keymap)
{
  drawtext(screen,x-(keymap->w >> 5)*strlen(text),y,text,keymap);
}

void drawtextMY(SDL_Surface *screen,Sint32 x,Sint32 y,char* text,SDL_Surface *keymap)
{
  drawtext(screen,x,y-(keymap->h >> 4),text,keymap);
}

void drawtextMXMY(SDL_Surface *screen,Uint16 x,Uint16 y,char* text,SDL_Surface *keymap)
{
  drawtext(screen,x-(keymap->w >> 5)*strlen(text),y-(keymap->h >> 4),text,keymap);
}

void setFrustumf2(Sint32 *matrix, Sint32 left, Sint32 right, Sint32 bottom, Sint32 top,
                  Sint32 znear, Sint32 zfar)
{
    Sint32 temp, temp2, temp3, temp4;
    temp = 2 * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    matrix[0] = (temp << ACCURACY) / temp2;
    matrix[1] = 0<<ACCURACY;
    matrix[2] = 0<<ACCURACY;
    matrix[3] = 0<<ACCURACY;
    matrix[4] = 0<<ACCURACY;
    matrix[5] = ((temp << HALF_ACCURACY) / temp3) << HALF_ACCURACY;
    matrix[6] = 0<<ACCURACY;
    matrix[7] = 0<<ACCURACY;
    matrix[8] = (((right + left)<<HALF_ACCURACY) / temp2) << HALF_ACCURACY;
    matrix[9] = (((top + bottom)<<HALF_ACCURACY) / temp3) << HALF_ACCURACY;
    matrix[10] = (((-zfar - znear)<<HALF_ACCURACY) / temp4) << HALF_ACCURACY;
    matrix[11] = -1<<ACCURACY;
    matrix[12] = 0<<ACCURACY;
    matrix[13] = 0<<ACCURACY;
    matrix[14] = ((((-temp >> HALF_ACCURACY) * (zfar >> HALF_ACCURACY)) << HALF_ACCURACY) / temp4) << HALF_ACCURACY;
    matrix[15] = 0<<ACCURACY;
}

void setPerspective(float fovyInDegrees, float aspectRatio,
                      float znear, float zfar)
{
    float ymax, xmax;
    ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);
    //ymin = -ymax;
    //xmin = -ymax * aspectRatio;
    xmax = ymax * aspectRatio;
    setFrustumf2(projectionMatrix,(Sint32)(-xmax*ACCURACY_FACTOR),
                                  (Sint32)(xmax*ACCURACY_FACTOR),
                                  (Sint32)(-ymax*ACCURACY_FACTOR),
                                  (Sint32)(ymax*ACCURACY_FACTOR),
                                  (Sint32)(znear*ACCURACY_FACTOR),
                                  (Sint32)(zfar*ACCURACY_FACTOR));
}

#ifndef ARMCPU
#ifndef X86CPU   
void hline(SDL_Surface* screen,Sint32 x,Sint32 y,Sint32 l_,Uint16 color_,char check)
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
  Uint16* pixel=(Uint16*)screen->pixels;
  Uint32 pos=(x+y*engineWindowX);
  int a;
  for (a=pos;a<pos+l_;a++)
    pixel[a]=color_;
}    
#endif
#endif

#ifdef X86CPU
void hline(SDL_Surface* screen,Sint32 x,Sint32 y,Sint32 l_,Uint16 color_,char check)
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
  Uint16* pixel=(Uint16*)screen->pixels;
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

void triangle(SDL_Surface* screen,Sint16 x1,Sint16 y1,Sint16 x2,Sint16 y2,Sint16 x3,Sint16 y3,Sint16 color)
{
  int mny=y1;
  int mxy=y1;
  if (y2<mny) mny=y2;
  if (y2>mxy) mxy=y2;
  if (y3<mny) mny=y3;
  if (y3>mxy) mxy=y3;

  if (mny<0)
    mny=0;
  if (mxy>=engineWindowY)
    mxy=engineWindowY-1;
  if (mny>engineWindowY-1)
    return;
  if (mxy<0)
    return;

  int mul1=x1-x3; int div1=y1-y3;
  int mul2=x2-x1; int div2=y2-y1;
  int mul3=x3-x2; int div3=y3-y2;

  SDL_LockSurface(screen);

  int yc;
  for (yc=mny;yc<=mxy;yc++)
  {
    int mnx=engineWindowX;
    int mxx=-1;
    if (y3>=yc || y1>=yc)
      if (y3<=yc || y1<=yc)
        if (y3!=y1)
        {
          int x=(yc-y3)*mul1 / div1+x3;
          if (x<mnx)
            mnx=x;
          if (x>mxx)
            mxx=x;
        }
    if (y1>=yc || y2>=yc)
      if (y1<=yc || y2<=yc)
        if (y1!=y2)
        {
          int x=(yc-y1)*mul2 / div2+x1;
          if (x<mnx)
            mnx=x;
          if (x>mxx)
            mxx=x;
        }
    if (y2>=yc || y3>=yc)
      if (y2<=yc || y3<=yc)
        if (y2!=y3)
        {
          int x=(yc-y2)*mul3 / div3+x2;
          if (x<mnx)
            mnx=x;
          if (x>mxx)
            mxx=x;
        }
    if (mnx<0)
      mnx=0;
    if (mxx>engineWindowX-1)
      mxx=engineWindowX-1;
    if (mnx<=mxx)
      hline(screen,mnx,yc,mxx-mnx+1,color,1);
  }
  SDL_UnlockSurface(screen);
}

void quad(SDL_Surface* screen,Sint16 x1,Sint16 y1,Sint16 x2,Sint16 y2,Sint16 x3,Sint16 y3,Sint16 x4,Sint16 y4,Uint16 color)
{
  int mny=y1;
  int mxy=y1;
  if (y2<mny) mny=y2;
  if (y2>mxy) mxy=y2;
  if (y3<mny) mny=y3;
  if (y3>mxy) mxy=y3;
  if (y4<mny) mny=y4;
  if (y4>mxy) mxy=y4;

  if (mny<0)
    mny=0;
  if (mxy>=engineWindowY)
    mxy=engineWindowY-1;
  if (mny>engineWindowY-1)
    return;
  if (mxy<0)
    return;

  int mul1=x1-x4; int div1=y1-y4;
  int mul2=x2-x1; int div2=y2-y1;
  int mul3=x3-x2; int div3=y3-y2;
  int mul4=x4-x3; int div4=y4-y3;

  SDL_LockSurface(screen);  

  int yc;
  for (yc=mny;yc<=mxy;yc++)
  {
    int mnx=engineWindowX;
    int mxx=-1;
    if (y4>=yc || y1>=yc)
      if (y4<=yc || y1<=yc)
        if (y4!=y1)
        {
          int x=(yc-y4)*mul1 / div1+x4;
          if (x<mnx)
            mnx=x;
          if (x>mxx)
            mxx=x;
        }
    if (y1>=yc || y2>=yc)
      if (y1<=yc || y2<=yc)
        if (y1!=y2)
        {
          int x=(yc-y1)*mul2 / div2+x1;
          if (x<mnx)
            mnx=x;
          if (x>mxx)
            mxx=x;
        }
    if (y2>=yc || y3>=yc)
      if (y2<=yc || y3<=yc)
        if (y2!=y3)
        {
          int x=(yc-y2)*mul3 / div3+x2;
          if (x<mnx)
            mnx=x;
          if (x>mxx)
            mxx=x;
        }
    if (y3>=yc || y4>=yc)
      if (y3<=yc || y4<=yc)
        if (y3!=y4)
        {
          int x=(yc-y3)*mul4 / div4+x3;
          if (x<mnx)
            mnx=x;
          if (x>mxx)
            mxx=x;
        }
    if (mnx<0)
      mnx=0;
    if (mxx>engineWindowX-1)
      mxx=engineWindowX-1;
    if (mnx<=mxx)
      hline(screen,mnx,yc,mxx-mnx+1,color,1);
  }
  SDL_UnlockSurface(screen);
}

void clearScreen(SDL_Surface* screen,Uint16 color)
{
  SDL_LockSurface(screen);
  hline(screen,0,0,engineWindowX-1,0,0);
  hline(screen,0,1,engineWindowX*(engineWindowY-1),color,0); //the last scanlines makes bugs -_-
  hline(screen,0,engineWindowY-1,engineWindowX-1,0,0);
  SDL_UnlockSurface(screen);
}

void ellipse(SDL_Surface* screen,Sint16 mx,Sint16 my,Sint16 rx,Sint16 ry,Uint16 color)
{
  Sint16 y;
  //rx++;
  //ry++;
  for (y=my-ry;y<=my+ry;y++)
  {
    Sint16 dividend = (ry*ry);
    Sint16 root;
    if (dividend==0)
      root = 0;
    else
      root = fpsqrt((ry*ry*rx*rx-(y-my)*(y-my)*rx*rx)/dividend)>>HALF_ACCURACY;
    /*if (root==0)
      continue;
    if (y==my)
      root--;*/
    hline(screen,mx-root,y,root*2+1,color,1);
  }  
}
