 /* This file is part of sparrow3d.
  * Sparrow3d is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  *
  * Sparrow3d is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Foobar.  If not, see <http://www.gnu.org/licenses/>
  *
  * For feedback and questions about my Files and Projects please mail me,
  * Alexander Matthes (Ziz) , zizsdl_at_googlemail.com */

#include "sparrowCore.h"
#include "sparrowMath.h"
#include "sparrowPrimitives.h"
#include "sparrowMapping.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#include <windows.h>
#endif

#if defined CLOSEDDINGUX
	#define DOUBLEBUFFERING_BLIT_AND_FLIP
#endif

int spWindowX = 0;
int spWindowY = 0;
int spZoom;
SDL_Surface* spScreen; //the real screen
SDL_Surface* spWindow; //the thing we draw to
SDL_Joystick **spJoy = NULL;
char spDone;
int spFPS;
spInput spGenericInput;
int debug_time;
int sp_touchscreen_emulation = 0;
int sp_switch_button = -1;
int sp_ok_button = -1;
int sp_touchscreen_dx;
int sp_touchscreen_dy;
char sp_caching = 1;
int sp_axis_was_used[SP_INPUT_AXIS_COUNT];
SDL_Surface* spVirtualKeyboard[2] = {NULL,NULL};
SDL_Surface* spVirtualKeyboardInternal[2] = {NULL,NULL};
SDL_Surface* spVirtualKeyboardSelect[2] = {NULL,NULL};
int spVirtualKeyboardState = SP_VIRTUAL_KEYBOARD_NEVER;
Sint32 spVirtualKeyboardMask = 0;
Sint32 spVirtualKeyboardX = 5; //Q
Sint32 spVirtualKeyboardY = 0; //Q
Sint32 spVirtualKeyboardPositionX;
Sint32 spVirtualKeyboardPositionY;
Sint32 spVirtualKeyboardTime = 0;
Sint32 spVirtualKeyboardShift = 0;
//for long pressing of a key
SDL_keysym spLastKey = {0,(SDLKey)0,(SDLMod)0,0};
SDL_keysym spVirtualLastKey = {0,(SDLKey)0,(SDLMod)0,0};
int spLastKeyCountDown = 0;
int spVirtualLastKeyCountDown = 0;
char spVirtualKeyboardMap[3][20] =
	{{'\"','#','$','%','!','q','w','e','r','t','y','u','i','o','p', SDLK_BACKSPACE,'7','8','9','-'},
	 { '(',')','^','@','?', 1 ,'a','s','d','f','g','h','j','k','l',            '*','4','5','6','+'},
	 { '_',';',':',',','.','z','x','c','v',' ',' ',' ','b','n','m',            '0','1','2','3','='}};
char spVirtualKeyboardMapShift[3][20] =
	{{'~', '`','\'','&','|', 'Q','W','E','R','T','Y','U','I','O','P', SDLK_BACKSPACE,'7','8','9','-'},
	 {'[', ']', '{','}','?',  1 ,'A','S','D','F','G','H','J','K','L',            '*','4','5','6','+'},
	 {'/','\\', '<','>','\'','Z','X','C','V',' ',' ',' ','B','N','M',            '0','1','2','3','='}};
	 //1 is "shift"...
int spKeyboardReturnIgnore = 0;
int spKeyboardReturnStops = 0;
int spLastAxisType = 0; //digital
int spLastFirstTime = 0;
char spIconName[512] = "";
char spWindowName[512] = "";

int spVirtualKeyboardSpaceButton = -1;
int spVirtualKeyboardBackspaceButton = -1;

int __spMapDesktopHack;
int __spMapDesktopButton[SP_MAPPING_MAX];

typedef struct sp_cache_struct *sp_cache_pointer;
typedef struct sp_cache_struct {
	char* name;
	SDL_Surface* surface;
	Uint32 ref;
	Uint32 name_hash;
	Uint32 surface_hash;
	sp_cache_pointer prev;
	sp_cache_pointer next;
} sp_cache;

sp_cache_pointer sp_cache_name[SP_CACHE_SIZE];
sp_cache_pointer sp_cache_surface[SP_CACHE_SIZE];
sp_cache_pointer sp_first_cache_line = NULL;

int spCoreIsInitialized = 0;

PREFIX void spSetupWindowAttributes(char* title,char* iconName)
{
	if (title)
		sprintf(spWindowName,"%s",title);
	if (iconName)
		sprintf(spIconName,"%s",iconName);
}

PREFIX void spSetDefaultWindowSize( int w, int h )
{
	spWindowX = w;
	spWindowY = h;
}

PREFIX void spInitCore( void )
{
	if (spCoreIsInitialized)
		return;
	spCoreIsInitialized = 1;
	int i;

	debug_time = 0;
	TTF_Init();
#ifdef PANDORA
	spWindowX = 800;
	spWindowY = 480;
#elif defined MAEMO5
	spWindowX = 800;
	spWindowY = 480;
#elif defined MAEMO6
	spWindowX = 854;
	spWindowY = 480;
#elif defined GP2X
	//F100, F200, Open2x and WIZ
	spWindowX = 320;
	spWindowY = 240;
#elif defined CAANOO
	spWindowX = 320;
	spWindowY = 240;
#elif defined DINGUX
	spWindowX = 320;
	spWindowY = 240;
#elif defined GCW
	spWindowX = 320;
	spWindowY = 240;
#else
	//only setting, if now default value set!
	if ( !spWindowX )
		spWindowX = 320;
	if ( !spWindowY )
		spWindowY = 240;
#endif
	spZoom = SP_ONE;
	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO );
	if (spWindowName[0])
		SDL_WM_SetCaption(spWindowName,NULL);
	if (spIconName[0])
	{
		SDL_Surface* icon = IMG_Load(spIconName);
		if (icon)
			SDL_WM_SetIcon( icon, 0 );
		else
			printf("%s does not exist.\n",spIconName);
	}
	spJoy = NULL;
	//#ifdef MOBILE_DEVICE
	printf( "Found %i Joysticks\n", SDL_NumJoysticks() );
	if ( SDL_NumJoysticks() > 0 )
	{
		spJoy = ( SDL_Joystick** )malloc( SDL_NumJoysticks() * sizeof( SDL_Joystick* ) );
		for ( i = 0; i < SDL_NumJoysticks(); i++ )
		{
			spJoy[i] = SDL_JoystickOpen( i );
			if ( strcmp(SDL_JoystickName( i ),"VirtualBox USB Tablet") == 0 || strcmp(SDL_JoystickName( i ),"VirtualBox mouse integration") == 0)
			{
				printf( "  Ignored Joystick %i (%s) because of Virtualbox\n", i, SDL_JoystickName( i ) );
				SDL_JoystickClose( spJoy[i] );
				spJoy[i] = NULL;
			}
			else
				printf( "  Opened Joystick %i (%s)\n", i, SDL_JoystickName( i ) );
		}
	}
	//#endif
	spScreen = NULL;
	spWindow = NULL;
	spDone = 0;
	spFPS = 0;
	#ifdef CAANOO
		spLastAxisType = 1;
	#else
		spLastAxisType = 0;
	#endif
	spResetButtonsState();
	spResetAxisState();
	for (i = 0; i < SP_INPUT_AXIS_COUNT; i++)
		sp_axis_was_used[i] = 0;
	spGenericInput.touchscreen.pressed = 0;
	spGenericInput.touchscreen.x = 0;
	spGenericInput.touchscreen.y = 0;
	spGenericInput.keyboard.buffer = NULL;
	spGenericInput.keyboard.len = 0;
	spGenericInput.keyboard.pos = 0;
	spGenericInput.keyboard.lastSize = 0;

#ifdef GP2X
	//f100, f200, open2x and wiz
	spGenericInput.supports_keyboard = 0;
#elif defined CAANOO
	spGenericInput.supports_keyboard = 0;
#elif defined DINGUX
	spGenericInput.supports_keyboard = 0;
#elif defined GCW
	spGenericInput.supports_keyboard = 0;
#else // PANDORA and PCs
	spGenericInput.supports_keyboard = 1;
#endif

	spInitPrimitives();
	spInitMath();
	spInitMapping();
	memset(sp_cache_name,0,SP_CACHE_SIZE*sizeof(sp_cache_pointer));
	memset(sp_cache_surface,0,SP_CACHE_SIZE*sizeof(sp_cache_pointer));
}

PREFIX void spPrintDebug( char* text )
{
	Sint32 time = SDL_GetTicks();
	int time_diff = time - debug_time;
	if ( time_diff > 100 )
		printf( "%05i (%3i LONG!): %s\n", time, time_diff, text );
	else
		printf( "%05i (%3i): %s\n", time, time_diff, text );
	debug_time = time;
}

int spFullscreen = 1;
int spAllowResize = 0;

PREFIX void spResizeWindow( int x, int y, int fullscreen, int allowresize )
{
	spFullscreen = fullscreen;
	spAllowResize = allowresize;
	//if the renderTarget is the screen and the screen is recreated, the pointer
	//will be invalid. We have to check for that:
	int recallSelectRenderTarget = 0;
	if (spWindow && spWindow == spGetRenderTarget())
	{
		//the old target will be invalid in the end
		spSelectRenderTarget(NULL);
		recallSelectRenderTarget = 1;
	}
#if defined DOUBLEBUFFERING_BLIT || defined DOUBLEBUFFERING_BLIT_AND_FLIP
	spScreen = SDL_SetVideoMode( x, y, 16, SP_SURFACE_FLAGS | SDL_FULLSCREEN );
	SDL_Surface* surface = SDL_CreateRGBSurface( SP_SURFACE_FLAGS, x, y, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0 );
	spWindow = SDL_DisplayFormat( surface );
	SDL_FreeSurface( surface );
#else
	spScreen = NULL;
	#ifdef DESKTOP
		spWindow = SDL_SetVideoMode( x, y, 16, SDL_DOUBLEBUF | SP_SURFACE_FLAGS | ( allowresize ? SDL_RESIZABLE : 0 ) | ( fullscreen ? SDL_FULLSCREEN : 0 ) );
	#else
		spWindow = SDL_SetVideoMode( x, y, 16, SP_SURFACE_FLAGS | SDL_DOUBLEBUF | ( fullscreen ? SDL_FULLSCREEN : 0 ) );
	#endif
#endif
	if ( x % 2 != 0 )
		spWindowX = x + 1;
	else
		spWindowX = x ;
	spWindowY = y;
	spZoom = spMin( ( spWindowX << SP_ACCURACY ) / 320, ( spWindowY << SP_ACCURACY ) / 240 ); //at 320x240 == 1.0
	SDL_ShowCursor( SDL_DISABLE );
	if (recallSelectRenderTarget)
	  spSelectRenderTarget(spGetWindowSurface());
}

