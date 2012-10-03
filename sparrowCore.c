/*
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms
 of the GNU Lesser General Public license (the  "LGPL License"), in which case the
 provisions of LGPL License are applicable instead of those
 above.

 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "sparrowCore.h"
#include "sparrowMath.h"
#include "sparrowPrimitives.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include <string.h>

int spWindowX = 0;
int spWindowY = 0;
int spZoom;
SDL_Surface* spScreen; //the real screen
SDL_Surface* spWindow; //the thing we draw to
SDL_Joystick **spJoy = NULL;
char spDone;
int spFPS;
TspInput spInput;
int debug_time;
int sp_touchscreen_emulation = 0;
int sp_switch_button = -1;
int sp_ok_button = -1;
int sp_touchscreen_dx;
int sp_touchscreen_dy;
char sp_caching = 1;
int sp_axis_was_used[SP_INPUT_AXIS_COUNT];
SDL_Surface* spVirtualKeyboard = NULL;
int spVirtualKeyboardState = SP_VIRTUAL_KEYBOARD_NEVER;
Sint32 spVirtualKeyboardMask = 0;

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

PREFIX void spSetDefaultWindowSize( int w, int h )
{
	spWindowX = w;
	spWindowY = h;
}

PREFIX void spInitCore( void )
{
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
#else
	//only setting, if now default value set!
	if ( !spWindowX )
		spWindowX = 320;
	if ( !spWindowY )
		spWindowY = 240;
#endif
	spZoom = SP_ONE;
	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO );
	spJoy = NULL;
	//#ifdef MOBILE_DEVICE
	printf( "Found %i Joysticks\n", SDL_NumJoysticks() );
	if ( SDL_NumJoysticks() > 0 )
	{
		spJoy = ( SDL_Joystick** )malloc( SDL_NumJoysticks() * sizeof( SDL_Joystick* ) );
		for ( i = 0; i < SDL_NumJoysticks(); i++ )
		{
			spJoy[i] = SDL_JoystickOpen( i );
			printf( "  Opened Joystick %i (%s)\n", i, SDL_JoystickName( i ) );
		}
	}
	//#endif
	spScreen = NULL;
	spWindow = NULL;
	spDone = 0;
	spFPS = 0;
	spResetButtonsState();
	spResetAxisState();
	for (i = 0; i < SP_INPUT_AXIS_COUNT; i++)
		sp_axis_was_used[i] = 0;
	spInput.touchscreen.pressed = 0;
	spInput.touchscreen.x = 0;
	spInput.touchscreen.y = 0;
	spInput.keyboard.buffer = NULL;
	spInput.keyboard.len = 0;
	spInput.keyboard.pos = 0;
	spInput.keyboard.lastSize = 0;

#ifdef GP2X
	//f100, f200, open2x and wiz
	spInput.supports_keyboard = 0;
#elif defined CAANOO
	spInput.supports_keyboard = 0;
#elif defined DINGUX
	spInput.supports_keyboard = 0;
#else // PANDORA and PCs
	spInput.supports_keyboard = 1;
#endif

	spInitPrimitives();
	spInitMath();
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

PREFIX void spResizeWindow( int x, int y, int fullscreen, int allowresize )
{
	//if the renderTarget is the screen and the screen is recreated, the pointer
	//will be invalid. We have to check for that:
	int recallSelectRenderTarget = 0;
	if (spWindow == spGetRenderTarget())
		recallSelectRenderTarget = 1;
#ifdef GP2X
	spScreen = SDL_SetVideoMode( x, y, 16, SDL_HWSURFACE );
	SDL_Surface* surface = SDL_CreateRGBSurface( SDL_HWSURFACE, x, y, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0 );
	spWindow = SDL_DisplayFormat( surface );
	SDL_FreeSurface( surface );
#elif defined CAANOO
	spScreen = SDL_SetVideoMode( x, y, 16, SDL_HWSURFACE );
	SDL_Surface* surface = SDL_CreateRGBSurface( SDL_HWSURFACE, x, y, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0 );
	spWindow = SDL_DisplayFormat( surface );
	SDL_FreeSurface( surface );
#elif defined DINGUX
	spScreen = SDL_SetVideoMode( x, y, 16, SDL_HWSURFACE | SDL_FULLSCREEN );
	SDL_Surface* surface = SDL_CreateRGBSurface( SDL_HWSURFACE, x, y, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0 );
	spWindow = SDL_DisplayFormat( surface );
	SDL_FreeSurface( surface );
#elif defined PANDORA
	spScreen = NULL;
	spWindow = SDL_SetVideoMode( x, y, 16, SDL_HWSURFACE | SDL_DOUBLEBUF | ( fullscreen ? SDL_FULLSCREEN : 0 ) );
#elif defined MAEMO
	spScreen = NULL;
	spWindow = SDL_SetVideoMode( x, y, 16, SDL_HWSURFACE | SDL_DOUBLEBUF | ( fullscreen ? SDL_FULLSCREEN : 0 ) );
#else
	/*x=800;
	y=480;*/
	spScreen = NULL;
	spWindow = SDL_SetVideoMode( x, y, 16, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_HWPALETTE | ( allowresize ? SDL_RESIZABLE : 0 ) | ( fullscreen ? SDL_FULLSCREEN : 0 ) );
	//spWindow=SDL_SetVideoMode(x,y,16,SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_NOFRAME);
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
	#ifdef X86CPU
		return spCreateWindow( 0, 0, 0, 1 );
	#else
		return spCreateWindow( 0, 0, 1, 1 );
	#endif
}

