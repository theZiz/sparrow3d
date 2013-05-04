/*
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms of the
 GNU Lesser General Public license (the  "LGPL License") version 2 or higher, in
 which case the provisions of LGPL License are applicable instead of those above
 
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

/* File: sparrowSprite
 * 
 * SparrowSprite is about drawing sprites. Sprites are small animations
 * of surfaces with scaling and rotation. Scale and rotation are more
 * expensive, than just blitting, keep that in mind. So rotation = 0 and
 * scaling = SP_ONE (fixed point value!) will be fastest. If you disable
 * caching, it may need a lot of space, because every subsprite then
 * owns it own copy of the surface.*/
#ifndef _SPARROW_SPRITE_H
#define _SPARROW_SPRITE_H

#include "sparrow3d.h"
#include <SDL.h>

typedef struct spSubSpriteStruct *spSubSpritePointer;
typedef struct spSpriteCollectionStruct *spSpriteCollectionPointer;
typedef struct spSpriteStruct *spSpritePointer;

/* Type: spSubSprite
 * 
 * a sprite consists of different subSprites. Every subSprite contains
 * one surface (or a part of it), a duration and an age. If the age is
 * greater than the duration, the next subSprite will be drawn. Think of
 * it as a frame of a sprite.
 * 
 * Variables:
 * surface - the surface of the subsprite
 * sx,sy - the position in the surface to be drawn for tiling. If sx is
 * smaller than 0 no tilling is done
 * sw,sh - the width and height of the part of the surface
 * duration - the time the surface shall be seen. E.g. 50 ms would mean
 * an animation with 20 fps
 * age - how long was this subsprite seeable yet?
 * before,next - internal linked list pointers*/
typedef struct spSubSpriteStruct
{
	SDL_Surface* surface;
	Sint32 sx, sy, sw, sh;
	Sint32 duration;
	Sint32 age;
	spSubSpritePointer before;
	spSubSpritePointer next;
} spSubSprite;

/* Type: spSprite
 * 
 * The sprite struct itself.
 * 
 * Variables:
 * wholeDuration - the sum of all durations of all subSprites. See
 * <spSubSprite> for more about "duration"
 * wholeAge - the whole age of the animation
 * maxWidth,maxHeight - every subSprite can have different sizes. These
 * are the biggest width and height
 * rotation - the rotation of the sprite in fixed point radiant
 * zoomX, zoomY - the zoom of the sprite. SP_ONE means no zoom
 * firstSub - double linked list of the subSprites. See <spSubSprite>
 * for more detailed information
 * momSub - the active subSprite (and image) to draw
 * name - the name of the sprite. Needed for <spSpriteCollection>
 * collection - the collection the sprite is saved in. Can be NULL
 * next - pointer to the next sprite pointer in the linked list from
 * the collection. Do not change.*/
typedef struct spSpriteStruct
{
	Sint32 wholeDuration; 
	Sint32 wholeAge; 
	Sint32 maxWidth, maxHeight;
	Sint32 rotation;
	Sint32 zoomX, zoomY;
	spSubSpritePointer firstSub;
	spSubSpritePointer momSub;
	char* name;
	spSpriteCollectionPointer collection;
	spSpritePointer next;
} spSprite;

/* Type: spSpriteCollection
 * 
 * This is a collection of different sprites of one kind, e.g the
 * sprite of the character of your game. In that case you can have
 * different named sprites in this collection e.g. for running left,
 * running right, jumping left, jumping right, standing left, standing
 * right and so on and can easyly choose between them e.g. by calling
 * > spSelectSprite(collection,"running left");
 * 
 * Variables:
 * firstSprite - the first sprite in the collection
 * active - the choosen sprite*/
typedef struct spSpriteCollectionStruct
{
	spSpritePointer firstSprite;
	spSpritePointer active;
} spSpriteCollection;


/* Function: spNewSprite
 * 
 * Creates a new (empty) sprite.
 * 
 * Parameters:
 * name - the name of the sprite. Needed for finding in a collection,
 * but not necessary for the sprite itself, so if not needed feel free
 * to pass NULL
 * 
 * Returns:
 * spSprite* - Pointer to a <spSprite> struct*/
PREFIX spSpritePointer spNewSprite(char* name);

/* Function: spDeleteSprite
 * 
 * Deletes a sprite and *all* subsprites. If the sprite is
 * in a collection (see below), it is removed from there, too.
 * 
 * Parameters:
 * sprite - the sprite to be removed*/
PREFIX void spDeleteSprite( spSpritePointer sprite);

