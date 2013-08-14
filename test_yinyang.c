/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_yinyang.h"
#include <sparrow3d.h>
#include <strings.h>
SDL_Surface *yin;
SDL_Surface *yang;
int yinyang_light = 1;
int yinyang_perspective = 1;

void init_yinyang()
{
	yin = spLoadSurface( "./data/yinyang.png" );
	yang = spUniqueCopySurface( yin );
	spSelectRenderTarget(yin);
	spFloodFill(384,384,SP_ALPHA_COLOR);
	spFloodFill(256,128,SP_ALPHA_COLOR);
	spSelectRenderTarget(yang);
	spFloodFill(128,128,SP_ALPHA_COLOR);
	spFloodFill(256,384,SP_ALPHA_COLOR);}

char* caption_yinyang(char* caption)
{
	sprintf(caption,"Cosmos & stuff...");
	return caption;
}

char* settings_yinyang(char* caption,int button)
{
	switch (button)
	{
		case SP_BUTTON_A:
			if (yinyang_perspective)
				sprintf(caption,"[A] Perspective fix");
			else
				sprintf(caption,"[A] Wobbly texture");
			break;
		case SP_BUTTON_B:
			if (yinyang_light)
				sprintf(caption,"[B] Light on");
			else
				sprintf(caption,"[B] Light off");
			break;
	}
	return caption;
}

void draw_yinyang(int rotation)
{
	spSetPerspectiveTextureMapping( yinyang_perspective );
	spSetLight( yinyang_light );

	spSetBlending( spFloatToFixed(0.5f));
	spTranslate(0,0,spIntToFixed(-3));
	spRotateX( SP_PI/4 );
	spRotateY( rotation );
	Sint32 matrix[16];
	memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) ); //glPush()
	spRotateZ( - rotation * 4 );
	spBindTexture(yin);
	spQuadTex3D( - spFloatToFixed( 1.00f ),   spFloatToFixed( 1.00f ), 0,          1,          1,
							 - spFloatToFixed( 1.00f ), - spFloatToFixed( 1.00f ), 0,          1, yin->h - 1,
								 spFloatToFixed( 1.00f ), - spFloatToFixed( 1.00f ), 0, yin->w - 1, yin->h - 1,
								 spFloatToFixed( 1.00f ),   spFloatToFixed( 1.00f ), 0, yin->w - 1,          1, 65535);
	spQuadTex3D(   spFloatToFixed( 1.00f ),   spFloatToFixed( 1.00f ), 0, yin->w - 1,          1,
								 spFloatToFixed( 1.00f ), - spFloatToFixed( 1.00f ), 0, yin->w - 1, yin->h - 1,
							 - spFloatToFixed( 1.00f ), - spFloatToFixed( 1.00f ), 0,          1, yin->h - 1,
							 - spFloatToFixed( 1.00f ),   spFloatToFixed( 1.00f ), 0,          1,          1, 65535);
	memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) ); //glPop()

	memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) ); //glPush()
	spRotateX( SP_PI/2 );
	spRotateZ( - rotation * 4 );
	spBindTexture(yang);
	spQuadTex3D( - spFloatToFixed( 1.00f ),   spFloatToFixed( 1.00f ), 0,          1,          1,
							 - spFloatToFixed( 1.00f ), - spFloatToFixed( 1.00f ), 0,          1, yin->h - 1,
								 spFloatToFixed( 1.00f ), - spFloatToFixed( 1.00f ), 0, yin->w - 1, yin->h - 1,
								 spFloatToFixed( 1.00f ),   spFloatToFixed( 1.00f ), 0, yin->w - 1,          1, 65535);
	spQuadTex3D(   spFloatToFixed( 1.00f ),   spFloatToFixed( 1.00f ), 0, yin->w - 1,          1,
								 spFloatToFixed( 1.00f ), - spFloatToFixed( 1.00f ), 0, yin->w - 1, yin->h - 1,
							 - spFloatToFixed( 1.00f ), - spFloatToFixed( 1.00f ), 0,          1, yin->h - 1,
							 - spFloatToFixed( 1.00f ),   spFloatToFixed( 1.00f ), 0,          1,          1, 65535);
	memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) ); //glPop()
	spSetBlending( SP_ONE );

	spSetPerspectiveTextureMapping(0);
	spSetLight( 1 );
}

void calc_yinyang()
{
	if ( spGetInput()->button[SP_BUTTON_A] )
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		yinyang_perspective = 1-yinyang_perspective;
	}
	if ( spGetInput()->button[SP_BUTTON_B] )
	{
		spGetInput()->button[SP_BUTTON_B] = 0;
		yinyang_light = 1-yinyang_light;
	}
}

void quit_yinyang()
{
	spDeleteSurface(yin);
	spDeleteSurface(yang);
}
