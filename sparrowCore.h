/*
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms of the
 GNU Lesser General Public license (the  "LGPL License") version 2 or higher, in
 which case the provisions of LGPL License are applicable instead of those above
 
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

#include <SDL.h>
#include "sparrowDefines.h"

/* The minimal and maximal value of the analog axis */
#define SP_ANALOG_AXIS_MIN -32768
#define SP_ANALOG_AXIS_MAX  32767

/* if a REAL input device (not the sparrow3d generic input device!) has
 * a analog stick, these are the limits, from which the generic axis are
 * uneven 0 or reseted to 0 again*/
#define SP_JOYSTICK_MIN_TRIGGER_ON -24576
#define SP_JOYSTICK_MIN_TRIGGER_OFF -8192
#define SP_JOYSTICK_MAX_TRIGGER_ON 24576
#define SP_JOYSTICK_MAX_TRIGGER_OFF	8192

/* The size of the surface cache */
#define SP_CACHE_SIZE 2048

#define SP_INPUT_BUTTON_COUNT 20
#define SP_INPUT_AXIS_COUNT 2

#define SP_KEYBOARD_FIRST_WAIT 600
#define SP_KEYBOARD_WAIT 100

#define SP_VIRTUAL_KEYBOARD_NEVER 0
#define SP_VIRTUAL_KEYBOARD_IF_NEEDED 1
#define SP_VIRTUAL_KEYBOARD_ALWAYS 2
#define SP_VIRTUAL_KEYBOARD_FIRST_WAIT 300
#define SP_VIRTUAL_KEYBOARD_WAIT 100

/* This struct contains information about the generic input device
 * sparrowCore provides, which is same on EVERY target */
typedef struct SspInput *PspInput;
typedef struct SspInput
{
	/* the two axis of the input device. Every axis can be -1 (left/up),
	 * 0 (no direction) or 1 (right/down) */
	signed char axis[SP_INPUT_AXIS_COUNT];
	/* the generic input device has 20 buttons, but many buttons are
	 * just for compatibility reason to the gp2x-family. You should only
	 * use button 8 to 18 (11 buttons) or better: the #defines for the
	 * buttons like SP_BUTTON_START or SP_BUTTON_A (see sparrowDefines.h) */
	char button[SP_INPUT_BUTTON_COUNT];
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
		int pos,len,lastSize; //some internal variables about the input. Do not change!
	} keyboard;
	/* These analog axis is the SAME like above, but with more different states
	 * instead of a trinary -1, 0, 1 decision. If a device doesn't have a analog
	 * stick, it will emulated as full pressed, but axis like the pandora nubs or
	 * an analog stick on a gamepad will be presented here with full SDL accuracy*/
	Sint16 analog_axis[SP_INPUT_AXIS_COUNT];
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
 * function. If spCalc returns a value != 0, the loop breaks. The steps
 * are the ms since the last frame. Always > 0! */
PREFIX int spLoop( void ( *spDraw )( void ), int ( *spCalc )( Uint32 steps ), Uint32 minwait, void ( *spResize )( Uint16 w, Uint16 h ), void ( *spEvent )( SDL_Event *e ) );

/* spFlip draws the changes in the window Surface on the screen. */
PREFIX void spFlip( void );

/* spGetInput returns a pointer of the spInput struct, where the input values
 * will be set. DON'T FREE IT ON YOUR OWN! */
PREFIX PspInput spGetInput( void );

/* Resets the state of any button in spInput to 0 (=unpressed) */
PREFIX void spResetButtonsState( void );

/* Resets the state of any joystick or D-Pad axis in spInput to 0 (=no
 * deflexion) */
PREFIX void spResetAxisState( void );

