/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_primitives.h"
#include <sparrow3d.h>
#include <strings.h>
int primitives_z = 1;
int primitives_blending = 0;

void init_primitives()
{
}

char* caption_primitives(char* caption)
{
	sprintf(caption,"Primitives");
	return caption;
}

char* settings_primitives(char* caption,int button)
{
	switch (button)
	{
		case SP_BUTTON_A:
			if (primitives_z)
				sprintf(caption,"[A] Z Set & Test");
			else
				sprintf(caption,"[A] No Z Test & Set");
			break;
		case SP_BUTTON_B:
			if (primitives_blending)
				sprintf(caption,"[B] Blending on");
			else
				sprintf(caption,"[B] Blending off");
			break;
	}
	return caption;
}

void draw_primitives(int rotation)
{
	spSetZTest( primitives_z );
	spSetZSet( primitives_z );
	spSetBlending( primitives_blending?spFloatToFixed(0.5):SP_ONE );
	SDL_Surface* screen = spGetWindowSurface();
	srand( 0 );
	int i;
	for ( i = 0; i < 8; i++ )
		spEllipseBorder( rand() % screen->w, rand() % screen->h, 0, rand() % screen->w / 4, rand() % screen->h / 4, 10, 20, rand() % 65536 );
	for ( i = 0; i < 8; i++ )
		spEllipse( rand() % screen->w, rand() % screen->h, 0, rand() % screen->w / 4, rand() % screen->h / 4, rand() % 65536 );
	for ( i = 0; i < 8; i++ )
		spRectangleBorder( rand() % screen->w, rand() % screen->h, 0,
							 rand() % screen->w / 2, rand() % screen->h / 2, 12, 6, rand() % 65536 );
	for ( i = 0; i < 8; i++ )
		spRectangle( rand() % screen->w, rand() % screen->h, 0,
					 rand() % screen->w / 2, rand() % screen->h / 2, rand() % 65536 );
	for ( i = 0; i < 64; i++ )
	{
		spSetLineWidth(rand()%8+1);
		spLine( rand() % screen->w, rand() % screen->h, 0,
		        rand() % screen->w, rand() % screen->h, 0, rand() % 65536 );
	}
	/*spSetLineWidth(5);
	spLine(292,113,0,29,192,0,12345);*/
	spSetLineWidth(1);
	spSetZTest( 1 );
	spSetZSet( 1 );
	spSetBlending( SP_ONE );
}

void calc_primitives()
{
	if ( spGetInput()->button[SP_BUTTON_A] )
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		primitives_z = 1-primitives_z;
	}
	if ( spGetInput()->button[SP_BUTTON_B] )
	{
		spGetInput()->button[SP_BUTTON_B] = 0;
		primitives_blending = 1-primitives_blending;
	}
}

void quit_primitives()
{
}
