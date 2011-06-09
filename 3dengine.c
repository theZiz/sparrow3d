#include "3dengine.h"

tdrawitem engineDrawlist[DRAWLIST_LENGTH];
int engineDrawlistCount;
SDL_Surface *engineKeymap = NULL;
SDL_Surface *engineKeymapBlue = NULL;
SDL_Surface *engineKeymapGreen = NULL;
SDL_Surface *engineKeymapRed = NULL;
SDL_Surface *engineSurface;
SDL_Surface *engineScreen;
SDL_Joystick *engineJoy = NULL;
Uint32 engineFps = 0;
Sint32 engineViewportX;
Sint32 engineViewportY;
char engineGlobalDone = 0;
tEngineInput engineInput;
int engineMuteKey = 0;
Sint32 globalzoom=1<<ACCURACY;
tlight light[8];
tlight ambient;
char lighton;

void engineLight(char on)
{
  lighton=on;
}

void engineSetAmbient(Sint32 r,Sint32 g,Sint32 b)
{
  ambient.r=r;
  ambient.g=g;
  ambient.b=b;
}

plight engineGetLightPointer()
{
  return light;
}

Uint32 engineGetFps()
{
  return engineFps;
}

void engineSetMuteKey(int key)
{
  engineMuteKey=key;
}

int engineGetMuteKey()
{
  return engineMuteKey;
}

pEngineInput engineGetInput()
{
  return &engineInput;
}

void engineSetGlobalDone()
{
  engineGlobalDone=1;
}

SDL_Surface* engineGetSurface(int nr)
{
  switch (nr)
  {
    case SURFACE_SURFACE: return engineSurface;
    case SURFACE_SCREEN: return engineScreen;
    case SURFACE_KEYMAP: return engineKeymap;
    case SURFACE_KEYMAP_BLUE: return engineKeymapBlue;
    case SURFACE_KEYMAP_GREEN: return engineKeymapGreen;
    case SURFACE_KEYMAP_RED: return engineKeymapRed;
  }
}

Sint32 min(Sint32 a,Sint32 b)
{
  if (a<b)
    return a;
  return b;
}

Sint32 min4(Sint32 a,Sint32 b,Sint32 c,Sint32 d)
{
  Sint32 min=2147483647; //2^31-1
  int i=0;
  for (i=0;i<3;i++)
  {
    if (a<min)
      min=a;
    if (b<min)
      min=b;
    if (c<min)
      min=c;
    if (d<min)
      min=d;
  }
  return min;
}

Sint32 min3(Sint32 a,Sint32 b,Sint32 c)
{
  Sint32 min=2147483647; //2^31-1
  int i=0;
  for (i=0;i<2;i++)
  {
    if (a<min)
      min=a;
    if (b<min)
      min=b;
    if (c<min)
      min=c;
  }
  return min;
}

Sint32 max(Sint32 a,Sint32 b)
{
  if (a>b)
    return a;
  return b;
}

void calcNormal(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 x2,Sint32 y2,Sint32 z2,
                Sint32 x3,Sint32 y3,Sint32 z3,Sint32* normale)
{
  normale[0]=((y1-y2)>>HALF_ACCURACY)*((z2-z3)>>HALF_ACCURACY)
            -((z1-z2)>>HALF_ACCURACY)*((y2-y3)>>HALF_ACCURACY);
  normale[1]=((z1-z2)>>HALF_ACCURACY)*((x2-x3)>>HALF_ACCURACY)
            -((x1-x2)>>HALF_ACCURACY)*((z2-z3)>>HALF_ACCURACY);
  normale[2]=((x1-x2)>>HALF_ACCURACY)*((y2-y3)>>HALF_ACCURACY)
            -((y1-y2)>>HALF_ACCURACY)*((x2-x3)>>HALF_ACCURACY);
}

//senquack - fast ARM ASM 16:16 fixed point divide routine:
//					Credit goes to Henry Thomas and the website is
//					http://me.henri.net/fp-div.html
//int32_t fpdiv(register int32_t numerator, register int32_t denominator)
#ifdef ARMCPU
int fpdiv(register int numerator, register int denominator)
{
//senquack
//    register int32_t quotient;
    register int quotient;
//    asm("num     .req %[numerator]      @ Map Register Equates\n\t"
    __asm__ volatile ("num     .req %[numerator]      @ Map Register Equates\n\t"
        "den     .req %[denominator]\n\t"
        "mod     .req r2\n\t"
        "cnt     .req r3\n\t"
        "quo     .req r4\n\t"
        "sign    .req r12\n\t"
        /* set sign and ensure numerator and denominator are positive */
        "cmp den, #0                    @ exceptioin if den == zero\n\t"
        "beq .div0\n\t"
        "eor sign, num, den             @ sign = num ^ den\n\t"
        "rsbmi den, den, #0             @ den = -den if den < 0\n\t"
        "subs mod, den, #1              @ mod = den - 1\n\t"
        "beq .div1                      @ return if den == 1\n\t"
        "movs cnt, num                  @ num = -num if num < 0\n\t"
        "rsbmi num, num, #0\n\t"
        /* skip if deniminator >= numerator */
        "movs cnt, num, lsr #16         @ return if den >= num << 16\n\t"
        "bne .cont\n\t"
        "cmp den, num, lsl #16\n\t"
        "bhs .numLeDen\n\t"
    "\n.cont:\n\t"
        /* test if denominator is a power of two */
        "tst den, mod                   @ if(den & (den - 1) == 0)\n\t"
//senquack - was missing terminating quote here:
        "beq .powerOf2                  @ den is power of 2\n\t"
        /* count leading zeros */
        "stmfd sp!, {r4}                @ push r4 (quo) onto the stack\n\t"
        "mov cnt, #28                   @ count difference in leading zeros\n\t"
        "mov mod, num, lsr #4           @ between num and den\n\t"
        "cmp den, mod, lsr #12; subls cnt, cnt, #16; movls mod, mod, lsr #16\n\t"
        "cmp den, mod, lsr #4 ; subls cnt, cnt, #8 ; movls mod, mod, lsr #8\n\t"
        "cmp den, mod         ; subls cnt, cnt, #4 ; movls mod, mod, lsr #4\n\t"
        /* shift numerator left by cnt bits */
        "mov num, num, lsl cnt          @ mod:num = num << cnt\n\t"
        "mov quo, #0\n\t"
        "rsb den, den, #0               @ negate den for divide loop\n\t"
        /* skip cnt iterations in the divide loop */
        "adds num, num, num             @ start: num = mod:num / den\n\t"
        "add pc, pc, cnt, lsl #4        @ skip cnt x 4 x 4 iterations\n\t"
        "nop                            @ nop instruction takes care of pipeline\n\t"
        /* inner loop unrolled x 48 */
        ".rept 47                       @ inner loop x 48\n\t"
        "    adcs mod, den, mod, lsl #1\n\t"
        "    subcc mod, mod, den\n\t"
        "    adc quo, quo, quo\n\t"
        "    adds num, num, num\n\t"
        ".endr\n\t"
        "adcs mod, den, mod, lsl #1\n\t"
        "subcc mod, mod, den\n\t"
        "adc quo, quo, quo\n\t"
        /* negate quotient if signed */
        "cmp sign, #0                   @ negate quotient if sign < 0\n\t"
        "mov num, quo\n\t"
        "rsbmi num, num, #0\n\t"
        "ldmfd sp!, {r4}                @ pop r4 (quo) off the stack\n\t"
        "mov pc, lr                     @return\n\t"
        /* divide by zero handler */
    "\n.div0:\n\t"
        "mov num, #0\n\t"
        "mov pc, lr                     @return\n\t"
        /* divide by one handler */
    "\n.div1:\n\t"
        "cmp sign, #0\n\t"
        "mov num, num, asl #16\n\t"
        "rsbmi num, num, #0\n\t"
        "mov pc, lr                     @return\n\t"
        /* numerator less than or equal to denominator handler */
    "\n.numLeDen:\n\t"
        "mov num, #0                    @ quotient = 0 if num < den\n\t"
        "moveq num, sign, asr #31       @ negate quotient if sign < 0\n\t"
        "orreq num, num, #1             @ quotient = 1 if num == den\n\t"
        "mov pc, lr                     @return\n\t"
        /* power of two handler */
    "\n.powerOf2:\n\t"
        "mov cnt, #0\n\t"
        "cmp den, #(1 << 16); movhs cnt, #16    ; movhs den, den, lsr #16\n\t"
        "cmp den, #(1 << 8) ; addhs cnt, cnt, #8; movhs den, den, lsr #8\n\t"
        "cmp den, #(1 << 4) ; addhs cnt, cnt, #4; movhs den, den, lsr #4\n\t"
        "cmp den, #(1 << 2) ; addhi cnt, cnt, #3; addls cnt, cnt, den, lsr #1\n\t"
        "rsb mod, cnt, #32\n\t"
        "mov den, num, lsr #16          @ den:num = num << 16\n\t"
        "mov num, num, lsl #16\n\t"
        "mov num, num, lsr cnt          @ num = num >> cnt | den << mod\n\t"
        "orr num, num, den, lsl mod\n\t"
        "cmp sign, #0\n\t"
        "rsbmi num, num, #0             @ negate quotient if sign < 0"
        /* output registers */
        : [quotient] "=r" (quotient)
        /* input registers */
        : [numerator] "0" (numerator), [denominator] "r" (denominator)
        /* clobbered registers */
        : "r2" /* mod */, "r3" /* cnt */, "r12" /* sign */);
    return quotient;
}
#endif

void loadKeyMap()
{
  int i;
  if (engineKeymap!=NULL)
    SDL_FreeSurface(engineKeymap);
  SDL_Surface* surface;
  if (globalzoom<(2<<ACCURACY))
    surface=IMG_Load("./data/keymap_small.png");
  else
  if (globalzoom<(4<<ACCURACY))
    surface=IMG_Load("./data/keymap.png");
  else
    surface=IMG_Load("./data/keymap_big.png");
  
  engineKeymap=SDL_DisplayFormatAlpha(surface);
  
  engineKeymapBlue=SDL_DisplayFormatAlpha(surface);
  SDL_LockSurface(engineKeymapBlue);
  Uint32* pixel=engineKeymapBlue->pixels;
  for (i=0;i<engineKeymapBlue->w*engineKeymapBlue->h;i++)
    pixel[i]&=4278190080+255; //just Blue
  SDL_UnlockSurface(engineKeymapBlue);

  engineKeymapGreen=SDL_DisplayFormatAlpha(surface);
  SDL_LockSurface(engineKeymapGreen);
  pixel=engineKeymapGreen->pixels;
  for (i=0;i<engineKeymapGreen->w*engineKeymapGreen->h;i++)
    pixel[i]&=4278190080+256*255; //just Green
  SDL_UnlockSurface(engineKeymapGreen);
  
  engineKeymapRed=SDL_DisplayFormatAlpha(surface);
  SDL_LockSurface(engineKeymapRed);
  pixel=engineKeymapRed->pixels;
  for (i=0;i<engineKeymapRed->w*engineKeymapRed->h;i++)
    pixel[i]&=4278190080+65536*255; //just Red
  SDL_UnlockSurface(engineKeymapRed);
  
  SDL_FreeSurface(surface);

}

