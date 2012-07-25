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

/* sparrowCore is for creating a SDL window, creating optimal surfaces
 * for blitting on these window, for program main loops with feeedback
 * functions for drawing, calculation and different kinds of event
 * handling. Furthermore here are some helper functions like converting
 * colors or checking, whether a file exists. */
#ifndef _SPARROW_CORE_H
#define _SPARROW_CORE_H

#include "sparrowDefines.h"
#include <SDL.h>

/* The size of the surface cache */
#define SP_CACHE_SIZE 1024

/* This struct contains information about the generic input device
 * sparrowCore provides, which is same on EVERY target */
typedef struct SspInput *PspInput;
typedef struct SspInput
{
	/* the two axis of the input device. Every axis can be -1 (left/up),
	 * 0 or 1 (right/down) */
	signed char axis[2];
	/* the generic input device has 20 buttons, but many buttons are
	 * just for compatibility reason to the gp2x-family. You should only
	 * use button 8 to 18 (11 buttons) or better: the #defines for the
	 * buttons like SP_BUTTON_START or SP_BUTTON_A (see sparrowDefines.h) */
	char button[20];
	/* this variable says, whether the target supports a hardware
	 * keyboard. However: at least a software onscreen keyboard is
	 * always provided (TODO!) */
	char supports_keyboard;
	/* because of the principle of the least common factor, there is no
	 * support for mouses, but for touchscreens. The different is, that
	 * you CAN'T use the position provided here, if the touchscreen is
	 * not pressed! Please keep that in mind. For devices without
	 * touchscreen a emulation is provided */
	struct {int pressed,x,y;} touchscreen;
	struct {
		char *buffer; //buffer, where the input goes into
		char *filter; //filter, which signs are allowed (not implemented yet)
		int pos,len,lastSize; //some internal variables about the input. Do not change!
	} keyboard;
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

/* TODO: document me! */
PREFIX void spPollKeyboardInput( char *buffer, int bufferSize, char *filter );

/* TODO: document me! */
PREFIX void spStopKeyboardInput( void );

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

/* spLoadSurface loads a 16 Surface needed by the engine. If enabled it will be
 * cached. */
PREFIX SDL_Surface* spLoadSurface( char* name );

/* This creates a "copy" of a surface. If caching is enabled, it just
 * returns the cached surface pointer (which should be the same as the
 * parameter) and increases the reference counter. If caching is
 * disabled, a real copy is made. If you need a REAL copy every time,
 * use spUniqueCopySurface. */
PREFIX SDL_Surface* spCopySurface( SDL_Surface* surface );

/* This call creates a real copy of a surface - doesn't matter, whether
 * caching is enabled or not. */
PREFIX SDL_Surface* spUniqueCopySurface( SDL_Surface* surface );

/* spEnableCaching enables the caching surfaces. That means, that, if you load
 * a image twice, internal it is only loaded once to save RAM. If you WANT
 * one image in two different surfaces, disable caching (spriteCollection may
 * wont work then anymore) or use spLoadUncachedSurface instead of
 * spLoadSurface. At default caching is enabled. A reference counter counts,
 * how often a surface is loaded for spDeleteSurface*/
PREFIX void spEnableCaching();

/* spDisableCaching disables chaching of surfaces */
PREFIX void spDisableCaching();

/* spIsCachingEnabled says, whether caching of surfaces is enabled or
 * not */
PREFIX int spIsCachingEnabled();

/* spCreateSurface creates a 16 Surface 100% compatible to the engine. This
 * surface is not cached (because it has no filename to remember ;-) )*/
PREFIX SDL_Surface* spCreateSurface(int width,int height);

/* spDeleteSurface deletes a surface, if it is not cached or the reference
 * counter reaches 0. If the reference counter is greater 0, it is just
 * decreased. Don't call it more often than spCreateSurface for the same image!*/
PREFIX void spDeleteSurface( SDL_Surface* surface );

/* spClearCache deletes ALL cached surfaces! You can't use them anymore and has
 * to reload them! */
PREFIX void spClearCache();

/* spGetRGB returns a 16 bit RGB color*/
PREFIX Uint16 spGetRGB(int r, int g, int b );

/* spGetFastRGB returns like spGetRGB a 16 bit color out of RGB values,
 * but it is faster and a little bit inaccurate - if you don't mind,
 * don't see it or need it VERY often ingame, use this! */
#define spGetFastRGB(r,g,b) ((((r) >> 3)<<11)+(((g) >> 2)<<5)+((b) >> 3))

/* spGetHSV returns a 16 bit color defined by the HSV values */
PREFIX Uint16 spGetHSV( Sint32 h, Uint8 s, Uint8 v );

/* spFileExists tests, whether the file "filename" exists ;-) */
PREFIX int spFileExists( char* filename );

/* spReadOneLine reads one line from a SDL_RWops file. This line is
 * written to buffer. buffer_len is the length of buffer (with zero
 * byte!). If the end of file is reached, 1 is return, else 0.*/
PREFIX int spReadOneLine( SDL_RWops *file , char* buffer, int buffer_len);

/* spReadUntil reads from file to buffer until the end_sign or
 * buffer_len is reached. The end_sign is NOT part of the string.
 * If you read again from the file, the sign AFTER the end_sign will be
 * the next one. */
PREFIX int spReadUntil( SDL_RWops *file , char* buffer, int buffer_len, char end_sign);

#endif
