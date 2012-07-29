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

SDL_Surface *screen;
spSpriteCollectionPointer collection;
Sint32 rotation = 0;

void draw_function( void )
{
	//Cleaning the target (in this case the screen surface) with 0 (black).
	//In this case I don't clean the zBuffer, because I don't use it. ;-)
	//But you ARE able to use it, if you want!
	spResetZBuffer();
	spClearTarget( 34567 );


	//Set the origin:
	switch ( ( rotation >> 17 ) % 9 )
	{
	case 0:
		spSetHorizontalOrigin( SP_CENTER );
		spSetVerticalOrigin( SP_CENTER );
		break;
	case 1:
		spSetHorizontalOrigin( SP_LEFT );
		spSetVerticalOrigin( SP_CENTER );
		break;
	case 2:
		spSetHorizontalOrigin( SP_LEFT );
		spSetVerticalOrigin( SP_TOP );
		break;
	case 3:
		spSetHorizontalOrigin( SP_CENTER );
		spSetVerticalOrigin( SP_TOP );
		break;
	case 4:
		spSetHorizontalOrigin( SP_RIGHT );
		spSetVerticalOrigin( SP_TOP );
		break;
	case 5:
		spSetHorizontalOrigin( SP_RIGHT );
		spSetVerticalOrigin( SP_CENTER );
		break;
	case 6:
		spSetHorizontalOrigin( SP_RIGHT );
		spSetVerticalOrigin( SP_BOTTOM );
		break;
	case 7:
		spSetHorizontalOrigin( SP_CENTER );
		spSetVerticalOrigin( SP_BOTTOM );
		break;
	case 8:
		spSetHorizontalOrigin( SP_LEFT );
		spSetVerticalOrigin( SP_BOTTOM );
		break;
	}

	//Drawing the sprite in the middle of the screen
	spDrawSprite( screen->w / 2, screen->h / 2, -1, spActiveSprite(collection) );
	spDrawSprite( screen->w - 1, screen->h - 1, -1, spActiveSprite(collection) );
	spDrawSprite( screen->w - 1, 0, -1, spActiveSprite(collection) );
	spDrawSprite( 0, screen->h - 1, -1, spActiveSprite(collection) );
	spDrawSprite( 0, 0, -1, spActiveSprite(collection) );

	//mark the center
	//spEllipseBorder( screen->w / 2, screen->h / 2, -100, 60+spSin(rotation/2)/1000, 60+spCos(rotation/3)/1000, 20+spCos(rotation/7)/10000, 20+spSin(rotation/5)/10000, 0 );
	spEllipseBorder( screen->w / 2, screen->h / 2, -100, 6, 6, 2, 2, 0 );

	//Show it!
	spFlip();
}


int calc_function( Uint32 steps )
{
	//update rotation counter
	rotation += steps * 32;
	//if "Start" (or "Menu" for Wiz or "Home" for Caanoo or "Return" at the P) is
	//pressed: exit
	if ( spGetInput()->button[SP_BUTTON_START] )
		return 1;

	//Update the sprite (next picture)
	spUpdateSprite( spActiveSprite(collection), steps );
	//rotozoom the sprite (without rotozoom it's faster):
	spActiveSprite(collection)->zoomX = spSin( rotation * 8 ) / 2 + ( 3 << SP_ACCURACY - 1 );
	spActiveSprite(collection)->zoomY = spCos( rotation * 6 ) / 2 + ( 3 << SP_ACCURACY - 1 );
	spActiveSprite(collection)->rotation = rotation * 4;

	return 0;
}

int main( int argc, char **argv )
{
	//sparrow3D Init
	spSetDefaultWindowSize( 640, 480 ); //Creates a 640x480 window at PC instead of 320x240
	spInitCore();

	//Setup
	screen = spCreateDefaultWindow();
	spSelectRenderTarget(screen);

	//Loading the collection
	collection = spLoadSpriteCollection("./data/exampleSprite.ssc",NULL);

	//We don't want to use the zBuffer in any way
	spSetZSet(0);
	spSetZTest(0);

	//All glory the main loop
	//every frame the draw_function is called
	//every frame the calc_function is called with the past time as argument
	//at least 10 ms have to be between to frames (max 100 fps)
	//if the window is resized, the resize feedback-function is called (again)
	spLoop( draw_function, calc_function, 10, NULL, NULL );

	//Winter Wrap up, Winter Wrap up
	spDeleteSpriteCollection( collection , 0);
	spQuitCore();
	return 0;
}
