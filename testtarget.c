/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include "../sparrow3d/sparrow3d.h"
#ifdef SDL_INCLUDE_SUBDIR
#include <SDL/SDL_image.h>
#else
#include <SDL_image.h>
#endif

SDL_Surface *screen;
SDL_Surface *texture;
SDL_Surface *scientist;
spSpritePointer sprite;
int rotation = 0;

void draw_test( void )
{
  //drawing on the texture
  spSelectRenderTarget(texture);
	spClearTarget(65535);
	spSetZSet(0);
	spSetZTest(0);
	sprite->rotation = 0;
	spDrawSprite( texture->w / 4, texture->h / 4, -1, sprite );
	sprite->zoomX = spSin( rotation * 8 ) + ( 3 << SP_ACCURACY - 1 );
	sprite->zoomY = spCos( rotation * 6 ) + ( 3 << SP_ACCURACY - 1 );
	spDrawSprite( 3 * texture->w / 4, texture->h / 4, -1, sprite );
	sprite->rotation = rotation * 4;
	spDrawSprite( texture->w / 4, 3 * texture->h / 4, -1, sprite );
	sprite->zoomX = 1 << SP_ACCURACY;
	sprite->zoomY = 1 << SP_ACCURACY;
	spDrawSprite( 3 * texture->w / 4, 3 * texture->h / 4, -1, sprite );

  //drawing on the screen
  spSelectRenderTarget(screen);
	spClearTarget(0);
	spSetZSet(1);
	spSetZTest(1);
  spResetZBuffer();
	spIdentity();
	spBindTexture(texture);

	spTranslate( 0,0, ( -7 << SP_ACCURACY ) );
	spRotateX( rotation );
	spRotateY( rotation );
	spRotateZ( rotation );

	//Front / Back
	spQuadTex3D( -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, texture->h - 1,
				 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, 0,
				 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, texture->w - 1, 0,
				 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, texture->w - 1, texture->h - 1, 65535);
	spQuadTex3D( 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, texture->h - 1,
				 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
				 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, texture->w - 1, 0,
				 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, texture->w - 1, texture->h - 1, 65535 );
	//Left / Right
	spQuadTex3D( -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, texture->h - 1,
				 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
				 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, texture->w - 1, 0,
				 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, texture->w - 1, texture->h - 1, 65535 );
	spQuadTex3D( 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, texture->h - 1,
				 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
				 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, texture->w - 1, 0,
				 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, texture->w - 1, texture->h - 1, 65535 );
	//Up / Down
	spQuadTex3D( 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, texture->h - 1,
				 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
				 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, texture->w - 1, 0,
				 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, texture->w - 1, texture->h - 1, 65535 );
	spQuadTex3D( -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, texture->h - 1,
				 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
				 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, texture->w - 1, 0,
				 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, texture->w - 1, texture->h - 1, 65535 );

	spFlip();
}


int calc_test( Uint32 steps )
{
	spUpdateSprite( sprite, steps );
	rotation += steps << SP_ACCURACY - 11;
	if ( spGetInput()->button[SP_BUTTON_START] )
		return 1;
	return 0;
}

void resize( Uint16 w, Uint16 h )
{
	//Setup of the new/resized window
	spSetPerspective( 50.0, ( float )spGetWindowSurface()->w / ( float )spGetWindowSurface()->h, 0.1, 100 );
}


int main( int argc, char **argv )
{
	//sparrow3D Init
	//spSetDefaultWindowSize( 640, 480 ); //Creates a 640x480 window at PC instead of 320x240
	spInitCore();

	//Setup
	screen = spCreateDefaultWindow();
	spSelectRenderTarget(screen);
	resize( screen->w, screen->h );

	//Textures loading
	scientist = spLoadSurface( "./data/science_guy_frames01.png" );
	texture = spCreateSurface(256,256);


	//Sprite Creating
	sprite = spNewSprite();
	int i;
	for ( i = 0; i < 9; i++ )
		spNewSubSpriteWithTiling( sprite, scientist, i * 24 + 1, 1, 22, 46, 100 );

  spSetLight( 1 );

	//All glory the main loop
	spLoop( draw_test, calc_test, 10, resize, NULL );

	//Winter Wrap up, Winter Wrap up …
	spDeleteSprite( sprite );
	SDL_FreeSurface( scientist );
	SDL_FreeSurface( texture );
	spQuitCore();
	return 0;
}