PREFIX SDL_Surface* spCreateWindow( int width, int height, int fullscreen, int allowresize )
{
	spResizeWindow( ( !width ? spWindowX : width ), ( !height ? spWindowY : height ), fullscreen, allowresize );
	return spWindow;
}

PREFIX SDL_Surface* spCreateDefaultWindow( void )
{
	#ifdef DESKTOP
		return spCreateWindow( 0, 0, 0, 1 );
	#else
		return spCreateWindow( 0, 0, 1, 1 );
	#endif
}

PREFIX SDL_Surface* spGetWindowSurface( void )
{
	return spWindow;
}

static void spInternalZoomBlit(SDL_Surface* source,int sx,int sy,int sw,int sh,SDL_Surface* dest,int dx,int dy,int dw,int dh)
{
	SDL_LockSurface(source);
	SDL_LockSurface(dest);
	Uint16* src = (Uint16*)source->pixels;
	Uint16* des = (Uint16*)dest->pixels;
	Sint32 add_x = (sw  << SP_ACCURACY)/dw;
	Sint32 add_y = (sh << SP_ACCURACY)/dh;
	int u,v;
	Sint32 X,Y=sy << SP_ACCURACY;
	Sint32 src_m = source->pitch/source->format->BytesPerPixel;
	Sint32 des_m = dest->pitch/dest->format->BytesPerPixel;
	for (v = dy; v < dy+dh; v++)
	{
		X = sx << SP_ACCURACY;
		for (u = dx; u < dx+dw; u++)
		{
			des[u+v*des_m] = src[(X>>SP_ACCURACY)+(Y>>SP_ACCURACY)*src_m];
			X+=add_x;
		}
		Y+=add_y;
	}
	SDL_UnlockSurface(source);
	SDL_UnlockSurface(dest);
}

static void spInternalUpdateVirtualKeyboard( void )
{
	SDL_Rect dest;
	dest.x = spVirtualKeyboardX * spVirtualKeyboardInternal[spVirtualKeyboardShift]->w / 20;
	dest.y = spVirtualKeyboardY * spVirtualKeyboardInternal[spVirtualKeyboardShift]->h / 3;
	dest.w = spVirtualKeyboardInternal[spVirtualKeyboardShift]->w/20;
	dest.h = spVirtualKeyboardInternal[spVirtualKeyboardShift]->h/3;
	SDL_BlitSurface( spVirtualKeyboardSelect[0], NULL, spVirtualKeyboard[spVirtualKeyboardShift], &dest );
}

static void spInternalResetVirtualKeyboard( void )
{
	SDL_BlitSurface( spVirtualKeyboardInternal[spVirtualKeyboardShift], NULL, spVirtualKeyboard[spVirtualKeyboardShift], NULL );
}

static void spInternalCleanVirtualKeyboard( void )
{
	SDL_Rect src,dest;
	dest.x = spVirtualKeyboardX * spVirtualKeyboardInternal[spVirtualKeyboardShift]->w / 20;
	dest.y = spVirtualKeyboardY * spVirtualKeyboardInternal[spVirtualKeyboardShift]->h / 3;
	dest.w = spVirtualKeyboardInternal[spVirtualKeyboardShift]->w/20;
	dest.h = spVirtualKeyboardInternal[spVirtualKeyboardShift]->h/3;
	src.x = spVirtualKeyboardX * spVirtualKeyboardInternal[spVirtualKeyboardShift]->w / 20;
	src.y = spVirtualKeyboardY * spVirtualKeyboardInternal[spVirtualKeyboardShift]->h / 3;
	src.w = spVirtualKeyboardInternal[spVirtualKeyboardShift]->w/20;
	src.h = spVirtualKeyboardInternal[spVirtualKeyboardShift]->h/3;
	SDL_BlitSurface( spVirtualKeyboardInternal[spVirtualKeyboardShift], &src, spVirtualKeyboard[spVirtualKeyboardShift], &dest );
}

static void spHandleKeyboardInput( const SDL_keysym pressedKey)
{
	if ( pressedKey.sym == SDLK_BACKSPACE )
	{
		if ( spGenericInput.keyboard.pos > 0 )
		{
			if ( spGenericInput.keyboard.lastSize == 0 ) // need to determine size of last char in buffer
			{
				int I;
				for ( I = strlen( spGenericInput.keyboard.buffer ) - 1; I >= 0; --I )
				{
					++spGenericInput.keyboard.lastSize;
					if ( !((Uint8)(spGenericInput.keyboard.buffer[I] & 128) == 128 && (Uint8)(spGenericInput.keyboard.buffer[I] & 64) == 0)) //no follower bit
						break;
				}
			}
			if ( spGenericInput.keyboard.lastSize > 0 )
			{
				spGenericInput.keyboard.pos -= spGenericInput.keyboard.lastSize;
				spGenericInput.keyboard.buffer[spGenericInput.keyboard.pos] = '\0';
				spGenericInput.keyboard.lastSize = 0;
			}
		}
	}
	else if ( pressedKey.sym == SDLK_RETURN )
	{
		if ( spGenericInput.keyboard.pos + 1 <= spGenericInput.keyboard.len && spKeyboardReturnIgnore == 0)
		{
			strcat( spGenericInput.keyboard.buffer, "\n" );
			spGenericInput.keyboard.lastSize = 1;
			spGenericInput.keyboard.pos += 1;
		}
		if (spKeyboardReturnStops)
			spStopKeyboardInput();
	}
	else if ( pressedKey.sym >= SDLK_SPACE )
	{
		Uint16 c = pressedKey.unicode;
		char temp[5];
		spFontGetUTF8FromUnicode( c, temp, 5 );
		int s = strlen( temp );
		if ( spGenericInput.keyboard.pos + s <= spGenericInput.keyboard.len )
		{
			strcat( spGenericInput.keyboard.buffer, temp );
			spGenericInput.keyboard.lastSize = s;
			spGenericInput.keyboard.pos += s;
		}
	}
}

