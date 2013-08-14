/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_fill.h"
#include <sparrow3d.h>
#include <strings.h>
int fill_z = 1;
int fill_light = 1;

void init_fill()
{

}

char* caption_fill(char* caption)
{
	sprintf(caption,"Fulfilling");
	return caption;
}

char* settings_fill(char* caption,int button)
{
	switch (button)
	{
		case SP_BUTTON_A:
			if (fill_z)
				sprintf(caption,"[A] Z Set & Test");
			else
				sprintf(caption,"[A] No Z Set & Test");
			break;
		case SP_BUTTON_B:
			if (fill_light)
				sprintf(caption,"[B] Light on");
			else
				sprintf(caption,"[B] Light off");
			break;
	}
	return caption;
}

void draw_fill(int rotation)
{
	spSetZSet( fill_z );
	spSetZTest( fill_z );
	spSetLight( fill_light );
	Sint32 matrix[16];
	spTranslate( 0, 0, spFloatToFixed( -11.0f ) + spSin( rotation * 4 ) * 3 );
	spRotateZ( rotation );
	spRotateX( spSin( rotation ) >> 2 );
	spRotateY( spCos( rotation * 2 ) >> 2 );
	int x, y;
	for ( x = -6; x <= 6; x++ )
		for ( y = -6; y <= 6; y++ )
			if ( x + y & 1 )
			{
				Sint32 matrix[16];
				memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) );
				spTranslate( spIntToFixed( x * 2 ), spIntToFixed( y * 2 ), 0 );
				spQuad3D( -SP_ONE, SP_ONE, 0,
							-SP_ONE, -SP_ONE, 0,
							SP_ONE, -SP_ONE, 0,
							SP_ONE, SP_ONE, 0, 32767 );
				memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) );
			}
	for ( x = -6; x <= 6; x++ )
		for ( y = -6; y <= 6; y++ )
			if ( !( x + y & 1 ) )
			{
				Sint32 matrix[16];
				memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) );
				spTranslate( spIntToFixed( x * 2 ), spIntToFixed( y * 2 ), 0 );
				spQuad3D( -SP_ONE, SP_ONE, spFloatToFixed( 2.0f ),
							-SP_ONE, -SP_ONE, spFloatToFixed( 2.0f ),
							SP_ONE, -SP_ONE, spFloatToFixed( 2.0f ),
							SP_ONE, SP_ONE, spFloatToFixed( 2.0f ), 65535 );
				//top
				//if (y<0)
				spQuad3D( -SP_ONE, SP_ONE, spFloatToFixed( 2.0f ),
							SP_ONE, SP_ONE, spFloatToFixed( 2.0f ),
							SP_ONE, SP_ONE, 0,
							-SP_ONE, SP_ONE, 0, 65535 );
				//bottom
				//if (y>0)
				spQuad3D( -SP_ONE, -SP_ONE, 0,
							SP_ONE, -SP_ONE, 0,
							SP_ONE, -SP_ONE, spFloatToFixed( 2.0f ),
							-SP_ONE, -SP_ONE, spFloatToFixed( 2.0f ), 65535 );
				//left
				//if (x>0)
				spQuad3D( -SP_ONE, -SP_ONE, spFloatToFixed( 2.0f ),
							-SP_ONE, SP_ONE, spFloatToFixed( 2.0f ),
							-SP_ONE, SP_ONE, 0,
							-SP_ONE, -SP_ONE, 0, 65535 );
				//right
				//if (x<0)
				spQuad3D( SP_ONE, -SP_ONE, 0,
							SP_ONE, SP_ONE, 0,
							SP_ONE, SP_ONE, spFloatToFixed( 2.0f ),
							SP_ONE, -SP_ONE, spFloatToFixed( 2.0f ), 65535 );
				memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) );
			}
	spSetZSet( 1 );
	spSetZTest( 1 );
	spSetLight( 1 );
}

void calc_fill()
{
	if ( spGetInput()->button[SP_BUTTON_A] )
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		fill_z = 1-fill_z;
	}
	if ( spGetInput()->button[SP_BUTTON_B] )
	{
		spGetInput()->button[SP_BUTTON_B] = 0;
		fill_light = 1-fill_light;
	}
}

void quit_fill()
{
}
