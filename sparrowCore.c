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
#include "sparrowCore.h"
#include "sparrowMath.h"

int spWindowX;
int spWindowY;
int spZoom;
SDL_Surface* spScreen; //the real screen
SDL_Surface* spWindow; //the thing we draw to
SDL_Joystick *spJoy = NULL;
char spDone;
int spFPS;

PREFIX void spInitCore(void)
{
  TTF_Init();
  #ifdef PANDORA
    spWindowX = 800;
    spWindowY = 480;
  #else
    spWindowX = 320;
    spWindowY = 240;
  #endif 
  spZoom=1<<SP_ACCURACY;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK/* | SDL_INIT_NOPARACHUTE*/); 
  #ifdef MOBILE_DEVICE
    spJoy=SDL_JoystickOpen(0);
  #endif
  spScreen = NULL;
  spWindow = NULL;
  spDone = 0;
  spFPS = 0;
}

void spResizeWindow(int x,int y)
{
   #ifdef GP2X
    spScreen=SDL_SetVideoMode(x,y,16,SDL_HWSURFACE);
    SDL_Surface* surface=SDL_CreateRGBSurface(SDL_HWSURFACE,x,y, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0);
    spWindow=SDL_DisplayFormat(surface);
    SDL_FreeSurface(surface);
  #elif defined CAANOO
    spScreen=SDL_SetVideoMode(x,y,16,SDL_HWSURFACE);
    SDL_Surface* surface=SDL_CreateRGBSurface(SDL_HWSURFACE,x,y, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0);
    spWindow=SDL_DisplayFormat(surface);
    SDL_FreeSurface(surface);
  #elif defined DINGOO
    spScreen=NULL;
    spWindow=SDL_SetVideoMode(x,y,16,SDL_SWSURFACE);
  #elif defined PANDORA
    spScreen=NULL;
    spWindow=SDL_SetVideoMode(x,y,16,SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  #else
    /*x=800;
    y=480;*/
    spScreen=NULL;
    spWindow=SDL_SetVideoMode(x,y,16,SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_HWPALETTE | SDL_VIDEORESIZE);
    //spWindow=SDL_SetVideoMode(x,y,16,SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_NOFRAME);
  #endif  
  if (x % 2 != 0)
    spWindowX = x+1;
  else
    spWindowX = x ;
  spWindowY = y;
  spZoom=spMin((spWindowX<<SP_ACCURACY)/320,(spWindowY<<SP_ACCURACY)/240);  //Bei 320x240 == 1
}

PREFIX SDL_Surface* spCreateWindow(void)
{
  spResizeWindow(spWindowX,spWindowY);
  return spWindow;
}

PREFIX SDL_Surface* spGetWindowSurface(void)
{
  return spWindow;
}

PREFIX int spLoop(void (*spDraw)(void),int (*spCalc)(Uint32 steps),Uint32 minwait)
{
  Uint32 bigsteps=0;
  Uint32 frames=0;
  int back=0;
  Uint32 steps=0;
  int olderticks=SDL_GetTicks();
  int oldticks=SDL_GetTicks();
  int newticks=SDL_GetTicks();
  while(back==0 && !spDone ) {
      oldticks=newticks;
      olderticks=oldticks;
      newticks=SDL_GetTicks();
      //spHandleEvent();
      //spUpdateAxis(0);
      //spUpdateAxis(1);
      if (newticks-oldticks > 0)
        back = spCalc(steps);
      steps+=newticks-olderticks;
      if (steps>minwait)
      {
        spDraw();
        frames++;
        bigsteps=bigsteps+steps;
        while (bigsteps>=1000)
        {
          bigsteps-=1000;
          spFPS=frames;
          printf("FPS: %i\n",spFPS);
          frames=0;
        }
        steps=0;
      }
      if (newticks == oldticks)
        SDL_Delay(1);
  }
  return back;  
}

PREFIX void spFlip(void)
{
  //The Flip
  #ifdef GP2X
    SDL_BlitSurface(spWindow, NULL, spScreen, NULL);
  #elif defined CAANOO
    SDL_BlitSurface(spWindow, NULL, spScreen, NULL);
  #elif defined PANDORA
    /*int arg = 0;
    ioctl(fbdev, FBIO_WAITFORVSYNC, &arg);*/
    SDL_Flip(spWindow);
  #else //PC, Dingoo and Pandora
    SDL_Flip(spWindow);
  #endif
}

PREFIX void spQuitCore(void)
{
  #ifdef MOBILE_DEVICE
    SDL_JoystickClose(spJoy);
  #endif
  SDL_Quit();
}
