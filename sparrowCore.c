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
#include "sparrowPrimitives.h"
#include <SDL_ttf.h>

int spWindowX;
int spWindowY;
int spZoom;
SDL_Surface* spScreen; //the real screen
SDL_Surface* spWindow; //the thing we draw to
SDL_Joystick *spJoy = NULL;
char spDone;
int spFPS;
TspInput spInput;

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
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO/* | SDL_INIT_NOPARACHUTE*/); 
  #ifdef MOBILE_DEVICE
    spJoy=SDL_JoystickOpen(0);
  #endif
  spScreen = NULL;
  spWindow = NULL;
  spDone = 0;
  spFPS = 0;
  int i;
  for (i=0;i<20;i++)
    spInput.button[i]=0;
  spInput.axis[0]=0;
  spInput.axis[1]=0;
  spInitPrimitives();
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

int spHandleEvent(void)
{
  int result = 0;
  #ifdef PANDORA
    /*int s = pnd_evdev_dpad_state ( pnd_evdev_dpads );
    if ( s & pnd_evdev_pandora )
      spDone=1;*/
  #endif
  char new_axis = 0;
  SDL_Event event; 
  while (SDL_PollEvent(&event)==1)
  {
    switch (event.type)
    {
      case SDL_JOYBUTTONDOWN:
        spInput.button[event.jbutton.button]=1;
        break;
      case SDL_JOYBUTTONUP:
        spInput.button[event.jbutton.button]=0;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
          case SDLK_LEFT:
            spInput.axis[0]=-1;
            break;
          case SDLK_RIGHT:
            spInput.axis[0]= 1;
            break;
          case SDLK_UP:
            spInput.axis[1]= 1;
            break;
          case SDLK_DOWN:
            spInput.axis[1]=-1;
            break;
          #ifdef DINGOO
            case SDLK_RETURN:
              spInput.button[SP_BUTTON_START]=1;
              break;
            case SDLK_SPACE:
              spInput.button[SP_BUTTON_X]=1;
              break;
            case SDLK_LSHIFT:
              spInput.button[SP_BUTTON_Y]=1;
              break;
            case SDLK_LCTRL:
              spInput.button[SP_BUTTON_A]=1;
              break;
            case SDLK_LALT:
              spInput.button[SP_BUTTON_B]=1;
              break;
            case SDLK_ESCAPE:
              spInput.button[SP_BUTTON_SELECT]=1;
              break;
            case SDLK_TAB:
              spInput.button[SP_BUTTON_L]=1;
              break;
            case SDLK_BACKSPACE:
              spInput.button[SP_BUTTON_R]=1;
              break;
            case SDLK_s:
              spInput.button[SP_BUTTON_VOLPLUS]=1;
              break;
            case SDLK_u:
              spInput.button[SP_BUTTON_VOLMINUS]=1;
              break;
          #elif defined PANDORA
//            case SDLK_MENU:
//              spDone=1;
//              break;
            case SDLK_PAGEDOWN:
              spInput.button[SP_BUTTON_X]=1;
              break;
            case SDLK_PAGEUP:
              spInput.button[SP_BUTTON_Y]=1;
              break;
            case SDLK_HOME:
              spInput.button[SP_BUTTON_A]=1;
              break;
            case SDLK_END:
              spInput.button[SP_BUTTON_B]=1;
              break;
            case SDLK_LCTRL:
              spInput.button[SP_BUTTON_SELECT]=1;
              break;
            case SDLK_RSHIFT:
              spInput.button[SP_BUTTON_L]=1;
              break;
            case SDLK_RCTRL:
              spInput.button[SP_BUTTON_R]=1;
              break;
            case SDLK_LALT:
              spInput.button[SP_BUTTON_START]=1;
              break;
          #else //PC
            case SDLK_KP_ENTER:
            case SDLK_RETURN:
              spInput.button[SP_BUTTON_START]=1;
              break;
            case SDLK_a:
              spInput.button[SP_BUTTON_A]=1;
              break;
            case SDLK_d:
              spInput.button[SP_BUTTON_B]=1;
              break;
            case SDLK_w:
              spInput.button[SP_BUTTON_Y]=1;
              break;
            case SDLK_s:
              spInput.button[SP_BUTTON_X]=1;
              break;
            case SDLK_BACKSPACE:
              spInput.button[SP_BUTTON_SELECT]=1;
              break;
            case SDLK_PLUS:
              spInput.button[SP_BUTTON_VOLPLUS]=1;
              break;
            case SDLK_MINUS:
              spInput.button[SP_BUTTON_VOLMINUS]=1;
              break;
            case SDLK_q:
              spInput.button[SP_BUTTON_L]=1;
              break;
            case SDLK_e:
              spInput.button[SP_BUTTON_R]=1;
              break;
          #endif
          case SDLK_m:
            //spMuteKey=1;
            break;
        }
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym)
        {
          case SDLK_LEFT:
            if (spInput.axis[0] == -1)
              spInput.axis[0]= 0;
            break;
          case SDLK_RIGHT:
            if (spInput.axis[0] == 1)
              spInput.axis[0]= 0;
            break;
          case SDLK_UP:
            if (spInput.axis[1] == 1)
              spInput.axis[1]= 0;
            break;
          case SDLK_DOWN:
            if (spInput.axis[1] == -1)
              spInput.axis[1]= 0;
            break;
          #ifdef DINGOO
            case SDLK_RETURN:
              spInput.button[SP_BUTTON_START]=0;
              break;
            case SDLK_SPACE:
              spInput.button[SP_BUTTON_X]=0;
              break;
            case SDLK_LSHIFT:
              spInput.button[SP_BUTTON_Y]=0;
              break;
            case SDLK_LCTRL:
              spInput.button[SP_BUTTON_A]=0;
              break;
            case SDLK_LALT:
              spInput.button[SP_BUTTON_B]=0;
              break;
            case SDLK_ESCAPE:
              spInput.button[SP_BUTTON_SELECT]=0;
              break;
            case SDLK_TAB:
              spInput.button[SP_BUTTON_L]=0;
              break;
            case SDLK_BACKSPACE:
              spInput.button[SP_BUTTON_R]=0;
              break;
            case SDLK_s:
              spInput.button[SP_BUTTON_VOLPLUS]=0;
              break;
            case SDLK_u:
              spInput.button[SP_BUTTON_VOLMINUS]=0;
              break;
          #elif defined PANDORA
            case SDLK_PAGEDOWN:
              spInput.button[SP_BUTTON_X]=0;
              break;
            case SDLK_PAGEUP:
              spInput.button[SP_BUTTON_Y]=0;
              break;
            case SDLK_HOME:
              spInput.button[SP_BUTTON_A]=0;
              break;
            case SDLK_END:
              spInput.button[SP_BUTTON_B]=0;
              break;
            case SDLK_LCTRL:
              spInput.button[SP_BUTTON_SELECT]=0;
              break;
            case SDLK_RSHIFT:
              spInput.button[SP_BUTTON_L]=0;
              break;
            case SDLK_RCTRL:
              spInput.button[SP_BUTTON_R]=0;
              break;
            case SDLK_LALT:
              spInput.button[SP_BUTTON_START]=0;
              break;
          #else //PC
            case SDLK_KP_ENTER:
            case SDLK_RETURN:
              spInput.button[SP_BUTTON_START]=0;
              break;
            case SDLK_a:
              spInput.button[SP_BUTTON_A]=0;
              break;
            case SDLK_d:
              spInput.button[SP_BUTTON_B]=0;
              break;
            case SDLK_w:
              spInput.button[SP_BUTTON_Y]=0;
              break;
            case SDLK_s:
              spInput.button[SP_BUTTON_X]=0;
              break;
            case SDLK_BACKSPACE:
              spInput.button[SP_BUTTON_SELECT]=0;
              break;
            case SDLK_PLUS:
              spInput.button[SP_BUTTON_VOLPLUS]=0;
              break;
            case SDLK_MINUS:
              spInput.button[SP_BUTTON_VOLMINUS]=0;
              break;
            case SDLK_q:
              spInput.button[SP_BUTTON_L]=0;
              break;
            case SDLK_e:
              spInput.button[SP_BUTTON_R]=0;
              break;
          #endif
          case SDLK_m:
            //spMuteKey=0;
            break;
        }
        break;
      case SDL_JOYAXISMOTION:
        if (event.jaxis.axis==0)
        {
          if (event.jaxis.value<SP_JOYSTICK_MIN)
            spInput.axis[event.jaxis.axis]=-1;
          else if (event.jaxis.value>SP_JOYSTICK_MAX)
            spInput.axis[event.jaxis.axis]= 1;
          else
            spInput.axis[event.jaxis.axis]= 0;
        }
        if (event.jaxis.axis==1)
        {
          if (event.jaxis.value<SP_JOYSTICK_MIN)
            spInput.axis[event.jaxis.axis]= 1;
          else if (event.jaxis.value>SP_JOYSTICK_MAX)
            spInput.axis[event.jaxis.axis]=-1;
          else
            spInput.axis[event.jaxis.axis]= 0;
        }
        break;  
      case SDL_QUIT:
        spDone=1;
        break;
      case SDL_VIDEORESIZE:
        spResizeWindow(event.resize.w,event.resize.h);
        result = 1;
        break;
    }
  }
  return result;
}

