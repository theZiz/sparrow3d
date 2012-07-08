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
#ifndef _SPARROW_SPRITE_H
#define _SPARROW_SPRITE_H

#include "sparrow3d.h"
#include <SDL.h>

typedef struct spSubSpriteStruct *spSubSpritePointer;
typedef struct spSubSpriteStruct
{
	SDL_Surface* surface;
	Sint32 sx, sy, sw, sh; //for Tiling. If sx<0 no tiling will be done
	Sint32 duration; //how long will this surface be seeable
	Sint32 age; //how long was it seeable yet
	spSubSpritePointer before;
	spSubSpritePointer next;
} spSubSprite;

typedef struct spSpriteStruct *spSpritePointer;
typedef struct spSpriteStruct
{
	Sint32 wholeDuration; //the duration of all subSprites.
	Sint32 wholeAge;
	Sint32 maxWidth, maxHeight; //the max high and weight the whole sprite will have (unrotated)
	Sint32 rotation;
	Sint32 zoomX, zoomY;
	spSubSpritePointer firstSub; //double linked list of subsprites
	spSubSpritePointer momSub; //for drawing and update. Don't touch it.
} spSprite;

PREFIX spSpritePointer spNewSprite();

PREFIX void spDeleteSprite( spSpritePointer sprite );

PREFIX spSubSpritePointer spNewSubSpriteNoTiling( spSpritePointer sprite, SDL_Surface* surface, Sint32 duration );

PREFIX spSubSpritePointer spNewSubSpriteWithTiling( spSpritePointer sprite, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 duration );

PREFIX void spNewSubSpriteTilingRow( spSpritePointer sprite, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 hopw,Sint32 hoph, Sint32 count,Sint32 duration );

PREFIX void spUpdateSprite( spSpritePointer sprite, Sint32 time );

PREFIX void spSetSpriteRotation( spSpritePointer sprite, Sint32 rotation );

PREFIX void spSetSpriteZoom( spSpritePointer sprite, Sint32 zoomX, Sint32 zoomY );

PREFIX void spDrawSprite( Sint32 x, Sint32 y, Sint32 z, spSpritePointer sprite );

PREFIX void spDrawSprite3D( Sint32 x, Sint32 y, Sint32 z, spSpritePointer sprite );

#endif
