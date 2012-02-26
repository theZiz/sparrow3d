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
#include "sparrowSprite.h"

PREFIX spSpritePointer spNewSprite()
{
  spSpritePointer sprite = (spSpritePointer)malloc(sizeof(spSprite));
  sprite->wholeDuration = 0;
  sprite->wholeAge = 0;
  sprite->maxWidth = 0;
  sprite->maxHeight = 0;
  sprite->rotation = 0;
  sprite->zoomX = 1<<SP_ACCURACY;
  sprite->zoomY = 1<<SP_ACCURACY;
  sprite->firstSub = NULL;
  sprite->momSub = NULL;  
  return sprite;
}

PREFIX void spDeleteSprite(spSpritePointer sprite)
{
  if (sprite->firstSub == NULL)
    return;
  spSubSpritePointer momSub = sprite->firstSub;
  do 
  {
    spSubSpritePointer next = momSub->next;
    free(momSub);
    momSub = next;
  }
  while (momSub != sprite->firstSub);
  free(sprite);
}

PREFIX spSubSpritePointer spNewSubSpriteNoTiling(spSpritePointer sprite,SDL_Surface* surface,Sint32 duration)
{
  spNewSubSpriteWithTiling(sprite,surface,-1,-1,surface->w,surface->h,duration);
}

PREFIX spSubSpritePointer spNewSubSpriteWithTiling(spSpritePointer sprite,SDL_Surface* surface,Sint32 sx,Sint32 sy,Sint32 sw,Sint32 sh,Sint32 duration)
{
  spSubSpritePointer sub = (spSubSpritePointer)malloc(sizeof(spSubSprite));
  sub->surface = surface;
  sub->sx = sx;
  sub->sy = sy;
  sub->sw = sw;
  sub->sh = sh;
  sub->duration = duration;
  sub->age = 0;
  
  sprite->wholeDuration += duration;
  if (sw > sprite->maxWidth)
    sprite->maxWidth = sw;
  if (sh > sprite->maxHeight)
    sprite->maxHeight = sh;
  
  if (sprite->firstSub)
  {
    sub->next = sprite->firstSub;
    sub->before = sprite->firstSub->before;
    sprite->firstSub->before->next = sub;
    sprite->firstSub->before = sub;
  }
  else
  {
    sub->next = sub;
    sub->before = sub;
    sprite->firstSub = sub;
    sprite->momSub = sub;
  }
}

PREFIX void spUpdateSprite(spSpritePointer sprite,Sint32 time)
{
  if (sprite->momSub == NULL)
    return;
  while (time > sprite->wholeDuration)
    time -= sprite->wholeDuration;
  while (time > sprite->momSub->duration-sprite->momSub->age)
  {
    time -= sprite->momSub->duration - sprite->momSub->age;
    sprite->momSub->age = 0;
    sprite->momSub = sprite->momSub->next;
  }
  sprite->momSub->age += time;
}

PREFIX void spSetSpriteRotation(spSpritePointer sprite,Sint32 rotation)
{
  sprite->rotation = rotation;
}

PREFIX void spSetSpriteZoom(spSpritePointer sprite,Sint32 zoomX,Sint32 zoomY)
{
  sprite->zoomX = zoomX;
  sprite->zoomY = zoomY;
}

PREFIX void spDrawSprite(Sint32 x,Sint32 y,Sint32 z,spSpritePointer sprite)
{
  if (sprite->rotation == 0 &&
      sprite->zoomX == (1 << SP_ACCURACY) &&
      sprite->zoomY == (1 << SP_ACCURACY))
  {
    if (sprite->momSub->sx < 0)
      spBlitSurface(x,y,z,sprite->momSub->surface);
    else
      spBlitSurfacePart(x,y,z,sprite->momSub->surface,sprite->momSub->sx,sprite->momSub->sy,sprite->momSub->sw,sprite->momSub->sh);
  }
  else
  {
    if (sprite->momSub->sx < 0)
      spRotozoomSurface(x,y,z,sprite->momSub->surface,sprite->zoomX,sprite->zoomY,sprite->rotation);
    else
      spRotozoomSurfacePart(x,y,z,sprite->momSub->surface,sprite->momSub->sx,sprite->momSub->sy,sprite->momSub->sw,sprite->momSub->sh,sprite->zoomX,sprite->zoomY,sprite->rotation);
  }  
}

PREFIX void spDrawSprite3D(Sint32 x,Sint32 y,Sint32 z,spSpritePointer sprite)
{
  if (sprite->momSub->sx < 0)
    spRotozoomSurface3D(x,y,z,sprite->momSub->surface,sprite->zoomX,sprite->zoomY,sprite->rotation);
  else
    spRotozoomSurfacePart3D(x,y,z,sprite->momSub->surface,sprite->momSub->sx,sprite->momSub->sy,sprite->momSub->sw,sprite->momSub->sh,sprite->zoomX,sprite->zoomY,sprite->rotation);
}
