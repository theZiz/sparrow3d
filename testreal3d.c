/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include <string.h>
#include "sparrow3d.h"

//#define SCALE_UP

#define DISTANCE 0.06f
#define Z0 5.0f
#define MIN_Z (-SP_ONE*3/2)
#define MAX_Z (-SP_ONE*10)

spFontPointer font = NULL;
Sint32 left_projection[16];
Sint32 right_projection[16];
SDL_Surface* right_screen;
#ifdef SCALE_UP
SDL_Surface* real_screen;
#endif
SDL_Surface* screen = NULL;
SDL_Surface *sparrow;
Sint32 rotation;
Uint16 leftColor;
Uint16 rightColor;
int crossedEyes = 0;

void resize( Uint16 w, Uint16 h )
{
	#ifdef SCALE_UP
		if (screen)
			spDeleteSurface(screen);
		screen = spCreateSurface(real_screen->w/2,real_screen->h/2);
	#endif
	if (right_screen)
		spDeleteSurface(right_screen);
	right_screen = spCreateSurface(screen->w,screen->h);

	spStereoCreateProjectionMatrixes( left_projection, right_projection, 45.0, ( float )screen->w / ( float )screen->h, 1.0, 100.0f, Z0, DISTANCE , crossedEyes);

	int scale = 0;
	#ifdef SCALE_UP
		scale++;
	#endif
	//Font Loading
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./font/StayPuft.ttf", 17 * spGetSizeFactor() >> SP_ACCURACY+scale );
	spFontAdd( font, SP_FONT_GROUP_ASCII,              rightColor | leftColor); //whole ASCII
	spFontAddButton( font, 'R', SP_BUTTON_START_NAME,  rightColor | leftColor, SP_ALPHA_COLOR ); //Return == START
	spFontAddButton( font, 'B', SP_BUTTON_SELECT_NAME, rightColor | leftColor, SP_ALPHA_COLOR ); //Backspace == SELECT
	spFontAddButton( font, 'q', SP_BUTTON_L_NAME,      rightColor | leftColor, SP_ALPHA_COLOR ); // q == L
	spFontAddButton( font, 'e', SP_BUTTON_R_NAME,      rightColor | leftColor, SP_ALPHA_COLOR ); // e == R
	spFontAddButton( font, 'a', SP_BUTTON_LEFT_NAME,   rightColor | leftColor, SP_ALPHA_COLOR ); //a == left button
	spFontAddButton( font, 'd', SP_BUTTON_RIGHT_NAME,  rightColor | leftColor, SP_ALPHA_COLOR ); // d == right button
	spFontAddButton( font, 'w', SP_BUTTON_UP_NAME,     rightColor | leftColor, SP_ALPHA_COLOR ); // w == up button
	spFontAddButton( font, 's', SP_BUTTON_DOWN_NAME,   rightColor | leftColor, SP_ALPHA_COLOR ); // s == down button
}

void draw_test(void)
{
	Sint32* modellViewMatrix=spGetMatrix();
	int eye;
	for (eye = 0; eye < 2; eye++)
	{
		Uint16 color;
		spIdentity();
		switch (eye)
		{
			case 0:
				//LEFT
				spSelectRenderTarget(screen);
				memcpy(spGetProjectionMatrix(), left_projection,sizeof(Sint32)*16);
				color = leftColor;
				break;
			case 1:
				//RIGHT
				spSelectRenderTarget(right_screen);
				memcpy(spGetProjectionMatrix(),right_projection,sizeof(Sint32)*16);
				color = rightColor;
				break;
		}	
		spResetZBuffer();
		spClearTarget( 0 );
		spSetZSet(1);
		spSetZTest(1);
		spSetAlphaTest(1);
		spSetLight(1);
		
		spTranslate(0,0, MIN_Z+( MIN_Z+MAX_Z >> 1 ));
		spBindTexture( sparrow );
		
		spRotateZ(SP_PI/4);

		int i;
		for (i = 0; i < 6; i++)
		{
			Sint32 matrix[16];
			memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) ); //glPush()
			Sint32 rot = rotation;
			rot += i*SP_PI/3;
			spTranslate(0,spMul(spCos(rot),MIN_Z+MAX_Z >> 4),spMul(spSin(rot),MIN_Z+MAX_Z >> 1));
			spRotateX( rotation );
			spRotateY( rotation );
			spRotateZ( rotation );
			spScale(1 << SP_ACCURACY -3,1 << SP_ACCURACY -3,1 << SP_ACCURACY -3);
			spQuadTex3D( -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, sparrow->h - 1,
						 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, 0,
						 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, sparrow->w - 1, 0,
						 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, sparrow->w - 1, sparrow->h - 1, color );
			spQuadTex3D( 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, sparrow->h - 1,
						 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
						 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, sparrow->w - 1, 0,
						 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, sparrow->w - 1, sparrow->h - 1, color );
			//Left / Right
			spQuadTex3D( -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, sparrow->h - 1,
						 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
						 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, sparrow->w - 1, 0,
						 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, sparrow->w - 1, sparrow->h - 1, color );
			spQuadTex3D( 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, sparrow->h - 1,
						 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
						 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, sparrow->w - 1, 0,
						 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, sparrow->w - 1, sparrow->h - 1, color );
			//Up / Down
			spQuadTex3D( 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, sparrow->h - 1,
						 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
						 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, sparrow->w - 1, 0,
						 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, sparrow->w - 1, sparrow->h - 1, color );
			spQuadTex3D( -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, sparrow->h - 1,
						 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
						 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, sparrow->w - 1, 0,
						 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, sparrow->w - 1, sparrow->h - 1, color );
			memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) ); //glPop()
		}
	}

	spStereoMergeSurfaces(screen,right_screen,crossedEyes);
	spSelectRenderTarget(screen);
	char buffer[256];
	sprintf(buffer,"FPS: %i",spGetFPS());
	spFontDrawRight(screen->w-2,screen->h-2-font->maxheight,1, buffer, font );
	#ifdef SCALE_UP
	spScale2XSmooth(screen,real_screen);
	#endif
	spFlip();
}

int calc_test(Uint32 steps)
{
	rotation += steps << SP_ACCURACY - 11;
	PspInput engineInput = spGetInput();
	if (engineInput->button[SP_BUTTON_START])
		return 1;
	return 0;
}

int main(int argc, char **argv)
{
	if (crossedEyes)
	{
		leftColor  = spGetFastRGB(255,255,255);
		rightColor = spGetFastRGB(255,255,255);
	}
	else
	{
		leftColor  = spGetFastRGB(255,  0,  0);
		rightColor = spGetFastRGB(  0,200,200);
	}
	spInitCore();
	//Setup
	#ifdef SCALE_UP
	real_screen = spCreateDefaultWindow();
	resize( real_screen->w, real_screen->h );
	#else
	screen = spCreateDefaultWindow();
	resize( screen->w, screen->h );
	#endif
	sparrow = spLoadSurface( "./data/sparrow.png" );
	spLoop(draw_test,calc_test,10,resize,NULL);
	spDeleteSurface(sparrow);
	#ifdef SCALE_UP
	spDeleteSurface(screen);
	#endif
	spQuitCore();
	return 0;
}
