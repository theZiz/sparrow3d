/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_gears.h"
#include <sparrow3d.h>
#include <strings.h>
int gears_light = 1;
int gears_blending = 0;
spModelPointer wheel[16];

void init_gears()
{
	int i;
	for (i = 0; i < 15; i++)
		wheel[i] = spMeshLoadObj( "./data/wheel.obj", NULL, spGetHSV(i*2*SP_PI/15,255,255));
}

char* caption_gears(char* caption)
{
	sprintf(caption,"Möbius Wheel");
	return caption;
}

char* settings_gears(char* caption,int button)
{
	switch (button)
	{
		case SP_BUTTON_A:
			if (gears_blending)
				sprintf(caption,"[A] Blending On");
			else
				sprintf(caption,"[A] Blending Off");
			break;
		case SP_BUTTON_B:
			if (gears_light)
				sprintf(caption,"[B] Light on");
			else
				sprintf(caption,"[B] Light off");
			break;
	}
	return caption;
}

void draw_gears(int rotation)
{
	spSetBlending( gears_blending?spFloatToFixed(0.5):SP_ONE );
	spSetLight( gears_light );
	spSetAlphaTest( 0 );
	spTranslate( 0, 0, spFloatToFixed( -8.0f ));
	spRotateX(rotation);
	spRotateY(rotation);
	spRotateZ(rotation);
	Sint32 matrix[32];

	memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) ); //glPush()
	spTranslate(0,spFloatToFixed( 2.0f ),0);
	spSetLightPosition(0,0,0,0);
	spSetLightColor(0,0,SP_ONE,SP_ONE);
	spEllipse3D(0,0,0,spFloatToFixed( 0.125f ),spFloatToFixed( 0.125f ),spGetFastRGB(0,255,255));

	spTranslate(0,spFloatToFixed( -4.0f ),0);
	spEnableLight(1,1);
	spSetLightPosition(1,0,0,0);
	spSetLightColor(1,SP_ONE,SP_ONE,0);
	spEllipse3D(0,0,0,spFloatToFixed( 0.125f ),spFloatToFixed( 0.125f ),spGetFastRGB(255,255,0));
	memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) ); //glPop()
	
	int i;
	for (i = 0; i < 15; i++)
	{
		memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) ); //glPush()
		
		spRotateY(SP_PI*i*4/15);
		spTranslate(0,0,spFloatToFixed( 2.1875f ));
		
		spRotateX(SP_PI*i/15);
		spRotateZ( (i & 1) ?rotation*4:(-rotation*4+SP_PI/11));
		spMesh3D( wheel[i], 1 );
		
		memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) ); //glPop()
	}
	spEnableLight(1,0);
	spSetBlending( SP_ONE );
	spSetLight( 1 );
}

void calc_gears()
{
	if ( spGetInput()->button[SP_BUTTON_A] )
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		gears_blending = 1-gears_blending;
	}
	if ( spGetInput()->button[SP_BUTTON_B] )
	{
		spGetInput()->button[SP_BUTTON_B] = 0;
		gears_light = 1-gears_light;
	}
}

void quit_gears()
{
	int i;
	for (i = 0; i < 15; i++)
		spMeshDelete(wheel[i]);
}