/* Prints all following keyboard input (that is numbers, letters and symbols)
 * into the passed buffer.
 * buffersize sets the maximum size of the passed buffer
 * enter_key_mask says, which buttons select a key on the virtual keyboard.
 * Use SP_BUTTON_XXX_MASK with | to select the buttons.
 * Unicode might be used when necessary, your current locale will also be used.
 * This function does not halt execution, rather the buffer is filled over the
 * next frames until it is full or polling is stopped.
 * Only backspace is allowed for editing.
 * On devices without a physical keyboard you have to make sure to show some
 * kind of on-screen keyboard allowing for input.
 * If input is polled, spIsInputLocked returns 1.*/
PREFIX void spPollKeyboardInput( char *buffer, int bufferSize, Sint32 enter_key_mask);

/* Stops keyboard input. Any input following this call will not be passed to a
 * buffer, rather will be ignored.
 * This function will not be called automatically when the buffer passed to
 * spPollKeyboardInput is full.
 * If input polling is stopped, spIsInputLocked returns 0.*/
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

/* Copies and scales source to destination very fast. Attention! Destination
 * HAVE TO HAVE the size source->w*2*source->h*2! Furthermore the pixel depth
 * must be 2 (16 bit graphics). Smooth is much slower and does not always look
 * better.*/
PREFIX void spScale2XFast(SDL_Surface* source,SDL_Surface* destination);
PREFIX void spScale2XSmooth(SDL_Surface* source,SDL_Surface* destination);

/* Adds a border to the surface with the background color backgroundcolor. */
PREFIX void spAddBorder(SDL_Surface* surface, Uint16 borderColor,Uint16 backgroundcolor);

/* Sets up a virtual keyboard especially for systems without a keyboard like the
 * gp2x, caanoo, etc. The keyboard uses the input devices for input. Only if
 * Keyboard input is pulled, the keyboard is useable! state can be:
 * SP_VIRTUAL_KEYBOARD_NEVER = never used. Default.
 * SP_VIRTUAL_KEYBOARD_IF_NEEDED = Used if keyboard input is polled on system
 * without a hardware keyboard.
 * SP_VIRTUAL_KEYBOARD_ALWAYS = Always used if needed. Even on system with
 * hardware keyboards. Makes not much sense on systems with a hardware keyboard,
 * but useful for developing and testing.
 * You have to add a design, if you want to show something! The keyboard reacts
 * to input, nevertheless, if you don't pass something, but if you don't draw
 * anything nothing will be seeable... A reference design with the exact position
 * of the buttons is in the sparrow3d folder in data named "keyboard.xcf". This
 * file is public domain. Use it, how ever you want. Internal a copy of the
 * design with this size (x,y) will be saved, so you can free the surface
 * afterwards. However don't forget to recall this function on resize. After
 * that you can use spGetVirtualKeyboard to get a SDL_Surface with the size
 * (x,y), which can be drawn by you. The axis and button information of the
 * generic input device will be used for entering stuff. Use spIsInputLocked
 * to determine, whether a input is polled or not!
 * x and y are necessary for the touchscreen support (offset of input).*/
PREFIX void spSetVirtualKeyboard(int state,int x,int y,int width,int height,SDL_Surface* design,SDL_Surface* shiftDesign);

/* This functions returns the precalculated and prescaled keyboard design.
 * Returns 0, if the virtual keyboard is deactivated or not surface was passed*/
PREFIX SDL_Surface* spGetVirtualKeyboard();

/* spIsKeyboardPolled says, whether input from the keyboard is polled.
 * Usefull for that you don't react to "ingame controls" if e.g. a
 * highscore name is entered or to show the virtual keyboard. */
PREFIX int spIsKeyboardPolled();

/* This functions merges two same sized (!) surfaces to one. The left one will
 * be overwritten be the or-merge, if "crossed" is 0. That means every pixel of
 * the left surface will be: left[p] = left[p] | right[p]. If "crossed" is 1
 * every second line of every surface will be taken, that you get two clinched
 * pictures in one surface. Use the first one for coloured glasses stereo and
 * the second for crossing eye stereoscopy. */
PREFIX void spStereoMergeSurfaces(SDL_Surface* left,SDL_Surface* right,int crossed);
#endif