inline static int spHandleEvent( void ( *spEvent )( SDL_Event *e ) )
{
	int result = 0;
#ifdef PANDORA
	/*int s = pnd_evdev_dpad_state ( pnd_evdev_dpads );
	if ( s & pnd_evdev_pandora )
	  spDone=1;*/
#endif
	char new_axis = 0;
#ifdef CORE_DEBUG
	int counter = 0;
#endif
	SDL_Event event;
	while ( SDL_PollEvent( &event ) == 1 )
	{
#ifdef CORE_DEBUG
		counter++;
		char buffer[32];
		sprintf( buffer, "    Fetching Event %i", counter );
		spPrintDebug( buffer );
#endif
		switch ( event.type )
		{
			case SDL_MOUSEBUTTONDOWN:
			  spGenericInput.touchscreen.pressed = 1;
			  spGenericInput.touchscreen.x = event.button.x;
			  spGenericInput.touchscreen.y = event.button.y;
			  break;
			case SDL_MOUSEBUTTONUP:
			  spGenericInput.touchscreen.pressed = 0;
			  break;
			case SDL_MOUSEMOTION:
				if (spGenericInput.touchscreen.pressed)
				{
					spGenericInput.touchscreen.x = event.motion.x;
					spGenericInput.touchscreen.y = event.motion.y;
				}
				break;
			case SDL_JOYBUTTONDOWN:
			  #ifdef F100
					if (event.jbutton.button == sp_switch_button ||
					   (sp_touchscreen_emulation && (event.jbutton.button == sp_ok_button ||
					   (event.jbutton.button >= SP_AXIS_UP && event.jbutton.button <= SP_AXIS_RIGHTUP))))
					{
						if (event.jbutton.button == sp_switch_button)
						  sp_touchscreen_emulation = 1-sp_touchscreen_emulation;
						if (sp_touchscreen_emulation)
						{
							//moving the cursor *not tested yet*
							switch (event.jbutton.button)
							{
								case SP_AXIS_RIGHT:case SP_AXIS_RIGHTDOWN:case SP_AXIS_RIGHTUP:
								  sp_touchscreen_dx = +1;
								  break;
								case SP_AXIS_LEFT:case SP_AXIS_LEFTDOWN:case SP_AXIS_LEFTUP:
								  sp_touchscreen_dx = -1;
								  break;
								default:
								  sp_touchscreen_dx = 0;
								  break;
							}
							switch (event.jbutton.button)
							{
								case SP_AXIS_RIGHTDOWN:case SP_AXIS_DOWN:case SP_AXIS_LEFTDOWN:
								  sp_touchscreen_dy = +1;
								  break;
								case SP_AXIS_RIGHTUP:case SP_AXIS_UP:case SP_AXIS_LEFTUP:
								  sp_touchscreen_dy = -1;
								  break;
								default:
								  sp_touchscreen_dy = 0;
								  break;
							}
							//clicking *not tested yet*
							if (event.jbutton.button == sp_ok_button)
							{
								SDL_Event newevent;
								newevent.button.type = SDL_MOUSEBUTTONDOWN;
								newevent.button.button = SDL_BUTTON_LEFT;
								newevent.button.state = SDL_PRESSED;
								newevent.button.x = spGenericInput.touchscreen.x;
								newevent.button.y = spGenericInput.touchscreen.y;
								SDL_PushEvent(&event);
							}
						}
					}
					else
			  #endif
				spGenericInput.button[event.jbutton.button] = 1;
				break;
			case SDL_JOYBUTTONUP:
			  #ifdef F100
					if (event.jbutton.button == sp_switch_button ||
					   (sp_touchscreen_emulation && (event.jbutton.button == sp_ok_button ||
					   (event.jbutton.button >= SP_AXIS_UP && event.jbutton.button <= SP_AXIS_RIGHTUP))))
					{
						//releasing delta movement
						if (sp_touchscreen_emulation)
						{
							//moving the cursor *not tested yet*
							switch (event.jbutton.button)
							{
								case SP_AXIS_RIGHT:case SP_AXIS_RIGHTDOWN:case SP_AXIS_RIGHTUP:
								  sp_touchscreen_dx = 0;
								  break;
								case SP_AXIS_LEFT:case SP_AXIS_LEFTDOWN:case SP_AXIS_LEFTUP:
								  sp_touchscreen_dx = 0;
								  break;
								default:
								  sp_touchscreen_dx = 0;
								  break;
							}
							switch (event.jbutton.button)
							{
								case SP_AXIS_RIGHTDOWN:case SP_AXIS_DOWN:case SP_AXIS_LEFTDOWN:
								  sp_touchscreen_dy = 0;
								  break;
								case SP_AXIS_RIGHTUP:case SP_AXIS_UP:case SP_AXIS_LEFTUP:
								  sp_touchscreen_dy = 0;
								  break;
								default:
								  sp_touchscreen_dy = 0;
								  break;
							}
							//clicking *not tested yet*
							if (event.jbutton.button == sp_ok_button)
							{
								SDL_Event newevent;
								newevent.button.type = SDL_MOUSEBUTTONUP;
								newevent.button.button = SDL_BUTTON_LEFT;
								newevent.button.state = SDL_RELEASED;
								newevent.button.x = spGenericInput.touchscreen.x;
								newevent.button.y = spGenericInput.touchscreen.y;
								SDL_PushEvent(&event);
							}
						}

					}
					else
			  #endif
				spGenericInput.button[event.jbutton.button] = 0;
				break;
			case SDL_KEYDOWN:
				if (__spMapDesktopHack)
					__spMapDesktopButton[event.key.keysym.sym] = 1;
				//GCW and DINGUX use the "keyboard" for buttons
				#if (!(defined GCW)) && (!(defined DINGUX))
					if ( spGenericInput.keyboard.buffer
					#ifdef DESKTOP
					&& spVirtualKeyboardState != SP_VIRTUAL_KEYBOARD_ALWAYS
					#endif
					)
					{
						spHandleKeyboardInput(event.key.keysym);
						spLastKey = event.key.keysym;
						spLastKeyCountDown = SP_KEYBOARD_FIRST_WAIT;
					}
				#endif
				switch ( event.key.keysym.sym )
				{
				case SDLK_LEFT:
					spGenericInput.axis[0] = -1;
					spGenericInput.analog_axis[0] = SP_ANALOG_AXIS_MIN;
					spLastAxisType = 0;
					break;
				case SDLK_RIGHT:
					spGenericInput.axis[0] = 1;
					spGenericInput.analog_axis[0] = SP_ANALOG_AXIS_MAX;
					spLastAxisType = 0;
					break;
				case SDLK_UP:
					spGenericInput.axis[1] = -1;
					spGenericInput.analog_axis[1] = SP_ANALOG_AXIS_MIN;
					spLastAxisType = 0;
					break;
				case SDLK_DOWN:
					spGenericInput.axis[1] = 1;
					spGenericInput.analog_axis[1] = SP_ANALOG_AXIS_MAX;
					spLastAxisType = 0;
					break;
			#ifdef DINGUX
				case SDLK_RETURN:
					spGenericInput.button[SP_BUTTON_START] = 1;
					break;
				case SDLK_SPACE:
					spGenericInput.button[SP_BUTTON_X] = 1;
					break;
				case SDLK_LSHIFT:
					spGenericInput.button[SP_BUTTON_Y] = 1;
					break;
				case SDLK_LCTRL:
					spGenericInput.button[SP_BUTTON_A] = 1;
					break;
				case SDLK_LALT:
					spGenericInput.button[SP_BUTTON_B] = 1;
					break;
				case SDLK_ESCAPE:
					spGenericInput.button[SP_BUTTON_SELECT] = 1;
					break;
				case SDLK_TAB:
					spGenericInput.button[SP_BUTTON_L] = 1;
					break;
				case SDLK_BACKSPACE:
					spGenericInput.button[SP_BUTTON_R] = 1;
					break;
			#elif defined GCW
				case SDLK_RETURN:
					spGenericInput.button[SP_BUTTON_START] = 1;
					break;
				case SDLK_SPACE:
					spGenericInput.button[SP_BUTTON_Y] = 1;
					break;
				case SDLK_LSHIFT:
					spGenericInput.button[SP_BUTTON_X] = 1;
					break;
				case SDLK_LCTRL:
					spGenericInput.button[SP_BUTTON_A] = 1;
					break;
				case SDLK_LALT:
					spGenericInput.button[SP_BUTTON_B] = 1;
					break;
				case SDLK_ESCAPE:
					spGenericInput.button[SP_BUTTON_SELECT] = 1;
					break;
				case SDLK_TAB:
					spGenericInput.button[SP_BUTTON_L] = 1;
					break;
				case SDLK_BACKSPACE:
					spGenericInput.button[SP_BUTTON_R] = 1;
					break;
			#elif defined PANDORA
//				case SDLK_MENU:
//					spDone=1;
//					break;
				case SDLK_PAGEDOWN:
					spGenericInput.button[SP_BUTTON_X] = 1;
					break;
				case SDLK_PAGEUP:
					spGenericInput.button[SP_BUTTON_Y] = 1;
					break;
				case SDLK_HOME:
					spGenericInput.button[SP_BUTTON_A] = 1;
					break;
				case SDLK_END:
					spGenericInput.button[SP_BUTTON_B] = 1;
					break;
				case SDLK_LCTRL:
					spGenericInput.button[SP_BUTTON_SELECT] = 1;
					break;
				case SDLK_RSHIFT:
					spGenericInput.button[SP_BUTTON_L] = 1;
					break;
				case SDLK_RCTRL:
					spGenericInput.button[SP_BUTTON_R] = 1;
					break;
				case SDLK_LALT:
					spGenericInput.button[SP_BUTTON_START] = 1;
					break;
			#else //PC
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					spGenericInput.button[SP_BUTTON_START] = 1;
					break;
					case SDLK_LCTRL:
						spGenericInput.button[SP_BUTTON_A_NOWASD] = 1;
						break;
					case SDLK_LALT:
						spGenericInput.button[SP_BUTTON_B_NOWASD] = 1;
						break;
					case SDLK_MENU:
						spGenericInput.button[SP_BUTTON_X_NOWASD] = 1;
						break;
					case SDLK_LSHIFT:case SDLK_RSHIFT:
						spGenericInput.button[SP_BUTTON_Y_NOWASD] = 1;
						break;
					case SDLK_ESCAPE:
						spGenericInput.button[SP_BUTTON_SELECT_NOWASD] = 1;
						break;
					case SDLK_PAGEUP:
						spGenericInput.button[SP_BUTTON_L_NOWASD] = 1;
						break;
					case SDLK_PAGEDOWN:
						spGenericInput.button[SP_BUTTON_R_NOWASD] = 1;
						break;
					case SDLK_a:
						spGenericInput.button[SP_BUTTON_A] = 1;
						break;
					case SDLK_d:
						spGenericInput.button[SP_BUTTON_B] = 1;
						break;
					case SDLK_w:
						spGenericInput.button[SP_BUTTON_Y] = 1;
						break;
					case SDLK_s:
						spGenericInput.button[SP_BUTTON_X] = 1;
						break;
					case SDLK_BACKSPACE:
						spGenericInput.button[SP_BUTTON_SELECT] = 1;
						break;
					case SDLK_q:
						spGenericInput.button[SP_BUTTON_L] = 1;
						break;
					case SDLK_e:
						spGenericInput.button[SP_BUTTON_R] = 1;
						break;
			#endif
				}
				break;
			case SDL_KEYUP:
				if (__spMapDesktopHack)
					__spMapDesktopButton[event.key.keysym.sym] = 0;
				//GCW and DINGUX use the "keyboard" for buttons
				#if (!(defined GCW)) && (!(defined DINGUX))
					if ( spGenericInput.keyboard.buffer )
					{
						spLastKey.unicode = 0;
						spLastKeyCountDown = 0;
					}
				#endif
				#ifdef PANDORA
				if (event.key.keysym.scancode == 147)
				{
					if (spFullscreen)
						spResizeWindow(790,460,0,spAllowResize);
					else
						spResizeWindow(800,480,1,spAllowResize);
					result = 1;
				}
				#endif
				switch ( event.key.keysym.sym )
				{
				case SDLK_LEFT:
					if ( spGenericInput.axis[0] == -1 || spGenericInput.analog_axis[0] < 0)
					{
						spGenericInput.axis[0] = 0;
						spGenericInput.analog_axis[0] = 0;
						spLastAxisType = 0;
					}
					break;
				case SDLK_RIGHT:
					if ( spGenericInput.axis[0] == 1 || spGenericInput.analog_axis[0] > 0)
					{
						spGenericInput.axis[0] = 0;
						spGenericInput.analog_axis[0] = 0;
						spLastAxisType = 0;
					}
					break;
				case SDLK_UP:
					if ( spGenericInput.axis[1] == -1 || spGenericInput.analog_axis[1] < 0)
					{
						spGenericInput.axis[1] = 0;
						spGenericInput.analog_axis[1] = 0;
						spLastAxisType = 0;
					}
					break;
				case SDLK_DOWN:
					if ( spGenericInput.axis[1] == 1 || spGenericInput.analog_axis[1] > 0)
					{
						spGenericInput.axis[1] = 0;
						spGenericInput.analog_axis[1] = 0;
						spLastAxisType = 0;
					}
					break;
			#ifdef DINGUX
				case SDLK_RETURN:
					spGenericInput.button[SP_BUTTON_START] = 0;
					break;
				case SDLK_SPACE:
					spGenericInput.button[SP_BUTTON_X] = 0;
					break;
				case SDLK_LSHIFT:
					spGenericInput.button[SP_BUTTON_Y] = 0;
					break;
				case SDLK_LCTRL:
					spGenericInput.button[SP_BUTTON_A] = 0;
					break;
				case SDLK_LALT:
					spGenericInput.button[SP_BUTTON_B] = 0;
					break;
				case SDLK_ESCAPE:
					spGenericInput.button[SP_BUTTON_SELECT] = 0;
					break;
				case SDLK_TAB:
					spGenericInput.button[SP_BUTTON_L] = 0;
					break;
				case SDLK_BACKSPACE:
					spGenericInput.button[SP_BUTTON_R] = 0;
					break;
			#elif defined GCW
				case SDLK_RETURN:
					spGenericInput.button[SP_BUTTON_START] = 0;
					break;
				case SDLK_SPACE:
					spGenericInput.button[SP_BUTTON_Y] = 0;
					break;
				case SDLK_LSHIFT:
					spGenericInput.button[SP_BUTTON_X] = 0;
					break;
				case SDLK_LCTRL:
					spGenericInput.button[SP_BUTTON_A] = 0;
					break;
				case SDLK_LALT:
					spGenericInput.button[SP_BUTTON_B] = 0;
					break;
				case SDLK_ESCAPE:
					spGenericInput.button[SP_BUTTON_SELECT] = 0;
					break;
				case SDLK_TAB:
					spGenericInput.button[SP_BUTTON_L] = 0;
					break;
				case SDLK_BACKSPACE:
					spGenericInput.button[SP_BUTTON_R] = 0;
					break;
			#elif defined PANDORA
				case SDLK_PAGEDOWN:
					spGenericInput.button[SP_BUTTON_X] = 0;
					break;
				case SDLK_PAGEUP:
					spGenericInput.button[SP_BUTTON_Y] = 0;
					break;
				case SDLK_HOME:
					spGenericInput.button[SP_BUTTON_A] = 0;
					break;
				case SDLK_END:
					spGenericInput.button[SP_BUTTON_B] = 0;
					break;
				case SDLK_LCTRL:
					spGenericInput.button[SP_BUTTON_SELECT] = 0;
					break;
				case SDLK_RSHIFT:
					spGenericInput.button[SP_BUTTON_L] = 0;
					break;
				case SDLK_RCTRL:
					spGenericInput.button[SP_BUTTON_R] = 0;
					break;
				case SDLK_LALT:
					spGenericInput.button[SP_BUTTON_START] = 0;
					break;
			#else //PC
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					spGenericInput.button[SP_BUTTON_START] = 0;
					break;
					case SDLK_LCTRL:
						spGenericInput.button[SP_BUTTON_A_NOWASD] = 0;
						break;
					case SDLK_LALT:
						spGenericInput.button[SP_BUTTON_B_NOWASD] = 0;
						break;
					case SDLK_MENU:
						spGenericInput.button[SP_BUTTON_X_NOWASD] = 0;
						break;
					case SDLK_LSHIFT:case SDLK_RSHIFT:
						spGenericInput.button[SP_BUTTON_Y_NOWASD] = 0;
						break;
					case SDLK_ESCAPE:
						spGenericInput.button[SP_BUTTON_SELECT_NOWASD] = 0;
						break;
					case SDLK_PAGEUP:
						spGenericInput.button[SP_BUTTON_L_NOWASD] = 0;
						break;
					case SDLK_PAGEDOWN:
						spGenericInput.button[SP_BUTTON_R_NOWASD] = 0;
						break;
					case SDLK_a:
						spGenericInput.button[SP_BUTTON_A] = 0;
						break;
					case SDLK_d:
						spGenericInput.button[SP_BUTTON_B] = 0;
						break;
					case SDLK_w:
						spGenericInput.button[SP_BUTTON_Y] = 0;
						break;
					case SDLK_s:
						spGenericInput.button[SP_BUTTON_X] = 0;
						break;
					case SDLK_BACKSPACE:
						spGenericInput.button[SP_BUTTON_SELECT] = 0;
						break;
					case SDLK_q:
						spGenericInput.button[SP_BUTTON_L] = 0;
						break;
					case SDLK_e:
						spGenericInput.button[SP_BUTTON_R] = 0;
						break;
			#endif
				}
				break;
			case SDL_JOYAXISMOTION:
				spGenericInput.analog_axis[event.jaxis.axis & 1] = event.jaxis.value;
				if ( event.jaxis.value < SP_JOYSTICK_MIN_TRIGGER_ON && sp_axis_was_used[event.jaxis.axis & 1] != -1 )
				{
					spGenericInput.axis[event.jaxis.axis & 1] = -1;
					sp_axis_was_used[event.jaxis.axis & 1] = -1;
					spLastAxisType = 1;
				}
				else
				if ( event.jaxis.value > SP_JOYSTICK_MAX_TRIGGER_ON && sp_axis_was_used[event.jaxis.axis & 1] != 1)
				{
					spGenericInput.axis[event.jaxis.axis & 1] = 1;
					sp_axis_was_used[event.jaxis.axis & 1] = 1;
					spLastAxisType = 1;
				}
				else
				if (event.jaxis.value > SP_JOYSTICK_MIN_TRIGGER_OFF && event.jaxis.value < SP_JOYSTICK_MAX_TRIGGER_OFF  && sp_axis_was_used[event.jaxis.axis & 1] != 0)
				{
					spGenericInput.axis[event.jaxis.axis & 1] = 0;
					sp_axis_was_used[event.jaxis.axis & 1] = 0;
					spLastAxisType = 1;
				}
				break;
			case SDL_QUIT:
				spDone = 1;
				break;
			case SDL_VIDEORESIZE:
				spResizeWindow( event.resize.w, event.resize.h, 0, (spWindow->flags & SDL_RESIZABLE ? 1 : 0) );
				result = 1;
				break;
		}
		if ( spEvent )
			spEvent( &event );
	}
	return result;
}

