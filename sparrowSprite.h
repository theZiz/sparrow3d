 /* This file is part of sparrow3d.
  * Sparrow3d is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * Sparrow3d is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with Foobar.  If not, see <http://www.gnu.org/licenses/>
  * 
  * For feedback and questions about my Files and Projects please mail me,
  * Alexander Matthes (Ziz) , zizsdl_at_googlemail.com */

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

#include "sparrowDefines.h"
#include "sparrowCore.h"
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
 * surface (SDL_Surface*) - the surface of the subsprite
 * sx,sy (Sint32) - the position in the surface to be drawn for tiling. If sx is
 * smaller than 0 no tilling is done
 * sw,sh (Sint32) - the width and height of the part of the surface
 * duration (Sint32) - the time the surface shall be seen. E.g. 50 ms would mean
 * an animation with 20 fps
 * age (Sint32) - how long was this subsprite seeable yet?
 * before,next (spSubSprite*) - internal linked list pointers*/
typedef struct spSubSpriteStruct
{
	SDL_Surface* surface;
	Sint32 sx, sy, sw, sh;
	Sint32 duration;
	Sint32 age;
	spSubSpritePointer before;
	spSubSpritePointer next;
	Uint32 pixelCount;
	Uint16 averageColor;
} spSubSprite;

/* Type: spSprite
 * 
 * The sprite struct itself.
 * 
 * Variables:
 * wholeDuration (Sint32) - the sum of all durations of all subSprites. See
 * <spSubSprite> for more about "duration"
 * wholeAge (Sint32) - the whole age of the animation
 * maxWidth,maxHeight (Sint32) - every subSprite can have different sizes. These
 * are the biggest width and height
 * rotation (Sint32) - the rotation of the sprite in fixed point radiant
 * zoomX, zoomY (Sint32) - the zoom of the sprite. SP_ONE means no zoom
 * firstSub (spSubSprite*) - double linked list of the subSprites. See <spSubSprite>
 * for more detailed information
 * momSub (spSubSprite*) - the active subSprite (and image) to draw
 * name (char*) - the name of the sprite. Needed for <spSpriteCollection>
 * collection (spSpriteCollection*) - the collection the sprite is saved in. Can be NULL
 * next (spSprite*) - pointer to the next sprite pointer in the linked list from
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
 * firstSprite (spSprite*) - the first sprite in the collection
 * active (spSprite*) - the choosen sprite
 * author - author of the sprite. Doesn't have to be set
 * license - license of the sprite. Doesn't have to be set
 * comment - for a comment. Don't have to be set*/