void resizeWindow(int x,int y)
{
  #ifdef GP2X
    engineScreen=SDL_SetVideoMode(x,y,16,SDL_HWSURFACE/* | SDL_DOUBLEBUF*/);
    SDL_Surface* surface=SDL_CreateRGBSurface(SDL_HWSURFACE,x,y, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0);
    engineSurface=SDL_DisplayFormat(surface);
    SDL_FreeSurface(surface);
    //engineSurface=SDL_SetVideoMode(x,y,16,SDL_HWSURFACE | SDL_DOUBLEBUF);
  #elif defined CAANOO
    //engineSurface=SDL_SetVideoMode(x,y,16,SDL_DOUBLEBUF | SDL_SWSURFACE);
    engineScreen=SDL_SetVideoMode(x,y,16,SDL_HWSURFACE/* | SDL_DOUBLEBUF*/);
    SDL_Surface* surface=SDL_CreateRGBSurface(SDL_HWSURFACE,x,y, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0);
    engineSurface=SDL_DisplayFormat(surface);
    SDL_FreeSurface(surface);
  #elif defined DINGOO
    /*engineScreen=SDL_SetVideoMode(x,y,16,SDL_SWSURFACE);
    engineSurface=SDL_CreateRGBSurface(SDL_SWSURFACE,x,y, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0);*/
    engineSurface=SDL_SetVideoMode(x,y,16,/*SDL_HWPALETTE |*/ SDL_SWSURFACE);
  #elif defined PANDORA
    engineSurface=SDL_SetVideoMode(x,y,16,SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  #else
    /*x=800;
    y=480;*/
    engineSurface=SDL_SetVideoMode(x,y,16,SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_HWPALETTE | SDL_VIDEORESIZE);
    //engineSurface=SDL_SetVideoMode(x,y,16,SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_NOFRAME);
  #endif  
  if (x % 2 != 0)
    engineSetWindowX(x+1);
  else
    engineSetWindowX(x);
  engineSetWindowY(y);
  globalzoom=min((engineGetWindowX()<<ACCURACY)/320,(engineGetWindowY()<<ACCURACY)/240);  //Bei 320x240 == 1
  engineViewportX = engineGetWindowX()<<(HALF_ACCURACY-1);
  engineViewportY = engineGetWindowY()<<(HALF_ACCURACY-1);
  float p=(float)engineGetWindowX()/(float)engineGetWindowY();
  if (p>1.0)
    p=1.0/p;
  p=p*p;
  p=1;
  setPerspective(50.0,(float)engineGetWindowX()/(float)engineGetWindowY(),0.1,100);
  //printf("Set perspective to %f degress\n",45.0+15.0*p);
  loadKeyMap();
}

void initEngine()
{
   #ifdef PANDORA
    engineSetWindowX(800);
    engineSetWindowY(480);
  #else
    engineSetWindowX(320);
    engineSetWindowY(240);
  #endif 
  globalzoom=min((engineGetWindowX()<<ACCURACY)/320,(engineGetWindowY()<<ACCURACY)/240);  //GP2X sei default
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK/* | SDL_INIT_NOPARACHUTE*/); 
  //engineSurface=SDL_SetVideoMode(windowx,windowy,16,SDL_DOUBLEBUF | SDL_HWPALETTE | SDL_VIDEORESIZE);
  resizeWindow(engineGetWindowX(),engineGetWindowY());
  SDL_ShowCursor(0);
  #ifdef MOBILE_DEVICE
    engineJoy=SDL_JoystickOpen(0);
  #endif
  #ifdef PANDORA
    pnd_evdev_open(pnd_evdev_dpads);
    //fbdev = open("/dev/fb0", O_RDONLY);
  #endif
  loadKeyMap();
  initMath();
  engineDrawlistCount=0;
  int i;
  for (i=0;i<20;i++)
    engineInput.button[i]=0;
  engineInput.axis[0]=0;
  engineInput.axis[1]=0;
  for (i=0;i<8;i++)
  {
    light[i].active=(i==0)?1:0;
    light[i].x=0;
    light[i].y=0;
    light[i].z=0;
    light[i].r=224;
    light[i].g=224;
    light[i].b=224;
  }
  ambient.r=31;
  ambient.g=31;
  ambient.b=31;
  lighton=1;
}


void engineHandleEvent(void)
{
  #ifdef PANDORA
    int s = pnd_evdev_dpad_state ( pnd_evdev_dpads );
    if ( s & pnd_evdev_pandora )
      engineGlobalDone=1;
  #endif
  char new_axis = 0;
  SDL_Event event; 
  while (SDL_PollEvent(&event)==1)
  {
    switch (event.type)
    {
      case SDL_JOYBUTTONDOWN:
        engineInput.button[event.jbutton.button]=1;
        break;
      case SDL_JOYBUTTONUP:
        engineInput.button[event.jbutton.button]=0;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
          case SDLK_LEFT:
            engineInput.axis[0]=-1;
            break;
          case SDLK_RIGHT:
            engineInput.axis[0]= 1;
            break;
          case SDLK_UP:
            engineInput.axis[1]= 1;
            break;
          case SDLK_DOWN:
            engineInput.axis[1]=-1;
            break;
          #ifdef DINGOO
            case SDLK_RETURN:
              engineInput.button[BUTTON_START]=1;
              break;
            case SDLK_SPACE:
              engineInput.button[BUTTON_X]=1;
              break;
            case SDLK_LSHIFT:
              engineInput.button[BUTTON_Y]=1;
              break;
            case SDLK_LCTRL:
              engineInput.button[BUTTON_A]=1;
              break;
            case SDLK_LALT:
              engineInput.button[BUTTON_B]=1;
              break;
            case SDLK_ESCAPE:
              engineInput.button[BUTTON_SELECT]=1;
              break;
            case SDLK_TAB:
              engineInput.button[BUTTON_L]=1;
              break;
            case SDLK_BACKSPACE:
              engineInput.button[BUTTON_R]=1;
              break;
            case SDLK_s:
              engineInput.button[BUTTON_VOLPLUS]=1;
              break;
            case SDLK_u:
              engineInput.button[BUTTON_VOLMINUS]=1;
              break;
          #elif defined PANDORA
//            case SDLK_MENU:
//              engineGlobalDone=1;
//              break;
            case SDLK_PAGEDOWN:
              engineInput.button[BUTTON_X]=1;
              break;
            case SDLK_PAGEUP:
              engineInput.button[BUTTON_Y]=1;
              break;
            case SDLK_HOME:
              engineInput.button[BUTTON_A]=1;
              break;
            case SDLK_END:
              engineInput.button[BUTTON_B]=1;
              break;
            case SDLK_LCTRL:
              engineInput.button[BUTTON_SELECT]=1;
              break;
            case SDLK_RSHIFT:
              engineInput.button[BUTTON_L]=1;
              break;
            case SDLK_RCTRL:
              engineInput.button[BUTTON_R]=1;
              break;
            case SDLK_LALT:
              engineInput.button[BUTTON_START]=1;
              break;
          #else //PC
            case SDLK_RETURN:
              engineInput.button[BUTTON_START]=1;
              break;
            case SDLK_a:
              engineInput.button[BUTTON_A]=1;
              break;
            case SDLK_d:
              engineInput.button[BUTTON_B]=1;
              break;
            case SDLK_w:
              engineInput.button[BUTTON_Y]=1;
              break;
            case SDLK_s:
              engineInput.button[BUTTON_X]=1;
              break;
            case SDLK_BACKSPACE:
              engineInput.button[BUTTON_SELECT]=1;
              break;
            case SDLK_PLUS:
              engineInput.button[BUTTON_VOLPLUS]=1;
              break;
            case SDLK_MINUS:
              engineInput.button[BUTTON_VOLMINUS]=1;
              break;
            case SDLK_q:
              engineInput.button[BUTTON_L]=1;
              break;
            case SDLK_e:
              engineInput.button[BUTTON_R]=1;
              break;
          #endif
          case SDLK_m:
            engineMuteKey=1;
            break;
        }
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym)
        {
          case SDLK_LEFT:
            if (engineInput.axis[0] == -1)
              engineInput.axis[0]= 0;
            break;
          case SDLK_RIGHT:
            if (engineInput.axis[0] == 1)
              engineInput.axis[0]= 0;
            break;
          case SDLK_UP:
            if (engineInput.axis[1] == 1)
              engineInput.axis[1]= 0;
            break;
          case SDLK_DOWN:
            if (engineInput.axis[1] == -1)
              engineInput.axis[1]= 0;
            break;
          #ifdef DINGOO
            case SDLK_RETURN:
              engineInput.button[BUTTON_START]=0;
              break;
            case SDLK_SPACE:
              engineInput.button[BUTTON_X]=0;
              break;
            case SDLK_LSHIFT:
              engineInput.button[BUTTON_Y]=0;
              break;
            case SDLK_LCTRL:
              engineInput.button[BUTTON_A]=0;
              break;
            case SDLK_LALT:
              engineInput.button[BUTTON_B]=0;
              break;
            case SDLK_ESCAPE:
              engineInput.button[BUTTON_SELECT]=0;
              break;
            case SDLK_TAB:
              engineInput.button[BUTTON_L]=0;
              break;
            case SDLK_BACKSPACE:
              engineInput.button[BUTTON_R]=0;
              break;
            case SDLK_s:
              engineInput.button[BUTTON_VOLPLUS]=0;
              break;
            case SDLK_u:
              engineInput.button[BUTTON_VOLMINUS]=0;
              break;
          #elif defined PANDORA
            case SDLK_PAGEDOWN:
              engineInput.button[BUTTON_X]=0;
              break;
            case SDLK_PAGEUP:
              engineInput.button[BUTTON_Y]=0;
              break;
            case SDLK_HOME:
              engineInput.button[BUTTON_A]=0;
              break;
            case SDLK_END:
              engineInput.button[BUTTON_B]=0;
              break;
            case SDLK_LCTRL:
              engineInput.button[BUTTON_SELECT]=0;
              break;
            case SDLK_RSHIFT:
              engineInput.button[BUTTON_L]=0;
              break;
            case SDLK_RCTRL:
              engineInput.button[BUTTON_R]=0;
              break;
            case SDLK_LALT:
              engineInput.button[BUTTON_START]=0;
              break;
          #else //PC
            case SDLK_RETURN:
              engineInput.button[BUTTON_START]=0;
              break;
            case SDLK_a:
              engineInput.button[BUTTON_A]=0;
              break;
            case SDLK_d:
              engineInput.button[BUTTON_B]=0;
              break;
            case SDLK_w:
              engineInput.button[BUTTON_Y]=0;
              break;
            case SDLK_s:
              engineInput.button[BUTTON_X]=0;
              break;
            case SDLK_BACKSPACE:
              engineInput.button[BUTTON_SELECT]=0;
              break;
            case SDLK_PLUS:
              engineInput.button[BUTTON_VOLPLUS]=0;
              break;
            case SDLK_MINUS:
              engineInput.button[BUTTON_VOLMINUS]=0;
              break;
            case SDLK_q:
              engineInput.button[BUTTON_L]=0;
              break;
            case SDLK_e:
              engineInput.button[BUTTON_R]=0;
              break;
          #endif
          case SDLK_m:
            engineMuteKey=0;
            break;
        }
        break;
      case SDL_JOYAXISMOTION:
        if (event.jaxis.axis==0)
        {
          if (event.jaxis.value<JOYSTICK_MIN)
            engineInput.axis[event.jaxis.axis]=-1;
          else if (event.jaxis.value>JOYSTICK_MAX)
            engineInput.axis[event.jaxis.axis]= 1;
          else
            engineInput.axis[event.jaxis.axis]= 0;
        }
        if (event.jaxis.axis==1)
        {
          if (event.jaxis.value<JOYSTICK_MIN)
            engineInput.axis[event.jaxis.axis]= 1;
          else if (event.jaxis.value>JOYSTICK_MAX)
            engineInput.axis[event.jaxis.axis]=-1;
          else
            engineInput.axis[event.jaxis.axis]= 0;
        }
        break;  
      case SDL_QUIT:
        engineGlobalDone=1;
        break;
      case SDL_VIDEORESIZE:
        resizeWindow(event.resize.w,event.resize.h);
        break;
    }
  }
}

signed char engineGetAxis(int axis)
{
  #ifdef GP2X
    if (axis==0)
    {
      if (engineInput.button[AXIS_LEFTUP] ||
          engineInput.button[AXIS_LEFT]   ||
          engineInput.button[AXIS_LEFTDOWN])
        return -1;
      if (engineInput.button[AXIS_RIGHTUP] ||
          engineInput.button[AXIS_RIGHT]   ||
          engineInput.button[AXIS_RIGHTDOWN])
        return  1;
    }
    else
    {
      if (engineInput.button[AXIS_LEFTUP] ||
          engineInput.button[AXIS_UP]   ||
          engineInput.button[AXIS_RIGHTUP])
        return  1;
      if (engineInput.button[AXIS_LEFTDOWN] ||
          engineInput.button[AXIS_DOWN]   ||
          engineInput.button[AXIS_RIGHTDOWN])
        return -1;
    }
    return 0;
  #else
    return engineInput.axis[axis];
  #endif
}

int engineLoop(void (*engineDraw)(void),int (*engineCalc)(Uint32 steps),Uint32 minwait)
{
    Uint32 bigsteps=0;
    Uint32 frames=0;
    int back=0;
    Uint32 steps=0;
    Uint32 oldticks=SDL_GetTicks();
    Uint32 newticks=SDL_GetTicks();
    while( back==0 && engineGlobalDone==0 ) {
        oldticks=newticks;
        newticks=SDL_GetTicks();
        engineHandleEvent();
        steps+=newticks-oldticks;
        if (steps>minwait)
        {
          back = engineCalc(steps);
          engineDraw();
          frames++;
          bigsteps=bigsteps+steps;
          while (bigsteps>=1000)
          {
            bigsteps-=1000;
            engineFps=frames;
            frames=0;
          }
          steps=0;
        }
        else
          SDL_Delay(1);
    }
    return back;
}


void quitEngine(void)
{
  #ifdef MOBILE_DEVICE
    SDL_JoystickClose(engineJoy);
  #endif
  #ifdef PANDORA
    pnd_evdev_close(pnd_evdev_dpads);
  #endif 
  SDL_FreeSurface(engineKeymap);
  SDL_FreeSurface(engineSurface);  
  SDL_Quit(); 
}

