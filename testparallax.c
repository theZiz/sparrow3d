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
SDL_Surface *tile_map;
spSpritePointer sprite[8][4];
spSpriteCollectionPointer collection;
Sint32 rotation = 0;
int layer_count = 3;
int using_zbuffer = 0;
spFontPointer font = NULL;

void draw_function( void )
{
	if (using_zbuffer)
		spResetZBuffer();
	spClearTarget( 34567 );
	spSetZSet(using_zbuffer);
	spSetZTest(using_zbuffer);
	
	//Filling the screen with sprites. Just sprites. :)
	srand(0);
	int l;
	int x,y;
	if (using_zbuffer)
		for (l=layer_count-1; l >= 0; l--)
		{
			int layer = layer_count-1-l;
			int addx = spSin(rotation*(2<<layer)>>3)>>SP_ACCURACY-5;
			int addy = spCos(rotation*(2<<layer)>>3)>>SP_ACCURACY-5;
			for (x = -16; x <= spGetWindowSurface()->w+16; x+=32)
				for (y = -16; y <= spGetWindowSurface()->h+16; y+=32)
						spDrawSprite(x+addx,y+addy,l,sprite[rand()&7][rand()&3]);
		}
	else
		for (l=0; l < layer_count; l++)
		{
			int addx = spSin(rotation*(2<<l)>>3)>>SP_ACCURACY-5;
			int addy = spCos(rotation*(2<<l)>>3)>>SP_ACCURACY-5;
			for (x = -16; x <= spGetWindowSurface()->w+16; x+=32)
				for (y = -16; y <= spGetWindowSurface()->h+16; y+=32)
						spDrawSprite(x+addx,y+addy,l,sprite[rand()&7][rand()&3]);
		}
	
	
	spSetZSet(0);
	spSetZTest(0);
	spFontDraw( 0, 0, -1, "[L] Less Layer", font );
	spFontDrawRight( spGetWindowSurface()->w-1, 0, -1, "More Layer [R]", font );
	
	char buffer[256];
	sprintf(buffer,"Layer: %i",layer_count);
	spFontDraw( 0, spGetWindowSurface()->h-font->maxheight, -1, buffer, font );
	if (using_zbuffer)
		spFontDrawMiddle( spGetWindowSurface()->w/2, spGetWindowSurface()->h-font->maxheight, -1, "Z-Buffer: On [A]", font );
	else
		spFontDrawMiddle( spGetWindowSurface()->w/2, spGetWindowSurface()->h-font->maxheight, -1, "Z-Buffer: Off [A]", font );
	sprintf(buffer,"FPS: %i",spGetFPS());
	spFontDrawRight( spGetWindowSurface()->w-1, spGetWindowSurface()->h-font->maxheight, -1, buffer, font );
	
	

	spFlip();
}


int calc_function( Uint32 steps )
{
	rotation += steps * 32;
	if ( spGetInput()->button[SP_BUTTON_START] )
		return 1;
	if ( spGetInput()->button[SP_BUTTON_R] )
	{
		spGetInput()->button[SP_BUTTON_R] = 0;
		layer_count++;
		if (layer_count == 10)
			layer_count = 0;
	}
	if ( spGetInput()->button[SP_BUTTON_L] )
	{
		spGetInput()->button[SP_BUTTON_L] = 0;
		layer_count--;
		if (layer_count == -1)
			layer_count = 9;
	}
	if ( spGetInput()->button[SP_BUTTON_A] )
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		using_zbuffer = 1-using_zbuffer;
	}
		
	int a,b;
		for (a = 0; a < 8; a++)
			for (b = 0; b < 4; b++)
				spUpdateSprite(sprite[a][b],steps);
	
	return 0;
}

void resize(Uint16 w,Uint16 h)
{
	spFontShadeButtons(1);
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./font/StayPuft.ttf", 17 * spGetSizeFactor() >> SP_ACCURACY );
	spFontSetShadeColor(0);
	spFontAdd( font, SP_FONT_GROUP_ASCII, 65535 ); //whole ASCII
	spFontAdd( font, "äüöÄÜÖßẞ", 65535 ); //German stuff (same like spFontAdd( font, SP_FONT_GROUP_GERMAN, 0 ); )
	spFontAddBorder( font, 0 );
	spFontAddButton( font, 'A', SP_BUTTON_A_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'B', SP_BUTTON_B_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'X', SP_BUTTON_X_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'Y', SP_BUTTON_Y_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'L', SP_BUTTON_L_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'R', SP_BUTTON_R_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'S', SP_BUTTON_START_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'E', SP_BUTTON_SELECT_NAME, 65535, spGetRGB( 64, 64, 64 ) );
}

int main( int argc, char **argv )
{
	//sparrow3D Init
	spSetDefaultWindowSize( 640, 480 ); //Creates a 640x480 window at PC instead of 320x240
	spInitCore();

	//Setup
	screen = spCreateDefaultWindow();
	spSelectRenderTarget(screen);
	resize(screen->w,screen->h);

	//Tile map loading
	tile_map = spLoadSurface( "./data/grass_desert_tiles01.png" );

	//Loading the sprite
	collection = spNewSpriteCollection();
	int a,b;
	for (a = 0; a < 8; a++)
		for (b = 0; b < 4; b++)
		{
			sprite[a][b] = spNewSprite(NULL);
			spNewSubSpriteWithTiling(sprite[a][b],tile_map,32+a*32,32+b*32,32,32,1000);
			spAddSpriteToCollection(collection,sprite[a][b]);
		}

	spLoop( draw_function, calc_function, 10, resize, NULL );

	//Winter Wrap up, Winter Wrap up
	spFontDelete( font );
	spDeleteSpriteCollection( collection , 0);
	spDeleteSurface( tile_map );
	spQuitCore();
	return 0;
}