/*PREFIX signed char spGetAxis(int axis)
{
  #ifdef GP2X
    if (axis==0)
    {
      if (spInput.button[SP_AXIS_LEFTUP] ||
          spInput.button[SP_AXIS_LEFT]   ||
          spInput.button[SP_AXIS_LEFTDOWN])
        return -1;
      if (spInput.button[SP_AXIS_RIGHTUP] ||
          spInput.button[SP_AXIS_RIGHT]   ||
          spInput.button[SP_AXIS_RIGHTDOWN])
        return  1;
    }
    else
    {
      if (spInput.button[SP_AXIS_LEFTUP] ||
          spInput.button[SP_AXIS_UP]   ||
          spInput.button[SP_AXIS_RIGHTUP])
        return  1;
      if (spInput.button[SP_AXIS_LEFTDOWN] ||
          spInput.button[SP_AXIS_DOWN]   ||
          spInput.button[SP_AXIS_RIGHTDOWN])
        return -1;
    }
    return 0;
  #else
    return spInput.axis[axis];
  #endif
}*/

/*PREFIX void spSetAxis(int axis,signed char value)
{
  #ifdef GP2X
    if (axis==0)
    {
      spInput.button[SP_AXIS_LEFTUP] = 0;
      spInput.button[SP_AXIS_LEFT] = 0;
      spInput.button[SP_AXIS_LEFTDOWN] = 0;
      spInput.button[SP_AXIS_DOWN] = 0;
      spInput.button[SP_AXIS_RIGHTDOWN] = 0;
      spInput.button[SP_AXIS_RIGHT] = 0;
      spInput.button[SP_AXIS_RIGHTUP] = 0;
      spInput.button[SP_AXIS_UP] = 0;
      if (spInput.axis[1] == 1 && value == -1)
        spInput.button[SP_AXIS_LEFTUP] = 1;
      if (spInput.axis[1] == 0 && value == -1)
        spInput.button[SP_AXIS_LEFT] = 1;
      if (spInput.axis[1] == -1 && value == -1)
        spInput.button[SP_AXIS_LEFTDOWN] = 1;
      if (spInput.axis[1] == -1 && value == 0)
        spInput.button[SP_AXIS_DOWN] = 1;
      if (spInput.axis[1] == -1 && value == 1)
        spInput.button[SP_AXIS_RIGHTDOWN] = 1;
      if (spInput.axis[1] == 0 && value == 1)
        spInput.button[SP_AXIS_RIGHT] = 1;
      if (spInput.axis[1] == 1 && value == 1)
        spInput.button[SP_AXIS_RIGHTUP] = 1;
      if (spInput.axis[1] == 1 && value == 0)
        spInput.button[SP_AXIS_UP] = 1;
    }
    else
    {
      spInput.button[SP_AXIS_LEFTUP] = 0;
      spInput.button[SP_AXIS_LEFT] = 0;
      spInput.button[SP_AXIS_LEFTDOWN] = 0;
      spInput.button[SP_AXIS_DOWN] = 0;
      spInput.button[SP_AXIS_RIGHTDOWN] = 0;
      spInput.button[SP_AXIS_RIGHT] = 0;
      spInput.button[SP_AXIS_RIGHTUP] = 0;
      spInput.button[SP_AXIS_UP] = 0;
      if (value == 1 && spInput.axis[0] == -1)
        spInput.button[SP_AXIS_LEFTUP] = 1;
      if (value == 0 && spInput.axis[0] == -1)
        spInput.button[SP_AXIS_LEFT] = 1;
      if (value == -1 && spInput.axis[0] == -1)
        spInput.button[SP_AXIS_LEFTDOWN] = 1;
      if (value == -1 && spInput.axis[0] == 0)
        spInput.button[SP_AXIS_DOWN] = 1;
      if (value == -1 && spInput.axis[0] == 1)
        spInput.button[SP_AXIS_RIGHTDOWN] = 1;
      if (value == 0 && spInput.axis[0] == 1)
        spInput.button[SP_AXIS_RIGHT] = 1;
      if (value == 1 && spInput.axis[0] == 1)
        spInput.button[SP_AXIS_RIGHTUP] = 1;
      if (value == 1 && spInput.axis[0] == 0)
        spInput.button[SP_AXIS_UP] = 1;
        }
    spUpdateAxis(axis);
  #else
    spInput.axis[axis] = value;
  #endif
}*/