signed char gp2x_axis[SP_INPUT_AXIS_COUNT] = {0,0};

inline void spUpdateAxis( int axis )
{
#ifdef GP2X
	int old_state = spGenericInput.axis[axis];
	if ( axis == 0 )
	{
		spGenericInput.axis[0] = 0;
		if ( spGenericInput.button[SP_AXIS_LEFTUP] ||
				spGenericInput.button[SP_AXIS_LEFT]   ||
				spGenericInput.button[SP_AXIS_LEFTDOWN] )
			spGenericInput.axis[0] = -1;
		if ( spGenericInput.button[SP_AXIS_RIGHTUP] ||
				spGenericInput.button[SP_AXIS_RIGHT]   ||
				spGenericInput.button[SP_AXIS_RIGHTDOWN] )
			spGenericInput.axis[0] = 1;
	}
	else
	{
		spGenericInput.axis[1] = 0;
		if ( spGenericInput.button[SP_AXIS_LEFTUP] ||
				spGenericInput.button[SP_AXIS_UP]   ||
				spGenericInput.button[SP_AXIS_RIGHTUP] )
			spGenericInput.axis[1] = -1;
		if ( spGenericInput.button[SP_AXIS_LEFTDOWN] ||
				spGenericInput.button[SP_AXIS_DOWN]   ||
				spGenericInput.button[SP_AXIS_RIGHTDOWN] )
			spGenericInput.axis[1] = 1;
	}

	int sav_state = gp2x_axis[axis];
	gp2x_axis[axis] = spGenericInput.axis[axis];
	if (old_state != spGenericInput.axis[axis] && //it is different from the old state, but
		sav_state == spGenericInput.axis[axis]) //not different from the last input!
		spGenericInput.axis[axis] = old_state;
	//3
	if (spGenericInput.axis[axis] == -1)
		spGenericInput.analog_axis[axis] = SP_ANALOG_AXIS_MIN;
	else
	if (spGenericInput.axis[axis] ==  1)
		spGenericInput.analog_axis[axis] = SP_ANALOG_AXIS_MAX;
	else
	spGenericInput.analog_axis[axis] = 0;
#endif
}

void spHandleFakeKeyboard( int steps )
{
	if (spLastKey.unicode != 0)
	{
		spLastKeyCountDown -= steps;
		while (spLastKeyCountDown <= 0)
		{
			spLastKeyCountDown += SP_KEYBOARD_WAIT;
			spHandleKeyboardInput(spLastKey);
		}
	}
}

int spShiftStillPressed = 0;

void spClickVirtualKey(int steps,int x,int y)
{
	if (spVirtualKeyboardMap[y][x] == 1)
	{
		if (spShiftStillPressed == 0)
		{
			spInternalCleanVirtualKeyboard();
			spVirtualKeyboardShift = 1-spVirtualKeyboardShift;
			spInternalUpdateVirtualKeyboard();
			spShiftStillPressed = 1;
		}
		return;
	}
	char* ptr = (char*)spVirtualKeyboardMap;
	if (spVirtualKeyboardShift)
		ptr = (char*)spVirtualKeyboardMapShift;

	SDL_keysym key = {spVirtualKeyboardMap[y][x],
		(SDLKey)spVirtualKeyboardMap[y][x],(SDLMod)0,
		ptr[y*20+x]};

	if (spVirtualLastKey.sym == key.sym)
	{
		spVirtualLastKeyCountDown -= steps;
		while (spVirtualLastKeyCountDown <= 0)
		{
			spVirtualLastKeyCountDown += SP_KEYBOARD_WAIT;
			spHandleKeyboardInput(spVirtualLastKey);
		}
	}
	else
	{
		spHandleKeyboardInput(key);
		spVirtualLastKey = key;
		spVirtualLastKeyCountDown = SP_KEYBOARD_FIRST_WAIT;
	}
	if (spVirtualKeyboardShift)
	{
		spInternalCleanVirtualKeyboard();
		spVirtualKeyboardShift = 0;
		spInternalUpdateVirtualKeyboard();
	}
}

