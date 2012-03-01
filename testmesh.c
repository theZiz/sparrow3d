#include "../sparrow3d/sparrow3d.h"
#include <SDL_image.h>

SDL_Surface* screen;
SDL_Surface* texture;
spModelPointer mesh;
Sint32 rotation = 0;

void draw_function(void)
{
  //Reseting Z Buffer to the maximal negative value
  spResetZBuffer();
  //Cleaning the target (in this case the screen surface) with 0 (black).
  spClearTarget(0);
  //Reseting the ModelViewMatrix to an identity matrix.
  spIdentity();
  
  //Binding the texture
  spBindTexture(texture);
  
  //Going to the "position" -8.0f
  spTranslate(0,0,-8<<SP_ACCURACY);
  //rotation aorund every axis
  spRotateX(rotation);
  spRotateY(rotation);
  spRotateZ(rotation);
  //Drawing the mesh and no update of the edgelist 
  spMesh3D(mesh,0);
  //Show it!
  spFlip();
}


int calc_function(Uint32 steps)
{
  //update rotation counter
  rotation+=steps*32;
  //if "Start" (or "Menu" for Wiz or "Home" for Caanoo or "Return" at the P) is
  //pressed: exit
  if (spGetInput()->button[SP_BUTTON_START])
    return 1;
  return 0; 
}

void resize(Uint16 w,Uint16 h)
{
  //Selecting the renderTarget. It could be every surface
  spSelectRenderTarget(spGetWindowSurface());
  //Setup of the new/resized window
  spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,0.1,100);
}


int main(int argc, char **argv)
{
  //sparrow3D Init
  spInitCore();
  
  //Setup
  screen = spCreateWindow();
  resize(screen->w,screen->h);
  
  //Textures loading
  texture = spLoadSurface("./data/garfield.png");
  //Setting alpha test to 0, because the texture has pink parts
  spSetAlphaTest(0);
  //Mesh loading
  mesh = spMeshLoadObj("./data/bamuv.obj",texture,65535);
    
  //Light on! There is one "default" light at 0,0,0 with color white. We just use it.
  spSetLight(1);
  
  //All glory the main loop
  //every frame the draw_function is called
  //every frame the calc_function is called with the past time as argument
  //at least 10 ms have to be between to frames (max 100 fps)
  //if the window is resized, the resize feedback-function is called (again)
  spLoop(draw_function,calc_function,10,resize);
  
  //Winter Wrap up, Winter Wrap up
  spMeshDelete(mesh);
  SDL_FreeSurface(texture);
  spQuitCore();
  return 0;
}