/* Function: spNewSubSpriteNoTiling
 * 
 * Creates a new subSprite out of a whole surface.
 * 
 * Parameters:
 * sprite - the sprite, in which the subsprite shall be added
 * surface - the surface to be added. It will be "copied" with
 * <spCopySurface>. See <spCopySurface> for more information about
 * "copying" with enabled caching
 * duration - the duration of the subsprite in milliseconds
 * 
 * Returns:
 * spSubSprite* - a pointer to the created <spSubSprite> struct
 * 
 * See Also:
 * <spNewSubSpriteWithTiling>, <spNewSubSpriteTilingRow>*/
PREFIX spSubSpritePointer spNewSubSpriteNoTiling( spSpritePointer sprite, SDL_Surface* surface, Sint32 duration );

/* Function: spNewSubSpriteWithTiling
 * 
 * Creates a new subSprite out of a part of a surface (tiling).
 * 
 * Parameters:
 * sprite - the sprite, in which the subsprite shall be added
 * surface - the surface, which part shall be added. It will be "copied"
 * with <spCopySurface>. See <spCopySurface> for more information about
 * "copying" with enabled caching
 * sx,sy - position of the left top corner of the part of the surface to
 * be added
 * w,h - size of the part to be added
 * duration - the duration of the subsprite in milliseconds
 * 
 * Returns:
 * spSubSprite* - a pointer to the created <spSubSprite> struct
 * 
 * See Also:
 * <spNewSubSpriteNoTiling>, <spNewSubSpriteTilingRow>*/
PREFIX spSubSpritePointer spNewSubSpriteWithTiling( spSpritePointer sprite, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 duration );

/* Function: spNewSubSpriteTilingRow
 * 
 * Very similar to <spNewSubSpriteWithTiling>, but instead of adding
 * *one* part of a surface, multiple parts ("tiles") of the same surface
 * are added.
 * 
 * Parameters:
 * sprite - the sprite, in which the subsprites shall be added
 * surface - the surface, which parts shall be added. It will be
 * "copied" with <spCopySurface>. See <spCopySurface> for more
 * information about "copying" with enabled caching
 * sx,sy - position of the left top corner of the first part of the
 * surface to be added
 * w,h - size of the parts to be added
 * hopw - the x distance to the next tile. If you don't use borders,
 * hopw should be the same as w
 * hoph - the y distance to the next tile. If the right end of the
 * surface is reached, but not all parts added this functions "jumps"
 * hoph on the y-axis and resets the x-position, too (like a line break
 * in the image). If you don't use borders, hoph should be the same as h
 * count - the number of subSprites to be added.
 * duration - the duration of the subsprite in milliseconds
 * 
 * See Also:
 * <spNewSubSpriteNoTiling>, <spNewSubSpriteWithTiling>*/
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

/* Sometimes it is useful to encapsulated different sprites to one
 * bundle, e.g. if you have a character with different animationloops
 * like running left, jumping left, running right, jumping right, etc.
 * These bundles are called "spriteCollections" in sparrow3d. Every
 * sprite can (but have not to be!) in one collection. The name of the
 * sprite is used to identify it, so keep in mind to give every sprite
 * a unique name at creation time! */
PREFIX spSpriteCollectionPointer spNewSpriteCollection();

/* This deletes the collection "collection". If "keepSprites" is 0 the
 * sprites in the collection will be deleted, too. */
PREFIX void spDeleteSpriteCollection(spSpriteCollectionPointer collection, int keepSprites);

/* This adds the existing (!) sprite "sprite" to the collection
 * "collection". If sprite is already in a collection, it will
 * automaticly removed first. */
PREFIX void spAddSpriteToCollection(spSpriteCollectionPointer collection, spSpritePointer sprite);

/* This call removes a sprite from a collection and sets it collection
 * to NULL. The sprite is NOT deleted, but if you would delete it, it
 * would automaticly removed from the collection. */
PREFIX void spRemoveSpriteFromCollection(spSpritePointer sprite);

/* Sets the active sprite of the collection. If no sprite with the name
 * "name" is found, no change is made. */
PREFIX void spSelectSprite(spSpriteCollectionPointer collection,char* name);

/* Returns the active sprite of the sprite Collection. Same to
 * collection->active, but looks nicer, don't you think? ;-) */
PREFIX spSpritePointer spActiveSprite(spSpriteCollectionPointer collection);

/* Loads the a sprite Collection from a ssc-file (see folder data for
 * example files) and returns it as a sprite collection.
 * The fallback_surface is, if the image, which should have loaded, is
 * not found.*/
PREFIX spSpriteCollectionPointer spLoadSpriteCollection(char* filename,SDL_Surface* fallback_surface);

#endif
