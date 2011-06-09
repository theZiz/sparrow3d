#ifndef _3DENGINE_H
#define _3DENGINE_H

#include "graphicstuff.h"

/*#include <linux/fb.h>
#ifndef FBIO_WAITFORVSYNC
  #define FBIO_WAITFORVSYNC _IOW('F', 0x20, __u32)
#endif*/
#ifdef PANDORA
  #include <pnd_io_evdev.h>
  //int fbdev;
#endif

#define SURFACE_SURFACE 0
#define SURFACE_SCREEN 1
#define SURFACE_KEYMAP 2
#define SURFACE_KEYMAP_BLUE 3
#define SURFACE_KEYMAP_GREEN 4
#define SURFACE_KEYMAP_RED 5

#define KEYMAP_MAX 32
#define JOYSTICK_MIN -16384
#define JOYSTICK_MAX  16383
#define DRAWLIST_LENGTH 2048

#ifdef GP2X
  #define AXIS_UP 0
  #define AXIS_LEFTUP 1
  #define AXIS_LEFT 2
  #define AXIS_LEFTDOWN 3
  #define AXIS_DOWN 4
  #define AXIS_RIGHTDOWN 5
  #define AXIS_RIGHT 6
  #define AXIS_RIGHTUP 7
  #define BUTTON_START 8
  #define BUTTON_SELECT 9
  #define BUTTON_L 10
  #define BUTTON_R 11
  #define BUTTON_A 12
  #define BUTTON_B 13
  #define BUTTON_X 14
  #define BUTTON_Y 15
  #define BUTTON_VOLPLUS 16
  #define BUTTON_VOLMINUS 17
  #define BUTTON_TIP 18
  //just for different practices
  #define PRACTICE_OK BUTTON_B
#elif defined CAANOO
  #define BUTTON_A 0
  #define BUTTON_X 1
  #define BUTTON_B 2
  #define BUTTON_Y 3
  #define BUTTON_L 4
  #define BUTTON_R 5
  #define BUTTON_START 6
  #define BUTTON_TIP 7
  #define BUTTON_SELECT 7
  #define BUTTON_VOLPLUS 8
  #define BUTTON_VOLMINUS 9
  #define PRACTICE_OK BUTTON_B
#else //PC and DINGOO and PANDORA
  #define BUTTON_START 8
  #define BUTTON_SELECT 9
  #define BUTTON_L 10
  #define BUTTON_R 11
  #define BUTTON_A 12
  #define BUTTON_B 13
  #define BUTTON_X 14
  #define BUTTON_Y 15
  #define BUTTON_VOLPLUS 16
  #define BUTTON_VOLMINUS 17
  #define BUTTON_TIP 18
  #define PRACTICE_OK BUTTON_A
#endif

#ifdef GP2X
  #define BUTTON_START_NAME "Start"
  #define BUTTON_SELECT_NAME "Select"
  #define BUTTON_A_NAME "A"
  #define BUTTON_B_NAME "B"
  #define BUTTON_X_NAME "X"
  #define BUTTON_Y_NAME "Y"
  #define PAD_NAME "D-Pad"
  #define PRACTICE_OK_NAME BUTTON_B_NAME
#elif defined CAANOO
  #define BUTTON_START_NAME "Start"
  #define BUTTON_SELECT_NAME "Select"
  #define BUTTON_A_NAME "A"
  #define BUTTON_B_NAME "B"
  #define BUTTON_X_NAME "X"
  #define BUTTON_Y_NAME "Y"
  #define PAD_NAME "Stick"
  #define PRACTICE_OK_NAME BUTTON_B_NAME
#elif defined DINGOO
  #define BUTTON_START_NAME "Start"
  #define BUTTON_SELECT_NAME "Select"
  #define BUTTON_A_NAME "A"
  #define BUTTON_B_NAME "B"
  #define BUTTON_X_NAME "X"
  #define BUTTON_Y_NAME "Y"
  #define PAD_NAME "D-Pad"
  #define PRACTICE_OK_NAME BUTTON_A_NAME  
#elif defined PANDORA
  #define BUTTON_START_NAME "Start"
  #define BUTTON_SELECT_NAME "Select"
  #define BUTTON_A_NAME "A"
  #define BUTTON_B_NAME "B"
  #define BUTTON_X_NAME "X"
  #define BUTTON_Y_NAME "Y"
  #define PAD_NAME "D-Pad"
  #define PRACTICE_OK_NAME BUTTON_A_NAME  
#else
  #define BUTTON_START_NAME "Return"
  #define BUTTON_SELECT_NAME "Backspace"
  #define BUTTON_A_NAME "a"
  #define BUTTON_B_NAME "d"
  #define BUTTON_X_NAME "s"
  #define BUTTON_Y_NAME "w"
  #define PAD_NAME "Arrow Keys"
  #define PRACTICE_OK_NAME BUTTON_A_NAME  
#endif


typedef struct sgeoitem *pgeoitem;
typedef struct sgeoitem {
  Sint32 z;
  Uint16 color;
  Sint16 x1,y1,x2,y2,x3,y3,x4,y4;
} tgeoitem;