void spHandleVirtualKeyboard( int steps )
{
	if (spGenericInput.keyboard.buffer == NULL || spVirtualKeyboardState == SP_VIRTUAL_KEYBOARD_NEVER)
		return;
	int was_greater = spVirtualKeyboardTime > 0;
	if (spGenericInput.axis[0] == 0 && spGenericInput.axis[1] == 0)
		spVirtualKeyboardTime = 0;
	else
		spVirtualKeyboardTime -= steps;
	if (spVirtualKeyboardTime <= 0)
	{
		if (spGenericInput.axis[0] || spGenericInput.axis[1])
		{
			spInternalCleanVirtualKeyboard();
			if (spGenericInput.axis[0] < 0)
				spVirtualKeyboardX = (spVirtualKeyboardX+19) % 20;
			else
			if (spGenericInput.axis[0] > 0)
				spVirtualKeyboardX = (spVirtualKeyboardX+1) % 20;
			if (spGenericInput.axis[1] < 0)
				spVirtualKeyboardY = (spVirtualKeyboardY+2) % 3;
			else
			if (spGenericInput.axis[1] > 0)
				spVirtualKeyboardY = (spVirtualKeyboardY+1) % 3;
			spInternalUpdateVirtualKeyboard();
			if (was_greater)
				spVirtualKeyboardTime = SP_VIRTUAL_KEYBOARD_WAIT;
			else
				spVirtualKeyboardTime = SP_VIRTUAL_KEYBOARD_FIRST_WAIT;

			spVirtualLastKeyCountDown = SP_KEYBOARD_FIRST_WAIT;
			char* ptr = (char*)spVirtualKeyboardMap;
			if (spVirtualKeyboardShift)
				ptr = (char*)spVirtualKeyboardMapShift;
			SDL_keysym key = {spVirtualKeyboardMap[spVirtualKeyboardY][spVirtualKeyboardX],
				(SDLKey)spVirtualKeyboardMap[spVirtualKeyboardY][spVirtualKeyboardX],(SDLMod)0,
				ptr[spVirtualKeyboardY*20+spVirtualKeyboardX]};
			spVirtualLastKey = key;
		}
		else
			spVirtualKeyboardTime = 0;

	}
	int b;
	int noButton = 1;
	//Keyboard input
	for (b = 0; b < 31; b++)
	{
		if ((spVirtualKeyboardMask & (1 << b)) && spGenericInput.button[b])
		{
			noButton = 0;
			spClickVirtualKey(steps,spVirtualKeyboardX,spVirtualKeyboardY);
		}
	}
	//Special buttons
	if (spVirtualKeyboardBackspaceButton >= 0 && spVirtualKeyboardBackspaceButton < SP_INPUT_BUTTON_COUNT &&
		spGenericInput.button[spVirtualKeyboardBackspaceButton])
	{
		noButton = 0;
		spClickVirtualKey(steps,15,0);
	}
	if (spVirtualKeyboardSpaceButton >= 0 && spVirtualKeyboardSpaceButton < SP_INPUT_BUTTON_COUNT &&
		spGenericInput.button[spVirtualKeyboardSpaceButton])
	{
		noButton = 0;
		spClickVirtualKey(steps,10,2);
	}
	//Touchscreen input
	if ( spGenericInput.touchscreen.pressed &&
	    (spGenericInput.touchscreen.x-spVirtualKeyboardPositionX) >= 0 &&
	    (spGenericInput.touchscreen.x-spVirtualKeyboardPositionX) < spVirtualKeyboard[spVirtualKeyboardShift]->w &&
	    (spGenericInput.touchscreen.y-spVirtualKeyboardPositionY) >= 0 &&
	    (spGenericInput.touchscreen.y-spVirtualKeyboardPositionY) < spVirtualKeyboard[spVirtualKeyboardShift]->h)
	{
		spInternalCleanVirtualKeyboard();
		Sint32 clickX = spGenericInput.touchscreen.x-spVirtualKeyboardPositionX << SP_ACCURACY;
		Sint32 clickY = spGenericInput.touchscreen.y-spVirtualKeyboardPositionY << SP_ACCURACY;
		Sint32 divisor = (spVirtualKeyboard[spVirtualKeyboardShift]->w << SP_ACCURACY)/20;
		spVirtualKeyboardX = spDivHigh(clickX,divisor) >> SP_ACCURACY;
		       divisor = (spVirtualKeyboard[spVirtualKeyboardShift]->h << SP_ACCURACY)/3;
		spVirtualKeyboardY = spDivHigh(clickY,divisor) >> SP_ACCURACY;
		spInternalUpdateVirtualKeyboard();
		noButton = 0;
		spClickVirtualKey(steps,spVirtualKeyboardX,spVirtualKeyboardY);
	}
	if (noButton)
	{
		spShiftStillPressed = 0;
		spVirtualLastKey.sym = (SDLKey)0;
		spVirtualLastKeyCountDown = 0;
	}
}

Uint32 oldticks;
Uint32 olderticks;
Uint32 newticks;

PREFIX void spResetLoop( void )
{
	#ifdef DEBUG_SLOWMOTION
		oldticks = newticks = olderticks = SDL_GetTicks()/DEBUG_SLOWMOTION;
	#else
		oldticks = newticks = olderticks = SDL_GetTicks();
	#endif
}

PREFIX int spLoop( void ( *spDraw )( void ), int ( *spCalc )( Uint32 steps ), Uint32 minwait, void ( *spResize )( Uint16 w, Uint16 h ), void ( *spEvent )( SDL_Event *e ) )
{
	Uint32 bigsteps = 0;
	Uint32 frames = 0;
	int back = 0;
	//time since the last frame
	Uint32 steps = 0;
	//time from the last calculation
	#ifdef DEBUG_SLOWMOTION
		oldticks = SDL_GetTicks()/DEBUG_SLOWMOTION;
	#else
		oldticks = SDL_GetTicks();
	#endif
	//time of the current loop pass
	newticks = oldticks;

	while(!spDone )
	{
#ifdef CORE_DEBUG
		spPrintDebug( "Start mainloop" );
#endif
		oldticks = newticks;
		#ifdef DEBUG_SLOWMOTION
			newticks = SDL_GetTicks()/DEBUG_SLOWMOTION;
		#else
			newticks = SDL_GetTicks();
		#endif
		Uint32 diffticks = newticks-oldticks;
#ifdef F100
		//mouse movement emulation: *untested*
		if (sp_touchscreen_emulation)
		{
			//If I read right in the SDL code (why document it, if you can read it
			//in the code? -_-), SDL_WarpMouse should test for the screen dimension.
			int mouse_steps = newticks - oldticks;
			if (mouse_steps)
				SDL_WarpMouse(spGenericInput.touchscreen.x + sp_touchscreen_dx * mouse_steps,
											spGenericInput.touchscreen.y + sp_touchscreen_dy * mouse_steps);
		}
#endif
		if ( spHandleEvent( spEvent ) && spResize )
		{
			spResize( spWindowX, spWindowY );
#ifdef CORE_DEBUG
			spPrintDebug( "  Did events and resize" );
#endif
		}
#ifdef CORE_DEBUG
		else
			spPrintDebug( "  Did events" );
#endif
		spUpdateAxis( 0 );
		spUpdateAxis( 1 );
#ifdef CORE_DEBUG
		spPrintDebug( "  Did axis update" );
#endif
		//Calls with diffticks == 0 are not possible!
		if ( spCalc && diffticks)
		{
			back = spCalc( diffticks );
			if (back)
				break;
#ifdef CORE_DEBUG
			spPrintDebug( "  Did calc" );
#endif
			spHandleFakeKeyboard(diffticks);
#ifdef CORE_DEBUG
			spPrintDebug( "  Did fake keyboard events" );
#endif
			spHandleVirtualKeyboard(diffticks);
#ifdef CORE_DEBUG
			spPrintDebug( "  Did virtual keyboard events" );
#endif
		}
		steps += diffticks;
		if ( steps >= minwait )
		{
			if ( spDraw )
				spDraw();
#ifdef CORE_DEBUG
			spPrintDebug( "  Did draw" );
#endif
			frames++;
			bigsteps = bigsteps + steps;
			while ( bigsteps >= 1000 )
			{
				bigsteps -= 1000;
				spFPS = frames;
				frames = 0;
			}
			steps = 0;
		}
		#ifndef DO_NOT_USE_DELAY
		if (diffticks == 0)
			spSleep(200);
		#endif
	}
	return back;
}

PREFIX void spFlip( void )
{
	spWaitForDrawingThread();
	spUnlockRenderTarget();
#ifdef CORE_DEBUG
	spPrintDebug( "    Flip in" );
#endif
	//The Flip
#ifdef DOUBLEBUFFERING_BLIT
	SDL_BlitSurface( spWindow, NULL, spScreen, NULL );
#elif defined DOUBLEBUFFERING_BLIT_AND_FLIP
	SDL_BlitSurface( spWindow, NULL, spScreen, NULL );
	SDL_Flip(spScreen);
#else
	SDL_Flip( spWindow );
#endif
#ifdef CORE_DEBUG
	spPrintDebug( "    Flip out" );
#endif
	spUpdateTargetPixels();
	spLockRenderTarget();
}

