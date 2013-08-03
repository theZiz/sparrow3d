/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_tube.h"
#include <sparrow3d.h>
#include <strings.h>

SDL_Surface* tube_garfield;
SDL_Surface* tube_pepper;
int tube_z = 1;
int tube_light = 1;

void init_tube()
{
	tube_garfield = spLoadSurface( "./data/garfield.png" );
	tube_pepper = spLoadSurface( "./data/pepper.png" );	
}

char* caption_tube(char* caption)
{
	sprintf(caption,"3D Tube");
	return caption;
}

char* settings_tube(char* caption,int button)
{
	switch (button)
	{
		case SP_BUTTON_A:
			if (tube_z)
				sprintf(caption,"[A] Z Set & Test");
			else
				sprintf(caption,"[A] No Z Set & Test");
			break;
		case SP_BUTTON_B:
			if (tube_light)
				sprintf(caption,"[B] Light on");
			else
				sprintf(caption,"[B] Light off");
			break;
	}
	return caption;
}

void draw_tube(int rotation)
{
	spSetZSet( tube_z );
	spSetZTest( tube_z );
	spSetLight( tube_light );
	spBindTexture( tube_garfield );
	spSetAlphaTest( 1 );
	spTranslate( 0, 0, spFloatToFixed( -15.0f ));
	spRotateY( rotation );
	int a;
	for ( a = 0; a < 16; a++ )
	{
		spRotateY( SP_PI / 8 );
		Sint32 brightness = spMul( spCos( rotation + a * SP_PI / 8 ), abs( spCos( rotation + a * SP_PI / 8 )) ) / 2 + ( spFloatToFixed( 1.5f ) );
		Uint16 color = spGetRGB(brightness >> 8,brightness >> 8,brightness >> 8);
		int y;
		for ( y = -21; y <= 21; y += 7 )
		{
			if ( ( y + a ) & 8 )
				spQuadTex3D( -spFloatToFixed( 0.75f ), spIntToFixed( y ) / 4 + spFloatToFixed( 0.75f ), spFloatToFixed( 4.5f ), SP_FONT_EXTRASPACE, SP_FONT_EXTRASPACE,
							 -spFloatToFixed( 0.75f ), spIntToFixed( y ) / 4 - spFloatToFixed( 0.75f ), spFloatToFixed( 4.5f ), 1, tube_garfield->h - SP_FONT_EXTRASPACE - 1,
							 spFloatToFixed( 0.75f ), spIntToFixed( y ) / 4 - spFloatToFixed( 0.75f ), spFloatToFixed( 4.5f ), tube_garfield->w - SP_FONT_EXTRASPACE - 1, tube_garfield->h - SP_FONT_EXTRASPACE - 1,
							 spFloatToFixed( 0.75f ), spIntToFixed( y ) / 4 + spFloatToFixed( 0.75f ), spFloatToFixed( 4.5f ), tube_garfield->w - SP_FONT_EXTRASPACE - 1, SP_FONT_EXTRASPACE, color );

			else if ( ( y + a + 1 ) & 8 )
				spRectangle3D( 0, spIntToFixed( y ) / 4, spFloatToFixed( 4.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), SDL_GetTicks() / 128 );
			else if ( ( y + a + 2 ) & 8 )
				spEllipse3D( 0, spIntToFixed( y ) / 4, spFloatToFixed( 4.5f ), spFloatToFixed( 0.75f ), spFloatToFixed( 0.75f ), -SDL_GetTicks() / 128 );
			else if ( ( y + a + 3 ) & 8 )
				spRectangleBorder3D( 0, spIntToFixed( y ) / 4, spFloatToFixed( 4.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 0.25 ), spFloatToFixed( 0.25 ), SDL_GetTicks() / 64 );
			else if ( ( y + a + 4 ) & 8 )
				spEllipseBorder3D( 0, spIntToFixed( y ) / 4, spFloatToFixed( 4.5f ), spFloatToFixed( 0.75f ), spFloatToFixed( 0.75f ), spFloatToFixed( 0.25 ), spFloatToFixed( 0.25 ), -SDL_GetTicks() / 64 );
			else if ( ( y + a + 5 ) & 8 )
				spRotozoomSurface3D( 0, spIntToFixed( y ) / 4, spFloatToFixed( 4.5f ), tube_pepper, spSin( rotation * 4 ) + ( spFloatToFixed( 1.5f ) ), spCos( rotation * 8 ) + ( spFloatToFixed( 1.5f ) ), rotation );
			else
				spQuad3D( -spFloatToFixed( 0.75f ), spIntToFixed( y ) / 4 + spFloatToFixed( 0.75f ), spFloatToFixed( 4.5f ),
							-spFloatToFixed( 0.75f ), spIntToFixed( y ) / 4 - spFloatToFixed( 0.75f ), spFloatToFixed( 4.5f ),
							spFloatToFixed( 0.75f ), spIntToFixed( y ) / 4 - spFloatToFixed( 0.75f ), spFloatToFixed( 4.5f ),
							spFloatToFixed( 0.75f ), spIntToFixed( y ) / 4 + spFloatToFixed( 0.75f ), spFloatToFixed( 4.5f ), color );

		}
	}
	spSetZSet( 1 );
	spSetZTest( 1 );
	spSetLight( 1 );
}

void calc_tube()
{
	if ( spGetInput()->button[SP_BUTTON_A] )
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		tube_z = 1-tube_z;
	}
	if ( spGetInput()->button[SP_BUTTON_B] )
	{
		spGetInput()->button[SP_BUTTON_B] = 0;
		tube_light = 1-tube_light;
	}
}

void quit_tube()
{
	spDeleteSurface( tube_garfield );
	spDeleteSurface( tube_pepper );
}
