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
  
}

PREFIX void spDeleteSprite(spSpritePointer sprite)
{
  //TODO: Implement
}

PREFIX spSubSpritePointer spNewSubSpriteNoTiling(spSpritePointer sprite,SDL_Surface* surface,Sint32 duration)
{
  spNewSubSpriteWithTiling(sprite,surface,-1,-1,-1,-1,duration);
}

PREFIX spSubSpritePointer spNewSubSpriteWithTiling(spSpritePointer sprite,SDL_Surface* surface,Sint32 sx,Sint32 sy,Sint32 sw,Sint32 sh,Sint32 duration)
{
  spSubSpritePointer sub = (spSubSpritePointer)malloc(sizeof(spSubSprite));
}

PREFIX void spUpdateSprite(spSpritePointer sprite,Sint32 time)
{
  //TODO: Implement
}

PREFIX void spSetSpriteRotation(spSpritePointer sprite,Sint32 rotation)
{
  //TODO: Implement
}

PREFIX void spSetSpriteZoom(spSpritePointer sprite,Sint32 zoomX,Sint32 zoomY)
{
  //TODO: Implement
}

PREFIX void spDrawSprite(Sint32 x,Sint32 y,Sint32 z,spSpritePointer sprite)
{
  //TODO: Implement
}

PREFIX void spDrawSprite3D(Sint32 x,Sint32 y,Sint32 z,spSpritePointer sprite)
{
  //TODO: Implement
}