PREFIX PspInput spGetInput( void )
{
	return &spGenericInput;
}

PREFIX void spResetButtonsState( void )
{
	int I;
	for ( I = 0; I < SP_INPUT_BUTTON_COUNT; ++I )
		spGenericInput.button[I] = 0;
}

PREFIX void spResetAxisState( void )
{
	int I;
	for ( I = 0; I < SP_INPUT_AXIS_COUNT; ++I )
	{
		spGenericInput.axis[I] = 0;
		spGenericInput.analog_axis[I] = 0;
	}
}

PREFIX void spPollKeyboardInput( char *buffer, int bufferSize, Sint32 enter_key_mask)
{
	if ( bufferSize > 0 && buffer)
	{
		spGenericInput.keyboard.buffer = buffer;
		spGenericInput.keyboard.len = bufferSize;
		spGenericInput.keyboard.pos = strlen( buffer );
		spGenericInput.keyboard.lastSize = 0;
		spVirtualKeyboardMask = enter_key_mask;
		SDL_EnableUNICODE( 1 );
	}
	else
	{
		spStopKeyboardInput();
	}
}

PREFIX void spStopKeyboardInput( void )
{
	spGenericInput.keyboard.buffer = NULL;
	spGenericInput.keyboard.len = 0;
	spGenericInput.keyboard.pos = 0;
	spGenericInput.keyboard.lastSize = 0;
	spVirtualKeyboardMask = 0;
	spLastKey.unicode = 0;
	spLastKeyCountDown = 0;
	SDL_EnableUNICODE( 0 );
}

PREFIX void spSetTouchscreenEmulationButtons(int switch_button,int ok_button)
{
	sp_switch_button = switch_button;
	sp_ok_button = ok_button;
}

PREFIX void spQuitCore( void )
{
	//#ifdef MOBILE_DEVICE
	if ( SDL_NumJoysticks() > 0 )
	{
		int i;
		for ( i = 0; i < SDL_NumJoysticks(); i++ )
			if (spJoy[i])
				SDL_JoystickClose( spJoy[i] );
		free( spJoy );
	}
	//#endif
	spQuitPrimitives();
	SDL_Quit();
	spCoreIsInitialized = 0;
}

PREFIX int spGetFPS( void )
{
	return spFPS;
}

PREFIX Sint32 spGetSizeFactor( void )
{
	return spZoom;
}

SDL_Surface* spLoadUncachedSurface( const char* name )
{
	SDL_Surface* surface = IMG_Load( name );
	if ( !surface )
	{
		printf( "Failed to load surface \"%s\", uncool...\n", name );
		printf( "  Error was: \"%s\"\n", IMG_GetError() );
		return NULL;
	}
	SDL_Surface* result = SDL_ConvertSurface( surface , spWindow->format, spWindow->flags);
	SDL_FreeSurface( surface );
	return result;
}

SDL_Surface* spLoadUncachedSurfaceZoom( const char* name, Sint32 zoom)
{
	if (zoom == SP_ONE)
		return spLoadUncachedSurface( name );
	SDL_Surface* surface = spLoadUncachedSurface( name );
	if ( !surface )
		return NULL;
	int w = spFixedToInt(zoom*surface->w);
	int h = spFixedToInt(zoom*surface->h);
	SDL_Surface* temp = SDL_CreateRGBSurface( SP_SURFACE_FLAGS, w, h, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0 );
	SDL_Surface* result = SDL_DisplayFormat( temp );
	spInternalZoomBlit(surface,0,0,surface->w,surface->h,result,0,0,result->w,result->h);
	SDL_FreeSurface( surface );
	SDL_FreeSurface( temp );
	return result;
}

inline sp_cache_pointer sp_get_cached_surface_by_name( char* name)
{
	if (name == NULL)
		return NULL;
	sp_cache_pointer c;
	Uint32 hash = 0;
	int i;
	for (i = 0;name[i]!=0;i++)
		hash+=name[i];
  hash = hash & (SP_CACHE_SIZE-1);
	c = sp_cache_name[hash];
	//Searching in the fast cache
	if (c && c->name && strcmp(c->name,name) == 0)
		return c;
	//Searching in the slower, but accurate cache
	c = sp_first_cache_line;
	if (c)
	do
	{
		if (c->name && strcmp(c->name,name) == 0)
		{
			sp_cache_name[hash] = c;
			c->name_hash = hash;
			return c;
		}
		c = c->next;
	}
	while (c!=sp_first_cache_line);
	return NULL;
}

inline sp_cache_pointer sp_get_cached_surface_by_surface( SDL_Surface* surface )
{
	sp_cache_pointer c;
  Uint32 hash = (int)surface & (SP_CACHE_SIZE-1);
	c = sp_cache_surface[hash];
	//Searching in the fast cache
	if (c && c->surface == surface)
		return c;
	//Searching in the slower, but accurate cache
	c = sp_first_cache_line;
	if (c)
	do
	{
		if (c->surface == surface)
		{
			sp_cache_surface[hash] = c;
			c->surface_hash = hash;
			return c;
		}
		c = c->next;
	}
	while (c!=sp_first_cache_line);
	return NULL;
}

PREFIX SDL_Surface* spCopySurface( SDL_Surface* surface )
{
	if (surface == NULL)
		return NULL;
	if (sp_caching)
	{
		sp_cache_pointer c = sp_get_cached_surface_by_surface( surface );
		if (c)
		{
			c->ref++;
			return c->surface;
		}
		//not Null, but not the cache? O_o Lets add a copy and return
		printf("Can't find surface in cache. Will return a real copy.\n");
	}
	return spUniqueCopySurface( surface );
}

PREFIX SDL_Surface* spUniqueCopySurface( SDL_Surface* surface )
{
	SDL_Surface* result = spCreateSurface(surface->w,surface->h);
	SDL_SetColorKey( surface, 0, 0 );
	SDL_BlitSurface( surface, NULL, result, NULL );
	return result;
}

PREFIX SDL_Surface* spLoadSurfaceZoom( const char* name, Sint32 zoom)
{
	if (sp_caching)
	{
		char* nameTemp = (char*)malloc(strlen(name)+32); //extra space for the number
		if (zoom == SP_ONE)
			sprintf(nameTemp,"%s",name);
		else
			sprintf(nameTemp,"%s_//ZOOM//MEOW//ZOOM//%i",name,zoom); //This filename SHOULDN'T exist ANYWHERE.
		sp_cache_pointer c = sp_get_cached_surface_by_name(nameTemp);
		if (c)
		{
			c->ref++;
			spLastFirstTime = 0;
		}
		else
		{
			//not found, creating and adding
			SDL_Surface *surface = spLoadUncachedSurfaceZoom(name,zoom);
			if (surface == NULL)
				return NULL; //Null surfaces are not cached...
			c = (sp_cache_pointer)malloc(sizeof(sp_cache));
			c->surface = surface;
			c->name = (char*)malloc(strlen(nameTemp)+1);
			sprintf(c->name,"%s",nameTemp);
			c->ref = 1;
			c->name_hash = 0;
			int i;
			for (i = 0;name[i]!=0;i++)
				c->name_hash+=name[i];
			c->name_hash = c->name_hash & (SP_CACHE_SIZE-1);
			c->surface_hash = (int)c->surface & (SP_CACHE_SIZE-1);
			if (sp_first_cache_line)
			{
				sp_first_cache_line->prev->next = c;
				c->prev = sp_first_cache_line->prev;
				sp_first_cache_line->prev = c;
				c->next = sp_first_cache_line;
			}
			else
			{
				c->prev = c;
				c->next = c;
			}
			sp_first_cache_line = c;
			spLastFirstTime = 1;
		}
		free(nameTemp);
		return c->surface;
	}
	else
		return spLoadUncachedSurface(name);
}

PREFIX int spLastCachedSurfaceWasLoadedFirstTime( void )
{
	return spLastFirstTime;
}

PREFIX SDL_Surface* spLoadSurface( const char* name )
{
	return spLoadSurfaceZoom( name, SP_ONE );
}

PREFIX void spEnableCaching( void )
{
	sp_caching = 1;
}

PREFIX void spDisableCaching( void )
{
	sp_caching = 0;
}

PREFIX int spIsCachingEnabled( void )
{
	return sp_caching;
}

PREFIX void spClearCache( void )
{
	while (sp_first_cache_line)
	{
		free(sp_first_cache_line->name);
		sp_first_cache_line = sp_first_cache_line->next;
	}
	memset(sp_cache_name,0,SP_CACHE_SIZE*sizeof(sp_cache_pointer));
	memset(sp_cache_surface,0,SP_CACHE_SIZE*sizeof(sp_cache_pointer));
}

PREFIX SDL_Surface* spCreateSurface(int width,int height)
{
	SDL_Surface* surface = SDL_CreateRGBSurface( SP_SURFACE_FLAGS, width, height, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0 );
	SDL_Surface* result = SDL_DisplayFormat( surface );
	SDL_FreeSurface( surface );
	if (sp_caching)
	{
		sp_cache_pointer c = (sp_cache_pointer)malloc(sizeof(sp_cache));
		c->surface = result;
		c->name = NULL;
		c->ref = 1;
		c->name_hash = SP_CACHE_SIZE;
		c->surface_hash = (int)c->surface & (SP_CACHE_SIZE-1);
		if (sp_first_cache_line)
		{
			sp_first_cache_line->prev->next = c;
			c->prev = sp_first_cache_line->prev;
			sp_first_cache_line->prev = c;
			c->next = sp_first_cache_line;
		}
		else
		{
			c->prev = c;
			c->next = c;
		}
		sp_first_cache_line = c;
	}
	return result;
}

