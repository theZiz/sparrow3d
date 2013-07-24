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
Sint32 rotation = 0;
spFontPointer font = NULL;
char input[256] = "";
spNetTCPConnection connection;

void draw_function( void )
{
	spClearTarget( 0 );
	spFontDrawMiddle( spGetWindowSurface()->w/2, (spGetWindowSurface()->h-font->maxheight-spGetVirtualKeyboard()->h)/2, -1, input, font );
	spFontDraw( 1, 1, -1, "[E] Exit\n[S] Send", font );
	char buffer[256];
	sprintf(buffer,"FPS: %i",spGetFPS());
	spFontDrawRight( spGetWindowSurface()->w-1, spGetWindowSurface()->h-font->maxheight-spGetVirtualKeyboard()->h, -1, buffer, font );
	if (spIsKeyboardPolled() && spGetVirtualKeyboardState() == SP_VIRTUAL_KEYBOARD_ALWAYS)
		spBlitSurface(screen->w/2,screen->h-spGetVirtualKeyboard()->h/2,0,spGetVirtualKeyboard());
	spFlip();
}


int calc_function( Uint32 steps )
{
	rotation += steps * 32;
	if ( spGetInput()->button[SP_BUTTON_START] )
	{
		spGetInput()->button[SP_BUTTON_START] = 0;
		spNetSendTCP(connection,input,strlen(input)+1);
		input[0] = 0;
	}
	if ( spGetInput()->button[SP_BUTTON_SELECT] )
		return 1;
	return 0;
}

void resize(Uint16 w,Uint16 h)
{
	//Settings up the onboard keyboard:
	if (spGetSizeFactor() <= SP_ONE)
		spSetVirtualKeyboard(SP_VIRTUAL_KEYBOARD_IF_NEEDED,0,h-w*48/320,w,w*48/320,spLoadSurface("./data/keyboard320.png"),spLoadSurface("./data/keyboardShift320.png"));
	else
	if (spGetSizeFactor() <= 2*SP_ONE)
		spSetVirtualKeyboard(SP_VIRTUAL_KEYBOARD_IF_NEEDED,0,h-w*48/320,w,w*48/320,spLoadSurface("./data/keyboard640.png"),spLoadSurface("./data/keyboardShift640.png"));
	else
		spSetVirtualKeyboard(SP_VIRTUAL_KEYBOARD_IF_NEEDED,0,h-w*48/320,w,w*48/320,spLoadSurface("./data/keyboard1280.png"),spLoadSurface("./data/keyboardShift1280.png"));
		
	//Font Loading
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./font/Play-Bold.ttf", 12 * spGetSizeFactor() >> SP_ACCURACY );
	spFontAdd( font, SP_FONT_GROUP_ASCII, 0 ); //whole ASCII
	spFontAddBorder( font, 65535 );
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
	//spSetDefaultWindowSize( 640, 480 ); //Creates a 640x480 window at PC instead of 320x240
	spInitCore();
	spInitNet();
	printf("Init spNet\n");

	//Setup
	screen = spCreateDefaultWindow();
	spSelectRenderTarget(screen);
	resize(screen->w,screen->h);
	spSetZSet(0);
	spSetZTest(0);
	
	spNetIP ip;
	if (argc < 2)
		ip = spNetResolve("localhost",12345);
	else
	{
		printf("Connecting to %s\n",argv[1]);
		ip = spNetResolve(argv[1],12345);
	}
	connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
		sprintf(input,"No connection! I will crash!");
	
	spPollKeyboardInput(input,256,SP_BUTTON_RIGHT_MASK | SP_BUTTON_DOWN_MASK);

	spLoop( draw_function, calc_function, 10, resize, NULL );
	
	spNetCloseTCP(connection);

	//Winter Wrap up, Winter Wrap up
	spFontDelete( font );
	spQuitNet();
	spQuitCore();
	return 0;
}
