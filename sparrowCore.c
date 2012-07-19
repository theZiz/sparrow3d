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
	for ( i = 0; i < 20; i++ )
		spInput.button[i] = 0;
	spInput.axis[0] = 0;
	spInput.axis[1] = 0;
	spInput.touchscreen.pressed = 0;
	spInput.touchscreen.x = 0;
	spInput.touchscreen.y = 0;
	spInput.keyboard.buffer = NULL;
	spInput.keyboard.filter = NULL;
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
	spWindow = SDL_SetVideoMode( x, y, 16, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN );
#elif defined MAEMO
	spScreen = NULL;
	spWindow = SDL_SetVideoMode( x, y, 16, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN );
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
	return spCreateWindow( 0, 0, 0, 1 );
}

PREFIX SDL_Surface* spGetWindowSurface( void )
{
	return spWindow;
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
				switch ( event.key.keysym.sym )
				{
				case SDLK_LEFT:
					spInput.axis[0] = -1;
					break;
				case SDLK_RIGHT:
					spInput.axis[0] = 1;
					break;
				case SDLK_UP:
					spInput.axis[1] = 1;
					break;
				case SDLK_DOWN:
					spInput.axis[1] = -1;
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
				case SDLK_PLUS:
					spInput.button[SP_BUTTON_VOLPLUS] = 1;
					break;
				case SDLK_MINUS:
					spInput.button[SP_BUTTON_VOLMINUS] = 1;
					break;
				case SDLK_q:
					spInput.button[SP_BUTTON_L] = 1;
					break;
				case SDLK_e:
					spInput.button[SP_BUTTON_R] = 1;
					break;
			#endif
				case SDLK_m:
					//spMuteKey=1;
					break;
				}
				break;
			case SDL_KEYUP:
				switch ( event.key.keysym.sym )
				{
				case SDLK_LEFT:
					if ( spInput.axis[0] == -1 )
						spInput.axis[0] = 0;
					break;
				case SDLK_RIGHT:
					if ( spInput.axis[0] == 1 )
						spInput.axis[0] = 0;
					break;
				case SDLK_UP:
					if ( spInput.axis[1] == 1 )
						spInput.axis[1] = 0;
					break;
				case SDLK_DOWN:
					if ( spInput.axis[1] == -1 )
						spInput.axis[1] = 0;
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
				case SDLK_PLUS:
					spInput.button[SP_BUTTON_VOLPLUS] = 0;
					break;
				case SDLK_MINUS:
					spInput.button[SP_BUTTON_VOLMINUS] = 0;
					break;
				case SDLK_q:
					spInput.button[SP_BUTTON_L] = 0;
					break;
				case SDLK_e:
					spInput.button[SP_BUTTON_R] = 0;
					break;
			#endif
				case SDLK_m:
					//spMuteKey=0;
					break;
				}
				break;
			case SDL_JOYAXISMOTION:
				if ( !( event.jaxis.axis & 1 ) )
				{
					if ( event.jaxis.value < SP_JOYSTICK_MIN )
						spInput.axis[event.jaxis.axis] = -1;
					else if ( event.jaxis.value > SP_JOYSTICK_MAX )
						spInput.axis[event.jaxis.axis] = 1;
					else
						spInput.axis[event.jaxis.axis] = 0;
				}
				if ( event.jaxis.axis & 1 )
				{
					if ( event.jaxis.value < SP_JOYSTICK_MIN )
						spInput.axis[event.jaxis.axis] = 1;
					else if ( event.jaxis.value > SP_JOYSTICK_MAX )
						spInput.axis[event.jaxis.axis] = -1;
					else
						spInput.axis[event.jaxis.axis] = 0;
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

		if ( spInput.keyboard.buffer && event.type == SDL_KEYDOWN )
		{
			if ( event.key.keysym.sym == SDLK_BACKSPACE )
			{
				if ( spInput.keyboard.pos > 0 )
				{
					if ( spInput.keyboard.lastSize == 0 ) // need to determine size of last char in buffer
					{
						int I;
						for ( I = strlen( spInput.keyboard.buffer ) - 1; I >= 0; --I )
						{
							++spInput.keyboard.lastSize;
							if ( spInput.keyboard.buffer[I] >= 0 ) // unicode-signs are < 0
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
			else if ( event.key.keysym.sym >= SDLK_SPACE && event.key.keysym.sym <= SDLK_z )
			{
				Uint16 c = event.key.keysym.unicode;
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
	}
	else
	{
		spInput.axis[axis] = 0;
		if ( spInput.button[SP_AXIS_LEFTUP] ||
				spInput.button[SP_AXIS_UP]   ||
				spInput.button[SP_AXIS_RIGHTUP] )
			spInput.axis[axis] = 1;
		if ( spInput.button[SP_AXIS_LEFTDOWN] ||
				spInput.button[SP_AXIS_DOWN]   ||
				spInput.button[SP_AXIS_RIGHTDOWN] )
			spInput.axis[axis] = -1;
	}
#endif
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
	oldticks = olderticks;
	//time of the current loop pass
	newticks = olderticks;

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
		if ( spCalc )
			back = spCalc( diffticks );
#ifdef CORE_DEBUG
		spPrintDebug( "  Did calc" );
#endif
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

PREFIX void spPollKeyboardInput( char *buffer, int bufferSize, char *filter )
{
	if ( bufferSize > 0 )
	{
		spInput.keyboard.buffer = buffer;
		spInput.keyboard.filter = filter;
		spInput.keyboard.len = bufferSize;
		spInput.keyboard.pos = strlen( buffer );
		spInput.keyboard.lastSize = 0;
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
	spInput.keyboard.filter = NULL;
	spInput.keyboard.len = 0;
	spInput.keyboard.pos = 0;
	spInput.keyboard.lastSize = 0;
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
		for ( i = 0; i < SDL_NumJoysticks(); i++ );
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

PREFIX SDL_Surface* spLoadSurface( char* name )
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

PREFIX SDL_Surface* spCreateSurface(int width,int height)
{
	SDL_Surface* surface = SDL_CreateRGBSurface( SDL_HWSURFACE, width, height, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0 );
	SDL_Surface* result = SDL_DisplayFormat( surface );
	SDL_FreeSurface( surface );
	return result;
}

PREFIX void spDeleteSurface( SDL_Surface* surface )
{
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
	h = h % (2 * SP_PI);
	Uint8 hi = h / (SP_PI / 3);
	Sint32 f = ((3 * h << SP_HALF_ACCURACY) / SP_PI >> SP_HALF_ACCURACY) - (hi << SP_ACCURACY);
	Sint32 p = (v << (SP_HALF_ACCURACY - 8)) * ((1 << SP_HALF_ACCURACY) - (s << (SP_HALF_ACCURACY - 8)));
	Sint32 q = (v << (SP_HALF_ACCURACY - 8)) * ((1 << SP_HALF_ACCURACY) - (((f >> SP_HALF_ACCURACY) * (s << (SP_HALF_ACCURACY - 8))) >> SP_HALF_ACCURACY));
	Sint32 t = (v << (SP_HALF_ACCURACY - 8)) * ((1 << SP_HALF_ACCURACY) - ((((1 << SP_HALF_ACCURACY) - (f >> SP_HALF_ACCURACY)) * (s << (SP_HALF_ACCURACY - 8))) >> SP_HALF_ACCURACY));
	Uint8 r;
	Uint8 g;
	Uint8 b;
	switch (hi)
	{
	case 1:
		r = q >> (SP_ACCURACY - 8);
		g = v;
		b = p >> (SP_ACCURACY - 8);
		break;
	case 2:
		r = p >> (SP_ACCURACY - 8);
		g = v;
		b = t >> (SP_ACCURACY - 8);
		break;
	case 3:
		r = p >> (SP_ACCURACY - 8);
		g = q >> (SP_ACCURACY - 8);
		b = v;
		break;
	case 4:
		r = t >> (SP_ACCURACY - 8);
		g = p >> (SP_ACCURACY - 8);
		b = v;
		break;
	case 5:
		r = v;
		g = p >> (SP_ACCURACY - 8);
		b = q >> (SP_ACCURACY - 8);
		break;
	default:
		r = v;
		g = t >> (SP_ACCURACY - 8);
		b = p >> (SP_ACCURACY - 8);
		break;
	}
	/*Uint8 hi = ((h<<SP_HALF_ACCURACY)/(SP_PI/3))>>SP_HALF_ACCURACY;
	Sint32 f = ((3*h<<SP_HALF_ACCURACY)/SP_PI>>SP_HALF_ACCURACY)-(hi<<SP_ACCURACY);
	Sint32 p = (v<<(SP_HALF_ACCURACY-8))*((1<<SP_HALF_ACCURACY)-(s<<(SP_HALF_ACCURACY-8)));
	Sint32 q = (v<<(SP_HALF_ACCURACY-8))*((1<<SP_HALF_ACCURACY)-(((f>>SP_HALF_ACCURACY)*(s<<(SP_HALF_ACCURACY-8)))>>SP_HALF_ACCURACY));
	Sint32 t = (v<<(SP_HALF_ACCURACY-8))*((1<<SP_HALF_ACCURACY)-((((1<<SP_HALF_ACCURACY)-(f>>SP_HALF_ACCURACY))*(s<<(SP_HALF_ACCURACY-8)))>>SP_HALF_ACCURACY));
	Uint8 r;
	Uint8 g;
	Uint8 b;
	switch (hi)
	{
	  case 1:
	    r=q>>(SP_ACCURACY-8);
	    g=v;
	    b=p>>(SP_ACCURACY-8);
	    break;
	  case 2:
	    r=p>>(SP_ACCURACY-8);
	    g=v;
	    b=t>>(SP_ACCURACY-8);
	    break;
	  case 3:
	    r=p>>(SP_ACCURACY-8);
	    g=q>>(SP_ACCURACY-8);
	    b=v;
	    break;
	  case 4:
	    r=t>>(SP_ACCURACY-8);
	    g=p>>(SP_ACCURACY-8);
	    b=v;
	    break;
	  case 5:
	    r=v;
	    g=p>>(SP_ACCURACY-8);
	    b=q>>(SP_ACCURACY-8);
	    break;
	  default:
	    r=v;
	    g=t>>(SP_ACCURACY-8);
	    b=p>>(SP_ACCURACY-8);
	    break;
	}*/
	return ((r >> 3) << 11) + ((g >> 2) << 5) + (b >> 3);
}

PREFIX int spFileExists( char* filename )
{
  SDL_RWops *file = SDL_RWFromFile(filename, "rb");
  if (file)
  {
    SDL_RWclose(file);
    return 1;
  }
  return 0;
}
