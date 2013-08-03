/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_cube.h"
#include <sparrow3d.h>
#include <strings.h>
SDL_Surface *cube_texture;
SDL_Surface *cube_check;
int cube_light = 1;
int cube_perspective = 1;

void init_cube()
{
	cube_texture = spLoadSurface( "./data/garfield.png" );
	cube_check = spLoadSurfaceZoom( "./data/check.png",spFloatToFixed(1.0f));
}

char* caption_cube(char* caption)
{
	sprintf(caption,"Rotating Cube");
	return caption;
}

char* settings_cube(char* caption,int button)
{
	switch (button)
	{
		case SP_BUTTON_A:
			if (cube_perspective)
				sprintf(caption,"[A] Perspective fix");
			else
				sprintf(caption,"[A] Wobbly texture");
			break;
		case SP_BUTTON_B:
			if (cube_light)
				sprintf(caption,"[B] Light on");
			else
				sprintf(caption,"[B] Light off");
			break;
	}
	return caption;
}

void draw_cube(int rotation)
{
	spSetPerspectiveTextureMapping( cube_perspective );
	spSetLight( cube_light );
	spSetAlphaTest( 0 );
	/*spSetPattern8(0b10101010,
								0b11011101,
								0b10101010,
								0b01010101,
								0b10101010,
								0b11011101,
								0b10101010,
								0b01010101);*/
	spSetPattern8(170,//0b10101010, //0b doesn't work with older gcc versions
								 85,//0b01010101,
								170,//0b10101010,
								 85,//0b01010101,
								170,//0b10101010,
								 85,//0b01010101,
								170,//0b10101010,
								 85);//0b01010101);
	spSetAlphaPattern4x4(127,0);
	/*spSetPattern8(0b11111111,
								0b11000111,
								0b10111011,
								0b01101101,
								0b01010101,
								0b01101101,
								0b10111011,
								0b11000111);*/
	spTranslate( spSin( rotation / 3 ), spSin( rotation / 5 ), spFloatToFixed( -7.0f ) );
	spRotateX( rotation );
	spRotateY( rotation );
	spRotateZ( rotation );

	//Front / Back
	/*Uint16 color1 = 12345 | 31727;
	Uint16 color2 = 23456 | 31727;
	Uint16 color3 = 34567 | 31727;
	Uint16 color4 = 45678 | 31727;
	Uint16 color5 = 56789 | 31727;
	Uint16 color6 = 61234 | 31727;*/
	Uint16 color1 = 0xFFFF;
	Uint16 color2 = 0xFFFF;
	Uint16 color3 = 0xFFFF;
	Uint16 color4 = 0xFFFF;
	Uint16 color5 = 0xFFFF;
	Uint16 color6 = 0xFFFF;
	spBindTexture( cube_texture );
	spQuadTex3D( -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, cube_texture->h - 1,
				 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, 0,
				 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), cube_texture->w - 1, 0,
				 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), cube_texture->w - 1, cube_texture->h - 1, color1 );
	spQuadTex3D( spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, cube_texture->h - 1,
				 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
				 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), cube_texture->w - 1, 0,
				 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), cube_texture->w - 1, cube_texture->h - 1, color2 );
	//Left / Right
	spQuadTex3D( -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, cube_texture->h - 1,
				 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
				 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), cube_texture->w - 1, 0,
				 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), cube_texture->w - 1, cube_texture->h - 1, color3 );
	spBindTexture( cube_check );
	spQuadTex3D( spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, cube_texture->h - 1,
				 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
				 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), cube_texture->w - 1, 0,
				 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), cube_texture->w - 1, cube_texture->h - 1, color4 );
	//Up / Down
	spQuadTex3D( spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, cube_texture->h - 1,
				 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
				 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), cube_texture->w - 1, 0,
				 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), cube_texture->w - 1, cube_texture->h - 1, color5 );
	spQuadTex3D( -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, cube_texture->h - 1,
				 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
				 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), cube_texture->w - 1, 0,
				 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), cube_texture->w - 1, cube_texture->h - 1, color6 );
				 
	spSetAlphaPattern4x4((spSin(rotation) + SP_ONE) / 512 % 512,8);
	//Front / Back
	spTranslate( spFloatToFixed ( -3.0f ), 0, 0 );
	spQuad3D( -SP_ONE, SP_ONE, SP_ONE,
				-SP_ONE, -SP_ONE, SP_ONE,
				SP_ONE, -SP_ONE, SP_ONE,
				SP_ONE, SP_ONE, SP_ONE, spGetRGB(spFixedToInt((spSin(rotation*4)+SP_ONE)*127),spFixedToInt((spSin(rotation*2)+SP_ONE)*127),spFixedToInt((spSin(rotation)+SP_ONE)*127)) );
	spQuad3D( SP_ONE, SP_ONE, -SP_ONE,
				SP_ONE, -SP_ONE, -SP_ONE,
				-SP_ONE, -SP_ONE, -SP_ONE,
				-SP_ONE, SP_ONE, -SP_ONE, spGetRGB(spFixedToInt((spSin(rotation*4)+SP_ONE)*127),spFixedToInt((spSin(rotation*2)+SP_ONE)*127),spFixedToInt((spSin(rotation)+SP_ONE)*127)) );
	//Left / Right
	spQuad3D( -SP_ONE, SP_ONE, SP_ONE,
				-SP_ONE, SP_ONE, -SP_ONE,
				-SP_ONE, -SP_ONE, -SP_ONE,
				-SP_ONE, -SP_ONE, SP_ONE, spGetRGB(spFixedToInt((spSin(rotation*4)+SP_ONE)*127),spFixedToInt((spSin(rotation*2)+SP_ONE)*127),spFixedToInt((spSin(rotation)+SP_ONE)*127)) );
	spQuad3D( SP_ONE, -SP_ONE, SP_ONE,
				SP_ONE, -SP_ONE, -SP_ONE,
				SP_ONE, SP_ONE, -SP_ONE,
				SP_ONE, SP_ONE, SP_ONE, spGetRGB(spFixedToInt((spSin(rotation*4)+SP_ONE)*127),spFixedToInt((spSin(rotation*2)+SP_ONE)*127),spFixedToInt((spSin(rotation)+SP_ONE)*127)) );
	//Up / Down
	spQuad3D( SP_ONE, SP_ONE, SP_ONE,
				SP_ONE, SP_ONE, -SP_ONE,
				-SP_ONE, SP_ONE, -SP_ONE,
				-SP_ONE, SP_ONE, SP_ONE, spGetRGB(spFixedToInt((spSin(rotation*4)+SP_ONE)*127),spFixedToInt((spSin(rotation*2)+SP_ONE)*127),spFixedToInt((spSin(rotation)+SP_ONE)*127)) );
	spQuad3D( -SP_ONE, -SP_ONE, SP_ONE,
				-SP_ONE, -SP_ONE, -SP_ONE,
				SP_ONE, -SP_ONE, -SP_ONE,
				SP_ONE, -SP_ONE, SP_ONE, spGetRGB(spFixedToInt((spSin(rotation*4)+SP_ONE)*127),spFixedToInt((spSin(rotation*2)+SP_ONE)*127),spFixedToInt((spSin(rotation)+SP_ONE)*127)) );
	spDeactivatePattern();
	spSetPerspectiveTextureMapping(0);
	spSetLight( 1 );
}

void calc_cube()
{
	if ( spGetInput()->button[SP_BUTTON_A] )
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		cube_perspective = 1-cube_perspective;
	}
	if ( spGetInput()->button[SP_BUTTON_B] )
	{
		spGetInput()->button[SP_BUTTON_B] = 0;
		cube_light = 1-cube_light;
	}
}

void quit_cube()
{
	spDeleteSurface(cube_texture);
	spDeleteSurface(cube_check);
}
