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

/* sparrowSprite is about drawing sprites. Sprites are small animations
 * of surfaces with scaling and rotation. Scale and rotation are more
 * expensive, than just blitting, keep that in mind. So rotation = 0 and
 * scaling = SP_ONE (fixed point value!) will be fastest. If you disable
 * caching, it may need a lot of space, because every subsprite then
 * owns it own copy of the surface.*/
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

/* A spSpriteCollection is a collection of different sprites of one
 * kind. See below for more details. */
typedef struct spSpriteCollectionStruct *spSpriteCollectionPointer;

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
	char* name; //the name of the sprite. Important for spriteCollection
	spSpriteCollectionPointer collection;
	spSpritePointer next; //Used for spriteCollections. Do not modify!
} spSprite;

typedef struct spSpriteCollectionStruct
{
	spSpritePointer firstSprite; //the first sprite of the collection
	spSpritePointer active; //the selected sprite
} spSpriteCollection;

/* spNewSprite creates a new (empty) sprite. If you pass as name, it
 * will be copied and saved - important for sprite collections, but if
 * you don't use them, feel free to pass NULL.*/
PREFIX spSpritePointer spNewSprite(char* name);

/* spDeleteSprite deletes a sprite and all subsprites. If the sprite is
 * in a collection (see below), it is removed from there, too.*/
PREFIX void spDeleteSprite( spSpritePointer sprite);

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
