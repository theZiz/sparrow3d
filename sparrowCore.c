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
#ifdef SDL_INCLUDE_SUBDIR
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#endif

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
	spZoom = 1 << SP_ACCURACY;
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
}

PREFIX SDL_Surface* spCreateWindow( int width, int height, int fullscreen, int allowresize )
{
	spResizeWindow( ( !width ? spWindowX : width ), ( !height ? spWindowY : height ), fullscreen, allowresize );
	return spWindow;
}

PREFIX SDL_Surface* spCreateDefaultWindow( void )
{
	spCreateWindow( 0, 0, 0, 1 );
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
		case SDL_JOYBUTTONDOWN:
			spInput.button[event.jbutton.button] = 1;
			break;
		case SDL_JOYBUTTONUP:
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
//            case SDLK_MENU:
//              spDone=1;
//              break;
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
		if ( spEvent )
			spEvent( &event );
	}
#ifdef CAANOO
	spInput.button[SP_BUTTON_VOLPLUS] = 0;
	spInput.button[SP_BUTTON_VOLMINUS] = 0;
#endif
	return result;
}

/*PREFIX signed char spGetAxis(int axis)
{
  #ifdef GP2X
    if (axis==0)
    {
      if (spInput.button[SP_AXIS_LEFTUP] ||
          spInput.button[SP_AXIS_LEFT]   ||
          spInput.button[SP_AXIS_LEFTDOWN])
        return -1;
      if (spInput.button[SP_AXIS_RIGHTUP] ||
          spInput.button[SP_AXIS_RIGHT]   ||
          spInput.button[SP_AXIS_RIGHTDOWN])
        return  1;
    }
    else
    {
      if (spInput.button[SP_AXIS_LEFTUP] ||
          spInput.button[SP_AXIS_UP]   ||
          spInput.button[SP_AXIS_RIGHTUP])
        return  1;
      if (spInput.button[SP_AXIS_LEFTDOWN] ||
          spInput.button[SP_AXIS_DOWN]   ||
          spInput.button[SP_AXIS_RIGHTDOWN])
        return -1;
    }
    return 0;
  #else
    return spInput.axis[axis];
  #endif
}*/

/*PREFIX void spSetAxis(int axis,signed char value)
{
  #ifdef GP2X
    if (axis==0)
    {
      spInput.button[SP_AXIS_LEFTUP] = 0;
      spInput.button[SP_AXIS_LEFT] = 0;
      spInput.button[SP_AXIS_LEFTDOWN] = 0;
      spInput.button[SP_AXIS_DOWN] = 0;
      spInput.button[SP_AXIS_RIGHTDOWN] = 0;
      spInput.button[SP_AXIS_RIGHT] = 0;
      spInput.button[SP_AXIS_RIGHTUP] = 0;
      spInput.button[SP_AXIS_UP] = 0;
      if (spInput.axis[1] == 1 && value == -1)
        spInput.button[SP_AXIS_LEFTUP] = 1;
      if (spInput.axis[1] == 0 && value == -1)
        spInput.button[SP_AXIS_LEFT] = 1;
      if (spInput.axis[1] == -1 && value == -1)
        spInput.button[SP_AXIS_LEFTDOWN] = 1;
      if (spInput.axis[1] == -1 && value == 0)
        spInput.button[SP_AXIS_DOWN] = 1;
      if (spInput.axis[1] == -1 && value == 1)
        spInput.button[SP_AXIS_RIGHTDOWN] = 1;
      if (spInput.axis[1] == 0 && value == 1)
        spInput.button[SP_AXIS_RIGHT] = 1;
      if (spInput.axis[1] == 1 && value == 1)
        spInput.button[SP_AXIS_RIGHTUP] = 1;
      if (spInput.axis[1] == 1 && value == 0)
        spInput.button[SP_AXIS_UP] = 1;
    }
    else
    {
      spInput.button[SP_AXIS_LEFTUP] = 0;
      spInput.button[SP_AXIS_LEFT] = 0;
      spInput.button[SP_AXIS_LEFTDOWN] = 0;
      spInput.button[SP_AXIS_DOWN] = 0;
      spInput.button[SP_AXIS_RIGHTDOWN] = 0;
      spInput.button[SP_AXIS_RIGHT] = 0;
      spInput.button[SP_AXIS_RIGHTUP] = 0;
      spInput.button[SP_AXIS_UP] = 0;
      if (value == 1 && spInput.axis[0] == -1)
        spInput.button[SP_AXIS_LEFTUP] = 1;
      if (value == 0 && spInput.axis[0] == -1)
        spInput.button[SP_AXIS_LEFT] = 1;
      if (value == -1 && spInput.axis[0] == -1)
        spInput.button[SP_AXIS_LEFTDOWN] = 1;
      if (value == -1 && spInput.axis[0] == 0)
        spInput.button[SP_AXIS_DOWN] = 1;
      if (value == -1 && spInput.axis[0] == 1)
        spInput.button[SP_AXIS_RIGHTDOWN] = 1;
      if (value == 0 && spInput.axis[0] == 1)
        spInput.button[SP_AXIS_RIGHT] = 1;
      if (value == 1 && spInput.axis[0] == 1)
        spInput.button[SP_AXIS_RIGHTUP] = 1;
      if (value == 1 && spInput.axis[0] == 0)
        spInput.button[SP_AXIS_UP] = 1;
        }
    spUpdateAxis(axis);
  #else
    spInput.axis[axis] = value;
  #endif
}*/

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
	Uint32 steps = 0;
	olderticks = SDL_GetTicks();
	oldticks = olderticks;
	newticks = olderticks;

	if ( !spResize && spWindow && spWindow->flags & SDL_RESIZABLE )
	{
		printf( "You made the video surface resizable, but did not pass a resize function to spLoop!\n" );
		printf( "This would cause the application to crash on resize, but I am in a good mood today,\n" );
		printf( "so I am going to make the window non-resizable and your app not crash instead, m'kay...\n" );
		spResizeWindow( spWindow->w, spWindow->h, 0, 0 );
		spSelectRenderTarget( spGetWindowSurface() );
	}

	while( back == 0 && !spDone )
	{
#ifdef CORE_DEBUG
		spPrintDebug( "Start mainloop" );
#endif
		newticks = SDL_GetTicks();
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
		if ( newticks - oldticks > 0 )
		{
			if ( spCalc )
				back = spCalc( newticks - oldticks );
#ifdef CORE_DEBUG
			spPrintDebug( "  Did calc" );
#endif
			oldticks = newticks;
		}
		steps += newticks - olderticks;
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
				//printf("FPS: %i\n",spFPS);
				frames = 0;
			}
			//if (steps-minwait > 0)
			//  SDL_Delay(steps-minwait);
			steps = 0;
			olderticks = newticks;
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