PREFIX SDL_Surface* spGetWindowSurface( void )
{
	return spWindow;
}

//for long pressing of a key
SDL_keysym spLastKey = {0,0,0,0};
int spLastKeyCountDown = 0;

static void spHandleKeyboardInput( const SDL_keysym pressedKey)
{
	if ( pressedKey.sym == SDLK_BACKSPACE )
	{
		if ( spInput.keyboard.pos > 0 )
		{
			if ( spInput.keyboard.lastSize == 0 ) // need to determine size of last char in buffer
			{
				int I;
				for ( I = strlen( spInput.keyboard.buffer ) - 1; I >= 0; --I )
				{
					++spInput.keyboard.lastSize;
					if ( !((Uint8)(spInput.keyboard.buffer[I] & 128) == 128 && (Uint8)(spInput.keyboard.buffer[I] & 64) == 0)) //no follower bit
						break;
				}
			}
			if ( spInput.keyboard.lastSize > 0 )
			{
				spInput.keyboard.pos -= spInput.keyboard.lastSize;
				spInput.keyboard.buffer[spInput.keyboard.pos] = '\0';
				spInput.keyboard.lastSize = 0;
			}
		}
	}
	/*else if ( pressedKey.sym == SDLK_RETURN )
	{
		if ( spInput.keyboard.pos + 1 <= spInput.keyboard.len )
		{
			strcat( spInput.keyboard.buffer, "\n" );
			spInput.keyboard.lastSize = 1;
			spInput.keyboard.pos += 1;
		}
	}*/
	else if ( pressedKey.sym >= SDLK_SPACE )
	{
		Uint16 c = pressedKey.unicode;
		char temp[5];
		spFontGetUTF8FromUnicode( c, temp, 5 );
		int s = strlen( temp );
		if ( spInput.keyboard.pos + s <= spInput.keyboard.len )
		{
			strcat( spInput.keyboard.buffer, temp );
			spInput.keyboard.lastSize = s;
			spInput.keyboard.pos += s;
		}
	}
}

inline int spHandleEvent( void ( *spEvent )( SDL_Event *e ) )
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
		sprintf( buffer, "    Fetching Event %i\n", counter );
		spPrintDebug( buffer );
