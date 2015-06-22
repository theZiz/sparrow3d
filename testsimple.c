/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include <sparrow3d.h>
spFontPointer font = NULL;
SDL_Surface* picture = NULL;

void draw_test( void )
{
	spClearTarget( spGetRGB(64,64,64) );
	
	//For all drawing functions we can just ignore the z value, because
	//no z test or z set is used.
	spFontDrawMiddle( spGetWindowSurface()->w/2, 2, 0, "Simple blit test", font );
	spBlitSurface( spGetWindowSurface()->w/2, spGetWindowSurface()->h/2, 0, picture);
	spFontDrawMiddle( spGetWindowSurface()->w/2, spGetWindowSurface()->h-font->maxheight-2, 0, "Press [S] to exit", font );
	spFlip();
}

int calc_test( Uint32 steps )
{
	//the start button depends on the target. On the PC it is Return
	if ( spGetInput()->button[SP_BUTTON_START] )
		//When the calc function returns a value != 0, the game loop exists
		return 1;
	return 0;
}

void resize( Uint16 w, Uint16 h )
{
	//With sparrow3d you don't have to draw to the screen. You could
	//also draw to any other surface. So we have to tell sparrow3d once
	//at the beginning, that we want to draw to the screen.
	spSelectRenderTarget(spGetWindowSurface());
	
	//Font Loading
	
	//This makes the button look more beautiful
	spFontShadeButtons(1);
	
	//if a font is already loaded we need to delete it first
	if ( font )
		spFontDelete( font );

	//We need to load a font before we can use it
	font = spFontLoad( "./font/Play-Bold.ttf", spFixedToInt(10 * spGetSizeFactor()));
	
	//This sets the "background" colour of the font. 0 is black.
	spFontSetShadeColor(0);
	
	//Every letter you want to use, you have to add here. The last
	//parameter (65535) is the color. Use spGetRGB or the number of the
	//colour. 65535 is white.
	spFontAdd( font, SP_FONT_GROUP_ASCII, 65535 ); //whole ASCII
	
	//This adds a black border to the font. Just try, what happens
	//without this line
	spFontAddBorder( font, 0 );
	
	//We want, that [S] appears as a button depending on the target.
	//Have a look, how [S] in line 21 is drawn as a Button image.
	spFontAddButton( font, 'S', SP_BUTTON_START_NAME, 65535, spGetRGB( 64, 64, 64 ) );
}

int main( int argc, char **argv )
{
	//sparrow3D Init
	spSetDefaultWindowSize( 800, 480 ); //Creates a 800x480 window at PC instead of 320x240
	spInitCore();

	//Setup
	spCreateDefaultWindow();
	
	//Setting the target and the font for the first time
	resize( spGetWindowSurface()->w, spGetWindowSurface()->h );
	
	//Setting some options for speed improvements
	//In fact sparrow3d was made for 3d rendering, so for 2d stuff we
	//can deactivate some features like z set and z test:
	spSetZSet(0);
	spSetZTest(0);
	
	//Loading a picture
	picture = spLoadSurface( "./data/garfield.png" );

	//All glory the main loop
	spLoop( draw_test, calc_test, 10, resize, NULL );

	//Deleting the font
	spFontDelete( font );
	
	//Deleting the picture
	spDeleteSurface( picture );
	
	//Finish sparrow3d
	spQuitCore();
	return 0;
}
