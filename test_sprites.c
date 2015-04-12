/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_sprites.h"
#include <sparrow3d.h>
#include <strings.h>
SDL_Surface *sprites_texture;
SDL_Surface *sprites_garfield;
spSpritePointer sprite;
spSpriteCollectionPointer collection;
int sprites_z = 1;

void init_sprites()
{
	sprites_garfield = spLoadSurface( "./data/garfield.png" );
	sprites_texture = spLoadSurface( "./data/science_guy_frames01.png" );
	collection = spLoadSpriteCollection("./data/exampleSprite.ssc",sprites_texture);
	sprite = spActiveSprite(collection);
}

char* caption_sprites(char* caption)
{
	sprintf(caption,"Sprites & Rotozoom\n(Sprite by %s)",collection->author);
	return caption;
}

char* settings_sprites(char* caption,int button)
{
	switch (button)
	{
		case SP_BUTTON_A:
			if (sprites_z)
				sprintf(caption,"[A] Z Set & Test");
			else
				sprintf(caption,"[A] No Z Test & Set");
			break;
		case SP_BUTTON_B:
				caption[0] = 0;
			break;
	}
	return caption;
}

void draw_sprites(int rotation)
{
	spSetZTest( sprites_z );
	spSetZSet( sprites_z );
	SDL_Surface* screen = spGetWindowSurface();
	spRotozoomSurface( screen->w / 4, screen->h / 4, 0, sprites_garfield, spSin( rotation * 4 ) + ( spFloatToFixed( 1.5f ) ) >> 2, spCos( rotation * 8 ) + ( spFloatToFixed( 1.5f ) ) >> 2, rotation );
	spRotozoomSurfacePart( 3 * screen->w / 4, screen->h / 4, 0, sprites_garfield, sprites_garfield->w / 4, sprites_garfield->h / 4, sprites_garfield->w / 2, sprites_garfield->w / 2, spSin( rotation * 4 ) + ( spFloatToFixed( 1.5f ) ) >> 1, spCos( rotation * 8 ) + ( spFloatToFixed( 1.5f ) ) >> 1, rotation );
	sprite->rotation = 0;
	spDrawSprite( screen->w / 5, 5 * screen->h / 8, 0, sprite );
	sprite->zoomX = spSin( rotation * 8 ) + ( spFloatToFixed( 1.5f ) );
	sprite->zoomY = spCos( rotation * 6 ) + ( spFloatToFixed( 1.5f ) );
	spDrawSprite( 2 * screen->w / 5, 5 * screen->h / 8, 0, sprite );
	sprite->zoomX *= -1;
	sprite->rotation = rotation * 4;
	sprite->zoomX = sprite->zoomY = SP_ONE*4;
	int i;
	for (i = 0; i < 32; i++)
		spDrawSprite( 3 * screen->w / 5, 5 * screen->h / 8, 0, sprite );
	sprite->zoomX = SP_ONE;
	sprite->zoomY = SP_ONE;
	spDrawSprite( 4 * screen->w / 5, 5 * screen->h / 8, 0, sprite );
	spSetZTest( 0 );
	spSetZSet( 0 );
}

void calc_sprites(int steps)
{
	if ( spGetInput()->button[SP_BUTTON_A] )
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		sprites_z = 1-sprites_z;
	}
	spUpdateSprite( sprite, steps );
}

void quit_sprites()
{
	spDeleteSurface(sprites_garfield);
	spDeleteSurface(sprites_texture);
	spDeleteSpriteCollection(collection,0);
}