#endif
		switch ( event.type )
		{
			case SDL_MOUSEBUTTONDOWN:
			  spInput.touchscreen.pressed = 1;
			  spInput.touchscreen.x = event.button.x;
			  spInput.touchscreen.y = event.button.y;
			  break;
			case SDL_MOUSEBUTTONUP:
			  spInput.touchscreen.pressed = 0;
			  break;
			case SDL_MOUSEMOTION:
				if (spInput.touchscreen.pressed)
				{
					spInput.touchscreen.x = event.motion.x;
					spInput.touchscreen.y = event.motion.y;
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
								newevent.button.x = spInput.touchscreen.x;
								newevent.button.y = spInput.touchscreen.y;
								SDL_PushEvent(&event);
							}
						}
					}
					else
			  #endif
				spInput.button[event.jbutton.button] = 1;
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
								newevent.button.x = spInput.touchscreen.x;
								newevent.button.y = spInput.touchscreen.y;
								SDL_PushEvent(&event);
							}
						}

					}
					else
			  #endif
				spInput.button[event.jbutton.button] = 0;
				break;
			case SDL_KEYDOWN:
				if ( spInput.keyboard.buffer )
				{
					spHandleKeyboardInput(event.key.keysym);
					spLastKey = event.key.keysym;
					spLastKeyCountDown = SP_KEYBOARD_FIRST_WAIT;
				}
				switch ( event.key.keysym.sym )
				{
				case SDLK_LEFT:
					spInput.axis[0] = -1;
					spInput.analog_axis[0] = SP_ANALOG_AXIS_MIN;
					break;
				case SDLK_RIGHT:
					spInput.axis[0] = 1;
					spInput.analog_axis[0] = SP_ANALOG_AXIS_MAX;
					break;
				case SDLK_UP:
					spInput.axis[1] = -1;
					spInput.analog_axis[1] = SP_ANALOG_AXIS_MIN;
					break;
				case SDLK_DOWN:
					spInput.axis[1] = 1;
					spInput.analog_axis[1] = SP_ANALOG_AXIS_MAX;
					break;
			#ifdef DINGUX
				case SDLK_RETURN:
					spInput.button[SP_BUTTON_START] = 1;
					break;
				case SDLK_SPACE:
					spInput.button[SP_BUTTON_X] = 1;
					break;
				case SDLK_LSHIFT:
					spInput.button[SP_BUTTON_Y] = 1;
					break;
				case SDLK_LCTRL:
					spInput.button[SP_BUTTON_A] = 1;
					break;
				case SDLK_LALT:
					spInput.button[SP_BUTTON_B] = 1;
					break;
				case SDLK_ESCAPE:
					spInput.button[SP_BUTTON_SELECT] = 1;
					break;
				case SDLK_TAB:
					spInput.button[SP_BUTTON_L] = 1;
					break;
				case SDLK_BACKSPACE:
					spInput.button[SP_BUTTON_R] = 1;
					break;
				case SDLK_s:
					spInput.button[SP_BUTTON_VOLPLUS] = 1;
					break;
				case SDLK_u:
					spInput.button[SP_BUTTON_VOLMINUS] = 1;
					break;
			#elif defined PANDORA
//				case SDLK_MENU:
//					spDone=1;
//					break;
				case SDLK_PAGEDOWN:
					spInput.button[SP_BUTTON_X] = 1;
					break;
				case SDLK_PAGEUP:
					spInput.button[SP_BUTTON_Y] = 1;
					break;
				case SDLK_HOME:
					spInput.button[SP_BUTTON_A] = 1;
					break;
				case SDLK_END:
					spInput.button[SP_BUTTON_B] = 1;
					break;
				case SDLK_LCTRL:
					spInput.button[SP_BUTTON_SELECT] = 1;
					break;
				case SDLK_RSHIFT:
					spInput.button[SP_BUTTON_L] = 1;
					break;
				case SDLK_RCTRL:
					spInput.button[SP_BUTTON_R] = 1;
					break;
				case SDLK_LALT:
					spInput.button[SP_BUTTON_START] = 1;
					break;
			#else //PC
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					spInput.button[SP_BUTTON_START] = 1;
					break;
				#ifdef DO_USE_NOT_WASD_BUTTONS
					case SDLK_MODE:
						spInput.button[SP_BUTTON_A] = 1;
						break;
					case SDLK_LCTRL:
						spInput.button[SP_BUTTON_B] = 1;
						break;
					case SDLK_LALT:case SDLK_RALT:
						spInput.button[SP_BUTTON_Y] = 1;
						break;
					case SDLK_LSUPER:case SDLK_RSUPER:
						spInput.button[SP_BUTTON_X] = 1;
						break;
					case SDLK_ESCAPE:
						spInput.button[SP_BUTTON_SELECT] = 1;
						break;
					case SDLK_PAGEUP:
						spInput.button[SP_BUTTON_L] = 1;
						break;
					case SDLK_PAGEDOWN:
						spInput.button[SP_BUTTON_R] = 1;
						break;
				#else
					case SDLK_a:
						spInput.button[SP_BUTTON_A] = 1;
						break;
					case SDLK_d:
						spInput.button[SP_BUTTON_B] = 1;
						break;
					case SDLK_w:
						spInput.button[SP_BUTTON_Y] = 1;
						break;
					case SDLK_s:
						spInput.button[SP_BUTTON_X] = 1;
						break;
					case SDLK_BACKSPACE:
						spInput.button[SP_BUTTON_SELECT] = 1;
						break;
					case SDLK_q:
						spInput.button[SP_BUTTON_L] = 1;
						break;
					case SDLK_e:
						spInput.button[SP_BUTTON_R] = 1;
						break;
				#endif
				case SDLK_PLUS:
					spInput.button[SP_BUTTON_VOLPLUS] = 1;
					break;
				case SDLK_MINUS:
					spInput.button[SP_BUTTON_VOLMINUS] = 1;
					break;
			#endif
				}
				break;
			case SDL_KEYUP:
				if ( spInput.keyboard.buffer )
				{
					spLastKey.unicode = 0;
					spLastKeyCountDown = 0;
				}
				switch ( event.key.keysym.sym )
				{
				case SDLK_LEFT:
					if ( spInput.axis[0] == -1 )
					{
						spInput.axis[0] = 0;
						spInput.analog_axis[0] = 0;
					}
					break;
				case SDLK_RIGHT:
					if ( spInput.axis[0] == 1 )
					{
						spInput.axis[0] = 0;
						spInput.analog_axis[0] = 0;
					}
					break;
				case SDLK_UP:
					if ( spInput.axis[1] == -1 )
					{
						spInput.axis[1] = 0;
						spInput.analog_axis[1] = 0;
					}
					break;
				case SDLK_DOWN:
					if ( spInput.axis[1] == 1 )
					{
						spInput.axis[1] = 0;
						spInput.analog_axis[1] = 0;
					}
					break;
			#ifdef DINGUX
				case SDLK_RETURN:
					spInput.button[SP_BUTTON_START] = 0;
					break;
				case SDLK_SPACE:
					spInput.button[SP_BUTTON_X] = 0;
					break;
				case SDLK_LSHIFT:
					spInput.button[SP_BUTTON_Y] = 0;
					break;
				case SDLK_LCTRL:
					spInput.button[SP_BUTTON_A] = 0;
					break;
				case SDLK_LALT:
					spInput.button[SP_BUTTON_B] = 0;
					break;
				case SDLK_ESCAPE:
					spInput.button[SP_BUTTON_SELECT] = 0;
					break;
				case SDLK_TAB:
					spInput.button[SP_BUTTON_L] = 0;
					break;
				case SDLK_BACKSPACE:
					spInput.button[SP_BUTTON_R] = 0;
					break;
				case SDLK_s:
					spInput.button[SP_BUTTON_VOLPLUS] = 0;
					break;
				case SDLK_u:
					spInput.button[SP_BUTTON_VOLMINUS] = 0;
					break;
			#elif defined PANDORA
				case SDLK_PAGEDOWN:
					spInput.button[SP_BUTTON_X] = 0;
					break;
				case SDLK_PAGEUP:
					spInput.button[SP_BUTTON_Y] = 0;
					break;
				case SDLK_HOME:
					spInput.button[SP_BUTTON_A] = 0;
					break;
				case SDLK_END:
					spInput.button[SP_BUTTON_B] = 0;
					break;
				case SDLK_LCTRL:
					spInput.button[SP_BUTTON_SELECT] = 0;
					break;
				case SDLK_RSHIFT:
					spInput.button[SP_BUTTON_L] = 0;
					break;
				case SDLK_RCTRL:
					spInput.button[SP_BUTTON_R] = 0;
					break;
				case SDLK_LALT:
					spInput.button[SP_BUTTON_START] = 0;
					break;
			#else //PC
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					spInput.button[SP_BUTTON_START] = 0;
					break;
				#ifdef DO_USE_NOT_WASD_BUTTONS
					case SDLK_MODE:
						spInput.button[SP_BUTTON_A] = 0;
						break;
					case SDLK_LCTRL:
						spInput.button[SP_BUTTON_B] = 0;
						break;
					case SDLK_LALT:case SDLK_RALT:
						spInput.button[SP_BUTTON_Y] = 0;
						break;
					case SDLK_LSUPER:case SDLK_RSUPER:
						spInput.button[SP_BUTTON_X] = 0;
						break;
					case SDLK_ESCAPE:
						spInput.button[SP_BUTTON_SELECT] = 0;
						break;
					case SDLK_PAGEUP:
						spInput.button[SP_BUTTON_L] = 0;
						break;
					case SDLK_PAGEDOWN:
						spInput.button[SP_BUTTON_R] = 0;
						break;
				#else
					case SDLK_a:
						spInput.button[SP_BUTTON_A] = 0;
						break;
					case SDLK_d:
						spInput.button[SP_BUTTON_B] = 0;
						break;
					case SDLK_w:
						spInput.button[SP_BUTTON_Y] = 0;
						break;
					case SDLK_s:
						spInput.button[SP_BUTTON_X] = 0;
						break;
					case SDLK_BACKSPACE:
						spInput.button[SP_BUTTON_SELECT] = 0;
						break;
					case SDLK_q:
						spInput.button[SP_BUTTON_L] = 0;
						break;
					case SDLK_e:
						spInput.button[SP_BUTTON_R] = 0;
						break;
				#endif
				case SDLK_PLUS:
					spInput.button[SP_BUTTON_VOLPLUS] = 0;
					break;
				case SDLK_MINUS:
					spInput.button[SP_BUTTON_VOLMINUS] = 0;
					break;				
			#endif
				}
				break;
			case SDL_JOYAXISMOTION:
				spInput.analog_axis[event.jaxis.axis & 1] = event.jaxis.value;
				if ( event.jaxis.value < SP_JOYSTICK_MIN_TRIGGER_ON && sp_axis_was_used[event.jaxis.axis & 1] != -1 )
				{
					spInput.axis[event.jaxis.axis & 1] = -1;
					sp_axis_was_used[event.jaxis.axis & 1] = -1;
				}
				else
				if ( event.jaxis.value > SP_JOYSTICK_MAX_TRIGGER_ON && sp_axis_was_used[event.jaxis.axis & 1] != 1)
				{
					spInput.axis[event.jaxis.axis & 1] = 1;
					sp_axis_was_used[event.jaxis.axis & 1] = 1;
				}
				else
				if (event.jaxis.value > SP_JOYSTICK_MIN_TRIGGER_OFF && event.jaxis.value < SP_JOYSTICK_MAX_TRIGGER_OFF)
				{
					spInput.axis[event.jaxis.axis & 1] = 0;
					sp_axis_was_used[event.jaxis.axis & 1] = 0;
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
#ifdef CAANOO
	spInput.button[SP_BUTTON_VOLPLUS] = 0;
	spInput.button[SP_BUTTON_VOLMINUS] = 0;
#endif
	return result;
}

