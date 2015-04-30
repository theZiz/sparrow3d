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
#include <math.h>
#include <string.h>
#include "test_cube.h"
#include "test_tube.h"
#include "test_fill.h"
#include "test_mesh.h"
#include "test_primitives.h"
#include "test_sprites.h"
#include "test_gears.h"
#include "test_yinyang.h"
#include "test_target.h"
#include "test_text.h"
#include "test_mapping.h"
SDL_Surface *screen;
Sint32 rotation = 0;
spFontPointer font = NULL;
int pause = 0;
int threading = 0;
int test = 0;
int no_movement = 0;
char input[32] = "";
#define TEST_COUNT 11

void draw_test( void )
{
	/* This is in fact the "end" of the last (!) drawing function. Why am I doing
	 * this? If drawing in an extra thread is activated the drawing thread has the
	 * time of the last calc function to finish drawing, too. ;) */
	spSetZSet( 0 );
	spSetZTest( 0 );
	spSetAlphaTest( 1 );
	spFontDraw( 2, font-> maxheight+2, 0, "[L] Previous", font );
	spFontDrawRight( screen->w - 2 , font-> maxheight+2, 0, "[R] next", font );
	spFontDrawRight( screen->w - 2 , 2, 0, "[S] Exit", font );
	if (test != 10)
	{
		switch (spIsKeyboardPolled())
		{
			case 0: spFontDraw( 2, 2, 0, "[E] Enter Text", font ); break;
			case 1: spFontDraw( 2, 2, 0, "[E] Finish Text", font ); break;
		}
		switch (threading)
		{
			case 0: spFontDrawRight( screen->w - 2, screen->h - 1*font-> maxheight, 0, "[X] No draw thread", font ); break;
			case 1: spFontDrawRight( screen->w - 2, screen->h - 1*font-> maxheight, 0, "[X] Extra draw thread", font ); break;
		}
		switch (pause)
		{
			case 0: spFontDrawRight( screen->w - 2, screen->h - 2*font-> maxheight, 0, "[Y] Pause", font ); break;
			case 1: spFontDrawRight( screen->w - 2, screen->h - 2*font-> maxheight, 0, "[Y] Unpause", font ); break;
		}
	}
	if (spIsKeyboardPolled() && spGetVirtualKeyboardState() == SP_VIRTUAL_KEYBOARD_ALWAYS)
		spFontDrawMiddle( screen->w /2, screen->h - font-> maxheight-spGetVirtualKeyboard()->h, 0, input, font );
	else
		spFontDrawMiddle( screen->w /2, screen->h - 2*font-> maxheight, 0, input, font );
	char buffer[256];
	switch ( test )
	{
		case 0:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_cube(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_cube(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_cube(buffer,SP_BUTTON_B), font);
			break;
		case 1:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_tube(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_tube(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_tube(buffer,SP_BUTTON_B), font);
			break;
		case 2:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_fill(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_fill(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_fill(buffer,SP_BUTTON_B), font);
			break;
		case 3:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_mesh(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_mesh(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_mesh(buffer,SP_BUTTON_B), font);
			break;
		case 4:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_primitives(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_primitives(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_primitives(buffer,SP_BUTTON_B), font);
			break;
		case 5:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_sprites(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_sprites(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_sprites(buffer,SP_BUTTON_B), font);
			break;
		case 6:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_gears(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_gears(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_gears(buffer,SP_BUTTON_B), font);
			break;
		case 7:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_yinyang(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_yinyang(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_yinyang(buffer,SP_BUTTON_B), font);
			break;
		case 8:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_target(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_target(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_target(buffer,SP_BUTTON_B), font);
			break;
		case 9:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_text(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_text(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_text(buffer,SP_BUTTON_B), font);
			break;
		case 10:
			spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, caption_mapping(buffer), font );
			spFontDraw( 2, screen->h - 1*font->maxheight,0, settings_mapping(buffer,SP_BUTTON_A), font);
			spFontDraw( 2, screen->h - 2*font->maxheight,0, settings_mapping(buffer,SP_BUTTON_B), font);
			break;
	}

	sprintf( buffer, "fps: %i", spGetFPS() );
	spFontDrawMiddle( screen->w/2, 1, 0, buffer, font );
	if (spIsKeyboardPolled() && spGetVirtualKeyboardState() == SP_VIRTUAL_KEYBOARD_ALWAYS)
		spBlitSurface(screen->w/2,screen->h-spGetVirtualKeyboard()->h/2,0,spGetVirtualKeyboard());
	spFlip();
	spResetZBuffer();
	spClearTarget( spGetRGB(64,64,64) );
	spIdentity();

	spSetLightPosition(0,spFloatToFixed( 0.875f ),spFloatToFixed( 0.875f ),spFloatToFixed( 0.875f ));
	spSetLightColor(0,SP_ONE,SP_ONE,SP_ONE);
	spSetZSet( 1 );
	spSetZTest( 1 );
	switch ( test )
	{
	case 10:
		draw_mapping(rotation,font);
		break;
	case 9:
		draw_text(rotation,font);
		break;
	case 8:
		draw_target(rotation);
		break;
	case 7:
		draw_yinyang(rotation);
		break;
	case 6:
		draw_gears(rotation);
		break;
	case 5:
		draw_sprites(rotation);
		break;
	case 4:
		draw_primitives(rotation);
		break;
	case 3:
		draw_mesh(rotation);
		break;
	case 2:
		draw_fill(rotation);
		break;
	case 1:
		draw_tube(rotation);
		break;
	case 0:
		draw_cube(rotation);
		break;
	}
}


int calc_test( Uint32 steps )
{
	if ( spGetInput()->button[SP_BUTTON_START] )
		return 1;
	int i;
	if (no_movement)
		rotation = 20000;//SP_PI;
	else
		rotation += steps*32;

	if ( test!=10 && spIsKeyboardPolled())
	{
		if ( spGetInput()->button[SP_BUTTON_SELECT] )
		{
			spGetInput()->button[SP_BUTTON_SELECT] = 0;
			spStopKeyboardInput();
		}
		return 0;
	}
	if ( test!=10 && spGetInput()->button[SP_BUTTON_X] )
	{
		spGetInput()->button[SP_BUTTON_X] = 0;
		threading = (threading+1) % 2;
		spDrawInExtraThread(threading);
	}
	if ( test!=10 && spGetInput()->button[SP_BUTTON_Y] )
	{
		spGetInput()->button[SP_BUTTON_Y] = 0;
		pause = 1-pause;
	}
	if (pause)
		rotation -= steps*32;

	if ( spGetInput()->button[SP_BUTTON_R] )
	{
		spGetInput()->button[SP_BUTTON_R] = 0;
		test = ( test + 1 ) % TEST_COUNT;
	}
	if ( spGetInput()->button[SP_BUTTON_L] )
	{
		spGetInput()->button[SP_BUTTON_L] = 0;
		test = ( test + TEST_COUNT -1 ) % TEST_COUNT;
	}
	switch (test)
	{
		case 0:
			calc_cube();
			break;
		case 1:
			calc_tube();
			break;
		case 2:
			calc_fill();
			break;
		case 3:
			calc_mesh();
			break;
		case 4:
			calc_primitives();
			break;
		case 5:
			calc_sprites(steps);
			break;
		case 6:
			calc_gears();
			break;
		case 7:
			calc_yinyang();
			break;
		case 8:
			calc_target(steps);
			break;
		case 9:
			calc_text(steps);
			break;
		case 10:
			calc_mapping(steps);
			break;
	}
	if ( spGetInput()->button[SP_BUTTON_SELECT] )
	{
		spGetInput()->button[SP_BUTTON_SELECT] = 0;
		spPollKeyboardInput(input,32,SP_BUTTON_A_MASK | SP_BUTTON_B_MASK);
	}

	return 0;
}

void resize( Uint16 w, Uint16 h )
{
	spSelectRenderTarget(spGetWindowSurface());
	//Settings up the onboard keyboard:
	if (spGetSizeFactor() <= SP_ONE)
		spSetVirtualKeyboard(SP_VIRTUAL_KEYBOARD_IF_NEEDED,0,h-w*48/320,w,w*48/320,spLoadSurface("./data/keyboard320.png"),spLoadSurface("./data/keyboardShift320.png"));
	else
	if (spGetSizeFactor() <= 2*SP_ONE)
		spSetVirtualKeyboard(SP_VIRTUAL_KEYBOARD_IF_NEEDED,0,h-w*48/320,w,w*48/320,spLoadSurface("./data/keyboard640.png"),spLoadSurface("./data/keyboardShift640.png"));
	else
		spSetVirtualKeyboard(SP_VIRTUAL_KEYBOARD_IF_NEEDED,0,h-w*48/320,w,w*48/320,spLoadSurface("./data/keyboard1280.png"),spLoadSurface("./data/keyboardShift1280.png"));

	//Setup of the new/resized window
	spSetPerspective( 45.0, ( float )spGetWindowSurface()->w / ( float )spGetWindowSurface()->h, 1.0f, 100.0f );

	//Font Loading
	spFontShadeButtons(1);
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./font/Play-Bold.ttf", spFixedToInt(10 * spGetSizeFactor()));
	spFontSetShadeColor(0);
	spFontAdd( font, SP_FONT_GROUP_ASCII, 65535 ); //whole ASCII
	spFontAdd( font, "äüöÄÜÖßẞ", 65535 ); //German stuff (same like spFontAdd( font, SP_FONT_GROUP_GERMAN, 0 ); )
	spFontAddBorder( font, 0 );
	spFontSetButtonStrategy(SP_FONT_BUTTON);
	spFontAddButton( font, 'A', SP_BUTTON_A_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'B', SP_BUTTON_B_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'X', SP_BUTTON_X_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'Y', SP_BUTTON_Y_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'L', SP_BUTTON_L_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'R', SP_BUTTON_R_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontSetButtonStrategy(SP_FONT_INTELLIGENT);
	spFontAddButton( font, 'S', SP_BUTTON_START_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'E', SP_BUTTON_SELECT_NAME, 65535, spGetRGB( 64, 64, 64 ) );
}

void eventHandling(SDL_Event *event)
{
	if (event->type == SDL_KEYDOWN)
	{
		int lastKey = event->key.keysym.unicode;
		char buffer[5];
		printf("keydown event 0x%x = \"%s\" keysym=%i\n",lastKey,spFontGetUTF8FromUnicode(lastKey,buffer,5),event->key.keysym.sym);
	}
}

int main( int argc, char **argv )
{
	if (argc > 1)
		test = atoi(argv[1]);
	//sparrow3D Init
	spSetDefaultWindowSize( 1024, 768 ); //Creates a 800x480 window at PC instead of 320x240
	spInitCore();

	//Setup
	screen = spCreateDefaultWindow();
	spUsePrecalculatedNormals(0);
	resize( screen->w, screen->h );

	char* parameter1 = NULL;
	char* parameter2 = NULL;
	if (argc > 2)
		parameter1 = argv[2];
	if (argc > 3)
		parameter2 = argv[3];

	init_cube();
	init_tube();
	init_fill();
	if (test == 3)
		init_mesh(parameter1,parameter2);
	else
		init_mesh(NULL,NULL);
	init_primitives();
	init_sprites();
	init_gears();
	init_yinyang();
	init_target();
	init_text(argc,argv,font);
	init_mapping();
	spSelectRenderTarget(screen);

	spSetAffineTextureHack(0);
	spSetLight(1);

	//All glory the main loop
	spLoop( draw_test, calc_test, 10, resize, eventHandling );

	//Winter Wrap up, Winter Wrap up z
	spFontDelete( font );
	quit_cube();
	quit_tube();
	quit_fill();
	quit_mesh();
	quit_primitives();
	quit_sprites();
	quit_gears();
	quit_yinyang();
	quit_target();
	quit_text();
	quit_mapping();
	spQuitCore();
	return 0;
}
