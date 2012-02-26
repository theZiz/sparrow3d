/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,     
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com                         
*/
#include "../sparrow3d/sparrow3d.h"
#include <SDL_image.h>

SDL_Surface* screen;
SDL_Surface* garfield;
SDL_Surface* pepper;
spModelPointer mesh;
Sint32 rotation = 0;
spFontPointer font = NULL;
int quality=0;
Uint32 fpssum = 0;
Sint32 divisor = -5000;
int test = 0;
int clear = 1;
int count;
int zTest = 1;
int zSet = 1;

void draw_test(void)
{
  spResetZBuffer();
  if (clear)
    spClearTarget(0);
  spIdentity();
  
  count = 0;
  
  spBindTexture(garfield);
  spSetLight(quality);
  spSetCulling(1);
  spSetZSet(zSet);
  spSetZTest(zTest);
  int i;

  switch (test)
  {
    case 9:
        //spRotozoomSurface(screen->w/2,screen->h/2,-1,garfield,spSin(rotation*4)+(3<<SP_ACCURACY-1),spCos(rotation*8)+(3<<SP_ACCURACY-1),rotation);
        spRotozoomSurface(screen->w/4,screen->h/2,-1,garfield,spSin(rotation*4)+(3<<SP_ACCURACY-1)>>2,spCos(rotation*8)+(3<<SP_ACCURACY-1)>>2,rotation);
        spRotozoomSurfacePart(3*screen->w/4,screen->h/2,-1,garfield,garfield->w/4,garfield->h/4,garfield->w/2,garfield->w/2,spSin(rotation*4)+(3<<SP_ACCURACY-1)>>1,spCos(rotation*8)+(3<<SP_ACCURACY-1)>>1,rotation);
      break;
    case 8:
      srand(0);
      for (i = 0;i<20;i++)
        spEllipseBorder(rand()%screen->w,rand()%screen->h,-1,rand()%screen->w/4,rand()%screen->h/4,10,20,rand()%65536);
      break;
    case 7:
      srand(0);
      for (i = 0;i<20;i++)
        spEllipse(rand()%screen->w,rand()%screen->h,-1,rand()%screen->w/4,rand()%screen->h/4,rand()%65536);
      break;
    case 6:
      srand(0);
      for (i = 0;i<20;i++)
        spRectangleBorder(rand()%screen->w,rand()%screen->h,-1,
                          rand()%screen->w/2,rand()%screen->h/2,12,6,rand()%65536);
      break;
    case 5:
      srand(0);
      for (i = 0;i<20;i++)
        spRectangle(rand()%screen->w,rand()%screen->h,-1,
                    rand()%screen->w/2,rand()%screen->h/2,rand()%65536);
      break;
    case 4:
      srand(0);
      for (i = 0;i<1000;i++)
        spLine(rand()%screen->w,rand()%screen->h,-1,
               rand()%screen->w,rand()%screen->h,-1,rand()%65536);
      break;
    case 3:
      spSetAlphaTest(0);
      spTranslate(0,0,-8<<SP_ACCURACY);
      spRotateX(rotation);
      spRotateY(rotation);
      spRotateZ(rotation);
      count = spMesh3D(mesh,1);
      /*spSetZTest(0);
      for (i = 0;i < mesh->edgeCount; i++)
        if (mesh->edge[i].status == 0)
          spLine3D(mesh->point[mesh->edge[i].point[0]].x,
                   mesh->point[mesh->edge[i].point[0]].y,
                   mesh->point[mesh->edge[i].point[0]].z,
                   mesh->point[mesh->edge[i].point[1]].x,
                   mesh->point[mesh->edge[i].point[1]].y,
                   mesh->point[mesh->edge[i].point[1]].z,0);
      for (i = 0;i < mesh->texEdgeCount; i++)
        if (mesh->texEdge[i].status == 0)
          spLine3D(mesh->texPoint[mesh->texEdge[i].point[0]].x,
                   mesh->texPoint[mesh->texEdge[i].point[0]].y,
                   mesh->texPoint[mesh->texEdge[i].point[0]].z,
                   mesh->texPoint[mesh->texEdge[i].point[1]].x,
                   mesh->texPoint[mesh->texEdge[i].point[1]].y,
                   mesh->texPoint[mesh->texEdge[i].point[1]].z,0);*/
      break;
    case 2:
      spSetAlphaTest(1);
      spTranslate(0,0,(-10<<SP_ACCURACY)+spSin(rotation*4)*4);  
      int x,y;
      for (x = -5; x <= 5; x++)
        for (y = -3; y<= 3; y++)
        {
          Sint32 matrix[16];
          memcpy(matrix,spGetMatrix(),16*sizeof(Sint32));
          spTranslate(x<<SP_ACCURACY+1,y<<SP_ACCURACY+1,0);
          if (x+y & 1)
          {
            spQuad3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 0,
                     -1<<SP_ACCURACY,-1<<SP_ACCURACY, 0,
                      1<<SP_ACCURACY,-1<<SP_ACCURACY, 0,
                      1<<SP_ACCURACY, 1<<SP_ACCURACY, 0,32767);
            //spQuadTex3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 0,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
            //            -1<<SP_ACCURACY,-1<<SP_ACCURACY, 0,1,garfield->h-SP_FONT_EXTRASPACE-1,
            //             1<<SP_ACCURACY,-1<<SP_ACCURACY, 0,garfield->w-SP_FONT_EXTRASPACE-1,garfield->h-SP_FONT_EXTRASPACE-1,
            //             1<<SP_ACCURACY, 1<<SP_ACCURACY, 0,garfield->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,65535);        
          }
          else
          {
            spQuad3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,
                     -1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,
                      1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,
                      1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,65535);
            //top
            if (y<0)
            spQuad3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,
                      1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,
                      1<<SP_ACCURACY, 1<<SP_ACCURACY, 0<<SP_ACCURACY,
                     -1<<SP_ACCURACY, 1<<SP_ACCURACY, 0<<SP_ACCURACY,12345);
            //bottom
            if (y>0)
            spQuad3D(-1<<SP_ACCURACY,-1<<SP_ACCURACY, 0<<SP_ACCURACY,
                      1<<SP_ACCURACY,-1<<SP_ACCURACY, 0<<SP_ACCURACY,
                      1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,
                     -1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,23456);
            //left
            if (x>0)
            spQuad3D(-1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,
                     -1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,
                     -1<<SP_ACCURACY, 1<<SP_ACCURACY, 0<<SP_ACCURACY,
                     -1<<SP_ACCURACY,-1<<SP_ACCURACY, 0<<SP_ACCURACY,34567);
            //right
            if (x<0)
            spQuad3D( 1<<SP_ACCURACY,-1<<SP_ACCURACY, 0<<SP_ACCURACY,
                      1<<SP_ACCURACY, 1<<SP_ACCURACY, 0<<SP_ACCURACY,
                      1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,
                      1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,45678);
          }
          memcpy(spGetMatrix(),matrix,16*sizeof(Sint32));      
        }
      break;
    case 1:
      spSetAlphaTest(1);
      spTranslate(0,0,-15<<SP_ACCURACY);  
      spRotateY(rotation);
      int a;
      for (a = 0; a<16; a++)
      {
        spRotateY(SP_PI/8);
        Sint32 brightness = (spCos(rotation+a*SP_PI/8)>>SP_HALF_ACCURACY)*abs(spCos(rotation+a*SP_PI/8)>>SP_HALF_ACCURACY)/2+(3<<SP_ACCURACY-1);
        Uint16 color = ((brightness>>SP_ACCURACY-4)<<11)+((brightness>>SP_ACCURACY-5)<<5)+(brightness>>SP_ACCURACY-4);
        for (y = -21; y<=21; y+=7)
        {
          if ((y+a) & 8)
            spQuadTex3D(-3<<SP_ACCURACY-2,y+3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
                        -3<<SP_ACCURACY-2,y-3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,1,garfield->h-SP_FONT_EXTRASPACE-1,
                         3<<SP_ACCURACY-2,y-3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,garfield->w-SP_FONT_EXTRASPACE-1,garfield->h-SP_FONT_EXTRASPACE-1,
                         3<<SP_ACCURACY-2,y+3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,garfield->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,color);
          
          else
          if ((y+a+1) & 8)
            spRectangle3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,3<<SP_ACCURACY-1,3<<SP_ACCURACY-1,SDL_GetTicks()/128);
          else
          if ((y+a+2) & 8)
            spEllipse3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,3<<SP_ACCURACY-2,3<<SP_ACCURACY-2,-SDL_GetTicks()/128);
          else
          if ((y+a+3) & 8)
            spRectangleBorder3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,3<<SP_ACCURACY-1,3<<SP_ACCURACY-1,1<<SP_ACCURACY-2,1<<SP_ACCURACY-2,SDL_GetTicks()/64);
          else
          if ((y+a+4) & 8)
            spEllipseBorder3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,3<<SP_ACCURACY-2,3<<SP_ACCURACY-2,1<<SP_ACCURACY-2,1<<SP_ACCURACY-2,-SDL_GetTicks()/64);
          else
          if ((y+a+5) & 8)
            //spBlit3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,pepper);
            spRotozoomSurface3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,pepper,spSin(rotation*4)+(3<<SP_ACCURACY-1),spCos(rotation*8)+(3<<SP_ACCURACY-1),rotation);
          else
            spQuad3D(-3<<SP_ACCURACY-2,y+3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,
                     -3<<SP_ACCURACY-2,y-3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,
                      3<<SP_ACCURACY-2,y-3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,
                      3<<SP_ACCURACY-2,y+3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,color);
          
        }
      }
      break;
    case 0: 
      spSetAlphaTest(0);
      spTranslate(spSin(rotation/3),spSin(rotation/5),(-7<<SP_ACCURACY));
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
      break;
  }
  spSetZSet(0);
  spSetZTest(0);
  spSetAlphaTest(1);
  if (quality)
    spFontDraw(0,screen->h-font->maxheight,-1,"Light",font);
  else
    spFontDraw(0,screen->h-font->maxheight,-1,"No Light",font);
  if (clear)
    spFontDraw(0,2,-1,"Do clear",font);
  else
    spFontDraw(0,2,-1,"Do not clear",font);
  char buffer[256];
  sprintf(buffer,"Z-Test %i",zTest);
  spFontDraw(0,2+font->maxheight,-1,buffer,font);
  sprintf(buffer,"Z-Set %i",zSet);
  spFontDraw(0,2+font->maxheight*2,-1,buffer,font);
  sprintf(buffer,"%i Faces",mesh->quadCount+mesh->triangleCount+mesh->texQuadCount+mesh->texTriangleCount);
  spFontDrawRight(screen->w-2,2,-1,buffer,font);
  sprintf(buffer,"%i Drawn",count);
  spFontDrawRight(screen->w-2,2+font->maxheight,-1,buffer,font);
  sprintf(buffer,"%02i:%02i",divisor/60000,(divisor/1000)%60);
  spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,-1,buffer,font);
  sprintf(buffer,"fps: %i",spGetFPS());
  spFontDrawRight(screen->w-2,screen->h-font->maxheight,-1,buffer,font);
  
  spFlip();
}