inline void spUpdateAxis( int axis )
{
#ifdef GP2X
	if ( axis == 0 )
	{
		spInput.axis[axis] = 0;
		if ( spInput.button[SP_AXIS_LEFTUP] ||
				spInput.button[SP_AXIS_LEFT]   ||
				spInput.button[SP_AXIS_LEFTDOWN] )
			spInput.axis[axis] = -1;
		if ( spInput.button[SP_AXIS_RIGHTUP] ||
				spInput.button[SP_AXIS_RIGHT]   ||
				spInput.button[SP_AXIS_RIGHTDOWN] )
			spInput.axis[axis] = 1;
		if (spInput.axis[axis] == -1)
			spInput.analog_axis[axis] = SP_ANALOG_AXIS_MIN;
		else
		if (spInput.axis[axis] ==  1)
			spInput.analog_axis[axis] = SP_ANALOG_AXIS_MAX;
		else
		spInput.analog_axis[axis] = 0;
	}
	else
	{
		spInput.axis[axis] = 0;
		if ( spInput.button[SP_AXIS_LEFTUP] ||
				spInput.button[SP_AXIS_UP]   ||
				spInput.button[SP_AXIS_RIGHTUP] )
			spInput.axis[axis] = -1;
		if ( spInput.button[SP_AXIS_LEFTDOWN] ||
				spInput.button[SP_AXIS_DOWN]   ||
				spInput.button[SP_AXIS_RIGHTDOWN] )
			spInput.axis[axis] = 1;
		if (spInput.axis[axis] == -1)
			spInput.analog_axis[axis] = SP_ANALOG_AXIS_MIN;
		else
		if (spInput.axis[axis] ==  1)
			spInput.analog_axis[axis] = SP_ANALOG_AXIS_MAX;
		else
		spInput.analog_axis[axis] = 0;
	}
#endif
}

