#include "3dengine.h"
#include "meshloader.h"

pmesh mesh;
pmesh sphere;
SDL_Surface *pepper;
Sint32 w=0;

char pause=0;

void draw_test(void)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();

  engineClearScreen(getRGB(63,63,63));
  setModellViewMatrixIdentity();

  modellViewMatrix[14]=(-7<<ACCURACY);
    
  Sint32 matrix[16];
  
  /*int x,y;
  for (x=-2;x<=2;x++)
    for (y=-2;y<=2;y++)
    {
      memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);

      modellViewMatrix[12]=x*3<<ACCURACY;
      modellViewMatrix[13]=y*3<<ACCURACY;
      
      //engineRotate( mysin(w)*2,2<<ACCURACY,-3<<ACCURACY,w);
      engineRotate(0,-1,0,w);
      draw_cube(1<<ACCURACY,1<<ACCURACY,1<<ACCURACY,getRGB(255,255,255));

      memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
    }*/
  engineRotate(-1<<ACCURACY,-2<<ACCURACY,-3<<ACCURACY,w);    
  
  //int a=SDL_GetTicks();
  drawMesh(mesh,65535);//getRGB(((1<<ACCURACY)+mysin(w))>>(ACCURACY-7),((1<<ACCURACY)+mysin(w+MY_PI))>>(ACCURACY-7),((1<<ACCURACY)+mycos(w))>>(ACCURACY-7)));
  
  drawMeshXYZ(0,0, 4<<ACCURACY,sphere,getRGB(0,192,0));
  engineEllipse(0,0,-4<<ACCURACY,1<<ACCURACY,1<<ACCURACY,getRGB(192,0,0));
  engineEllipse(0, 4<<ACCURACY,0,1<<ACCURACY,1<<ACCURACY,getRGB(0,0,192));
  engineDrawSurface(0,-4<<ACCURACY,0,pepper);
  engineDrawSurface( 4<<ACCURACY,0,0,pepper);
  drawMeshXYZ(-4<<ACCURACY,0,0,sphere,getRGB(192,192,0));

  //int b=SDL_GetTicks();
  //printf("Mesh: %i ms\n",b-a);
  engineDrawList();

  drawtextMXMY(engineGetSurface(SURFACE_SURFACE),engineWindowX/2,engineWindowY-(engineGetSurface(SURFACE_KEYMAP)->h>>4),"Press "PRACTICE_OK_NAME" to quit",engineGetSurface(SURFACE_KEYMAP));
  char buffer[16];
  sprintf(buffer,"%i",engineGetFps());
  drawtextMY(engineGetSurface(SURFACE_SURFACE),0,engineWindowY-(engineGetSurface(SURFACE_KEYMAP)->h>>4),buffer,engineGetSurface(SURFACE_KEYMAP));

  //a=SDL_GetTicks();
  engineFlip();
  //b=SDL_GetTicks();
  //printf("Flip: %i ms\n",b-a);

}

int calc_test(Uint32 steps)
{
  pEngineInput engineInput = engineGetInput();
  if (engineInput->button[BUTTON_START])
  {
    pause=!pause;
    engineInput->button[BUTTON_START]=0;
  }
  if (!pause)
    w+=(steps*16)%(2*MY_PI);
  if (engineInput->button[PRACTICE_OK])
    return 1;
  return 0; 
}

int main(int argc, char **argv)
{
  mesh=loadMesh("./data/testmesh.obj");
  sphere=loadMesh("./data/sphere.obj");
  initEngine();
  plight light=engineGetLightPointer();
  light[0].active=1;
  light[0].x=-2<<ACCURACY;
  light[0].r=0;
  light[0].g=0;
  light[1].active=1;
  light[1].x=+2<<ACCURACY;
  light[1].r=0;
  light[1].b=0;
  pepper = IMG_Load("./data/pepper.png");
  engineLoop(draw_test,calc_test,10); //max 100 fps, wenn kein vsync vorhanden
  SDL_FreeSurface(pepper);
  quitEngine();  
  freeMesh(mesh);
  freeMesh(sphere);
  return 0;
}
