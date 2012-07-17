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

/* sparrowSprite is about drawing sprites. Sprites are small animations
 * of surfaces with scaling and rotation. Scale and rotation are more
 * expensive, than just blitting, keep that in mind. So rotation = 0 and
 * scaling = SP_ONE (fixed point value!) will be fastest. */
#ifndef _SPARROW_SPRITE_H
#define _SPARROW_SPRITE_H

#include "sparrow3d.h"
#include <SDL.h>

/* a sprite consists of differnt subSprites. Every subsprite contains
 * one surface (or a part of it), a duration and an age. If the age is
 * greater than the duration, the next subSprite will be drawn */
typedef struct spSubSpriteStruct *spSubSpritePointer;
typedef struct spSubSpriteStruct
{
	SDL_Surface* surface; //the surface
	Sint32 sx, sy, sw, sh; //for Tiling. If sx<0 no tiling will be done
	Sint32 duration; //how long will this surface be seeable
	Sint32 age; //how long was it seeable yet
	spSubSpritePointer before;
	spSubSpritePointer next;
} spSubSprite;

/* The sprite struct */
typedef struct spSpriteStruct *spSpritePointer;
typedef struct spSpriteStruct
{
	Sint32 wholeDuration; //the duration of all subSprites.
	/* the age of all subSprites. Of wholeAge is greater than
	 * wholeDuration, the sprite restarts */
	Sint32 wholeAge; 
	Sint32 maxWidth, maxHeight; //the max high and weight the whole sprite will have (unrotated)
	Sint32 rotation; //the rotation of the sprite. A radiant fixed point value
	Sint32 zoomX, zoomY; //the zoom of the sprite, fixed point values. SP_ONE means no zoom
	spSubSpritePointer firstSub; //double linked list of subsprites
	spSubSpritePointer momSub; //for drawing and update. Don't touch it.
} spSprite;

/* spNewSprite creates a new (empty) sprite */
PREFIX spSpritePointer spNewSprite();

/* spDeleteSprite deletes a sprite and all subsprites. */
PREFIX void spDeleteSprite( spSpritePointer sprite );

/* spNewSubSpriteNoTiling creates a new subSprite out of a whole surface
 * and add it to the sprite "sprite" with the duration "duration". */
PREFIX spSubSpritePointer spNewSubSpriteNoTiling( spSpritePointer sprite, SDL_Surface* surface, Sint32 duration );

/* spNewSubSpriteWithTiling is very similar to spNewSubSpriteNoTiling,
 * but instead of a surface a PART of the surface is passed. sx and sy
 * are the left/top egde of the part and sw and sh the width and high.*/
PREFIX spSubSpritePointer spNewSubSpriteWithTiling( spSpritePointer sprite, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 duration );


/* spNewSubSpriteTilingRow is very similar to spNewSubSpriteWithTiling -
 * But instead of adding ONE part of a surface, count parts of the
 * same surface "surface" are added, sw and sh are similar. sx and sy
 * are the coordinates of the FIRST subSprite. hopw is the width
 * to the next subSprite. If the border of the surface is reached, but
 * still subSprites to add, the next line (hoph under the first) will
 * be read. */
PREFIX void spNewSubSpriteTilingRow( spSpritePointer sprite, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 hopw,Sint32 hoph, Sint32 count,Sint32 duration );

/* Updates the sprite for time ms */
PREFIX void spUpdateSprite( spSpritePointer sprite, Sint32 time );

/* Sets the rotation of the sprite. Similar to sprite->rotation = … */
PREFIX void spSetSpriteRotation( spSpritePointer sprite, Sint32 rotation );

/* Sets the zoom of the sprite. Similar to sprite->zoomX = … or
 * sprite->zoomY = … */
PREFIX void spSetSpriteZoom( spSpritePointer sprite, Sint32 zoomX, Sint32 zoomY );

/* Draws the sprite @x,y on the screen with the z ordinate "z". The
 * handling of z is like spBlitSurface or spDrawSurface (zTest, zSet) */
PREFIX void spDrawSprite( Sint32 x, Sint32 y, Sint32 z, spSpritePointer sprite );

/* Works like spDrawSprite, but it projects the sprite first. */
PREFIX void spDrawSprite3D( Sint32 x, Sint32 y, Sint32 z, spSpritePointer sprite );

#endif