void spHandleFakeKeybard( int steps )
{
	if (spLastKey.unicode != 0)
	{
		spLastKeyCountDown -= steps;
		if (spLastKeyCountDown <= 0)
		{
			spLastKeyCountDown += SP_KEYBOARD_WAIT;
			spHandleKeyboardInput(spLastKey);
		}
	}
}

Uint32 oldticks;
Uint32 olderticks;
Uint32 newticks;

PREFIX int spLoop( void ( *spDraw )( void ), int ( *spCalc )( Uint32 steps ), Uint32 minwait, void ( *spResize )( Uint16 w, Uint16 h ), void ( *spEvent )( SDL_Event *e ) )
{
	Uint32 bigsteps = 0;
	Uint32 frames = 0;
	int back = 0;
	//time since the last frame
	Uint32 steps = 0;
	//time from the last calculation
	oldticks = SDL_GetTicks();
	//time of the current loop pass
	newticks = oldticks;

	while( back == 0 && !spDone )
	{
#ifdef CORE_DEBUG
		spPrintDebug( "Start mainloop" );
#endif
		oldticks = newticks;
		newticks = SDL_GetTicks();
		Uint32 diffticks = newticks-oldticks;
#ifdef F100
		//mouse movement emulation: *untested*
		if (sp_touchscreen_emulation)
		{
			//If I read right in the SDL code (why document it, if you can read it
			//in the code? -_-), SDL_WarpMouse should test for the screen dimension.
			int mouse_steps = newticks - oldticks;
			if (mouse_steps)
				SDL_WarpMouse(spInput.touchscreen.x + sp_touchscreen_dx * mouse_steps,
											spInput.touchscreen.y + sp_touchscreen_dy * mouse_steps);
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
		//Calls with diffticks == 0 are possible!
		if ( spCalc && diffticks)
		{
			back = spCalc( diffticks );
#ifdef CORE_DEBUG
			spPrintDebug( "  Did calc" );
#endif
			spHandleFakeKeybard(diffticks);
#ifdef CORE_DEBUG
			spPrintDebug( "  Did fake keyboard events" );
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
			SDL_Delay(1);
		#endif
	}
	return back;
}

PREFIX void spFlip( void )
{
#ifdef CORE_DEBUG
	spPrintDebug( "    Flip in" );
#endif
	//The Flip
#ifdef GP2X
	SDL_BlitSurface( spWindow, NULL, spScreen, NULL );
#elif defined CAANOO
	SDL_BlitSurface( spWindow, NULL, spScreen, NULL );
#elif defined PANDORA
	/*int arg = 0;
	ioctl(fbdev, FBIO_WAITFORVSYNC, &arg);*/
	SDL_Flip( spWindow );
#elif defined DINGUX
	SDL_BlitSurface( spWindow, NULL, spScreen, NULL );
	//SDL_Flip(spWindow);
#else //PC
	SDL_Flip( spWindow );
	//SDL_UpdateRect(spWindow, 0, 0, 0, 0);
#endif
#ifdef CORE_DEBUG
	spPrintDebug( "    Flip out" );
#endif
}

PREFIX PspInput spGetInput( void )
{
	return &spInput;
}

PREFIX void spResetButtonsState( void )
{
	int I;
	for ( I = 0; I < SP_INPUT_BUTTON_COUNT; ++I )
		spInput.button[I] = 0;
}

PREFIX void spResetAxisState( void )
{
	int I;
	for ( I = 0; I < SP_INPUT_AXIS_COUNT; ++I )
	{
		spInput.axis[I] = 0;
		spInput.analog_axis[I] = 0;
	}
}

PREFIX void spPollKeyboardInput( char *buffer, int bufferSize, Sint32 enter_key_mask)
{
	if ( bufferSize > 0 && buffer)
	{
		spInput.keyboard.buffer = buffer;
		spInput.keyboard.len = bufferSize;
		spInput.keyboard.pos = strlen( buffer );
		spInput.keyboard.lastSize = 0;
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
	spInput.keyboard.buffer = NULL;
	spInput.keyboard.len = 0;
	spInput.keyboard.pos = 0;
	spInput.keyboard.lastSize = 0;
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
			SDL_JoystickClose( spJoy[i] );
		free( spJoy );
	}
	//#endif
	spQuitPrimitives();
	SDL_Quit();
}

PREFIX int spGetFPS( void )
{
	return spFPS;
}

PREFIX Sint32 spGetSizeFactor( void )
{
	return spZoom;
}

SDL_Surface* spLoadUncachedSurface( char* name )
{
	SDL_Surface* surface = IMG_Load( name );
	/*printf("%s: %i\n",name,surface->format->BitsPerPixel);
	if (surface->format->BitsPerPixel==32) //removing alpha
	{
	  SDL_LockSurface(surface);
	  Uint8* pixel = (Uint8*)(surface->pixels);
	  int x,y;
	  for (x = 0; x < surface->w; x++)
	    for (y = 0; y < surface->h; y++)
	    {
	      if (pixel[(x+y*surface->w)*4+3] > 128)
	      {
	        pixel[(x+y*surface->w)*4+3] = 255;
	        if (pixel[(x+y*surface->w)*4+0] == 255 &&
	            pixel[(x+y*surface->w)*4+1] == 0   &&
	            pixel[(x+y*surface->w)*4+2] == 255 ) //removing pink
	        {
	          pixel[(x+y*surface->w)*4+0] = 247;
	          pixel[(x+y*surface->w)*4+2] = 247;
	        }
	      }
	      else
	      {
	        pixel[(x+y*surface->w)*4+0] = 255;
	        pixel[(x+y*surface->w)*4+1] = 0;
	        pixel[(x+y*surface->w)*4+2] = 255;
	        pixel[(x+y*surface->w)*4+3] = 255;
	      }
	    }

	  SDL_UnlockSurface(surface);
	}*/
	if ( !surface )
	{
		printf( "Failed to load surface \"%s\", uncool...\n", name );
		printf( "  Error was: \"%s\"\n", IMG_GetError() );
		return NULL;
	}
	SDL_Surface* result = SDL_DisplayFormat( surface );
	SDL_FreeSurface( surface );
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

PREFIX SDL_Surface* spLoadSurface( char* name )
{
	if (sp_caching)
	{
		sp_cache_pointer c = sp_get_cached_surface_by_name(name);
		if (c)
			c->ref++;
		else
		{
			//not found, creating and adding
			SDL_Surface *surface = spLoadUncachedSurface(name);
			if (surface == NULL)
				return NULL; //Null surfaces are not cached...
			c = (sp_cache_pointer)malloc(sizeof(sp_cache));
			c->surface = surface;
			c->name = (char*)malloc(strlen(name)+1);
			sprintf(c->name,"%s",name);
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
		}
		return c->surface;
	}
	else
		return spLoadUncachedSurface(name);
}

PREFIX void spEnableCaching()
{
	sp_caching = 1;
}

PREFIX void spDisableCaching()
{
	sp_caching = 0;
}

PREFIX int spIsCachingEnabled()
{
	return sp_caching;
}

PREFIX void spClearCache()
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
	SDL_Surface* surface = SDL_CreateRGBSurface( SDL_HWSURFACE, width, height, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0 );
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

PREFIX Uint16 spGetHSV(Sint32 h, Uint8 s, Uint8 v)
{
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
	SDL_LockSurface( source );
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

	SDL_UnlockSurface( source );
	SDL_UnlockSurface( destination );
}

PREFIX void spScale2XFast(SDL_Surface* source,SDL_Surface* destination)
{
	SDL_LockSurface( source );
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
	SDL_UnlockSurface( source );
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

PREFIX void spSetVirtualKeyboard(int state,int x,int y,SDL_Surface* design)
{
	spVirtualKeyboardState = state;
	if (spVirtualKeyboard)
		spDeleteSurface(spVirtualKeyboard);
	if (design == NULL)
	{
		spVirtualKeyboard = design;
		return;
	}
	spVirtualKeyboard = spCreateSurface(x,y);
	SDL_LockSurface(design);
	SDL_LockSurface(spVirtualKeyboard);
	Uint16* des = (Uint16*)design->pixels;
	Uint16* vir = (Uint16*)spVirtualKeyboard->pixels;
	int design_w = (design->w/21)*20;
	Sint32 add_x = (design_w  << SP_ACCURACY)/x;
	Sint32 add_y = (design->h << SP_ACCURACY)/y;
	int u,v;
	Sint32 X,Y=0;
	Sint32 vir_m = spVirtualKeyboard->pitch/spVirtualKeyboard->format->BytesPerPixel;
	Sint32 des_m = design->pitch/design->format->BytesPerPixel;
	for (v = 0; v < y; v++)
	{
		X = 0;
		for (u = 0; u < x; u++)
		{
			vir[u+v*vir_m] = des[(X>>SP_ACCURACY)+(Y>>SP_ACCURACY)*des_m];
			X+=add_x;
		}
		Y+=add_y;
	}


	SDL_UnlockSurface(design);
	SDL_UnlockSurface(spVirtualKeyboard);
	/*SDL_Surface *oldTarget = spGetRenderTarget();
	spSelectRenderTarget(spVirtualKeyboard);
	
	Sint32 vert = spGetVerticalOrigin();
	Sint32 hori = spGetHorizontalOrigin();
	spSetVerticalOrigin(SP_TOP);
	spSetHorizontalOrigin(SP_LEFT);
	spSetZSet(0);
	spSetZTest(0);
	int design_w = (design->w/21)*20;
	spRotozoomSurfacePart(0,0,0,design,0,0,design_w,design->h,(x<<SP_ACCURACY)/design_w,(y<<SP_ACCURACY)/design->h,0);
	
	spSetVerticalOrigin(vert);
	spSetHorizontalOrigin(hori);
	if (oldTarget)
		spSelectRenderTarget(oldTarget);*/
}

PREFIX SDL_Surface* spGetVirtualKeyboard()
{
	return spVirtualKeyboard;
}

PREFIX int spIsKeyboardPolled()
{
	return (spInput.keyboard.buffer != NULL);
}