void engineClearScreen(Uint16 color)
{
  clearScreen(engineSurface,color);
}

void engineRotate(Sint32 x,Sint32 y,Sint32 z,Sint32 rad)
{
	//Rotationsmatrix:
	Sint32 s=mysin(rad);
	Sint32 c=mycos(rad);
  Sint32 l = fpsqrt((x>>HALF_ACCURACY)*(x>>HALF_ACCURACY)
                   +(y>>HALF_ACCURACY)*(y>>HALF_ACCURACY)
                   +(z>>HALF_ACCURACY)*(z>>HALF_ACCURACY));
	if (l==0)
	  return;
	x=((x<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
	y=((y<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
	z=((z<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
	Sint32 rotate[16];
  rotate[ 0]= c+(((x>>HALF_ACCURACY)*(x>>HALF_ACCURACY))>>HALF_ACCURACY)*(((1<<ACCURACY)-c)>>HALF_ACCURACY);
  rotate[ 4]=   (((x>>HALF_ACCURACY)*(y>>HALF_ACCURACY))>>HALF_ACCURACY)*(((1<<ACCURACY)-c)>>HALF_ACCURACY)-(z>>HALF_ACCURACY)*(s>>HALF_ACCURACY);
  rotate[ 8]=   (((x>>HALF_ACCURACY)*(z>>HALF_ACCURACY))>>HALF_ACCURACY)*(((1<<ACCURACY)-c)>>HALF_ACCURACY)+(y>>HALF_ACCURACY)*(s>>HALF_ACCURACY);
  //rotate[12]= 0<<ACCURACY;
  rotate[ 1]=   (((y>>HALF_ACCURACY)*(x>>HALF_ACCURACY))>>HALF_ACCURACY)*(((1<<ACCURACY)-c)>>HALF_ACCURACY)+(z>>HALF_ACCURACY)*(s>>HALF_ACCURACY);
  rotate[ 5]= c+(((y>>HALF_ACCURACY)*(y>>HALF_ACCURACY))>>HALF_ACCURACY)*(((1<<ACCURACY)-c)>>HALF_ACCURACY);
  rotate[ 9]=   (((y>>HALF_ACCURACY)*(z>>HALF_ACCURACY))>>HALF_ACCURACY)*(((1<<ACCURACY)-c)>>HALF_ACCURACY)-(x>>HALF_ACCURACY)*(s>>HALF_ACCURACY);
  //rotate[13]= 0<<ACCURACY;
  rotate[ 2]=   (((z>>HALF_ACCURACY)*(x>>HALF_ACCURACY))>>HALF_ACCURACY)*(((1<<ACCURACY)-c)>>HALF_ACCURACY)-(y>>HALF_ACCURACY)*(s>>HALF_ACCURACY);
  rotate[ 6]=   (((z>>HALF_ACCURACY)*(y>>HALF_ACCURACY))>>HALF_ACCURACY)*(((1<<ACCURACY)-c)>>HALF_ACCURACY)+(x>>HALF_ACCURACY)*(s>>HALF_ACCURACY);
  rotate[10]= c+(((z>>HALF_ACCURACY)*(z>>HALF_ACCURACY))>>HALF_ACCURACY)*(((1<<ACCURACY)-c)>>HALF_ACCURACY);
  //rotate[14]= 0<<ACCURACY;
  //rotate[ 3]= 0<<ACCURACY;
  //rotate[ 7]= 0<<ACCURACY;
  //rotate[11]= 0<<ACCURACY;
  //rotate[15]= 1<<ACCURACY;

  Sint32* modellViewMatrix=engineGetModellViewMatrix();
	Sint32 result[12];
	result[ 0]=(modellViewMatrix[ 0]>>HALF_ACCURACY)*(rotate[ 0]>>HALF_ACCURACY)+(modellViewMatrix[ 4]>>HALF_ACCURACY)*(rotate[ 1]>>HALF_ACCURACY)+(modellViewMatrix[ 8]>>HALF_ACCURACY)*(rotate[ 2]>>HALF_ACCURACY);
	result[ 1]=(modellViewMatrix[ 1]>>HALF_ACCURACY)*(rotate[ 0]>>HALF_ACCURACY)+(modellViewMatrix[ 5]>>HALF_ACCURACY)*(rotate[ 1]>>HALF_ACCURACY)+(modellViewMatrix[ 9]>>HALF_ACCURACY)*(rotate[ 2]>>HALF_ACCURACY);
	result[ 2]=(modellViewMatrix[ 2]>>HALF_ACCURACY)*(rotate[ 0]>>HALF_ACCURACY)+(modellViewMatrix[ 6]>>HALF_ACCURACY)*(rotate[ 1]>>HALF_ACCURACY)+(modellViewMatrix[10]>>HALF_ACCURACY)*(rotate[ 2]>>HALF_ACCURACY);
	result[ 3]=(modellViewMatrix[ 3]>>HALF_ACCURACY)*(rotate[ 0]>>HALF_ACCURACY)+(modellViewMatrix[ 7]>>HALF_ACCURACY)*(rotate[ 1]>>HALF_ACCURACY)+(modellViewMatrix[11]>>HALF_ACCURACY)*(rotate[ 2]>>HALF_ACCURACY);

	result[ 4]=(modellViewMatrix[ 0]>>HALF_ACCURACY)*(rotate[ 4]>>HALF_ACCURACY)+(modellViewMatrix[ 4]>>HALF_ACCURACY)*(rotate[ 5]>>HALF_ACCURACY)+(modellViewMatrix[ 8]>>HALF_ACCURACY)*(rotate[ 6]>>HALF_ACCURACY);
	result[ 5]=(modellViewMatrix[ 1]>>HALF_ACCURACY)*(rotate[ 4]>>HALF_ACCURACY)+(modellViewMatrix[ 5]>>HALF_ACCURACY)*(rotate[ 5]>>HALF_ACCURACY)+(modellViewMatrix[ 9]>>HALF_ACCURACY)*(rotate[ 6]>>HALF_ACCURACY);
	result[ 6]=(modellViewMatrix[ 2]>>HALF_ACCURACY)*(rotate[ 4]>>HALF_ACCURACY)+(modellViewMatrix[ 6]>>HALF_ACCURACY)*(rotate[ 5]>>HALF_ACCURACY)+(modellViewMatrix[10]>>HALF_ACCURACY)*(rotate[ 6]>>HALF_ACCURACY);
	result[ 7]=(modellViewMatrix[ 3]>>HALF_ACCURACY)*(rotate[ 4]>>HALF_ACCURACY)+(modellViewMatrix[ 7]>>HALF_ACCURACY)*(rotate[ 5]>>HALF_ACCURACY)+(modellViewMatrix[11]>>HALF_ACCURACY)*(rotate[ 6]>>HALF_ACCURACY);

	result[ 8]=(modellViewMatrix[ 0]>>HALF_ACCURACY)*(rotate[ 8]>>HALF_ACCURACY)+(modellViewMatrix[ 4]>>HALF_ACCURACY)*(rotate[ 9]>>HALF_ACCURACY)+(modellViewMatrix[ 8]>>HALF_ACCURACY)*(rotate[10]>>HALF_ACCURACY);
	result[ 9]=(modellViewMatrix[ 1]>>HALF_ACCURACY)*(rotate[ 8]>>HALF_ACCURACY)+(modellViewMatrix[ 5]>>HALF_ACCURACY)*(rotate[ 9]>>HALF_ACCURACY)+(modellViewMatrix[ 9]>>HALF_ACCURACY)*(rotate[10]>>HALF_ACCURACY);
	result[10]=(modellViewMatrix[ 2]>>HALF_ACCURACY)*(rotate[ 8]>>HALF_ACCURACY)+(modellViewMatrix[ 6]>>HALF_ACCURACY)*(rotate[ 9]>>HALF_ACCURACY)+(modellViewMatrix[10]>>HALF_ACCURACY)*(rotate[10]>>HALF_ACCURACY);
	result[11]=(modellViewMatrix[ 3]>>HALF_ACCURACY)*(rotate[ 8]>>HALF_ACCURACY)+(modellViewMatrix[ 7]>>HALF_ACCURACY)*(rotate[ 9]>>HALF_ACCURACY)+(modellViewMatrix[11]>>HALF_ACCURACY)*(rotate[10]>>HALF_ACCURACY);

	memcpy(modellViewMatrix,result,sizeof(Sint32)*12);
}

void engineTranslate(Sint32 x,Sint32 y,Sint32 z)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();

  modellViewMatrix[12]=(modellViewMatrix[ 0]>>HALF_ACCURACY)*(x>>HALF_ACCURACY)+(modellViewMatrix[ 4]>>HALF_ACCURACY)*(y>>HALF_ACCURACY)+(modellViewMatrix[ 8]>>HALF_ACCURACY)*(z>>HALF_ACCURACY)+modellViewMatrix[12];
	modellViewMatrix[13]=(modellViewMatrix[ 1]>>HALF_ACCURACY)*(x>>HALF_ACCURACY)+(modellViewMatrix[ 5]>>HALF_ACCURACY)*(y>>HALF_ACCURACY)+(modellViewMatrix[ 9]>>HALF_ACCURACY)*(z>>HALF_ACCURACY)+modellViewMatrix[13];
	modellViewMatrix[14]=(modellViewMatrix[ 2]>>HALF_ACCURACY)*(x>>HALF_ACCURACY)+(modellViewMatrix[ 6]>>HALF_ACCURACY)*(y>>HALF_ACCURACY)+(modellViewMatrix[10]>>HALF_ACCURACY)*(z>>HALF_ACCURACY)+modellViewMatrix[14];
	modellViewMatrix[15]=(modellViewMatrix[ 3]>>HALF_ACCURACY)*(x>>HALF_ACCURACY)+(modellViewMatrix[ 7]>>HALF_ACCURACY)*(y>>HALF_ACCURACY)+(modellViewMatrix[11]>>HALF_ACCURACY)*(z>>HALF_ACCURACY)+modellViewMatrix[15];
}

void engineScale(Sint32 x,Sint32 y,Sint32 z)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();

  modellViewMatrix[ 0]=(modellViewMatrix[ 0]>>HALF_ACCURACY)*(x>>HALF_ACCURACY);
	modellViewMatrix[ 5]=(modellViewMatrix[ 5]>>HALF_ACCURACY)*(y>>HALF_ACCURACY);
	modellViewMatrix[10]=(modellViewMatrix[10]>>HALF_ACCURACY)*(z>>HALF_ACCURACY);
}

int sortedInsert(tdrawitem item)
{
  int c;
  for (c=0;c<engineDrawlistCount;c++)
  {
    if (engineDrawlist[c].content.all.z<item.content.all.z)
    {
      int d=engineDrawlistCount;
      if (d>=DRAWLIST_LENGTH)
        d--;
      else
        engineDrawlistCount++;
      for (;d>c;d--)
        engineDrawlist[d]=engineDrawlist[d-1];
      engineDrawlist[c]=item;
      return c;
    }
  }
  if (engineDrawlistCount==DRAWLIST_LENGTH)
    return -1;
  engineDrawlist[engineDrawlistCount]=item;
  engineDrawlistCount++;
  return 0;
}

Uint16 engineGetLightingColor(Sint32 x,Sint32 y,Sint32 z,Sint32* normal,Uint16 color)
{
  if (!lighton)
    return color;
  Sint32 r=ambient.r;
  Sint32 g=ambient.g;
  Sint32 b=ambient.b;
  int i;
  for (i=0;i<8;i++)
    if (light[i].active)
    {
      Sint32 ox=x-light[i].x;
      Sint32 oy=y-light[i].y;
      Sint32 oz=z-light[i].z;

      Sint32 ol = fpsqrt((ox>>HALF_ACCURACY)*(ox>>HALF_ACCURACY)+
                         (oy>>HALF_ACCURACY)*(oy>>HALF_ACCURACY)+
                         (oz>>HALF_ACCURACY)*(oz>>HALF_ACCURACY));
      if (ol==0)
        continue;
      
      ox=((ox<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
      oy=((oy<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
      oz=((oz<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
      
      Sint32 ac = (ox>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)
                 +(oy>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)
                 +(oz>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY);
                 
      if (ac<0)
      {
        Sint32 factor=-ac;//(-3*(ac>>2)+(1<<(ACCURACY-2)));

        if (factor<0)
          factor=0;
        if (factor>(1<<ACCURACY))
          factor=1<<ACCURACY;
        r+=(((color>>11) & 31)*8*(factor*light[i].r>>8))>>ACCURACY;
        g+=(((color>>5 ) & 63)*4*(factor*light[i].g>>8))>>ACCURACY;
        b+=(((color    ) & 31)*8*(factor*light[i].b>>8))>>ACCURACY;
      }
    }
  if (r>255)
    r=255;
  if (g>255)
    g=255;
  if (b>255)
    b=255;
  return getRGB(r,g,b);
}

void engineQuad(Sint32 x1,Sint32 y1,Sint32 z1,
                Sint32 x2,Sint32 y2,Sint32 z2,
                Sint32 x3,Sint32 y3,Sint32 z3,
                Sint32 x4,Sint32 y4,Sint32 z4,Uint16 color)
{ 
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  Sint32* projectionMatrix=engineGetProjectionMatrix();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();
  Sint32 tx1 = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 ty1 = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tz1 = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[10] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tw1 = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[11] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);

  Sint32 tx2 = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(x2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(y2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(z2 >> HALF_ACCURACY)
             + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 ty2 = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(x2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(y2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(z2 >> HALF_ACCURACY)
             + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tz2 = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(x2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(y2 >> HALF_ACCURACY)
             + (modellViewMatrix[10] >> HALF_ACCURACY)*(z2 >> HALF_ACCURACY)
             + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tw2 = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(x2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(y2 >> HALF_ACCURACY)
             + (modellViewMatrix[11] >> HALF_ACCURACY)*(z2 >> HALF_ACCURACY)
             + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);

  Sint32 tx3 = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(x3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(y3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(z3 >> HALF_ACCURACY)
             + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 ty3 = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(x3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(y3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(z3 >> HALF_ACCURACY)
             + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tz3 = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(x3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(y3 >> HALF_ACCURACY)
             + (modellViewMatrix[10] >> HALF_ACCURACY)*(z3 >> HALF_ACCURACY)
             + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tw3 = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(x3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(y3 >> HALF_ACCURACY)
             + (modellViewMatrix[11] >> HALF_ACCURACY)*(z3 >> HALF_ACCURACY)
             + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);


  //Normale berechnen
  Sint32 normal[3];
  calcNormal(tx1,ty1,tz1,tx2,ty2,tz2,tx3,ty3,tz3,normal);
  //Sint32 l=(Sint32)(sqrt((float)normal[0]*(float)normal[0]+(float)normal[1]*(float)normal[1]+(float)normal[2]*(float)normal[2]));
  Sint32 l = fpsqrt((normal[0]>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)+
                    (normal[1]>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)+
                    (normal[2]>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY));
  if (l==0)
    return;        
  Sint32 tx4 = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(x4 >> HALF_ACCURACY)
             + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(y4 >> HALF_ACCURACY)
             + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(z4 >> HALF_ACCURACY)
             + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 ty4 = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(x4 >> HALF_ACCURACY)
             + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(y4 >> HALF_ACCURACY)
             + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(z4 >> HALF_ACCURACY)
             + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tz4 = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(x4 >> HALF_ACCURACY)
             + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(y4 >> HALF_ACCURACY)
             + (modellViewMatrix[10] >> HALF_ACCURACY)*(z4 >> HALF_ACCURACY)
             + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tw4 = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(x4 >> HALF_ACCURACY)
             + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(y4 >> HALF_ACCURACY)
             + (modellViewMatrix[11] >> HALF_ACCURACY)*(z4 >> HALF_ACCURACY)
             + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);

  //Vector von Betrachter zu Fläche
  Sint32 ox = ((tx1+tx2+tx3+tx4)>>2);
  Sint32 oy = ((ty1+ty2+ty3+ty4)>>2);
  Sint32 oz = ((tz1+tz2+tz3+tz4)>>2);
  //Sint32 ol = (Sint32)(sqrt((float)ox*(float)ox+(float)oy*(float)oy+(float)oz*(float)oz));
  Sint32 ol = fpsqrt((ox>>HALF_ACCURACY)*(ox>>HALF_ACCURACY)+
                     (oy>>HALF_ACCURACY)*(oy>>HALF_ACCURACY)+
                     (oz>>HALF_ACCURACY)*(oz>>HALF_ACCURACY));
  if (ol==0)
    return;
  ox=((ox<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
  oy=((oy<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
  oz=((oz<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
  normal[0]=((normal[0]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
  normal[1]=((normal[1]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
  normal[2]=((normal[2]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
  
  
  Sint32 ac = (ox>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)
             +(oy>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)
             +(oz>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY);
  //Sint32 temp = ((l>>HALF_ACCURACY)*(ol >> HALF_ACCURACY));
  //if (temp==0)
    //return;
  //ac = ((ac<<HALF_ACCURACY) / temp)<<HALF_ACCURACY;
  
  if (ac>0)
    return;

         x1 = (projectionMatrix[ 0] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 4] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            + (projectionMatrix[ 8] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[12] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
         y1 = //(projectionMatrix[ 1] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 5] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 9] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[13] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
         /*z1 = (projectionMatrix[ 2] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 6] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            + (projectionMatrix[10] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[14] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY);*/
  Sint32 w1 = /*(projectionMatrix[ 3] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 7] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)*/
            + (projectionMatrix[11] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[15] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
      
  Sint32 nx1 = ((x1<<HALF_ACCURACY)/w1);
  Sint32 ny1 = ((y1<<HALF_ACCURACY)/w1);
  //Sint32 nz1 = ((z1<<HALF_ACCURACY)/w1);

         x2 = (projectionMatrix[ 0] >> HALF_ACCURACY)*(tx2 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 4] >> HALF_ACCURACY)*(ty2 >> HALF_ACCURACY)
            + (projectionMatrix[ 8] >> HALF_ACCURACY)*(tz2 >> HALF_ACCURACY)
            + (projectionMatrix[12] >> HALF_ACCURACY)*(tw2 >> HALF_ACCURACY)*/;
         y2 = //(projectionMatrix[ 1] >> HALF_ACCURACY)*(tx2 >> HALF_ACCURACY)
            + (projectionMatrix[ 5] >> HALF_ACCURACY)*(ty2 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 9] >> HALF_ACCURACY)*(tz2 >> HALF_ACCURACY)
            + (projectionMatrix[13] >> HALF_ACCURACY)*(tw2 >> HALF_ACCURACY)*/;
         /*z2 = (projectionMatrix[ 2] >> HALF_ACCURACY)*(tx2 >> HALF_ACCURACY)
            + (projectionMatrix[ 6] >> HALF_ACCURACY)*(ty2 >> HALF_ACCURACY)
            + (projectionMatrix[10] >> HALF_ACCURACY)*(tz2 >> HALF_ACCURACY)
            + (projectionMatrix[14] >> HALF_ACCURACY)*(tw2 >> HALF_ACCURACY);*/
  Sint32 w2 = /*(projectionMatrix[ 3] >> HALF_ACCURACY)*(tx2 >> HALF_ACCURACY)
            + (projectionMatrix[ 7] >> HALF_ACCURACY)*(ty2 >> HALF_ACCURACY)*/
            + (projectionMatrix[11] >> HALF_ACCURACY)*(tz2 >> HALF_ACCURACY)
            /*+ (projectionMatrix[15] >> HALF_ACCURACY)*(tw2 >> HALF_ACCURACY)*/;
      
  Sint32 nx2 = ((x2<<HALF_ACCURACY)/w2);
  Sint32 ny2 = ((y2<<HALF_ACCURACY)/w2);
  //Sint32 nz2 = ((z2<<HALF_ACCURACY)/w2);

         x3 = (projectionMatrix[ 0] >> HALF_ACCURACY)*(tx3 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 4] >> HALF_ACCURACY)*(ty3 >> HALF_ACCURACY)
            + (projectionMatrix[ 8] >> HALF_ACCURACY)*(tz3 >> HALF_ACCURACY)
            + (projectionMatrix[12] >> HALF_ACCURACY)*(tw3 >> HALF_ACCURACY)*/;
         y3 = //(projectionMatrix[ 1] >> HALF_ACCURACY)*(tx3 >> HALF_ACCURACY)
            + (projectionMatrix[ 5] >> HALF_ACCURACY)*(ty3 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 9] >> HALF_ACCURACY)*(tz3 >> HALF_ACCURACY)
            + (projectionMatrix[13] >> HALF_ACCURACY)*(tw3 >> HALF_ACCURACY)*/;
/*         z3 = (projectionMatrix[ 2] >> HALF_ACCURACY)*(tx3 >> HALF_ACCURACY)
            + (projectionMatrix[ 6] >> HALF_ACCURACY)*(ty3 >> HALF_ACCURACY)
            + (projectionMatrix[10] >> HALF_ACCURACY)*(tz3 >> HALF_ACCURACY)
            + (projectionMatrix[14] >> HALF_ACCURACY)*(tw3 >> HALF_ACCURACY);*/
  Sint32 w3 =/* (projectionMatrix[ 3] >> HALF_ACCURACY)*(tx3 >> HALF_ACCURACY)
            + (projectionMatrix[ 7] >> HALF_ACCURACY)*(ty3 >> HALF_ACCURACY)*/
            + (projectionMatrix[11] >> HALF_ACCURACY)*(tz3 >> HALF_ACCURACY)
            /*+ (projectionMatrix[15] >> HALF_ACCURACY)*(tw3 >> HALF_ACCURACY)*/;
      
  Sint32 nx3 = ((x3<<HALF_ACCURACY)/w3);
  Sint32 ny3 = ((y3<<HALF_ACCURACY)/w3);
  //Sint32 nz3 = ((z3<<HALF_ACCURACY)/w3);

         x4 = (projectionMatrix[ 0] >> HALF_ACCURACY)*(tx4 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 4] >> HALF_ACCURACY)*(ty4 >> HALF_ACCURACY)
            + (projectionMatrix[ 8] >> HALF_ACCURACY)*(tz4 >> HALF_ACCURACY)
            + (projectionMatrix[12] >> HALF_ACCURACY)*(tw4 >> HALF_ACCURACY)*/;
         y4 = //(projectionMatrix[ 1] >> HALF_ACCURACY)*(tx4 >> HALF_ACCURACY)
            + (projectionMatrix[ 5] >> HALF_ACCURACY)*(ty4 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 9] >> HALF_ACCURACY)*(tz4 >> HALF_ACCURACY)
            + (projectionMatrix[13] >> HALF_ACCURACY)*(tw4 >> HALF_ACCURACY)*/;
/*         z4 = (projectionMatrix[ 2] >> HALF_ACCURACY)*(tx4 >> HALF_ACCURACY)
            + (projectionMatrix[ 6] >> HALF_ACCURACY)*(ty4 >> HALF_ACCURACY)
            + (projectionMatrix[10] >> HALF_ACCURACY)*(tz4 >> HALF_ACCURACY)
            + (projectionMatrix[14] >> HALF_ACCURACY)*(tw4 >> HALF_ACCURACY);*/
  Sint32 w4 = /*(projectionMatrix[ 3] >> HALF_ACCURACY)*(tx4 >> HALF_ACCURACY)
            + (projectionMatrix[ 7] >> HALF_ACCURACY)*(ty4 >> HALF_ACCURACY)*/
            + (projectionMatrix[11] >> HALF_ACCURACY)*(tz4 >> HALF_ACCURACY)
            /*+ (projectionMatrix[15] >> HALF_ACCURACY)*(tw4 >> HALF_ACCURACY)*/;
      
  Sint32 nx4 = ((x4<<HALF_ACCURACY)/w4);
  Sint32 ny4 = ((y4<<HALF_ACCURACY)/w4);
  //Sint32 nz4 = ((z4<<HALF_ACCURACY)/w4);
 
  
  tdrawitem item;
  item.type=1;
  item.content.geo.z= -ol;//z1+ z2+ z3+ z4+(min4( z1, z2, z3, z4)>>1); //smallest z
  item.content.geo.color = engineGetLightingColor((tx1+tx2+tx3+tx4)>>2,
                                                  (ty1+ty2+ty3+ty4)>>2,
                                                  (tz1+tz2+tz3+tz4)>>2,normal,color);
  item.content.geo.x1=(engineWindowX >> 1)+((nx1*engineViewportX) >> ACCURACY);
  item.content.geo.y1=(engineWindowY >> 1)-((ny1*engineViewportY) >> ACCURACY);
  item.content.geo.x2=(engineWindowX >> 1)+((nx2*engineViewportX) >> ACCURACY);
  item.content.geo.y2=(engineWindowY >> 1)-((ny2*engineViewportY) >> ACCURACY);
  item.content.geo.x3=(engineWindowX >> 1)+((nx3*engineViewportX) >> ACCURACY);
  item.content.geo.y3=(engineWindowY >> 1)-((ny3*engineViewportY) >> ACCURACY);
  item.content.geo.x4=(engineWindowX >> 1)+((nx4*engineViewportX) >> ACCURACY);
  item.content.geo.y4=(engineWindowY >> 1)-((ny4*engineViewportY) >> ACCURACY);

  sortedInsert(item);
  /*quad(engineSurface,(engineWindowX >> 1)+(((nx1 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),(engineWindowY >> 1)-(((ny1 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),
                     (engineWindowX >> 1)+(((nx2 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),(engineWindowY >> 1)-(((ny2 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),
                     (engineWindowX >> 1)+(((nx3 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),(engineWindowY >> 1)-(((ny3 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),
                     (engineWindowX >> 1)+(((nx4 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),(engineWindowY >> 1)-(((ny4 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),color);*/
}

void engineTriangle(Sint32 x1,Sint32 y1,Sint32 z1,
                    Sint32 x2,Sint32 y2,Sint32 z2,
                    Sint32 x3,Sint32 y3,Sint32 z3,Uint16 color)
{   
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  Sint32* projectionMatrix=engineGetProjectionMatrix();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();
  Sint32 tx1 = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 ty1 = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tz1 = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[10] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tw1 = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[11] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);

  Sint32 tx2 = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(x2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(y2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(z2 >> HALF_ACCURACY)
             + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 ty2 = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(x2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(y2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(z2 >> HALF_ACCURACY)
             + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tz2 = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(x2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(y2 >> HALF_ACCURACY)
             + (modellViewMatrix[10] >> HALF_ACCURACY)*(z2 >> HALF_ACCURACY)
             + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tw2 = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(x2 >> HALF_ACCURACY)
             + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(y2 >> HALF_ACCURACY)
             + (modellViewMatrix[11] >> HALF_ACCURACY)*(z2 >> HALF_ACCURACY)
             + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);

  Sint32 tx3 = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(x3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(y3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(z3 >> HALF_ACCURACY)
             + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 ty3 = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(x3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(y3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(z3 >> HALF_ACCURACY)
             + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tz3 = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(x3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(y3 >> HALF_ACCURACY)
             + (modellViewMatrix[10] >> HALF_ACCURACY)*(z3 >> HALF_ACCURACY)
             + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tw3 = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(x3 >> HALF_ACCURACY)
             + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(y3 >> HALF_ACCURACY)
             + (modellViewMatrix[11] >> HALF_ACCURACY)*(z3 >> HALF_ACCURACY)
             + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);


  //Normale berechnen
  Sint32 normal[3];
  calcNormal(tx1,ty1,tz1,tx2,ty2,tz2,tx3,ty3,tz3,normal);
  //Sint32 l=(Sint32)(sqrt((float)normal[0]*(float)normal[0]+(float)normal[1]*(float)normal[1]+(float)normal[2]*(float)normal[2]));
  Sint32 l = fpsqrt((normal[0]>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)+
                    (normal[1]>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)+
                    (normal[2]>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY));
	if (l==0)
	  return;
  //Vector von Betrachter zu Fläche
  Sint32 ox = ((tx1+tx2+tx3)/3);
  Sint32 oy = ((ty1+ty2+ty3)/3);
  Sint32 oz = ((tz1+tz2+tz3)/3);
  //Sint32 ol = (Sint32)(sqrt((float)ox*(float)ox+(float)oy*(float)oy+(float)oz*(float)oz));
  Sint32 ol = fpsqrt((ox>>HALF_ACCURACY)*(ox>>HALF_ACCURACY)+
                     (oy>>HALF_ACCURACY)*(oy>>HALF_ACCURACY)+
                     (oz>>HALF_ACCURACY)*(oz>>HALF_ACCURACY));
  if (ol==0)
    return;
  
  ox=((ox<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
  oy=((oy<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
  oz=((oz<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
  normal[0]=((normal[0]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
  normal[1]=((normal[1]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
  normal[2]=((normal[2]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
  
  
  Sint32 ac = (ox>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)
             +(oy>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)
             +(oz>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY);
  
  
  /*Sint32 temp = ((l>>HALF_ACCURACY)*(ol >> HALF_ACCURACY));
  if (temp==0)
    return;
         ac = ((ac<<HALF_ACCURACY) / temp)<<HALF_ACCURACY;*/
  
  if (ac>0)
    return;

         x1 = (projectionMatrix[ 0] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 4] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            + (projectionMatrix[ 8] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[12] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
         y1 = //(projectionMatrix[ 1] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 5] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 9] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[13] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
         /*z1 = (projectionMatrix[ 2] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 6] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            + (projectionMatrix[10] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[14] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY);*/
  Sint32 w1 = /*(projectionMatrix[ 3] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 7] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)*/
            + (projectionMatrix[11] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[15] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
      
  Sint32 nx1 = ((x1<<HALF_ACCURACY)/w1);
  Sint32 ny1 = ((y1<<HALF_ACCURACY)/w1);
  //Sint32 nz1 = ((z1<<HALF_ACCURACY)/w1);

         x2 = (projectionMatrix[ 0] >> HALF_ACCURACY)*(tx2 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 4] >> HALF_ACCURACY)*(ty2 >> HALF_ACCURACY)
            + (projectionMatrix[ 8] >> HALF_ACCURACY)*(tz2 >> HALF_ACCURACY)
            + (projectionMatrix[12] >> HALF_ACCURACY)*(tw2 >> HALF_ACCURACY)*/;
         y2 = //(projectionMatrix[ 1] >> HALF_ACCURACY)*(tx2 >> HALF_ACCURACY)
            + (projectionMatrix[ 5] >> HALF_ACCURACY)*(ty2 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 9] >> HALF_ACCURACY)*(tz2 >> HALF_ACCURACY)
            + (projectionMatrix[13] >> HALF_ACCURACY)*(tw2 >> HALF_ACCURACY)*/;
         /*z2 = (projectionMatrix[ 2] >> HALF_ACCURACY)*(tx2 >> HALF_ACCURACY)
            + (projectionMatrix[ 6] >> HALF_ACCURACY)*(ty2 >> HALF_ACCURACY)
            + (projectionMatrix[10] >> HALF_ACCURACY)*(tz2 >> HALF_ACCURACY)
            + (projectionMatrix[14] >> HALF_ACCURACY)*(tw2 >> HALF_ACCURACY);*/
  Sint32 w2 = /*(projectionMatrix[ 3] >> HALF_ACCURACY)*(tx2 >> HALF_ACCURACY)
            + (projectionMatrix[ 7] >> HALF_ACCURACY)*(ty2 >> HALF_ACCURACY)*/
            + (projectionMatrix[11] >> HALF_ACCURACY)*(tz2 >> HALF_ACCURACY)
            /*+ (projectionMatrix[15] >> HALF_ACCURACY)*(tw2 >> HALF_ACCURACY)*/;
      
  Sint32 nx2 = ((x2<<HALF_ACCURACY)/w2);
  Sint32 ny2 = ((y2<<HALF_ACCURACY)/w2);
  //Sint32 nz2 = ((z2<<HALF_ACCURACY)/w2);

         x3 = (projectionMatrix[ 0] >> HALF_ACCURACY)*(tx3 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 4] >> HALF_ACCURACY)*(ty3 >> HALF_ACCURACY)
            + (projectionMatrix[ 8] >> HALF_ACCURACY)*(tz3 >> HALF_ACCURACY)
            + (projectionMatrix[12] >> HALF_ACCURACY)*(tw3 >> HALF_ACCURACY)*/;
         y3 = //(projectionMatrix[ 1] >> HALF_ACCURACY)*(tx3 >> HALF_ACCURACY)
            + (projectionMatrix[ 5] >> HALF_ACCURACY)*(ty3 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 9] >> HALF_ACCURACY)*(tz3 >> HALF_ACCURACY)
            + (projectionMatrix[13] >> HALF_ACCURACY)*(tw3 >> HALF_ACCURACY)*/;
/*         z3 = (projectionMatrix[ 2] >> HALF_ACCURACY)*(tx3 >> HALF_ACCURACY)
            + (projectionMatrix[ 6] >> HALF_ACCURACY)*(ty3 >> HALF_ACCURACY)
            + (projectionMatrix[10] >> HALF_ACCURACY)*(tz3 >> HALF_ACCURACY)
            + (projectionMatrix[14] >> HALF_ACCURACY)*(tw3 >> HALF_ACCURACY);*/
  Sint32 w3 =/* (projectionMatrix[ 3] >> HALF_ACCURACY)*(tx3 >> HALF_ACCURACY)
            + (projectionMatrix[ 7] >> HALF_ACCURACY)*(ty3 >> HALF_ACCURACY)*/
            + (projectionMatrix[11] >> HALF_ACCURACY)*(tz3 >> HALF_ACCURACY)
            /*+ (projectionMatrix[15] >> HALF_ACCURACY)*(tw3 >> HALF_ACCURACY)*/;
      
  Sint32 nx3 = ((x3<<HALF_ACCURACY)/w3);
  Sint32 ny3 = ((y3<<HALF_ACCURACY)/w3);
  //Sint32 nz3 = ((z3<<HALF_ACCURACY)/w3);
  
  tdrawitem item;
  item.type=0;
  item.content.geo.z=-ol;//min3(tz1,tz2,tz3); //smallest z
  item.content.geo.color = engineGetLightingColor((tx1+tx2+tx3)/3,
                                                  (ty1+ty2+ty3)/3,
                                                  (tz1+tz2+tz3)/3,normal,color);
  item.content.geo.x1=(engineWindowX >> 1)+((nx1*engineViewportX) >> ACCURACY);
  item.content.geo.y1=(engineWindowY >> 1)-((ny1*engineViewportY) >> ACCURACY);
  item.content.geo.x2=(engineWindowX >> 1)+((nx2*engineViewportX) >> ACCURACY);
  item.content.geo.y2=(engineWindowY >> 1)-((ny2*engineViewportY) >> ACCURACY);
  item.content.geo.x3=(engineWindowX >> 1)+((nx3*engineViewportX) >> ACCURACY);
  item.content.geo.y3=(engineWindowY >> 1)-((ny3*engineViewportY) >> ACCURACY);

  sortedInsert(item);
}

void engineDrawTextMXMY(Sint32 x1,Sint32 y1,Sint32 z1,char* text)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  Sint32* projectionMatrix=engineGetProjectionMatrix();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();
  
  Sint32 tx1 = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 ty1 = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tz1 = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[10] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tw1 = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[11] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);

         x1 = (projectionMatrix[ 0] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 4] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            + (projectionMatrix[ 8] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[12] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
         y1 = //(projectionMatrix[ 1] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 5] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 9] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[13] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
         /*z1 = (projectionMatrix[ 2] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 6] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            + (projectionMatrix[10] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[14] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY);*/
  Sint32 w1 =/* (projectionMatrix[ 3] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 7] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)*/
            + (projectionMatrix[11] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[15] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
      
  Sint32 nx1 = ((x1<<HALF_ACCURACY)/w1);
  Sint32 ny1 = ((y1<<HALF_ACCURACY)/w1);

  //Vector von Betrachter zu Fläche o=t1
  //Sint32 ol = (Sint32)(sqrt((float)ox*(float)ox+(float)oy*(float)oy+(float)oz*(float)oz));
  Sint32 ol = fpsqrt((tx1>>HALF_ACCURACY)*(tx1>>HALF_ACCURACY)+
                     (ty1>>HALF_ACCURACY)*(ty1>>HALF_ACCURACY)+
                     (tz1>>HALF_ACCURACY)*(tz1>>HALF_ACCURACY));
  if (ol==0)
    return;

  tdrawitem item;
  item.type=3;
  item.content.text.z=-ol;//tz1;
  item.content.text.color=65535; //just for compatibility
  item.content.text.x1=(engineWindowX >> 1)+((nx1*engineViewportX) >> ACCURACY);
  item.content.text.y1=(engineWindowY >> 1)-((ny1*engineViewportY) >> ACCURACY);
  item.content.text.text=(char*)malloc(strlen(text)+1);
  sprintf(item.content.text.text,"%s",text);
  sortedInsert(item);

  //drawtextMXMY(engineSurface,(engineWindowX >> 1)+(((nx1 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),(engineWindowY >> 1)-(((ny1 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),text,engineKeymap);
}

void engineDrawSurface(Sint32 x1,Sint32 y1,Sint32 z1,SDL_Surface* surface)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  Sint32* projectionMatrix=engineGetProjectionMatrix();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();
  
  Sint32 tx1 = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 ty1 = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tz1 = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[10] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tw1 = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[11] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);

         x1 = (projectionMatrix[ 0] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 4] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            + (projectionMatrix[ 8] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[12] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
         y1 = //(projectionMatrix[ 1] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 5] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 9] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[13] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
         /*z1 = (projectionMatrix[ 2] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 6] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            + (projectionMatrix[10] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[14] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY);*/
  Sint32 w1 =/* (projectionMatrix[ 3] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 7] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)*/
            + (projectionMatrix[11] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[15] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
      
  Sint32 nx1 = ((x1<<HALF_ACCURACY)/w1);
  Sint32 ny1 = ((y1<<HALF_ACCURACY)/w1);

  //Vector von Betrachter zu Fläche o=t1
  //Sint32 ol = (Sint32)(sqrt((float)ox*(float)ox+(float)oy*(float)oy+(float)oz*(float)oz));
  Sint32 ol = fpsqrt((tx1>>HALF_ACCURACY)*(tx1>>HALF_ACCURACY)+
                     (ty1>>HALF_ACCURACY)*(ty1>>HALF_ACCURACY)+
                     (tz1>>HALF_ACCURACY)*(tz1>>HALF_ACCURACY));
  if (ol==0)
    return;
                     
  tdrawitem item;
  item.type=2;
  item.content.surface.z=-ol;//tz1;
  item.content.surface.color=65535; //just for compatibility
  item.content.surface.x1=(engineWindowX >> 1)+((nx1*engineViewportX) >> ACCURACY);
  item.content.surface.y1=(engineWindowY >> 1)-((ny1*engineViewportY) >> ACCURACY);
  item.content.surface.surface=surface;
  sortedInsert(item);

  //drawtextMXMY(engineSurface,(engineWindowX >> 1)+(((nx1 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),(engineWindowY >> 1)-(((ny1 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),text,engineKeymap);
}

void engineEllipseAdd(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 rx,Sint32 ry,Uint16 color,Sint32 add)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  Sint32* projectionMatrix=engineGetProjectionMatrix();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();
  
  Sint32 tx1 = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 ty1 = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tz1 = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[10] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
  Sint32 tw1 = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(x1 >> HALF_ACCURACY)
             + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(y1 >> HALF_ACCURACY)
             + (modellViewMatrix[11] >> HALF_ACCURACY)*(z1 >> HALF_ACCURACY)
             + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);

/*  Sint32 trx = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(rx >> HALF_ACCURACY); //just scale
  Sint32 try = (modellViewMatrix[ 5] >> HALF_ACCURACY)*(ry >> HALF_ACCURACY); //just scale*/

         x1 = (projectionMatrix[ 0] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 4] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            + (projectionMatrix[ 8] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[12] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
         y1 = //(projectionMatrix[ 1] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 5] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[ 9] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[13] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;
         /*z1 = (projectionMatrix[ 2] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 6] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)
            + (projectionMatrix[10] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            + (projectionMatrix[14] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY);*/
  Sint32 w1 =/* (projectionMatrix[ 3] >> HALF_ACCURACY)*(tx1 >> HALF_ACCURACY)
            + (projectionMatrix[ 7] >> HALF_ACCURACY)*(ty1 >> HALF_ACCURACY)*/
            + (projectionMatrix[11] >> HALF_ACCURACY)*(tz1 >> HALF_ACCURACY)
            /*+ (projectionMatrix[15] >> HALF_ACCURACY)*(tw1 >> HALF_ACCURACY)*/;

/*         rx = (projectionMatrix[ 0] >> HALF_ACCURACY)*(trx >> HALF_ACCURACY);
         ry = (projectionMatrix[ 5] >> HALF_ACCURACY)*(try >> HALF_ACCURACY);*/
      
  Sint32 nx1 = ((x1<<HALF_ACCURACY)/w1);
  Sint32 ny1 = ((y1<<HALF_ACCURACY)/w1);
  Sint32 nrx = ((rx<<HALF_ACCURACY)/w1);
  Sint32 nry = ((ry<<HALF_ACCURACY)/w1);

  //Vector von Betrachter zu Fläche o=t1
  //Sint32 ol = (Sint32)(sqrt((float)ox*(float)ox+(float)oy*(float)oy+(float)oz*(float)oz));
  Sint32 ol = fpsqrt((tx1>>HALF_ACCURACY)*(tx1>>HALF_ACCURACY)+
                     (ty1>>HALF_ACCURACY)*(ty1>>HALF_ACCURACY)+
                     (tz1>>HALF_ACCURACY)*(tz1>>HALF_ACCURACY));
  if (ol==0)
    return;

  tdrawitem item;
  item.type=4;
  item.content.ellipse.z=-ol;//tz1;
  //Color
  Sint32 normal[3];
  normal[0]=0;
  normal[1]=0;
  normal[2]=1<<ACCURACY;
  item.content.ellipse.color = engineGetLightingColor(tx1,ty1,tz1,normal,color);
  item.content.ellipse.x1=(engineWindowX >> 1)+((nx1*engineViewportX) >> ACCURACY);
  item.content.ellipse.y1=(engineWindowY >> 1)-((ny1*engineViewportY) >> ACCURACY);
  item.content.ellipse.rx=((nrx*min(engineViewportX,engineViewportY)) >> (ACCURACY-1))+add;
  item.content.ellipse.ry=((nry*min(engineViewportX,engineViewportY)) >> (ACCURACY-1))+add;
  sortedInsert(item);

  //drawtextMXMY(engineSurface,(engineWindowX >> 1)+(((nx1 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),(engineWindowY >> 1)-(((ny1 >> HALF_ACCURACY)*(globalzoom >> HALF_ACCURACY)) >> ACCURACY),text,engineKeymap);
}


void engineEllipse(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 rx,Sint32 ry,Uint16 color)
{
  engineEllipseAdd(x1,y1,z1,rx,ry,color,0);
}

void engineDrawList()
{
  //Drawing the List
  int i=engineDrawlistCount-1;
  SDL_Rect dstrect;
  SDL_Rect srcrect;
  for (;i>=0;i--)
  switch (engineDrawlist[i].type)
  {
    case 0: //triangle
      triangle(engineSurface,engineDrawlist[i].content.geo.x1,engineDrawlist[i].content.geo.y1,
                             engineDrawlist[i].content.geo.x2,engineDrawlist[i].content.geo.y2,
                             engineDrawlist[i].content.geo.x3,engineDrawlist[i].content.geo.y3,engineDrawlist[i].content.all.color);
      break;
    case 1: //quad
      quad(engineSurface,engineDrawlist[i].content.geo.x1,engineDrawlist[i].content.geo.y1,
                         engineDrawlist[i].content.geo.x2,engineDrawlist[i].content.geo.y2,
                         engineDrawlist[i].content.geo.x3,engineDrawlist[i].content.geo.y3,
                         engineDrawlist[i].content.geo.x4,engineDrawlist[i].content.geo.y4,engineDrawlist[i].content.all.color);
      break;
    case 2: //Surface
      dstrect.x=engineDrawlist[i].content.all.x1-(engineDrawlist[i].content.surface.surface->w>>1);
      dstrect.y=engineDrawlist[i].content.all.y1-(engineDrawlist[i].content.surface.surface->h>>1);
      dstrect.w=engineDrawlist[i].content.surface.surface->w;
      /*if (dstrect.y+engineDrawlist[i].content.surface.surface->h>=engineWindowY-1) //bug
        dstrect.h=engineWindowY-1-dstrect.y;
      else*/
        dstrect.h=engineDrawlist[i].content.surface.surface->h;
      srcrect.x=0;
      srcrect.y=0;
      srcrect.w=dstrect.w;
      srcrect.h=dstrect.h;
        
      if (dstrect.h<=engineDrawlist[i].content.surface.surface->h) //overflow => unsigned!
        SDL_BlitSurface(engineDrawlist[i].content.surface.surface, &srcrect, engineSurface, &dstrect);
      break;
    case 3: //text
      drawtextMXMY(engineSurface,engineDrawlist[i].content.all.x1,engineDrawlist[i].content.all.y1,engineDrawlist[i].content.text.text,engineKeymap);
      free(engineDrawlist[i].content.text.text);
      break;
    case 4: //ellipse
      ellipse(engineSurface,engineDrawlist[i].content.all.x1    ,engineDrawlist[i].content.all.y1,
                            engineDrawlist[i].content.ellipse.rx,engineDrawlist[i].content.ellipse.ry,
                            engineDrawlist[i].content.all.color);
      break;
  }
  engineDrawlistCount=0;  
}

void engineAddWhiteLayer(int alpha)
{
  int i;
  Uint16* pixel=(Uint16*)(engineSurface->pixels);
  for (i=0;i<engineGetWindowX()*engineGetWindowY();i++)
  {
    int r=( pixel[i]>>11     )*8+alpha;
    if (r>255)
      r=255;
    if (r<0)
      r=0;
    int g=((pixel[i]>>5) & 63)*4+alpha;
    if (g>255)
      g=255;
    if (g<0)
      g=0;
    int b=( pixel[i]     & 31)*8+alpha;
    if (b>255)
      b=255;
    if (b<0)
      b=0;
    pixel[i]=getRGB(r,g,b);
  }
  
}

void engineAddBlackLayer(int alpha)
{
  int i;
  Uint16* pixel=(Uint16*)(engineSurface->pixels);
  for (i=0;i<engineGetWindowX()*engineGetWindowY();i++)
  {
    int r=( pixel[i]>>11     )*8-alpha;
    if (r>255)
      r=255;
    if (r<0)
      r=0;
    int g=((pixel[i]>>5) & 63)*4-alpha;
    if (g>255)
      g=255;
    if (g<0)
      g=0;
    int b=( pixel[i]     & 31)*8-alpha;
    if (b>255)
      b=255;
    if (b<0)
      b=0;
    pixel[i]=getRGB(r,g,b);
  }
  
}

void engineFlip()
{
  //The Flip
  #ifdef GP2X
    SDL_BlitSurface(engineSurface, NULL, engineScreen, NULL);
    /*SDL_LockSurface(engineScreen);
    Uint16* input = (Uint16*)(engineSurface->pixels);
    Uint16* output = (Uint16*)(engineScreen->pixels);
    int a;
    for (a=0;a<engineWindowX*engineWindowY;a++)
      output[a]=input[a];
    SDL_UnlockSurface(engineScreen);*/
    //SDL_Flip(engineScreen);
    //SDL_Flip(engineScreen);
  #elif defined CAANOO
    SDL_BlitSurface(engineSurface, NULL, engineScreen, NULL);
  #elif defined PANDORA
    /*int arg = 0;
    ioctl(fbdev, FBIO_WAITFORVSYNC, &arg);*/
    SDL_Flip(engineSurface);
  #else //PC, Dingoo and Pandora
    SDL_Flip(engineSurface);
  #endif
}

void draw_cube(Sint32 x,Sint32 y,Sint32 z,Uint16 color)
{
 //front
  engineQuad(-x,-y, z,
              x,-y, z,
              x, y, z,
             -x, y, z,color);
  //back
  engineQuad(-x, y,-z,
              x, y,-z,
              x,-y,-z,
             -x,-y,-z,color);
  //left
  engineQuad(-x,-y,-z,
             -x,-y, z,
             -x, y, z,
             -x, y,-z,color);
  //right
  engineQuad( x,-y, z,
              x,-y,-z,
              x, y,-z,
              x, y, z,color);
  //up
  engineQuad( x, y,-z,
             -x, y,-z,
             -x, y, z,
              x, y, z,color);
  //down
  engineQuad(-x,-y,-z,
              x,-y,-z,
              x,-y, z,
             -x,-y, z,color);
}

void engineList(ppoint verticies,const int vcount,pquad quads,const int qcount,ptriangle triangles,const int tcount,Uint16 color)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  Sint32* projectionMatrix=engineGetProjectionMatrix();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();

  //ModellView & Projection
  int i;
  for (i=0;i<vcount;i++)
  {
    verticies[i].tx = (modellViewMatrix[ 0] >> HALF_ACCURACY)*((verticies[i].x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 4] >> HALF_ACCURACY)*((verticies[i].y) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 8] >> HALF_ACCURACY)*((verticies[i].z) >> HALF_ACCURACY)
                    + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
    verticies[i].ty = (modellViewMatrix[ 1] >> HALF_ACCURACY)*((verticies[i].x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 5] >> HALF_ACCURACY)*((verticies[i].y) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 9] >> HALF_ACCURACY)*((verticies[i].z) >> HALF_ACCURACY)
                    + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
    verticies[i].tz = (modellViewMatrix[ 2] >> HALF_ACCURACY)*((verticies[i].x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 6] >> HALF_ACCURACY)*((verticies[i].y) >> HALF_ACCURACY)
                    + (modellViewMatrix[10] >> HALF_ACCURACY)*((verticies[i].z) >> HALF_ACCURACY)
                    + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
    verticies[i].tw = (modellViewMatrix[ 3] >> HALF_ACCURACY)*((verticies[i].x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 7] >> HALF_ACCURACY)*((verticies[i].y) >> HALF_ACCURACY)
                    + (modellViewMatrix[11] >> HALF_ACCURACY)*((verticies[i].z) >> HALF_ACCURACY)
                    + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);    

    Sint32 cx = (projectionMatrix[ 0] >> HALF_ACCURACY)*(verticies[i].tx >> HALF_ACCURACY);
    Sint32 cy = (projectionMatrix[ 5] >> HALF_ACCURACY)*(verticies[i].ty >> HALF_ACCURACY);
    Sint32 cw = (projectionMatrix[11] >> HALF_ACCURACY)*(verticies[i].tz >> HALF_ACCURACY);
    if (cw==0)
    {
      if (verticies[i].tz<0)
        cw=-1;
      else
        cw=1;
    }
    verticies[i].nx = ((cx<<HALF_ACCURACY)/cw);
    verticies[i].ny = ((cy<<HALF_ACCURACY)/cw);
  }

  //Quads  
  for (i=0;i<qcount;i++)
  {
    //Normale
    Sint32 normal[3];
    calcNormal(verticies[quads[i].p[0]].tx,verticies[quads[i].p[0]].ty,verticies[quads[i].p[0]].tz,
               verticies[quads[i].p[1]].tx,verticies[quads[i].p[1]].ty,verticies[quads[i].p[1]].tz,
               verticies[quads[i].p[2]].tx,verticies[quads[i].p[2]].ty,verticies[quads[i].p[2]].tz,normal);
    Sint32 l = fpsqrt((normal[0]>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)+
                      (normal[1]>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)+
                      (normal[2]>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY));
    if (l==0)
      continue;
    //Vector From Viewer to Quad
    Sint32 ox = ((verticies[quads[i].p[0]].tx+verticies[quads[i].p[1]].tx+verticies[quads[i].p[2]].tx+verticies[quads[i].p[3]].tx)>>2);
    Sint32 oy = ((verticies[quads[i].p[0]].ty+verticies[quads[i].p[1]].ty+verticies[quads[i].p[2]].ty+verticies[quads[i].p[3]].ty)>>2);
    Sint32 oz = ((verticies[quads[i].p[0]].tz+verticies[quads[i].p[1]].tz+verticies[quads[i].p[2]].tz+verticies[quads[i].p[3]].tz)>>2);
    Sint32 ol = fpsqrt((ox>>HALF_ACCURACY)*(ox>>HALF_ACCURACY)+
                       (oy>>HALF_ACCURACY)*(oy>>HALF_ACCURACY)+
                       (oz>>HALF_ACCURACY)*(oz>>HALF_ACCURACY));
    if (ol==0)
      continue;
    ox=((ox<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oy=((oy<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oz=((oz<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    normal[0]=((normal[0]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[1]=((normal[1]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[2]=((normal[2]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    
    Sint32 ac = (ox>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)
               +(oy>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)
               +(oz>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY);
  
    if (ac>0)
      continue;
          
    tdrawitem item;
    item.type=1;
    item.content.geo.z=-ol;
    item.content.geo.color = engineGetLightingColor(
      (verticies[quads[i].p[0]].tx+verticies[quads[i].p[1]].tx+verticies[quads[i].p[2]].tx+verticies[quads[i].p[3]].tx)>>2,
      (verticies[quads[i].p[0]].ty+verticies[quads[i].p[1]].ty+verticies[quads[i].p[2]].ty+verticies[quads[i].p[3]].ty)>>2,
      (verticies[quads[i].p[0]].tz+verticies[quads[i].p[1]].tz+verticies[quads[i].p[2]].tz+verticies[quads[i].p[3]].tz)>>2,      
      normal,color);
    item.content.geo.x1=(engineWindowX >> 1)+((verticies[quads[i].p[0]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y1=(engineWindowY >> 1)-((verticies[quads[i].p[0]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x2=(engineWindowX >> 1)+((verticies[quads[i].p[1]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y2=(engineWindowY >> 1)-((verticies[quads[i].p[1]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x3=(engineWindowX >> 1)+((verticies[quads[i].p[2]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y3=(engineWindowY >> 1)-((verticies[quads[i].p[2]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x4=(engineWindowX >> 1)+((verticies[quads[i].p[3]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y4=(engineWindowY >> 1)-((verticies[quads[i].p[3]].ny*engineViewportY) >> ACCURACY);

    sortedInsert(item);
  }
  
  //Triangles
  for (i=0;i<tcount;i++)
  {
    //Normale
    Sint32 normal[3];
    calcNormal(verticies[triangles[i].p[0]].tx,verticies[triangles[i].p[0]].ty,verticies[triangles[i].p[0]].tz,
               verticies[triangles[i].p[1]].tx,verticies[triangles[i].p[1]].ty,verticies[triangles[i].p[1]].tz,
               verticies[triangles[i].p[2]].tx,verticies[triangles[i].p[2]].ty,verticies[triangles[i].p[2]].tz,normal);
    Sint32 l = fpsqrt((normal[0]>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)+
                      (normal[1]>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)+
                      (normal[2]>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY));
    if (l==0)
      continue;
    //Vector From Viewer to Triangle
    Sint32 ox = ((verticies[triangles[i].p[0]].tx+verticies[triangles[i].p[1]].tx+verticies[triangles[i].p[2]].tx)/3);
    Sint32 oy = ((verticies[triangles[i].p[0]].ty+verticies[triangles[i].p[1]].ty+verticies[triangles[i].p[2]].ty)/3);
    Sint32 oz = ((verticies[triangles[i].p[0]].tz+verticies[triangles[i].p[1]].tz+verticies[triangles[i].p[2]].tz)/3);
    Sint32 ol = fpsqrt((ox>>HALF_ACCURACY)*(ox>>HALF_ACCURACY)+
                       (oy>>HALF_ACCURACY)*(oy>>HALF_ACCURACY)+
                       (oz>>HALF_ACCURACY)*(oz>>HALF_ACCURACY));
    if (ol==0)
      continue;
  
    ox=((ox<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oy=((oy<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oz=((oz<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    normal[0]=((normal[0]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[1]=((normal[1]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[2]=((normal[2]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    
    Sint32 ac = (ox>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)
               +(oy>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)
               +(oz>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY);
  
    if (ac>0)
      continue;
      
    tdrawitem item;
    item.type=0;
    item.content.geo.z=-ol;
    item.content.geo.color = engineGetLightingColor(
      (verticies[triangles[i].p[0]].tx+verticies[triangles[i].p[1]].tx+verticies[triangles[i].p[2]].tx)/3,
      (verticies[triangles[i].p[0]].ty+verticies[triangles[i].p[1]].ty+verticies[triangles[i].p[2]].ty)/3,
      (verticies[triangles[i].p[0]].tz+verticies[triangles[i].p[1]].tz+verticies[triangles[i].p[2]].tz)/3,      
      normal,color);
    item.content.geo.x1=(engineWindowX >> 1)+((verticies[triangles[i].p[0]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y1=(engineWindowY >> 1)-((verticies[triangles[i].p[0]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x2=(engineWindowX >> 1)+((verticies[triangles[i].p[1]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y2=(engineWindowY >> 1)-((verticies[triangles[i].p[1]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x3=(engineWindowX >> 1)+((verticies[triangles[i].p[2]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y3=(engineWindowY >> 1)-((verticies[triangles[i].p[2]].ny*engineViewportY) >> ACCURACY);

    sortedInsert(item);
  }
}

void engineListXYZ(Sint32 x,Sint32 y,Sint32 z,ppoint verticies,const int vcount,pquad quads,const int qcount,ptriangle triangles,const int tcount,Uint16 color)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  Sint32* projectionMatrix=engineGetProjectionMatrix();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();
  //ModellView & Projection
  int i;
  for (i=0;i<vcount;i++)
  {
    verticies[i].tx = (modellViewMatrix[ 0] >> HALF_ACCURACY)*((verticies[i].x+x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 4] >> HALF_ACCURACY)*((verticies[i].y+y) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 8] >> HALF_ACCURACY)*((verticies[i].z+z) >> HALF_ACCURACY)
                    + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
    verticies[i].ty = (modellViewMatrix[ 1] >> HALF_ACCURACY)*((verticies[i].x+x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 5] >> HALF_ACCURACY)*((verticies[i].y+y) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 9] >> HALF_ACCURACY)*((verticies[i].z+z) >> HALF_ACCURACY)
                    + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
    verticies[i].tz = (modellViewMatrix[ 2] >> HALF_ACCURACY)*((verticies[i].x+x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 6] >> HALF_ACCURACY)*((verticies[i].y+y) >> HALF_ACCURACY)
                    + (modellViewMatrix[10] >> HALF_ACCURACY)*((verticies[i].z+z) >> HALF_ACCURACY)
                    + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
    verticies[i].tw = (modellViewMatrix[ 3] >> HALF_ACCURACY)*((verticies[i].x+x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 7] >> HALF_ACCURACY)*((verticies[i].y+y) >> HALF_ACCURACY)
                    + (modellViewMatrix[11] >> HALF_ACCURACY)*((verticies[i].z+z) >> HALF_ACCURACY)
                    + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);    

    Sint32 cx = (projectionMatrix[ 0] >> HALF_ACCURACY)*(verticies[i].tx >> HALF_ACCURACY);
    Sint32 cy = (projectionMatrix[ 5] >> HALF_ACCURACY)*(verticies[i].ty >> HALF_ACCURACY);
    Sint32 cw = (projectionMatrix[11] >> HALF_ACCURACY)*(verticies[i].tz >> HALF_ACCURACY);
    if (cw==0)
    {
      if (verticies[i].tz<0)
        cw=-1;
      else
        cw=1;
    }
    verticies[i].nx = ((cx<<HALF_ACCURACY)/cw);
    verticies[i].ny = ((cy<<HALF_ACCURACY)/cw);
  }

  //Quads  
  for (i=0;i<qcount;i++)
  {
    //Normale
    Sint32 normal[3];
    calcNormal(verticies[quads[i].p[0]].tx,verticies[quads[i].p[0]].ty,verticies[quads[i].p[0]].tz,
               verticies[quads[i].p[1]].tx,verticies[quads[i].p[1]].ty,verticies[quads[i].p[1]].tz,
               verticies[quads[i].p[2]].tx,verticies[quads[i].p[2]].ty,verticies[quads[i].p[2]].tz,normal);
    Sint32 l = fpsqrt((normal[0]>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)+
                      (normal[1]>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)+
                      (normal[2]>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY));
    if (l==0)
      continue;
    //Vector From Viewer to Quad
    Sint32 ox = ((verticies[quads[i].p[0]].tx+verticies[quads[i].p[1]].tx+verticies[quads[i].p[2]].tx+verticies[quads[i].p[3]].tx)>>2);
    Sint32 oy = ((verticies[quads[i].p[0]].ty+verticies[quads[i].p[1]].ty+verticies[quads[i].p[2]].ty+verticies[quads[i].p[3]].ty)>>2);
    Sint32 oz = ((verticies[quads[i].p[0]].tz+verticies[quads[i].p[1]].tz+verticies[quads[i].p[2]].tz+verticies[quads[i].p[3]].tz)>>2);
    Sint32 ol = fpsqrt((ox>>HALF_ACCURACY)*(ox>>HALF_ACCURACY)+
                       (oy>>HALF_ACCURACY)*(oy>>HALF_ACCURACY)+
                       (oz>>HALF_ACCURACY)*(oz>>HALF_ACCURACY));
    if (ol==0)
      continue;
  
    ox=((ox<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oy=((oy<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oz=((oz<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    normal[0]=((normal[0]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[1]=((normal[1]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[2]=((normal[2]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    
    Sint32 ac = (ox>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)
               +(oy>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)
               +(oz>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY);
  
    if (ac>0)
      continue;
          
    tdrawitem item;
    item.type=1;
    item.content.geo.z=-ol;
    item.content.geo.color = engineGetLightingColor(
      (verticies[quads[i].p[0]].tx+verticies[quads[i].p[1]].tx+verticies[quads[i].p[2]].tx+verticies[quads[i].p[3]].tx)>>2,
      (verticies[quads[i].p[0]].ty+verticies[quads[i].p[1]].ty+verticies[quads[i].p[2]].ty+verticies[quads[i].p[3]].ty)>>2,
      (verticies[quads[i].p[0]].tz+verticies[quads[i].p[1]].tz+verticies[quads[i].p[2]].tz+verticies[quads[i].p[3]].tz)>>2,      
      normal,color);
    item.content.geo.x1=(engineWindowX >> 1)+((verticies[quads[i].p[0]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y1=(engineWindowY >> 1)-((verticies[quads[i].p[0]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x2=(engineWindowX >> 1)+((verticies[quads[i].p[1]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y2=(engineWindowY >> 1)-((verticies[quads[i].p[1]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x3=(engineWindowX >> 1)+((verticies[quads[i].p[2]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y3=(engineWindowY >> 1)-((verticies[quads[i].p[2]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x4=(engineWindowX >> 1)+((verticies[quads[i].p[3]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y4=(engineWindowY >> 1)-((verticies[quads[i].p[3]].ny*engineViewportY) >> ACCURACY);

    sortedInsert(item);
  }
  
  //Triangles
  for (i=0;i<tcount;i++)
  {
    //Normale
    Sint32 normal[3];
    calcNormal(verticies[triangles[i].p[0]].tx,verticies[triangles[i].p[0]].ty,verticies[triangles[i].p[0]].tz,
               verticies[triangles[i].p[1]].tx,verticies[triangles[i].p[1]].ty,verticies[triangles[i].p[1]].tz,
               verticies[triangles[i].p[2]].tx,verticies[triangles[i].p[2]].ty,verticies[triangles[i].p[2]].tz,normal);
    Sint32 l = fpsqrt((normal[0]>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)+
                      (normal[1]>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)+
                      (normal[2]>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY));
    if (l==0)
      continue;
    //Vector From Viewer to Triangle
    Sint32 ox = ((verticies[triangles[i].p[0]].tx+verticies[triangles[i].p[1]].tx+verticies[triangles[i].p[2]].tx)/3);
    Sint32 oy = ((verticies[triangles[i].p[0]].ty+verticies[triangles[i].p[1]].ty+verticies[triangles[i].p[2]].ty)/3);
    Sint32 oz = ((verticies[triangles[i].p[0]].tz+verticies[triangles[i].p[1]].tz+verticies[triangles[i].p[2]].tz)/3);
    Sint32 ol = fpsqrt((ox>>HALF_ACCURACY)*(ox>>HALF_ACCURACY)+
                       (oy>>HALF_ACCURACY)*(oy>>HALF_ACCURACY)+
                       (oz>>HALF_ACCURACY)*(oz>>HALF_ACCURACY));
    if (ol==0)
      continue;
  
    ox=((ox<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oy=((oy<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oz=((oz<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    normal[0]=((normal[0]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[1]=((normal[1]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[2]=((normal[2]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    
    Sint32 ac = (ox>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)
               +(oy>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)
               +(oz>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY);
  
    if (ac>0)
      continue;
      
    
    tdrawitem item;
    item.type=0;
    item.content.geo.z=-ol;
    item.content.geo.color = engineGetLightingColor(
      (verticies[triangles[i].p[0]].tx+verticies[triangles[i].p[1]].tx+verticies[triangles[i].p[2]].tx)/3,
      (verticies[triangles[i].p[0]].ty+verticies[triangles[i].p[1]].ty+verticies[triangles[i].p[2]].ty)/3,
      (verticies[triangles[i].p[0]].tz+verticies[triangles[i].p[1]].tz+verticies[triangles[i].p[2]].tz)/3,      
      normal,color);
    item.content.geo.x1=(engineWindowX >> 1)+((verticies[triangles[i].p[0]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y1=(engineWindowY >> 1)-((verticies[triangles[i].p[0]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x2=(engineWindowX >> 1)+((verticies[triangles[i].p[1]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y2=(engineWindowY >> 1)-((verticies[triangles[i].p[1]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x3=(engineWindowX >> 1)+((verticies[triangles[i].p[2]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y3=(engineWindowY >> 1)-((verticies[triangles[i].p[2]].ny*engineViewportY) >> ACCURACY);

    sortedInsert(item);
  }
}

void engineListXYZS(Sint32 x,Sint32 y,Sint32 z,Sint32 s,ppoint verticies,const int vcount,pquad quads,const int qcount,ptriangle triangles,const int tcount,Uint16 color)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  Sint32* projectionMatrix=engineGetProjectionMatrix();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();
  //ModellView & Projection
  int i;
  for (i=0;i<vcount;i++)
  {
    verticies[i].tx = (modellViewMatrix[ 0] >> HALF_ACCURACY)*(((verticies[i].x>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 4] >> HALF_ACCURACY)*(((verticies[i].y>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+y) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 8] >> HALF_ACCURACY)*(((verticies[i].z>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+z) >> HALF_ACCURACY)
                    + (modellViewMatrix[12]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
    verticies[i].ty = (modellViewMatrix[ 1] >> HALF_ACCURACY)*(((verticies[i].x>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 5] >> HALF_ACCURACY)*(((verticies[i].y>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+y) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 9] >> HALF_ACCURACY)*(((verticies[i].z>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+z) >> HALF_ACCURACY)
                    + (modellViewMatrix[13]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
    verticies[i].tz = (modellViewMatrix[ 2] >> HALF_ACCURACY)*(((verticies[i].x>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 6] >> HALF_ACCURACY)*(((verticies[i].y>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+y) >> HALF_ACCURACY)
                    + (modellViewMatrix[10] >> HALF_ACCURACY)*(((verticies[i].z>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+z) >> HALF_ACCURACY)
                    + (modellViewMatrix[14]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);
    verticies[i].tw = (modellViewMatrix[ 3] >> HALF_ACCURACY)*(((verticies[i].x>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+x) >> HALF_ACCURACY)
                    + (modellViewMatrix[ 7] >> HALF_ACCURACY)*(((verticies[i].y>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+y) >> HALF_ACCURACY)
                    + (modellViewMatrix[11] >> HALF_ACCURACY)*(((verticies[i].z>>HALF_ACCURACY)*(s>>HALF_ACCURACY)+z) >> HALF_ACCURACY)
                    + (modellViewMatrix[15]);// >> HALF_ACCURACY)*( 1 << HALF_ACCURACY);    

    Sint32 cx = (projectionMatrix[ 0] >> HALF_ACCURACY)*(verticies[i].tx >> HALF_ACCURACY);
    Sint32 cy = (projectionMatrix[ 5] >> HALF_ACCURACY)*(verticies[i].ty >> HALF_ACCURACY);
    Sint32 cw = (projectionMatrix[11] >> HALF_ACCURACY)*(verticies[i].tz >> HALF_ACCURACY);
    if (cw==0)
    {
      if (verticies[i].tz<0)
        cw=-1;
      else
        cw=1;
    }
    verticies[i].nx = ((cx<<HALF_ACCURACY)/cw);
    verticies[i].ny = ((cy<<HALF_ACCURACY)/cw);
  }

  //Quads  
  for (i=0;i<qcount;i++)
  {
    //Normale
    Sint32 normal[3];
    calcNormal(verticies[quads[i].p[0]].tx,verticies[quads[i].p[0]].ty,verticies[quads[i].p[0]].tz,
               verticies[quads[i].p[1]].tx,verticies[quads[i].p[1]].ty,verticies[quads[i].p[1]].tz,
               verticies[quads[i].p[2]].tx,verticies[quads[i].p[2]].ty,verticies[quads[i].p[2]].tz,normal);
    
    Sint32 l = fpsqrt((normal[0]>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)+
                      (normal[1]>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)+
                      (normal[2]>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY));
    if (l==0)
      continue;
    //Vector From Viewer to Quad
    Sint32 ox = ((verticies[quads[i].p[0]].tx+verticies[quads[i].p[1]].tx+verticies[quads[i].p[2]].tx+verticies[quads[i].p[3]].tx)>>2);
    Sint32 oy = ((verticies[quads[i].p[0]].ty+verticies[quads[i].p[1]].ty+verticies[quads[i].p[2]].ty+verticies[quads[i].p[3]].ty)>>2);
    Sint32 oz = ((verticies[quads[i].p[0]].tz+verticies[quads[i].p[1]].tz+verticies[quads[i].p[2]].tz+verticies[quads[i].p[3]].tz)>>2);
    Sint32 ol = fpsqrt((ox>>HALF_ACCURACY)*(ox>>HALF_ACCURACY)+
                       (oy>>HALF_ACCURACY)*(oy>>HALF_ACCURACY)+
                       (oz>>HALF_ACCURACY)*(oz>>HALF_ACCURACY));
    
    if (ol==0)
      continue;

    ox=((ox<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oy=((oy<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oz=((oz<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    normal[0]=((normal[0]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[1]=((normal[1]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[2]=((normal[2]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    
    Sint32 ac = (ox>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)
               +(oy>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)
               +(oz>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY);
  
    if (ac>0)
      continue;
      
    tdrawitem item;
    item.type=1;
    item.content.geo.z=-ol;
    item.content.geo.color = engineGetLightingColor(
      (verticies[quads[i].p[0]].tx+verticies[quads[i].p[1]].tx+verticies[quads[i].p[2]].tx+verticies[quads[i].p[3]].tx)>>2,
      (verticies[quads[i].p[0]].ty+verticies[quads[i].p[1]].ty+verticies[quads[i].p[2]].ty+verticies[quads[i].p[3]].ty)>>2,
      (verticies[quads[i].p[0]].tz+verticies[quads[i].p[1]].tz+verticies[quads[i].p[2]].tz+verticies[quads[i].p[3]].tz)>>2,      
      normal,color);
    item.content.geo.x1=(engineWindowX >> 1)+((verticies[quads[i].p[0]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y1=(engineWindowY >> 1)-((verticies[quads[i].p[0]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x2=(engineWindowX >> 1)+((verticies[quads[i].p[1]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y2=(engineWindowY >> 1)-((verticies[quads[i].p[1]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x3=(engineWindowX >> 1)+((verticies[quads[i].p[2]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y3=(engineWindowY >> 1)-((verticies[quads[i].p[2]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x4=(engineWindowX >> 1)+((verticies[quads[i].p[3]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y4=(engineWindowY >> 1)-((verticies[quads[i].p[3]].ny*engineViewportY) >> ACCURACY);

    sortedInsert(item);
  }
  
  //Triangles
  for (i=0;i<tcount;i++)
  {
    //Normale
    Sint32 normal[3];
    calcNormal(verticies[triangles[i].p[0]].tx,verticies[triangles[i].p[0]].ty,verticies[triangles[i].p[0]].tz,
               verticies[triangles[i].p[1]].tx,verticies[triangles[i].p[1]].ty,verticies[triangles[i].p[1]].tz,
               verticies[triangles[i].p[2]].tx,verticies[triangles[i].p[2]].ty,verticies[triangles[i].p[2]].tz,normal);
    Sint32 l = fpsqrt((normal[0]>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)+
                      (normal[1]>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)+
                      (normal[2]>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY));
    if (l==0)
      continue;
    //Vector From Viewer to Triangle
    Sint32 ox = ((verticies[triangles[i].p[0]].tx+verticies[triangles[i].p[1]].tx+verticies[triangles[i].p[2]].tx)/3);
    Sint32 oy = ((verticies[triangles[i].p[0]].ty+verticies[triangles[i].p[1]].ty+verticies[triangles[i].p[2]].ty)/3);
    Sint32 oz = ((verticies[triangles[i].p[0]].tz+verticies[triangles[i].p[1]].tz+verticies[triangles[i].p[2]].tz)/3);
    Sint32 ol = fpsqrt((ox>>HALF_ACCURACY)*(ox>>HALF_ACCURACY)+
                       (oy>>HALF_ACCURACY)*(oy>>HALF_ACCURACY)+
                       (oz>>HALF_ACCURACY)*(oz>>HALF_ACCURACY));
    if (ol==0)
      continue;
  
    ox=((ox<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oy=((oy<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    oz=((oz<<HALF_ACCURACY)/ol)<<HALF_ACCURACY;
    normal[0]=((normal[0]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[1]=((normal[1]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    normal[2]=((normal[2]<<HALF_ACCURACY)/l)<<HALF_ACCURACY;
    
    Sint32 ac = (ox>>HALF_ACCURACY)*(normal[0]>>HALF_ACCURACY)
               +(oy>>HALF_ACCURACY)*(normal[1]>>HALF_ACCURACY)
               +(oz>>HALF_ACCURACY)*(normal[2]>>HALF_ACCURACY);
  
    if (ac>0)
      continue;
          
    tdrawitem item;
    item.type=0;
    item.content.geo.z=-ol;
    item.content.geo.color = engineGetLightingColor(
      (verticies[triangles[i].p[0]].tx+verticies[triangles[i].p[1]].tx+verticies[triangles[i].p[2]].tx)/3,
      (verticies[triangles[i].p[0]].ty+verticies[triangles[i].p[1]].ty+verticies[triangles[i].p[2]].ty)/3,
      (verticies[triangles[i].p[0]].tz+verticies[triangles[i].p[1]].tz+verticies[triangles[i].p[2]].tz)/3,      
      normal,color);
    item.content.geo.x1=(engineWindowX >> 1)+((verticies[triangles[i].p[0]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y1=(engineWindowY >> 1)-((verticies[triangles[i].p[0]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x2=(engineWindowX >> 1)+((verticies[triangles[i].p[1]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y2=(engineWindowY >> 1)-((verticies[triangles[i].p[1]].ny*engineViewportY) >> ACCURACY);
    item.content.geo.x3=(engineWindowX >> 1)+((verticies[triangles[i].p[2]].nx*engineViewportX) >> ACCURACY);
    item.content.geo.y3=(engineWindowY >> 1)-((verticies[triangles[i].p[2]].ny*engineViewportY) >> ACCURACY);

    sortedInsert(item);
  }
}
