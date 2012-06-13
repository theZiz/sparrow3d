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
#ifndef _SPARROW_CORE_H
#define _SPARROW_CORE_H

#include "sparrowDefines.h"
#ifdef SDL_INCLUDE_SUBDIR
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

typedef struct SspInput *PspInput;
typedef struct SspInput
{
	signed char axis[2];
	char button[20];
	char supports_keyboard;
	struct {Sint32 pressed,x,y;} touchscreen; //important: x and y are ONLY valid,
	//if the touchscreen is pressed! IF the touchscreen is pressed "pressed" is
	//between 0 and SP_ONE on touchscreens with levels of pressure sensitive, on
	//touchscreens without it will be 0 OR SP_ONE (true-false).
} TspInput;


/* Sets defaults values for the window. ONLY for PC (not handhelds!) and ONLY
 * useable BEFORE the spInitCore-function */

PREFIX void spSetDefaultWindowSize( int w, int h );

/* spInitCore initializes SDL, SDL_TTF and other stuff. */
PREFIX void spInitCore( void );

/* spPrintDebug prints debug Stuff*/
PREFIX void spPrintDebug( char* text );

/* spCreateWindow creates the Window in the plattform depended resolution.
 * You will get the created SDL_Surface to render to, whoever you want.
 * "fullscreen" and "allowresize" set up, whether the window is resizeable
 * and/or fullscreen. Theses values are ignored for most (all?) handhelds
 * because of just one resolution ;-)*/
PREFIX SDL_Surface* spCreateWindow( int width, int height, int fullscreen, int allowresize );

/* spCreateDefaultWindow creates a default window with no fullscreen for PC, but
 * resizeable for PC */
PREFIX SDL_Surface* spCreateDefaultWindow( void );

/* spGetWindowSurface returns the window Surface. */
PREFIX SDL_Surface* spGetWindowSurface( void );

/* spLoop starts a loop with spDraw as draw function and spCalc as calculation
 * function. If spCalc returns a value != 0, the loop breaks */
PREFIX int spLoop( void ( *spDraw )( void ), int ( *spCalc )( Uint32 steps ), Uint32 minwait, void ( *spResize )( Uint16 w, Uint16 h ), void ( *spEvent )( SDL_Event *e ) );

/* spFlip draws the changes in the window Surface on the screen. */
PREFIX void spFlip( void );

/* spGetInput returns a pointer of the spInput struct, where the input values
 * will be set. DON'T FREE IT ON YOUR OWN! */
PREFIX PspInput spGetInput( void );

/* spSetTouchscreenEmulations sets, whether on systems without touchscreen or
 * mouse (like the GP2X F100) touchscreen is emulated. IF it shall be emulated
 * you have to set a button (e.g. SP_BUTTON_SELECT), which will switch between
 * "normal mode" (where you get all buttons) and "touchscreen mode", where D-Pad
 * and ok_button (e.g. SP_PRACTICE_OK) will be blocked and used for moving and
 * using a cursor on the screen. Set switch_button to
 * SP_NO_TOUCHSCREEN_EMULATION (or -1) to deactivate emulation.*/
PREFIX void spSetTouchscreenEmulationButtons(int switch_button,int ok_button);

/* spQuitCore just quits the Core. If you don't use it, everytime you close
 * your game, the flying spaghetti monster will kill a kitten.*/
PREFIX void spQuitCore( void );

/* spGetFPS returns the FPS of the Loop Function */
PREFIX int spGetFPS( void );

/* spGetSizeFactor returns a fixed point factor for the screen size. */
PREFIX Sint32 spGetSizeFactor( void );

/* spLoadSurface loads a 16 Surface needed by the engine */
PREFIX SDL_Surface* spLoadSurface( char* name );

/* spCreateSurface creates a 16 Surface 100% compatible to the engine,
 * widht should be even.*/
PREFIX SDL_Surface* spCreateSurface(int width,int height);

/* spDeleteSurface does the same as SDL_FreeSurface: just deleting the surface*/
PREFIX void spDeleteSurface( SDL_Surface* surface );

#endif
