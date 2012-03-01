#include "../sparrow3d/sparrow3d.h"
#include <SDL_image.h>

SDL_Surface* screen;
SDL_Surface* tile_map;
spSpritePointer sprite;
Sint32 rotation = 0;

void draw_function(void)
{
  //Cleaning the target (in this case the screen surface) with 0 (black).
  //In this case I don't clean the zBuffer, because I don't use it. ;-)
  //But you ARE able to use it, if you want!
  spResetZBuffer();
  spClearTarget(34567);
  
  //mark the center
  spEllipseBorder(screen->w/2,screen->h/2,-100,6,6,2,2,0);
  
  //Set the origin:
  switch ((rotation>>17)%9)
  {
    case 0:
      spSetHorizontalOrigin(SP_CENTER);
      spSetVerticalOrigin(SP_CENTER);
      break;
    case 1:
      spSetHorizontalOrigin(SP_LEFT);
      spSetVerticalOrigin(SP_CENTER);
      break;
    case 2:
      spSetHorizontalOrigin(SP_LEFT);
      spSetVerticalOrigin(SP_TOP);
      break;
    case 3:
      spSetHorizontalOrigin(SP_CENTER);
      spSetVerticalOrigin(SP_TOP);
      break;
    case 4:
      spSetHorizontalOrigin(SP_RIGHT);
      spSetVerticalOrigin(SP_TOP);
      break;
    case 5:
      spSetHorizontalOrigin(SP_RIGHT);
      spSetVerticalOrigin(SP_CENTER);
      break;
    case 6:
      spSetHorizontalOrigin(SP_RIGHT);
      spSetVerticalOrigin(SP_BOTTOM);
      break;
    case 7:
      spSetHorizontalOrigin(SP_CENTER);
      spSetVerticalOrigin(SP_BOTTOM);
      break;
    case 8:
      spSetHorizontalOrigin(SP_LEFT);
      spSetVerticalOrigin(SP_BOTTOM);
      break;      
  }
  
  //Drawing the sprite in the middle of the screen
  spDrawSprite(screen->w/2,screen->h/2,-1,sprite);
  spDrawSprite(screen->w-1,screen->h-1,-1,sprite);
  spDrawSprite(screen->w-1,0,-1,sprite);
  spDrawSprite(0,screen->h-1,-1,sprite);
  spDrawSprite(0,0,-1,sprite);

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
    
  //Update the sprite (next picture)
  spUpdateSprite(sprite,steps);
  //rotozoom the sprite (without rotozoom it's faster):
  sprite->zoomX = spSin(rotation*8)/2+(3<<SP_ACCURACY-1);
  sprite->zoomY = spCos(rotation*6)/2+(3<<SP_ACCURACY-1);
  sprite->rotation = rotation*4;
  
  return 0; 
}

void resize(Uint16 w,Uint16 h)
{
  //Selecting the renderTarget. It could be every surface
  spSelectRenderTarget(spGetWindowSurface());
}

int main(int argc, char **argv)
{
  //sparrow3D Init
  spInitCore();
  
  //Setup
  screen = spCreateWindow();
  resize(screen->w,screen->h);
  
  //Tile map loading
  tile_map = spLoadSurface("./data/science_guy_frames01.png");
  
  //Creating an empty sprite
  sprite = spNewSprite();
  //Filling it with it subsprites.
  int i;
  for (i = 0;i<9;i++)
    spNewSubSpriteWithTiling(sprite,tile_map,i*24+1,1,22,46,100);

  //We don't want to use the zBuffer in any way
  
  //All glory the main loop
  //every frame the draw_function is called
  //every frame the calc_function is called with the past time as argument
  //at least 10 ms have to be between to frames (max 100 fps)
  //if the window is resized, the resize feedback-function is called (again)
  spLoop(draw_function,calc_function,10,resize);
  
  //Winter Wrap up, Winter Wrap up
  spDeleteSprite(sprite);
  SDL_FreeSurface(tile_map);
  spQuitCore();
  return 0;
}
