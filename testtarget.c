/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include <sparrow3d.h>
#include <SDL_image.h>

#define GRAPH_SIZE 128

SDL_Surface *screen;
SDL_Surface *texture;
SDL_Surface *graph;
SDL_Surface *garfield;
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
	sprite->zoomX = SP_ONE;
	sprite->zoomY = SP_ONE;
	spDrawSprite( 3 * texture->w / 4, 3 * texture->h / 4, -1, sprite );

  //drawing on the graph
  spSelectRenderTarget(graph);
	spClearTarget(01234);
	spSetZSet(0);
	spSetZTest(0);
	Uint16* pixeldata = spLockRenderTarget();
	int x;
	for (x = 0;x < GRAPH_SIZE; x++)
	{
		Sint32 sx = (x-GRAPH_SIZE/2)<<SP_ACCURACY-4;
		Sint32 sy = spSin(sx+rotation*16);
		int y = (sy>>SP_ACCURACY-4)+GRAPH_SIZE/2;
		if (y>=0 && y<GRAPH_SIZE)
      pixeldata[y*GRAPH_SIZE+x] = 56789;
		sy = spCos(sx+rotation*32);
		y = (sy>>SP_ACCURACY-4)+GRAPH_SIZE/2;
		if (y>=0 && y<GRAPH_SIZE)
      pixeldata[y*GRAPH_SIZE+x] = 45678;
  }
  spUnlockRenderTarget();
	for (x = 0;x < GRAPH_SIZE-1; x++)
	{
		Sint32 sx1 = (x-GRAPH_SIZE/2)<<SP_ACCURACY-4;
		Sint32 sy1 = spTan(sx1+rotation*8);
		Sint32 y1 = (sy1>>SP_ACCURACY-4)+GRAPH_SIZE/2;
		Sint32 sx2 = (x+1-GRAPH_SIZE/2)<<SP_ACCURACY-4;
		Sint32 sy2 = spTan(sx2+rotation*8);
		Sint32 y2 = (sy2>>SP_ACCURACY-4)+GRAPH_SIZE/2;
		spLine(x,y1,-1,x+1,y2,-1,34567);
		sy1 = spAcos(sx1/2+spSin(rotation*24));
		y1 = (sy1>>SP_ACCURACY-4)+GRAPH_SIZE/2;
		sy2 = spAcos(sx2/2+spSin(rotation*24));
		y2 = (sy2>>SP_ACCURACY-4)+GRAPH_SIZE/2;
		spLine(x,y1,-1,x+1,y2,-1,23456);
		sy1 = spAsin(sx1/2+spCos(rotation*24));
		y1 = (sy1>>SP_ACCURACY-4)+GRAPH_SIZE/2;
		sy2 = spAsin(sx2/2+spCos(rotation*24));
		y2 = (sy2>>SP_ACCURACY-4)+GRAPH_SIZE/2;
		spLine(x,y1,-1,x+1,y2,-1,12345);
  }
  
  //drawing on the screen
  spSelectRenderTarget(screen);
	spClearTarget(0);
	spSetZSet(1);
	spSetZTest(1);
  spResetZBuffer();
	spIdentity();

	spTranslate( 0,0, ( -7 << SP_ACCURACY ) );
	spRotateX( rotation );
	spRotateY( rotation );
	spRotateZ( rotation );

	//Front / Back
	spBindTexture(texture);
	spQuadTex3D( -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, texture->h - 1,
				 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, 0,
				 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, texture->w - 1, 0,
				 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, texture->w - 1, texture->h - 1, 65535);
	spQuadTex3D( 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, texture->h - 1,
				 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
				 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, texture->w - 1, 0,
				 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, texture->w - 1, texture->h - 1, 65535 );
	//Left / Right
	spBindTexture(graph);
	spQuadTex3D( -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, graph->h - 1,
				 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
				 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, graph->w - 1, 0,
				 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, graph->w - 1, graph->h - 1, 65535 );
	spQuadTex3D( 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, graph->h - 1,
				 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
				 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, graph->w - 1, 0,
				 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, graph->w - 1, graph->h - 1, 65535 );
	//Up / Down
	spBindTexture(garfield);
	spQuadTex3D( 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, garfield->h - 1,
				 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
				 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, garfield->w - 1, 0,
				 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, garfield->w - 1, garfield->h - 1, 65535 );
	spQuadTex3D( -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, garfield->h - 1,
				 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
				 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, garfield->w - 1, 0,
				 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, garfield->w - 1, garfield->h - 1, 65535 );

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
	spSetPerspective( 130.0, ( float )spGetWindowSurface()->w / ( float )spGetWindowSurface()->h, 0.1, 100 );
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
	garfield = spLoadSurface( "./data/garfield.png" );
	texture = spCreateSurface(256,256);
	graph = spCreateSurface(GRAPH_SIZE,GRAPH_SIZE);


	//Sprite Creating
	sprite = spNewSprite( NULL );
	int i;
	for ( i = 0; i < 9; i++ )
		spNewSubSpriteWithTiling( sprite, scientist, i * 24 + 1, 1, 22, 46, 100 );
		
	spSetLight( 1 );

	//All glory the main loop
	spSetPerspectiveTextureMapping(1);
	spLoop( draw_test, calc_test, 10, resize, NULL );

	//Winter Wrap up, Winter Wrap up …
	spDeleteSprite( sprite );
	SDL_FreeSurface( scientist );
	SDL_FreeSurface( texture );
	SDL_FreeSurface( graph );
	SDL_FreeSurface( garfield );
	spQuitCore();
	return 0;
}