typedef struct sallitem *pallitem;
typedef struct sallitem {
  Sint32 z;
  Uint16 color;
  Sint16 x1,y1;
} tallitem;

typedef struct ssurfaceitem *psurfaceitem;
typedef struct ssurfaceitem {
  Sint32 z;
  Uint16 color; //just for compatibility
  Sint16 x1,y1;
  SDL_Surface* surface;
} tsurfaceitem;

typedef struct stextitem *ptextitem;
typedef struct stextitem {
  Sint32 z;
  Uint16 color;
  Sint16 x1,y1; //middle point
  char* text;
} ttextitem;

typedef struct sellipseitem *pellipseitem;
typedef struct sellipseitem {
  Sint32 z;
  Uint16 color;
  Sint16 x1,y1; //middle point
  Sint16 rx,ry;
} tellipseitem;

typedef struct sdrawitem *pdrawitem;
typedef struct sdrawitem {
  char type; //0 triangle, 1 quad, 2 surface, 3 text, 4 Ellipse
  union {
    tallitem all;
    tgeoitem geo;
    ttextitem text;
    tsurfaceitem surface;
    tellipseitem ellipse;
  } content;
} tdrawitem;

typedef struct spoint *ppoint;
typedef struct spoint {
  Sint32 x,y,z;
  Sint32 tx,ty,tz,tw;
  Sint32 nx,ny;
} tpoint;

typedef struct squad *pquad;
typedef struct squad {
  Uint32 p[4];
} tquad;

typedef struct striangle *ptriangle;
typedef struct striangle {
  Uint32 p[3];
} ttriangle;

typedef struct sEngineInput *pEngineInput;
typedef struct sEngineInput {
  signed char axis[2];
  char button[20];
} tEngineInput;

typedef struct slight *plight;
typedef struct slight {
  Sint32 x,y,z;
  Sint32 r,g,b;
  char active;
} tlight;

void engineLight(char on);

void engineScale(Sint32 x,Sint32 y,Sint32 z);

void engineSetAmbient(Sint32 r,Sint32 g,Sint32 b);

plight engineGetLightPointer();

Uint32 engineGetFps();

void engineSetMuteKey(int key);

int engineGetMuteKey();

pEngineInput engineGetInput();

void engineSetGlobalDone();

SDL_Surface* engineGetSurface(int nr);

Sint32 min(Sint32 a,Sint32 b);

Sint32 min4(Sint32 a,Sint32 b,Sint32 c,Sint32 d);

Sint32 min3(Sint32 a,Sint32 b,Sint32 c);

Sint32 max(Sint32 a,Sint32 b);

void calcNormal(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 x2,Sint32 y2,Sint32 z2,
                Sint32 x3,Sint32 y3,Sint32 z3,Sint32* normale);

#ifdef ARMCPU
int fpdiv(register int numerator, register int denominator);
#endif

void loadKeyMap();

void resizeWindow(int x,int y);

void initEngine();

void engineHandleEvent(void);

signed char engineGetAxis(int axis);

int engineLoop(void (*engineDraw)(void),int (*engineCalc)(Uint32 steps),Uint32 minwait);

void quitEngine(void);

void engineClearScreen(Uint16 color);

void engineRotate(Sint32 x,Sint32 y,Sint32 z,Sint32 rad);

void engineTranslate(Sint32 x,Sint32 y,Sint32 z);

int sortedInsert(tdrawitem item);

void engineQuad(Sint32 x1,Sint32 y1,Sint32 z1,
                Sint32 x2,Sint32 y2,Sint32 z2,
                Sint32 x3,Sint32 y3,Sint32 z3,
                Sint32 x4,Sint32 y4,Sint32 z4,Uint16 color);

void engineTriangle(Sint32 x1,Sint32 y1,Sint32 z1,
                    Sint32 x2,Sint32 y2,Sint32 z2,
                    Sint32 x3,Sint32 y3,Sint32 z3,Uint16 color);

void engineDrawTextMXMY(Sint32 x1,Sint32 y1,Sint32 z1,char* text);

void engineDrawSurface(Sint32 x1,Sint32 y1,Sint32 z1,SDL_Surface* surface);

void engineEllipseAdd(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 rx,Sint32 ry,Uint16 color,Sint32 add);

void engineEllipse(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 rx,Sint32 ry,Uint16 color);

void engineDrawList();

void engineAddWhiteLayer(int alpha);

void engineAddBlackLayer(int alpha);

void engineFlip();

void draw_cube(Sint32 x,Sint32 y,Sint32 z,Uint16 color);

void engineList(ppoint verticies,const int vcount,pquad quads,const int qcount,ptriangle triangles,const int tcount,Uint16 color);

void engineListXYZ(Sint32 x,Sint32 y,Sint32 z,ppoint verticies,const int vcount,pquad quads,const int qcount,ptriangle triangles,const int tcount,Uint16 color);

void engineListXYZS(Sint32 x,Sint32 y,Sint32 z,Sint32 s,ppoint verticies,const int vcount,pquad quads,const int qcount,ptriangle triangles,const int tcount,Uint16 color);

#endif