typedef struct spSpriteCollectionStruct
{
	spSpritePointer firstSprite;
	spSpritePointer active;
	char author[256];
	char license[256];
	char comment[256];
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

/* Function: spUpdateSprite
 * 
 * Updates the sprite animation.
 * 
 * Parameters:
 * sprite - a pointer to a <spSprite> struct to update
 * time - the elapsed time*/
PREFIX void spUpdateSprite( spSpritePointer sprite, Sint32 time );

/* Function: spSetSpriteRotation
 * 
 * Sets the rotation of the sprite. Same as to sprite->rotation = …
 * 
 * Parameters:
 * sprite - a pointer to a <spSprite> struct to update
 * rotation - Sint32 fixed point radian rotation to set
 * 
 * See Also:
 * <spSetSpriteZoom>*/
PREFIX void spSetSpriteRotation( spSpritePointer sprite, Sint32 rotation );

/* Function: spSetSpriteZoom
 * 
 * Sets the zoom of the sprite. Same as to sprite->zoomX = … or
 * sprite->zoomY = …
 * 
 * Parameters:
 * sprite - a pointer to a <spSprite> struct to update
 * zoomX,zoomY - Sint32 fixed point zoom of the sprite. SP_ONE means no zoom
 * 
 * See Also:
 * <spSetSpriteRotation>*/
PREFIX void spSetSpriteZoom( spSpritePointer sprite, Sint32 zoomX, Sint32 zoomY );

/* Function: spDrawSprite
 * 
 * Draws a sprite with z set and test. For more information or deactivating
 * see <sparrowPrimitives>. The sprite is always drawn with the fastest possible
 * way. That means e.g. blitting, if no zoom and rotation is given.
 * 
 * Parameters:
 * x,y,z - position on screen and z coordinate for z test
 * sprite - a pointer to a <spSprite> struct to update
 * 
 * See Also:
 * <spDrawSprite3D> */
PREFIX void spDrawSprite( Sint32 x, Sint32 y, Sint32 z, spSpritePointer sprite );

/* Function: spDrawSprite3D
 * 
 * Draws a sprite with z set and test in 3d space. For more information or
 * deactivating the z stuff see <sparrowPrimitives>. The sprite is always drawn
 * with the fastest possible way. That means e.g. blitting, if no zoom and
 * rotation is given.
 * 
 * Parameters:
 * x,y,z - position in 3d space
 * sprite - a pointer to a <spSprite> struct to update
 * 
 * See Also:
 * <spDrawSprite> */
PREFIX void spDrawSprite3D( Sint32 x, Sint32 y, Sint32 z, spSpritePointer sprite );

/* Function: spNewSpriteCollection
 * 
 * Sometimes it is useful to encapsulated different sprites to one
 * bundle, e.g. if you have a character with different animationloops
 * like running left, jumping left, running right, jumping right, etc.
 * These bundles are called "spriteCollections" in sparrow3d. Every
 * sprite can (but have not to be!) in one collection. The name of the
 * sprite is used to identify it, so keep in mind to give every sprite
 * a unique name at creation time for this feature!
 * 
 * Returns:
 * spSpriteCollection* - a pointer to a fresh created <spSpriteCollection>*/
PREFIX spSpriteCollectionPointer spNewSpriteCollection( void );

/* Function: spDeleteSpriteCollection
 * 
 * Deletes a collection.
 * 
 * Parameters:
 * collection - the collection to be removed
 * keepSprites - if 1 the sprites in the collection are kept, if 0 the sprites
 * are deleted, too*/
PREFIX void spDeleteSpriteCollection(spSpriteCollectionPointer collection, int keepSprites);

/* Function: spAddSpriteToCollection
 * 
 * This adds a existing (!) sprite to a collection.
 * 
 * Parameters:
 * collection - the collection the sprite shall be added to
 * sprite - the sprite to add to the collection. If the sprite is already in a
 * collection, it is removed from the old collection*/
PREFIX void spAddSpriteToCollection(spSpriteCollectionPointer collection, spSpritePointer sprite);

/* Function: spRemoveSpriteFromCollection
 * 
 * Removes a sprite from a collection.
 * 
 * Parameters:
 * sprite - sprite to be removed from it's collection. The collection of the
 * sprite is set to NULL. The sprite is not deleted, just removed! However, if
 * you delete the sprite, it will be removed from it's collection automaticly*/
PREFIX void spRemoveSpriteFromCollection(spSpritePointer sprite);

/* Function: spSelectSprite
 * 
 * Sets the active sprite of the collection.
 * 
 * Parameters:
 * collection - collection to work with
 * name - name of the sprite to be choosen. If such a sprite doesn't exist, no
 * change is done*/
PREFIX void spSelectSprite(spSpriteCollectionPointer collection,char* name);

/* Function: spActiveSprite
 * 
 * Returns the active sprite of the sprite Collection. Same to
 * collection->active, but looks nicer, don't you think? ;-)
 * 
 * Parameters:
 * collection - the collection, which active sprite you want to know
 * 
 * Returns:
 * spSprite* - Pointer to the active <spSprite>*/
PREFIX spSpritePointer spActiveSprite(spSpriteCollectionPointer collection);

/* Function: spLoadSpriteCollection
 * 
 * Loads the a sprite Collection from a ssc-file and returns it as a sprite
 * collection. Use the "exampleSprite.ssc" in the data-folder of sparrow3d for
 * an example and documentation of the file format.
 * 
 * Parameters:
 * filename - file name of the ssc file
 * fallback_surface - this surface will be used, if the image file mentioned in
 * the ssc file is not found.
 * 
 * Returns
 * spSpriteCollection* - pointer to the loaded <spSpriteCollection>*/
PREFIX spSpriteCollectionPointer spLoadSpriteCollection(char* filename,SDL_Surface* fallback_surface);

/* Function spSpriteAverageColor
 * 
 * Gives the average color of the recent subsprite of a sprite.
 * 
 * Parameters:
 * sprite - a pointer to a <spSprite> struct
 * 
 * Returns:
 * Uint16 - average color*/
PREFIX Uint16 spSpriteAverageColor(spSpritePointer sprite);
#endif