int calc_test(Uint32 steps)
{
  int i;
  for (i = 0;i<steps;i++)
  {
    divisor++;
    if (divisor>0)
      fpssum+=spGetFPS();
    //if (divisor == 60000)
      //return 1;
  }
  rotation+=steps*32;
  if (spGetInput()->button[SP_BUTTON_A])
  {
    spGetInput()->button[SP_BUTTON_A] = 0;
    quality = 1-quality;
  }
  if (spGetInput()->button[SP_BUTTON_X])
  {
    spGetInput()->button[SP_BUTTON_X] = 0;
    clear = 1-clear;
  }
  if (spGetInput()->button[SP_BUTTON_B])
  {
    spGetInput()->button[SP_BUTTON_B] = 0;
    test = (test+1)%10;
  }
  if (spGetInput()->button[SP_BUTTON_Y])
  {
    spGetInput()->button[SP_BUTTON_Y] = 0;
    int meow = zTest + 2*zSet;
    meow = (meow+1)%4;
    zTest = meow%2;
    zSet = meow/2;
  }
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
  
  //Setup
  screen = spCreateWindow();
  resize(screen->w,screen->h);
  
  //Textures loading
  garfield = spLoadSurface("./data/garfield.png");
  pepper = spLoadSurface("./data/pepper.png");
  spBindTexture(garfield);
  
  //Mesh loading
  mesh = spMeshLoadObj("./data/testmeshuv_tri.obj",garfield,65535);
  //mesh = spMeshLoadObj("./data/bamuv.obj",garfield,65535);
  //mesh = spMeshLoadObj("./data/foobar.obj",garfield,65535);
  
  //All glory the main loop
  spLoop(draw_test,calc_test,10,resize);
  
  //Winter Wrap up, Winter Wrap up …
  spFontDelete(font);
  spMeshDelete(mesh);
  SDL_FreeSurface(garfield);
  SDL_FreeSurface(pepper);
  spQuitCore();
  printf("Average fps: %.1f\n",(float)fpssum/(float)divisor);
  return 0;
}
