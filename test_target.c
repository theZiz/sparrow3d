/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_target.h"
#include <sparrow3d.h>
#include <strings.h>
#define GRAPH_SIZE 128
SDL_Surface *target_texture;
SDL_Surface *target_graph;
SDL_Surface *target_garfield;
SDL_Surface *target_scientist;
spSpritePointer target_sprite;

void init_target()
{
	//Textures loading
	target_scientist = spLoadSurface( "./data/science_guy_frames01.png" );
	target_garfield = spLoadSurface( "./data/garfield.png" );
	target_texture = spCreateSurface(256,256);
	target_graph = spCreateSurface(GRAPH_SIZE,GRAPH_SIZE);


	//Sprite Creating
	target_sprite = spNewSprite( NULL );
	int i;
	for ( i = 0; i < 9; i++ )
		spNewSubSpriteWithTiling( target_sprite, target_scientist, i * 24 + 1, 1, 22, 46, 100 );
}

char* caption_target(char* caption)
{
	sprintf(caption,"Different targets");
	return caption;
}

char* settings_target(char* caption,int button)
{
	sprintf(caption,"");
	return caption;
}

void draw_target(int rotation)
{
	SDL_Surface* screen = spGetWindowSurface();
  //drawing on the target_texture
  spSelectRenderTarget(target_texture);
	spClearTarget(65535);
	spSetZSet(0);
	spSetZTest(0);
	target_sprite->rotation = 0;
	spDrawSprite( target_texture->w / 4, target_texture->h / 4, 0, target_sprite );
	target_sprite->zoomX = spSin( rotation * 8 ) + spFloatToFixed( 1.5f );
	target_sprite->zoomY = spCos( rotation * 6 ) + spFloatToFixed( 1.5f );
	spDrawSprite( 3 * target_texture->w / 4, target_texture->h / 4, 0, target_sprite );
	target_sprite->rotation = rotation * 4;
	spDrawSprite( target_texture->w / 4, 3 * target_texture->h / 4, 0, target_sprite );
	target_sprite->zoomX = SP_ONE;
	target_sprite->zoomY = SP_ONE;
	spDrawSprite( 3 * target_texture->w / 4, 3 * target_texture->h / 4, 0, target_sprite );

  //drawing on the target_graph
  spSelectRenderTarget(target_graph);
	spClearTarget(01234);
	spSetZSet(0);
	spSetZTest(0);
	Uint16* pixeldata = spGetTargetPixel();
	int x;
	for (x = 0;x < GRAPH_SIZE; x++)
	{
		Sint32 sx = spIntToFixed(x-GRAPH_SIZE/2)/16;
		Sint32 sy = spSin(sx+rotation*16);
		int y = spFixedToInt(sy*16)+GRAPH_SIZE/2;
		if (y>=0 && y<GRAPH_SIZE)
      pixeldata[y*GRAPH_SIZE+x] = 56789;
		sy = spCos(sx+rotation*32);
		y = spFixedToInt(sy*16)+GRAPH_SIZE/2;
		if (y>=0 && y<GRAPH_SIZE)
      pixeldata[y*GRAPH_SIZE+x] = 45678;
  }
  spUnlockRenderTarget();
	for (x = 0;x < GRAPH_SIZE-1; x++)
	{
		Sint32 sx1 = spIntToFixed(x-GRAPH_SIZE/2)/16;
		Sint32 sy1 = spTan(sx1+rotation*8);
		int y1 = spFixedToInt(sy1*16)+GRAPH_SIZE/2;
		Sint32 sx2 = spIntToFixed(x+1-GRAPH_SIZE/2)/16;
		Sint32 sy2 = spTan(sx2+rotation*8);
		int y2 = spFixedToInt(sy2*16)+GRAPH_SIZE/2;
		spLine(x,y1,0,x+1,y2,0,34567);
		sy1 = spAcos(sx1/2+spSin(rotation*24));
		y1 = spFixedToInt(sy1*16)+GRAPH_SIZE/2;
		sy2 = spAcos(sx2/2+spSin(rotation*24));
		y2 = spFixedToInt(sy2*16)+GRAPH_SIZE/2;
		spLine(x,y1,0,x+1,y2,0,23456);
		sy1 = spAsin(sx1/2+spCos(rotation*24));
		y1 = spFixedToInt(sy1*16)+GRAPH_SIZE/2;
		sy2 = spAsin(sx2/2+spCos(rotation*24));
		y2 = spFixedToInt(sy2*16)+GRAPH_SIZE/2;
		spLine(x,y1,0,x+1,y2,0,12345);
  }
  
  //drawing on the screen
  spSelectRenderTarget(screen);
	spClearTarget(0);
	spSetZSet(1);
	spSetZTest(1);
  spResetZBuffer();
	spIdentity();

	spTranslate( 0,0, spFloatToFixed( -7.0f ) );
	spRotateX( rotation );
	spRotateY( rotation );
	spRotateZ( rotation );

	//Front / Back
	spBindTexture(target_texture);
	spQuadTex3D( -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, target_texture->h - 1,
				 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, 0,
				 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), target_texture->w - 1, 0,
				 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), target_texture->w - 1, target_texture->h - 1, 65535);
	spQuadTex3D( spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, target_texture->h - 1,
				 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
				 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), target_texture->w - 1, 0,
				 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), target_texture->w - 1, target_texture->h - 1, 65535 );
	//Left / Right
	spBindTexture(target_graph);
	spQuadTex3D( -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, target_graph->h - 1,
				 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
				 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), target_graph->w - 1, 0,
				 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), target_graph->w - 1, target_graph->h - 1, 65535 );
	spQuadTex3D( spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, target_graph->h - 1,
				 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
				 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), target_graph->w - 1, 0,
				 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), target_graph->w - 1, target_graph->h - 1, 65535 );
	//Up / Down
	spBindTexture(target_garfield);
	spQuadTex3D( spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, target_garfield->h - 1,
				 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
				 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), target_garfield->w - 1, 0,
				 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), target_garfield->w - 1, target_garfield->h - 1, 65535 );
	spQuadTex3D( -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, target_garfield->h - 1,
				 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
				 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), target_garfield->w - 1, 0,
				 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), target_garfield->w - 1, target_garfield->h - 1, 65535 );
}

void calc_target(int steps)
{
	spUpdateSprite( target_sprite, steps );
}

void quit_target()
{
	spDeleteSprite( target_sprite );
	SDL_FreeSurface( target_scientist );
	SDL_FreeSurface( target_texture );
	SDL_FreeSurface( target_graph );
	SDL_FreeSurface( target_garfield );
}
