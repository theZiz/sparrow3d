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
SDL_Surface* pepper;
Sint32 rotation = 0;
spFontPointer font = NULL;

void draw_test(void)
{
  spResetZBuffer();
  spClearTarget(0);
  spIdentity();
  
  
  spTranslate(spSin(rotation>>2)*8,0,(-18<<SP_ACCURACY)+spSin(rotation)*8);
  spRotateY(rotation);
  
  spBindTexture(garfield);
  spSetCulling(0);
  spSetZSet(1);
  spSetZTest(1);
  spSetAlphaTest(1);
  int a,y;
  for (a = 0; a<16; a++)
  {
    spRotateY(SP_PI/8);
    Sint32 brightness = (spCos(rotation+a*SP_PI/8)>>SP_HALF_ACCURACY)*abs(spCos(rotation+a*SP_PI/8)>>SP_HALF_ACCURACY)/2+(3<<SP_ACCURACY-1);
    Uint16 color = ((brightness>>SP_ACCURACY-4)<<11)+((brightness>>SP_ACCURACY-5)<<5)+(brightness>>SP_ACCURACY-4);
    for (y = -21; y<=21; y+=7)
    {
      spQuadTex3D(-3<<SP_ACCURACY-2,y+3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
                  -3<<SP_ACCURACY-2,y-3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,1,garfield->h-SP_FONT_EXTRASPACE-1,
                   3<<SP_ACCURACY-2,y-3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,garfield->w-SP_FONT_EXTRASPACE-1,garfield->h-SP_FONT_EXTRASPACE-1,
                   3<<SP_ACCURACY-2,y+3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,garfield->w-SP_FONT_EXTRASPACE-1,8,color);
      //spBlit3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,/*pepper*/spFontGetLetter(font,'A')->surface);
    }
  }

  /*

  spTranslate(spSin(rotation/3),spSin(rotation/5),(-5<<SP_ACCURACY));
  spRotateX(rotation);
  spRotateY(rotation);
  spRotateZ(rotation);
  
  //Front / Back
  spQuadTex3D(-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,garfield->h-1,
              -3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,0,
               3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,0,
               3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,12345 | 31727);
  spQuadTex3D( 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,garfield->h-1,
               3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,0,
              -3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,0,
              -3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,23456 | 31727);
  //Left / Right
  spQuadTex3D(-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,garfield->h-1,
              -3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,0,
              -3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,0,
              -3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,34567 | 31727);
  spQuadTex3D( 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,garfield->h-1,
               3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,0,
               3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,0,
               3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,45678 | 31727);  
  //Up / Down
  spQuadTex3D( 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,garfield->h-1,
               3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,0,
              -3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,0,
              -3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,56789 | 31727);
  spQuadTex3D(-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,garfield->h-1,
              -3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,0,
               3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,0,
               3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,61234 | 31727);
  //Front / Back
  spTranslate(-3<<SP_ACCURACY,0,0);
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
    */          

  spSetZSet(0);
  spSetZTest(0);
  spFontDrawMiddle(screen->w/2,2,-1,"Yo dawg, hello World!!!",font);
  char buffer[16];
  sprintf(buffer,"fps: %i",spGetFPS());
  spFontDrawRight(screen->w,screen->h-font->maxheight,-1,buffer,font);
  
  spFlip();
}

int calc_test(Uint32 steps)
{
  rotation+=steps*32;
  if (spGetInput()->button[SP_BUTTON_START])
    return 1;
  return 0; 
}

void resize(Uint16 w,Uint16 h)
{
  //Setup of the new/resized window
  spSelectRenderTarget(spGetWindowSurface());
  spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,0.1,100);

  //Font Loading
  if (font)
    spFontDelete(font);
  font = spFontLoad("./font/StayPuft.ttf",20*spGetSizeFactor()>>SP_ACCURACY);
  spFontAddRange(font,' ','~',0);//whole ASCII
  spFontAddBorder(font,65535);
}


int main(int argc, char **argv)
{
  //sparrow3D Init
  spInitCore();
  spInitMath();
  
  //Setup
  screen = spCreateWindow();
  resize(screen->w,screen->h);
  
  
  //Textures loading
  SDL_Surface* surface = IMG_Load("./data/garfield.png");
  garfield = SDL_DisplayFormat(surface);
  SDL_FreeSurface(surface);
  surface = IMG_Load("./data/pepper.png");
  pepper = SDL_DisplayFormat(surface);
  SDL_FreeSurface(surface);
  spBindTexture(garfield);

  //All glory the main loop
  spLoop(draw_test,calc_test,10,resize);
  
  //Wrap up
  spFontDelete(font);
  SDL_FreeSurface(garfield);
  SDL_FreeSurface(pepper);
  spQuitCore();
  return 0;
}