void spUpdateAxis(int axis)
{
  #ifdef GP2X
    if (axis==0)
    {
      spInput.axis[axis] = 0;
      if (spInput.button[SP_AXIS_LEFTUP] ||
          spInput.button[SP_AXIS_LEFT]   ||
          spInput.button[SP_AXIS_LEFTDOWN])
        spInput.axis[axis] = -1;
      if (spInput.button[SP_AXIS_RIGHTUP] ||
          spInput.button[SP_AXIS_RIGHT]   ||
          spInput.button[SP_AXIS_RIGHTDOWN])
        spInput.axis[axis] = 1;
    }
    else
    {
      spInput.axis[axis] = 0;
      if (spInput.button[SP_AXIS_LEFTUP] ||
          spInput.button[SP_AXIS_UP]   ||
          spInput.button[SP_AXIS_RIGHTUP])
        spInput.axis[axis] = 1;
      if (spInput.button[SP_AXIS_LEFTDOWN] ||
          spInput.button[SP_AXIS_DOWN]   ||
          spInput.button[SP_AXIS_RIGHTDOWN])
        spInput.axis[axis] = -1;
    }
  #endif
}

Uint32 oldticks;
Uint32 olderticks;
Uint32 newticks;

PREFIX int spLoop(void (*spDraw)(void),int (*spCalc)(Uint32 steps),Uint32 minwait,void (*spResize)(Uint16 w,Uint16 h))
{
  Uint32 bigsteps=0;
  Uint32 frames=0;
  int back=0;
  Uint32 steps=0;
  olderticks=SDL_GetTicks();
  oldticks=olderticks;
  newticks=olderticks;
  while(back==0 && !spDone ) {
      newticks=SDL_GetTicks();
      if (spHandleEvent() && spResize)
        spResize(spWindowX,spWindowY);
      spUpdateAxis(0);
      spUpdateAxis(1);
      if (newticks-oldticks > 0)
      {
        back = spCalc(newticks-oldticks);
        oldticks = newticks;
      }
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
        olderticks = newticks;
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

PREFIX PspInput spGetInput(void)
{
  return &spInput;
}

PREFIX void spQuitCore(void)
{
  #ifdef MOBILE_DEVICE
    SDL_JoystickClose(spJoy);
  #endif
  SDL_Quit();
}
