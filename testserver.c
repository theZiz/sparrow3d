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
char received_lines[16][256];

int mom_line = 0;

void draw_function( void )
{
	spClearTarget( 0 );
	char buffer[512];
	int i;
	for (i = 0; i < 16; i++)
	{
		sprintf(buffer,"%2i: %s",i,received_lines[i]);
		spFontDrawMiddle( spGetWindowSurface()->w/2, spGetWindowSurface()->h*(i+1)/18, -1, buffer, font );
	}


	spFontDraw( 1, 1, -1, "[E] Exit", font );
	sprintf(buffer,"FPS: %i",spGetFPS());
	spFontDrawRight( spGetWindowSurface()->w-1, spGetWindowSurface()->h-font->maxheight, -1, buffer, font );
	spFlip();
}

spNetTCPServer server = NULL;
spNetTCPConnection connection = NULL;
SDL_Thread* oldThread = NULL;

int calc_function( Uint32 steps )
{
	if (connection == NULL)
	{
		connection = spNetAcceptTCP(server);
		if (connection)
		{
			spNetIP ip = spNetGetConnectionIP(connection);
			sprintf(received_lines[mom_line],"Connection from %i.%i.%i.%i",ip.address.ipv4_bytes[mom_line],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3]);
		}
	}
	if (connection != NULL)
	{
		SDL_Thread* thread;
		if ((thread = spNetReceiveTextUnblocked(connection,received_lines[(mom_line+1) & 15],256)) != oldThread)
		{
			oldThread = thread;
			/*int i;
			for (i = 0; i < 256; i++)
				if (received_lines[mom_line][i] == '\n')
				{
					received_lines[mom_line][i] = 0;
					break;
				}*/
			if (thread == (void*)(-1)) //Connection was closed :(
			{
				spNetCloseTCP(connection);
				connection = NULL;
			}
			else
				mom_line = (mom_line+1) & 15;			
		}
	}
	rotation += steps * 32;
	if ( spGetInput()->button[SP_BUTTON_SELECT] )
		return 1;
	return 0;
}

void resize(Uint16 w,Uint16 h)
{
	//Font Loading
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./font/Play-Bold.ttf", spFixedToInt(12 * spGetSizeFactor()));
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

	//Testing stuff ;)
	spNetIP ip = spNetResolve("www.google.de",80);
	printf("IP of google: %i.%i.%i.%i\n",ip.address.ipv4_bytes[0],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3]);
	char buffer[256];
	printf("Host of the IP of google:\"%s\"\n",spNetResolveHost(ip,buffer,256));
	printf("Open Connection to google\n");
	connection = spNetOpenClientTCP(ip);
	spNetSendText(connection,"GET /index.html\n");
	int res = 1;
	while (res==1)
	{
		res = spNetReceiveText(connection,buffer,256);
		printf("%s\n",buffer);
	}
	printf("Close Connection to google\n");
	spNetCloseTCP(connection);
	connection = NULL;
	//Real stuff
	int i;
	for (i = 0; i < 16; i++)
		sprintf(received_lines[i],"...");
	
	server = spNetOpenServerTCP(12345);
		
	//Setup
	screen = spCreateDefaultWindow();
	spSelectRenderTarget(screen);
	resize(screen->w,screen->h);
	spSetZSet(0);
	spSetZTest(0);
	
	spLoop( draw_function, calc_function, 10, resize, NULL );
	
	if (connection)
		spNetCloseTCP(connection);
	spNetCloseTCP(server);

	//Winter Wrap up, Winter Wrap up
	spFontDelete( font );
	spQuitNet();
	spQuitCore();
	return 0;
}
