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
#ifndef _WIN32
	#include <unistd.h>
#endif

SDL_Surface *screen;
spFontPointer font = NULL;
#define LINES 8
char received_lines[LINES][256];
int mom_line = 0;
char input[256] = "";
spNetTCPServer server = NULL;
SDL_Thread* oldThread = NULL;
spNetTCPConnection server_connection = NULL;
spNetTCPConnection client_connection;

void draw_function( void )
{
	spClearTarget( 0 );
	spFontDraw( 2, 2, -1, "[S] Exit", font );
	spFontDrawRight( spGetWindowSurface()->w-2, 2, -1, "[E] Send", font );
	spLine(2,font->maxheight+4,0,spGetWindowSurface()->w-2,font->maxheight+4,0,65535);
	spFontDrawMiddle( spGetWindowSurface()->w/2, spGetWindowSurface()->h*(3)/36, -1, "Server;", font );
	char buffer[512];
	int i;
	for (i = 0; i < LINES; i++)
	{
		sprintf(buffer,"%2i: %s",i,received_lines[i]);
		spFontDrawMiddle( spGetWindowSurface()->w/2, spGetWindowSurface()->h*(i+3)/18, -1, buffer, font );
	}
	spLine(2,spGetWindowSurface()->h*(LINES+3)/18+2,0,spGetWindowSurface()->w-2,spGetWindowSurface()->h*(LINES+3)/18+2,0,65535);
	spFontDrawMiddle( spGetWindowSurface()->w/2, spGetWindowSurface()->h*(LINES*2+7)/36, -1, "Client;", font );
	spFontDrawMiddle( spGetWindowSurface()->w/2, spGetWindowSurface()->h*(LINES+5)/18, -1, input, font );
	sprintf(buffer,"FPS: %i",spGetFPS());
	spFontDrawRight( spGetWindowSurface()->w-1, spGetWindowSurface()->h-font->maxheight-spGetVirtualKeyboard()->h, -1, buffer, font );
	if (spIsKeyboardPolled() && spGetVirtualKeyboardState() == SP_VIRTUAL_KEYBOARD_ALWAYS)
		spBlitSurface(screen->w/2,screen->h-spGetVirtualKeyboard()->h/2,0,spGetVirtualKeyboard());
	spFlip();
}

int calc_function( Uint32 steps )
{
	if ( spGetInput()->button[SP_BUTTON_SELECT] )
	{
		spGetInput()->button[SP_BUTTON_SELECT] = 0;
		spNetSendHTTP(client_connection,input);
		input[0] = 0;
	}
	if (server_connection == NULL)
	{
		server_connection = spNetAcceptTCP(server);
		if (server_connection)
		{
			spNetIP ip = spNetGetConnectionIP(server_connection);
			sprintf(received_lines[mom_line],"Connection from %i.%i.%i.%i",ip.address.ipv4_bytes[0],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3]);
		}
	}
	if (server_connection != NULL)
	{
		SDL_Thread* thread;
		if ((thread = spNetReceiveTCPUnblocked(server_connection,received_lines[(mom_line+1) % LINES],256)) != oldThread)
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
				spNetCloseTCP(server_connection);
				server_connection = NULL;
			}
			else
				mom_line = (mom_line+1) % LINES;			
		}
	}
	if ( spGetInput()->button[SP_BUTTON_START] )
		return 1;
	return 0;
}

void resize(Uint16 w,Uint16 h)
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
	spNetC4AProfilePointer profile = spNetC4AGetProfile();

	spNetC4ASetCaching(1);
	if (spNetC4ACommitScore(profile,"test",12345,NULL,10000) == 0)
	while (spNetC4AGetStatus() == SP_C4A_PROGRESS)
		#ifdef _WIN32
			Sleep(1);
		#else
			usleep(200);
		#endif
	//Testing stuff ;)
	spNetIP ip = spNetResolve("ziz.gp2x.de",80);
	printf("IP of ziz.gp2x.de: %i.%i.%i.%i\n",ip.address.ipv4_bytes[0],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3]);
	char buffer[256];
	printf("Host of the IP of ziz.gp2x.de:\"%s\"\n",spNetResolveHost(ip,buffer,256));
	printf("Open Connection to ziz.gp2x.de\n");
	server_connection = spNetOpenClientTCP(ip);
	spNetSendHTTP(server_connection,"GET /index.htm http/1.0\nHost: ziz.gp2x.de\n\n");
	int res = 1;
	while (res)
	{
		res = spNetReceiveHTTP(server_connection,buffer,255);
		buffer[res] = 0;
		printf("%s",buffer);
	}
	printf("Close Connection to ziz.gp2x.de\n");
	spNetCloseTCP(server_connection);
	server_connection = NULL;

	//C4A Example
	spNetC4AScorePointer score;
	if (profile)
	{
		printf("Your profile:\n");
		printf("Long Name: %s\n",profile->longname);
		printf("Short Name: %s\n",profile->shortname);
		printf("E-Mail: %s\n",profile->email);
		printf("Password: %s\n",profile->password);
	}
	else
		printf("No profile found. Put it to this folder or create it with compo4all!\n");
	
	//spNetC4AGetScoreOfMonth(&score,profile,"puzzletube_points",2013,6);
	spNetC4AGetScore(&score,profile,"puzzletube_points",10000);
	//Real stuff
	int i;
	for (i = 0; i < LINES; i++)
		sprintf(received_lines[i],"...");
	
	//Server setup
	server = spNetOpenServerTCP(12345);
		
	//Client setup
	if (argc < 2)
	{
		ip = spNetResolve("localhost",12345);
		printf("Connection to localhost\nUse ./testnet.sh SERVER to specify another server to connect to!\n");
	}
	else
	{
		printf("Connecting to %s\n",argv[1]);
		ip = spNetResolve(argv[1],12345);
	}
	client_connection = spNetOpenClientTCP(ip);
	if (client_connection == NULL)
		sprintf(input,"No client_connection! I will crash!");
	
	spPollKeyboardInput(input,256,SP_BUTTON_RIGHT_MASK | SP_BUTTON_DOWN_MASK);

	//Setup
	screen = spCreateDefaultWindow();
	resize(screen->w,screen->h);
	spSetZSet(0);
	spSetZTest(0);
	
	spLoop( draw_function, calc_function, 10, resize, NULL );
	
	spNetCloseTCP(client_connection);
	
	//After the loop because spNetC4AGetScore works in the background. ;)
	if (spNetC4AGetStatus() == SP_C4A_OK)
	{
		spNetC4AScorePointer mom = score;
		while (mom)
		{
			struct tm * local = localtime (&(mom->commitTime));
			printf("%2i.%2i.%i - %2i:%02i: %s (%s) - %i\n",local->tm_mday,local->tm_mon+1,local->tm_year+1900,local->tm_hour,local->tm_min,mom->longname,mom->shortname,mom->score);
			mom = mom->next;
		}
		spNetC4ADeleteScores(&score);
	}
	else
		printf("Fetshing Highscore is still running or failed with status code: %i\n",spNetC4AGetStatus());
	if (server_connection)
		spNetCloseTCP(server_connection);
	spNetCloseTCP(server);



	//Winter Wrap up, Winter Wrap up
	spFontDelete( font );
	spQuitNet();
	spQuitCore();
	return 0;
}