PREFIX void spDeleteSurface( SDL_Surface* surface )
{
	if (surface == NULL)
		return;

	sp_cache_pointer c = sp_get_cached_surface_by_surface( surface );

	if (c)
	{
		c->ref--;
		if (c->ref <= 0)
		{
			if (c->name)
				sp_cache_name[c->name_hash] = NULL;
			sp_cache_surface[c->surface_hash] = NULL;
			c->prev->next = c->next;
			c->next->prev = c->prev;
			if (sp_first_cache_line == c)
				sp_first_cache_line = c->next;
			if (sp_first_cache_line == c) //again? that means, it is a double linked chain with just ONE element - which we remove right now
				sp_first_cache_line = NULL;
			if (c->name)
				free(c->name);
			free(c);
			SDL_FreeSurface(surface);
		}
	}
	else
		SDL_FreeSurface(surface);
}

PREFIX Uint16 spGetRGB(int r, int g, int b )
{
  r = r+4 >> 3;
  if (r > 31)
    r = 31;
  g = g+2 >> 2;
  if (g > 63)
    g = 63;
  b = b+4 >> 3;
  if (b > 31)
    b = 31;
  return (r<<11)+(g<<5)+b;
  //return SDL_MapRGB(spGetWindowSurface()->format,r,g,b);
}

PREFIX Sint32 spGetHFromColor(Uint16 color)
{
	int r = spGetRFromColor(color);
	int g = spGetGFromColor(color);
	int b = spGetBFromColor(color);
	int max = spMax(r,spMax(g,b));
	int min = spMin(r,spMin(g,b));
	if (max == min)
		return 0;
	Sint32 h;
	if (max == r)
		h = SP_PI*0/3+SP_PI/3*(g-b)/(max-min);
	else
	if (max == g)
		h = SP_PI*2/3+SP_PI/3*(b-r)/(max-min);
	else
		h = SP_PI*4/3+SP_PI/3*(r-g)/(max-min);
	if (h < 0)
		h += SP_PI*2;
	return h;
}

PREFIX Uint8 spGetSFromColor(Uint16 color)
{
	int r = spGetRFromColor(color);
	int g = spGetGFromColor(color);
	int b = spGetBFromColor(color);
	int max = spMax(r,spMax(g,b));
	if (max == 0)
		return 0;
	int min = spMin(r,spMin(g,b));
	Sint32 s = (max-min)*255/max;
	return s;
}

PREFIX Uint8 spGetVFromColor(Uint16 color)
{
	int r = spGetRFromColor(color);
	int g = spGetGFromColor(color);
	int b = spGetBFromColor(color);
	return spMax(r,spMax(g,b));
}

PREFIX Uint16 spGetHSV(Sint32 h, Uint8 s, Uint8 v)
{
	h = h % (SP_PI*2);
	int hi = h / (SP_PI/3);
	Sint32 S = s;
	Sint32 V = v;
	Sint32 f = (spDivHigh(h,SP_PI/3) >> SP_ACCURACY-8) - (hi << 8);
	Sint32 p = V*(255-S) >> 8;
	Sint32 q = V*(255-(S*f >> 8)) >> 8;
	Sint32 t = V*(255-(S*(255-f) >> 8)) >> 8;
	Sint32 r,g,b;
	switch (hi)
	{
		case 0: case 6:
			r = V;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = V;
			b = p;
			break;
		case 2:
			r = p;
			g = V;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = V;
			break;
		case 4:
			r = t;
			g = p;
			b = V;
			break;
		case 5:
			r = V;
			g = p;
			b = q;
			break;
	}
	return ((r >> 3) << 11) + ((g >> 2) << 5) + (b >> 3);
}

/* AdvMAME2x
 *   A    --\ 1 2
 * C P B  --/ 3 4
 *   D
 *  1=P; 2=P; 3=P; 4=P;
 *  IF C==A AND C!=D AND A!=B => 1=A
 *  IF A==B AND A!=C AND B!=D => 2=B
 *  IF B==D AND B!=A AND D!=C => 4=D
 *  IF D==C AND D!=B AND C!=A => 3=C */
#define spScaleP(x,y) src[x+ y   *src_w]
#define spScaleA(x,y) src[x+(y-1)*src_w]
#define spScaleB(x,y) src[(x+1)+y*src_w]
#define spScaleC(x,y) src[(x-1)+y*src_w]
#define spScaleD(x,y) src[x+(y+1)*src_w]
#define spScalePixel(X,Y) \
	if (C == A && C!=D && A!=B) \
		dst[X   +(Y  )*dst_w] = A; \
	else \
		dst[X   +(Y  )*dst_w] = P; \
	\
	if (A == B && A!=C && B!=D) \
		dst[X+1 +(Y  )*dst_w] = B; \
	else \
		dst[X+1 +(Y  )*dst_w] = P; \
	\
	if (B == D && B!=A && D!=C) \
		dst[X+1 +(Y+1)*dst_w] = D; \
	else \
		dst[X+1 +(Y+1)*dst_w] = P; \
	\
	if (D == C && D!=B && C!=A) \
		dst[X   +(Y+1)*dst_w] = C; \
	else \
		dst[X   +(Y+1)*dst_w] = P;

PREFIX void spScale2XSmooth(SDL_Surface* source,SDL_Surface* destination)
{
	if (spGetRenderTarget() != source)
		SDL_LockSurface( source );
	if (spGetRenderTarget() != destination)
		SDL_LockSurface( destination );
	Uint16* src = (Uint16*)(source->pixels);
	Uint16* dst = (Uint16*)(destination->pixels);
	int src_w = source->pitch >> 1;
	int dst_w = destination->pitch >> 1;
	int x,y,A,B,C,D,P;
	//Y=0
	P = spScaleP(0,0);
	A = P; B = spScaleB(0,0); C = P; D = spScaleD(0,0);
	spScalePixel(0,0);
	for (x = 1; x < src_w-1; x++)
	{
		int X = x*2;
		P = spScaleP(x,0);
		A = P; B = spScaleB(x,0); C = spScaleC(x,0); D = spScaleD(x,0);
		spScalePixel(X,0);
	}
	P = spScaleP((src_w-1),0);
	A = P; B = P; C = spScaleC((src_w-1),0); D = spScaleD((src_w-1),0);
	spScalePixel((src_w-1)*2,0);

	//Y=1..n-1
	for (y = 1; y < source->h-1; y++)
	{
		int Y = y*2;
		P = spScaleP(0,y);
		A = spScaleA(0,y); B = spScaleB(0,y); C = P; D = spScaleD(0,y);
		spScalePixel(0,Y);
		for (x = 1; x < src_w-1; x++)
		{
			int X = x*2;
			P = spScaleP(x,y);
			A = spScaleA(x,y); B = spScaleB(x,y); C = spScaleC(x,y); D = spScaleD(x,y);
			spScalePixel(X,Y);
		}
		P = spScaleP((src_w-1),y);
		A = spScaleA((src_w-1),y); B = P; C = spScaleC((src_w-1),y); D = spScaleD((src_w-1),y);
		spScalePixel((src_w-1)*2,Y);
	}
	//Y = n
	P = spScaleP(0,(source->h-1));
	A = spScaleA(0,(source->h-1)); B = spScaleB(0,(source->h-1)); C = P; D = P;
	spScalePixel(0,(source->h-1)*2);
	for (x = 1; x < src_w-1; x++)
	{
		int X = x*2;
		P = spScaleP(x,(source->h-1));
		A = spScaleA(x,(source->h-1)); B = spScaleB(x,(source->h-1)); C = spScaleC(x,(source->h-1)); D = P;
		spScalePixel(X,(source->h-1)*2);
	}
	P = spScaleP((src_w-1),(source->h-1));
	A = spScaleA((src_w-1),(source->h-1)); B = P; C = spScaleC((src_w-1),(source->h-1)); D = P;
	spScalePixel((src_w-1)*2,(source->h-1)*2);

	if (spGetRenderTarget() != source)
		SDL_UnlockSurface( source );
	if (spGetRenderTarget() != destination)
		SDL_UnlockSurface( destination );
}

PREFIX void spScale2XFast(SDL_Surface* source,SDL_Surface* destination)
{
	if (spGetRenderTarget() != source)
		SDL_LockSurface( source );
	if (spGetRenderTarget() != destination)
		SDL_LockSurface( destination );
	int src_w = source->pitch >> 1;
	int dst_w = destination->pitch >> 1;
	Uint16* src = (Uint16*)(source->pixels);
	Uint16* dst = (Uint16*)(destination->pixels);
	int x,y;
	for (y = 0; y < source->h; y++)
	{
		int Y = y*2;
		for (x = 0; x < source->w; x++)
		{
			int X = x*2;
			int P = spScaleP(x,y);
			dst[X   +(Y  )*dst_w] = P;
			dst[X+1 +(Y  )*dst_w] = P;
			dst[X   +(Y+1)*dst_w] = P;
			dst[X+1 +(Y+1)*dst_w] = P;
		}
	}
	if (spGetRenderTarget() != source)
		SDL_UnlockSurface( source );
	if (spGetRenderTarget() != destination)
		SDL_UnlockSurface( destination );
}

PREFIX void spScaleDownSmooth(SDL_Surface* source,SDL_Surface* destination)
{
	if (spGetRenderTarget() != source)
		SDL_LockSurface( source );
	if (spGetRenderTarget() != destination)
		SDL_LockSurface( destination );
	int src_w = source->pitch >> 1;
	int dst_w = destination->pitch >> 1;
	Uint16* src = (Uint16*)(source->pixels);
	Uint16* dst = (Uint16*)(destination->pixels);
	int x,y;
	for (y = 0; y < destination->h; y++)
	{
		int Y = y*2;
		for (x = 0; x < destination->w; x++)
		{
			int X = x*2;
			Uint32 C = src[X+Y*src_w];
			int r = C & 63488;
			int g = C & 2016;
			int b = C & 31;
			C = src[X+1+Y*src_w];
			r += C & 63488;
			g += C & 2016;
			b += C & 31;
			C = src[X+(Y+1)*src_w];
			r += C & 63488;
			g += C & 2016;
			b += C & 31;
			C = src[X+1+(Y+1)*src_w];
			r += C & 63488;
			g += C & 2016;
			b += C & 31;
			dst[x+y*dst_w] = ((r>>2) & 63488) | ((g >> 2) & 2016) | ((b >> 2) & 31);
		}
	}
	if (spGetRenderTarget() != source)
		SDL_UnlockSurface( source );
	if (spGetRenderTarget() != destination)
		SDL_UnlockSurface( destination );
}

PREFIX void spScaleDownFast(SDL_Surface* source,SDL_Surface* destination)
{
	if (spGetRenderTarget() != source)
		SDL_LockSurface( source );
	if (spGetRenderTarget() != destination)
		SDL_LockSurface( destination );
	int src_w = source->pitch >> 1;
	int dst_w = destination->pitch >> 1;
	Uint16* src = (Uint16*)(source->pixels);
	Uint16* dst = (Uint16*)(destination->pixels);
	int x,y;
	for (y = 0; y < destination->h; y++)
	{
		int Y = y*2;
		for (x = 0; x < destination->w; x++)
		{
			int X = x*2;
			Uint32 C = src[X+Y*src_w];
			dst[x+y*dst_w] = C;
		}
	}
	if (spGetRenderTarget() != source)
		SDL_UnlockSurface( source );
	if (spGetRenderTarget() != destination)
		SDL_UnlockSurface( destination );
}
PREFIX void spAddBorder(SDL_Surface* surface, Uint16 borderColor,Uint16 backgroundcolor)
{
	SDL_LockSurface( surface );
	int width = surface->pitch >> 1;
	Uint16* pixel = (Uint16*)(surface->pixels);
	Uint16* copyPixel = (Uint16*)malloc(surface->h*width*sizeof(Uint16));
	memcpy(copyPixel,pixel,surface->h*width*sizeof(Uint16));
	int x,y;
	for (x = 0; x < surface->w; x++)
		for (y = 0; y < surface->h; y++)
		{
			if (copyPixel[x+y*width] == backgroundcolor && //pixel must be part of the background
				((x > 0 && copyPixel[x-1+y*width] != backgroundcolor) ||
				 (x < surface->w-1 && copyPixel[x+1+y*width] != backgroundcolor) ||
				 (y > 0 && copyPixel[x+(y-1)*width] != backgroundcolor) ||
				 (y < surface->h-1 && copyPixel[x+(y+1)*width] != backgroundcolor)))
			pixel[x+y*width] = borderColor;
		}
	SDL_UnlockSurface( surface );
}

PREFIX void spSetVirtualKeyboard(int state,int x,int y,int width,int height,SDL_Surface* design,SDL_Surface* shiftDesign)
{
	spVirtualKeyboardPositionX = x;
	spVirtualKeyboardPositionY = y;
	switch (state)
	{
		case SP_VIRTUAL_KEYBOARD_NEVER: spVirtualKeyboardState = SP_VIRTUAL_KEYBOARD_NEVER; break;
		#if defined PANDORA || defined DESKTOP
			case SP_VIRTUAL_KEYBOARD_IF_NEEDED: spVirtualKeyboardState = SP_VIRTUAL_KEYBOARD_NEVER; break;
		#else
			case SP_VIRTUAL_KEYBOARD_IF_NEEDED: spVirtualKeyboardState = SP_VIRTUAL_KEYBOARD_ALWAYS; break;
		#endif
		case SP_VIRTUAL_KEYBOARD_ALWAYS: spVirtualKeyboardState = SP_VIRTUAL_KEYBOARD_ALWAYS; break;
	}
	if (spVirtualKeyboard[0])
		spDeleteSurface(spVirtualKeyboard[0]);
	if (spVirtualKeyboard[1])
		spDeleteSurface(spVirtualKeyboard[1]);
	if (spVirtualKeyboardInternal[0])
		spDeleteSurface(spVirtualKeyboardInternal[0]);
	if (spVirtualKeyboardInternal[1])
		spDeleteSurface(spVirtualKeyboardInternal[1]);
	if (spVirtualKeyboardSelect[0])
		spDeleteSurface(spVirtualKeyboardSelect[0]);
	if (spVirtualKeyboardSelect[1])
		spDeleteSurface(spVirtualKeyboardSelect[1]);
	if (design == NULL || shiftDesign == NULL)
	{
		spVirtualKeyboard[0] = NULL;
		spVirtualKeyboardInternal[0] = NULL;
		spVirtualKeyboardSelect[0] = NULL;
		spVirtualKeyboard[1] = NULL;
		spVirtualKeyboardInternal[1] = NULL;
		spVirtualKeyboardSelect[1] = NULL;
		return;
	}
	spVirtualKeyboard[0] = spCreateSurface(width,height);
	spVirtualKeyboardInternal[0] = spCreateSurface(width,height);
	spVirtualKeyboardSelect[0] = spCreateSurface(width/20,height/3);
	spVirtualKeyboard[1] = spCreateSurface(width,height);
	spVirtualKeyboardInternal[1] = spCreateSurface(width,height);
	spVirtualKeyboardSelect[1] = spCreateSurface(width/20,height/3);

	spInternalZoomBlit(design,0,0,(design->w/21)*20,design->h,spVirtualKeyboardInternal[0],0,0,width,height);
	spInternalZoomBlit(design,(design->w/21)*20,design->h/3*1,(design->w/21),design->h/3,spVirtualKeyboardSelect[0],0,0,width/20,height/3);
	SDL_SetColorKey( spVirtualKeyboardSelect[0], SDL_SRCCOLORKEY, SP_ALPHA_COLOR );
	spInternalZoomBlit(shiftDesign,0,0,(shiftDesign->w/21)*20,shiftDesign->h,spVirtualKeyboardInternal[1],0,0,width,height);
	spInternalZoomBlit(shiftDesign,(shiftDesign->w/21)*20,shiftDesign->h/3*1,(shiftDesign->w/21),shiftDesign->h/3,spVirtualKeyboardSelect[1],0,0,width/20,height/3);
	SDL_SetColorKey( spVirtualKeyboardSelect[1], SDL_SRCCOLORKEY, SP_ALPHA_COLOR );

	SDL_BlitSurface(spVirtualKeyboardInternal[0],NULL,spVirtualKeyboard[0],NULL);
	SDL_BlitSurface(spVirtualKeyboardInternal[1],NULL,spVirtualKeyboard[1],NULL);
	spInternalUpdateVirtualKeyboard();
}

PREFIX int spGetVirtualKeyboardState( void )
{
	return spVirtualKeyboardState;
}

PREFIX SDL_Surface* spGetVirtualKeyboard( void )
{
	return spVirtualKeyboard[spVirtualKeyboardShift];
}

PREFIX void spSetVirtualKeyboardShiftState(int state)
{
	spVirtualKeyboardShift = state;
	spInternalResetVirtualKeyboard();
	spInternalUpdateVirtualKeyboard();
}

PREFIX int spGetVirtualKeyboardShiftState( void )
{
	return spVirtualKeyboardShift;
}

PREFIX int spIsKeyboardPolled( void )
{
	return (spGenericInput.keyboard.buffer != NULL);
}

PREFIX void spStereoMergeSurfaces(SDL_Surface* left,SDL_Surface* right,int crossed)
{
	if (left->w != right->w || left->h != right->h)
		return;
	int W = left->pitch/left->format->BytesPerPixel;
	int H = left->h;
	//merge
	SDL_LockSurface(left);
	SDL_LockSurface(right);
	if (crossed)
	{
		Uint16* right_pixels = (Uint16*)right->pixels;
		Uint16* pixels = (Uint16*)left->pixels;
		int x,y;
		for (x = 0; x < W/2; x++)
			for (y = 0; y < H; y++)
				pixels[x+y*W] = pixels[x*2+y*W];
		for (; x < W; x++)
			for (y = 0; y < H; y++)
				pixels[x+y*W] = right_pixels[(x-W/2)*2+y*W];
	}
	else
	{
		Uint32* right_pixels = (Uint32*)right->pixels;
		Uint32* pixels = (Uint32*)left->pixels;
		int i;
		int all = W*H/2;
		for (i = 0; i < all; i++)
		if (right_pixels[i])
			pixels[i] |= right_pixels[i];
	}
	SDL_UnlockSurface(left);
	SDL_UnlockSurface(right);
}

PREFIX void spSleep(Uint32 microSeconds)
{
	#ifdef REALGP2X
		//TODO: Implement!
		#ifdef DEBUG_SLOWMOTION
			int t = microSeconds/1000*DEBUG_SLOWMOTION;
		#else
			int t = microSeconds/1000;
		#endif
		if (t <= 0)
			t = 1;
		SDL_Delay(t);
	#elif defined _WIN32
		//#warning Implement me better for windows pleeeease. :)
		// There is no better way, sorry :)
		#ifdef DEBUG_SLOWMOTION
			int t = microSeconds/1000*DEBUG_SLOWMOTION;
		#else
			int t = microSeconds/1000;
		#endif
		SDL_Delay( (t < 1 ? 1 : t) );
	#else
		#ifdef DEBUG_SLOWMOTION
			usleep(microSeconds*DEBUG_SLOWMOTION);
		#else
			usleep(microSeconds);
		#endif
	#endif
}

PREFIX int spGetLastAxisType( void )
{
	return spLastAxisType;
}

PREFIX void spSetReturnBehavior(int ignore,int stops)
{
	spKeyboardReturnIgnore = ignore;
	spKeyboardReturnStops = stops;
}

PREFIX void spSetVirtualKeyboardSpaceButton(int button)
{
	spVirtualKeyboardSpaceButton = button;
}

PREFIX void spSetVirtualKeyboardBackspaceButton(int button)
{
	spVirtualKeyboardBackspaceButton = button;
}

