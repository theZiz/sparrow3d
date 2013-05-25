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

/* Title: sparrowCore
 * 
 * SparrowCore is for creating a SDL window, creating optimal surfaces
 * for blitting on these window, for program main loops with feeedback
 * functions for drawing, calculation and different kinds of event
 * handling. Furthermore here are some helper functions like converting
 * colors or checking, whether a file exists. */
#ifndef _SPARROW_CORE_H
#define _SPARROW_CORE_H

#include <SDL.h>
#include "sparrowDefines.h"

/* Defines: Range of the analog axis
 * 
 * SP_ANALOG_AXIS_MIN - The minimal value of the analog axis.
 * SP_ANALOG_AXIS_MAX - The maximal value of the analog axis. */
#define SP_ANALOG_AXIS_MIN -32768
#define SP_ANALOG_AXIS_MAX  32767

/* Defines: Trigger limits for analog sticks
 * 
 * If a REAL input device (not the sparrow3d generic input device!) has
 * analog sticks, these are the limits, from which the generic axes are
 * uneven 0 or reseted to 0 again.
 *
 * SP_JOYSTICK_MIN_TRIGGER_ON - maximal value for activating the negative axis.
 * SP_JOYSTICK_MIN_TRIGGER_OFF - maximal value for not setting the axis to zero.
 * SP_JOYSTICK_MAX_TRIGGER_ON - minimal value for activating the positive axis.
 * SP_JOYSTICK_MAX_TRIGGER_OFF - maximal value for not setting the axis to zero.*/
#define SP_JOYSTICK_MIN_TRIGGER_ON -24576
#define SP_JOYSTICK_MIN_TRIGGER_OFF -8192
#define SP_JOYSTICK_MAX_TRIGGER_ON 24576
#define SP_JOYSTICK_MAX_TRIGGER_OFF	8192

/* Defines: Generic button and axis count
 * 
 * SP_INPUT_BUTTON_COUNT - number of useable generic buttons.
 * SP_INPUT_AXIS_COUNT - number of useable generic axes.*/
#define SP_INPUT_BUTTON_COUNT 20
#define SP_INPUT_AXIS_COUNT 2

/* Defines: Keyboard waiting
 * 
 * If a key is pressed after a given time it will be pressed again. Since then
 * the time between two key pressing is shorter. You know this effect from
 * terminals like in the bash, cmd.exe (Windows) or under MS-DOS.
 * 
 * SP_KEYBOARD_FIRST_WAIT - Time to wait until the second key pressing.
 * SP_KEYBOARD_WAIT - Time to wait for longer key pressing.
 * SP_VIRTUAL_KEYBOARD_FIRST_WAIT - same like SP_KEYBOARD_FIRST_WAIT, but for
 * the virtual keyboard.
 * SP_VIRTUAL_KEYBOARD_WAIT - same like SP_KEYBOARD_WAIT, but for the virtual
 * keyboard.*/
#define SP_KEYBOARD_FIRST_WAIT 600
#define SP_KEYBOARD_WAIT 100
#define SP_VIRTUAL_KEYBOARD_FIRST_WAIT 300
#define SP_VIRTUAL_KEYBOARD_WAIT 100

/* Defines: Values for spSetVirtualKeyboard.
 * 
 * With <spSetVirtualKeyboard> you setup, when the virtual keyboard is used.
 * 
 * SP_VIRTUAL_KEYBOARD_NEVER - The virtual keyboard is never used.
 * SP_VIRTUAL_KEYBOARD_IF_NEEDED - The virtual keyboard is used, if no hardware
 * keyboard is avaible (GP2X, Dingoo, etc.).
 * SP_VIRTUAL_KEYBOARD_ALWAYS - The virtual keyboard is always used, even on
 * devices with keyboard like a PC or the pandora.*/
#define SP_VIRTUAL_KEYBOARD_NEVER 0
#define SP_VIRTUAL_KEYBOARD_IF_NEEDED 1
#define SP_VIRTUAL_KEYBOARD_ALWAYS 2

/* Define: SP_CACHE_SIZE
 * 
 * The size of the surface cache */
#define SP_CACHE_SIZE 2048

/* type: spInput
 * 
 * This struct contains information about the generic input device
 * sparrowCore provides, which is same on *every* device
 * 
 * Variables:
 * 
 * axis (char*) - the two axis of the input device. Every axis can be -1 (left/up),
 * 0 (no direction) or 1 (right/down)
 * button (char*) - the generic input device has 20 buttons, but many buttons are just
 * for compatibility reason to the gp2x-family. You should only use button 8 to
 * 18 (11 buttons) or better: the #defines for the buttons like SP_BUTTON_START
 * or SP_BUTTON_A (see sparrowDefines.h).
 * supports_keyboard (char) - this variable says, whether the target supports a hardware
 * keyboard. However: at least a software onscreen keyboard is always provided
 * touchscreen (struct of 3 ints) - because of the principle of the least common factor, there is
 * no support for mouses, but for touchscreens. The different is, that you CAN'T
 * use the position provided here if the touchscreen is not pressed! Please keep
 * that in mind. For devices without touchscreen an emulation is provided.
 * keyboard (internal struct)- internal struct for keyboard input. Do not change.
 * analog_axis (Sint16*) - a value between <SP_ANALOG_AXIS_MIN> and
 * <SP_ANALOG_AXIS_MAX>. For digital input (D-Pad) this will be always be the
 * MIN or MAX value, but for analog input (such as the Pandora's nubs or another
 * analog stick) you will get a value on the range of MIN..MAX as accurate as
 * SDL can poll the hardware. axis and analog_axis are not independant. If both
 * methods of input are present you will NOT be able to access both, instead the
 * one polled last by SDL will superseed the former.*/
typedef struct SspInput *PspInput;
typedef struct SspInput
{
	signed char axis[SP_INPUT_AXIS_COUNT];
	char button[SP_INPUT_BUTTON_COUNT];
	char supports_keyboard;
	struct {int pressed,x,y;} touchscreen;
	struct {
		char *buffer;
		int pos,len,lastSize;
	} keyboard;
	Sint16 analog_axis[SP_INPUT_AXIS_COUNT];
} spInput;


/* Function: spSetDefaultWindowSize
 * 
 * Sets defaults values for the window. *Only* for PC (not handhelds!) and *only*
 * useable *before* the spInitCore-function.
 * 
 * Parameters:
 * w - width of the window
 * h - height of the window*/

PREFIX void spSetDefaultWindowSize( int w, int h );

/* Function: spInitCore
 * 
 * spInitCore initializes SDL, SDL_TTF and other stuff.*/
PREFIX void spInitCore( void );

/* Function: spPrintDebug
 * 
 * spPrintDebug prints debug stuff with timestamp
 * 
 * Parameters:
 * text - debug information to print*/
PREFIX void spPrintDebug( char* text );

/* Function: spCreateWindow
 * 
 * Creates the Window in the plattform depended resolution.
 * You will get the created SDL_Surface to render to, whoever you want.
 * 
 * Parameters:
 * width - width of the window. Ignored for most handhelds because of fullscreen
 * height - height of the window. Ignored for most handhelds because of fullscreen
 * fullscreen - 1 means fullscreen, 0 means no fullscreen. Ignored for most
 * handhelds, because always fullscreen
 * allowresize - determines, whether the window is resizeable. Ignored for most
 * handhelds
 * 
 * Returns:
 * SDL_Surface* - Window surface. If you want to draw to it with sparrow3d,
 * dont't forget to call <spSelectRenderTarget>.*/
PREFIX SDL_Surface* spCreateWindow( int width, int height, int fullscreen, int allowresize );

/* Function: spCreateDefaultWindow
 * 
 * Creates a default window with no fullscreen for PC, but resizeable for PC.
 * 
 * Returns:
 * SDL_Surface* - the window surface*/
PREFIX SDL_Surface* spCreateDefaultWindow( void );

/* Function: spGetWindowSurface
 * 
 * Returns the window Surface.
 * 
 * Returns:
 * SDL_Surface* - the window surface*/
PREFIX SDL_Surface* spGetWindowSurface( void );

/* Function: spLoop
 * 
 * Starts a main loop
 * 
 * Parameters:
 * spDraw - the drawing function
 * spCalc - the calculation function. If it returns a value unequal 0 it loop
 * stops. The given steps are the ms since the last frame. Always > 0!
 * minwait - minimal time between two drawing steps. Usefull for setting maximal
 * frames per seconds, e.g. use 10 for 100 fps or 16 for ~62 fps.
 * spResize - Feedback function if the window is resized. The size of the window
 * is passed in _w_ and _h_. Use it e.g. for reloading resources in a higher or
 * lower resolution.
 * spEvent - if this feedback function is given, it gets every SDL event, that
 * has passed the mainloop to react to it (,too).
 * 
 * Returns:
 * int - the value spCalc returned*/
PREFIX int spLoop( void ( *spDraw )( void ), int ( *spCalc )( Uint32 steps ), Uint32 minwait, void ( *spResize )( Uint16 w, Uint16 h ), void ( *spEvent )( SDL_Event *e ) );

/* Function: spFlip
 * 
 * Draws the changes in the window Surface on the screen.*/
PREFIX void spFlip( void );

/* Function: spGetInput
 * 
 * Returns a pointer of the spInput struct, where the input values will be set.
 * *Dont't free it on your own!*
 * 
 * Returns:
 * PspInput - A pointer to a <spInput> struct*/
PREFIX PspInput spGetInput( void );

/* Function: spResetButtonsState
 * 
 * Resets the state of any button in the generic device to 0 (=unpressed)
 * 
 * See Also:
 * <spResetAxisState>*/
PREFIX void spResetButtonsState( void );

/* Function: spResetAxisState
 * 
 * Resets the state of any axis (digital and analog) in the generic device to 0
 * (=unpressed)
 * 
 * See Also:
 * <spResetButtonsState>*/
PREFIX void spResetAxisState( void );

/* Function: spPollKeyboardInput
 * 
 * Prints all following keyboard input (that is numbers, letters and symbols)
 * into the passed buffer.
 * This function does not halt execution, rather the buffer is filled over the
 * next frames until it is full or polling is stopped.
 * Only backspace is allowed for editing.
 * On devices without a physical keyboard you have to make sure to show some
 * kind of on-screen keyboard allowing for input.
 * 
 * Parameters:
 * buffer - the buffer for the input
 * buffersize - the maximum size of the passed buffer
 * enter_key_mask - determines which generic buttons are used for entering keys
 * on the virtual keyboards. E.g. SP_BUTTON_A | SP_BUTTON_B says, that A *or* B
 * can be used the enter stuff.*/
PREFIX void spPollKeyboardInput( char *buffer, int bufferSize, Sint32 enter_key_mask);

/* Function: spStopKeyboardInput:
 * 
 * Stops keyboard input. Any input following this call will not be passed to a
 * buffer, rather will be ignored. This function will not be called
 * automatically when the buffer passed to spPollKeyboardInput is full.*/
PREFIX void spStopKeyboardInput( void );

/* Function: spIsKeyboardPolled
 * 
 * Says, whether input from the keyboard is polled.
 * Usefull for that you don't react to "ingame controls" if e.g. a
 * highscore name is entered or to show the virtual keyboard.
 * 
 * Returns:
 * int - 1 if polled, 0 if not polled*/
PREFIX int spIsKeyboardPolled();

/* Function: spSetVirtualKeyboard
 * 
 * Sets up a virtual keyboard especially for systems without a keyboard like the
 * gp2x, caanoo, etc. The keyboard uses the input devices for input. Only if
 * Keyboard input is pulled, the keyboard is useable!
 * 
 * Parameters:
 * state - Can be <SP_VIRTUAL_KEYBOARD_NEVER>, <SP_VIRTUAL_KEYBOARD_IF_NEEDED>,
 * <SP_VIRTUAL_KEYBOARD_ALWAYS>. See definitions for the function.
 * x, y - determines where you will draw the onscreen keyboard (later).
 * width - width of the onscreen keyboard
 * height - height of the onscreen keyboard
 * Necessary for interpretating the touchscreen input (offset!)
 * design - You have to add a design, if you want to show something! The
 * keyboard reacts to input, nevertheless, if you don't pass something, but if
 * you don't draw anything nothing will be seeable... A reference design with
 * the exact position of the buttons is in the sparrow3d folder in data named
 * "keyboard.xcf". This file is public domain. Use it how ever you want.
 * Internal a copy of the design with this size (x,y) will be saved, so you can
 * free the surface afterwards. However don't forget to recall this function on
 * resize. After that you can use spGetVirtualKeyboard to get a SDL_Surface with
 * the size (width,height), which can (and have to!) be drawn by you.
 * shiftDesign - like _design_, but it is shown, if Shift is enabled.*/
PREFIX void spSetVirtualKeyboard(int state,int x,int y,int width,int height,SDL_Surface* design,SDL_Surface* shiftDesign);

/* Function: spGetVirtualKeyboard
 * 
 * This functions returns the precalculated and prescaled keyboard design.
 * 
 * Returns:
 * SDL_Surface* - NULL, if the virtual keyboard is deactivated or no surface was
 * passed, otherwise the keyboard surface with key-marking and shift specific
 * changes. Don't save the result, call it every frame.*/
PREFIX SDL_Surface* spGetVirtualKeyboard();

/* Function: spSetTouchscreenEmulationButtons
 * 
 * Sets, whether on systems without touchscreen or
 * mouse (like the GP2X F100) a touchscreen is emulated.
 * 
 * Parameters:
 * switch_button - if it shall be emulated you have to set a button
 * (e.g. SP_BUTTON_SELECT), which will switch between the "normal mode" (where
 * you get all buttons) and "touchscreen mode", where D-Pad and ok_button
 * (e.g. SP_PRACTICE_OK) will be blocked and used for moving and using a cursor
 * on the screen. Set switch_button to SP_NO_TOUCHSCREEN_EMULATION (or -1) to
 * deactivate emulation.
 * ok_button - the button for clicking the "mouse"*/
PREFIX void spSetTouchscreenEmulationButtons(int switch_button,int ok_button);

/* Function: spQuitCore
 * 
 * Just quits the Core. If you don't use it everytime you close
 * your game the flying spaghetti monster will kill a kitten.*/
PREFIX void spQuitCore( void );

/* Function: spGetFPS
 * 
 * Returns the FPS of the Loop Function
 * 
 * Returns:
 * int - the frame per second*/
PREFIX int spGetFPS( void );

/* Function: spGetSizeFactor
 * 
 * Returns a fixed point factor for the screen size.
 * 
 * Returns:
 * Sint32 - the size factor. It is SP_ONE for the gp2x (320 x 240)*/
PREFIX Sint32 spGetSizeFactor( void );

/* Function: spLoadSurface
 * 
 * Loads a 16 Surface needed by the engine. If enabled it will be
 * cached.
 * 
 * Parameters:
 * char* - filename of the image to load
 * 
 * Returns:
 * SDL_Surface* - the loaded surface*/
PREFIX SDL_Surface* spLoadSurface( char* name );

/* Function: spCopySurface
 * 
 * This creates a "copy" of a surface. If caching is enabled, it just
 * returns the cached surface pointer (which should be the same as the
 * parameter) and increases the reference counter. If caching is
 * disabled, a real copy is made. If you need a REAL copy every time,
 * use <spUniqueCopySurface>.
 * 
 * Parameters:
 * surface - the surface to be copied
 * 
 * Returns:
 * SDL_Surface* - the "copy"*/
PREFIX SDL_Surface* spCopySurface( SDL_Surface* surface );

/* Function: spUniqueCopySurface
 * 
 * This call creates a *real* copy of a surface - doesn't matter, whether
 * caching is enabled or not.
 * 
 * Parameters:
 * surface - the surface to be copied
 * 
 * Returns:
 * SDL_Surface* - the copy*/
PREFIX SDL_Surface* spUniqueCopySurface( SDL_Surface* surface );

/* Function: spEnableCaching
 * 
 * Enables caching surfaces. That means, that, if you load
 * an image twice, internal it is only loaded once to save RAM. If you WANT
 * one image in two different surfaces, disable caching (spriteCollection may
 * wont work then anymore) or create an unique copy with <spUniqueCopySurface>.
 * At default caching is enabled. A reference counter counts,
 * how often a surface is loaded for spDeleteSurface*/
PREFIX void spEnableCaching();

/* Function: spDisableCaching
 * 
 * Disables chaching of surfaces.*/
PREFIX void spDisableCaching();

/* Function: spIsCachingEnabled
 * 
 * Says, whether caching of surfaces is enabled or not.
 * 
 * Returns:
 * int - 1 means caching is enabled, 0 not enabled*/
PREFIX int spIsCachingEnabled();

/* Function: spCreateSurface
 * 
 * Creates a 16 Surface 100% compatible to the engine. This
 * surface is not cached (because it has no filename to remember ;-) )
 * 
 * Parameters:
 * width - the width of the surface
 * height - the height of the surace
 * 
 * Returns:
 * SDL_Surface* - the created surface*/
PREFIX SDL_Surface* spCreateSurface(int width,int height);

/* Function: spDeleteSurface
 * 
 * Deletes a surface, if it is not cached or the reference
 * counter reaches 0. If the reference counter is greater 0, it is just
 * decreased. Don't call it more often than spCreateSurface for the same image!
 * 
 * Parameters:
 * surface - the surface to be deleted*/
PREFIX void spDeleteSurface( SDL_Surface* surface );

/* Function: spClearCache
 * 
 * Deletes ALL cached surfaces! You can't use them anymore and have
 * to reload them! */
PREFIX void spClearCache();

/* Function: spGetRGB
 * 
 * Returns a 16 bit RGB color
 * 
 * Parameters:
 * r - red color parameter
 * g - green color parameter
 * b - blue color parameter
 * 
 * Returns:
 * Uint16 - 16 bit color value*/
PREFIX Uint16 spGetRGB(int r, int g, int b );

/* Function: spGetFastRGB
 * 
 * Returns like spGetRGB a 16 bit color out of RGB values,
 * but it is faster and a little bit inaccurate - if you don't mind,
 * don't see it or need it VERY often ingame, use this!
 * 
 * Parameters:
 * r - red color parameter
 * g - green color parameter
 * b - blue color parameter
 * 
 * Returns:
 * Uint16 - 16 bit color value*/
#define spGetFastRGB(r,g,b) ((((r) >> 3)<<11)+(((g) >> 2)<<5)+((b) >> 3))

/* Function: spGetHSV
 * 
 * Returns a 16 bit color defined by the HSV values.
 * 
 * Parameters:
 * h - the h value of the HSV model
 * s - the s value of the HSV model
 * v - the v value of the HSV model
 * 
 * Returns:
 * Uint16 - 16 bit color value*/
PREFIX Uint16 spGetHSV( Sint32 h, Uint8 s, Uint8 v );

/* Function: spScale2XFast
 * 
 * Copies and scales source to destination very fast. Attention! Destination
 * HAVE TO HAVE the size source->w*2*source->h*2! Furthermore the pixel depth
 * must be 2 (16 bit graphics).
 * 
 * Parameters:
 * source - source surface
 * destination - destination surface. Have to have the double width and double
 * height of the source surface.
 * 
 * See Also:
 * <spScale2XSmooth>*/
PREFIX void spScale2XFast(SDL_Surface* source,SDL_Surface* destination);

/* Function: spScale2XSmooth
 * 
 * Like spScale2XFast, but with the MAME2x scale up algo. A bit slower and may
 * look not good. However most of the time it will.
 * 
 * Parameters:
 * source - source surface
 * destination - destination surface. Have to have the double width and double
 * height of the source surface.
 * 
 * See Also:
 * <spScale2XFast>*/
PREFIX void spScale2XSmooth(SDL_Surface* source,SDL_Surface* destination);

/* Function: spAddBorder
 * 
 * Adds a border to the surface with the background color backgroundcolor.
 * 
 * Parameters
 * surface - the surface in which the border will be drawn
 * borderColor - the color of the border
 * backgroundcolor - the color of the background to determine where to draw a
 * border*/
PREFIX void spAddBorder(SDL_Surface* surface, Uint16 borderColor,Uint16 backgroundcolor);

/* Function: spStereoMergeSurfaces
 * 
 * This functions merges two same sized (!) surfaces to one. The left one will
 * be overwritten by a or-merge, if "crossed" is 0. That means every pixel of
 * the left surface will be: left[p] = left[p] | right[p]. If "crossed" is 1
 * every second line of every surface will be taken, that you get two clinched
 * pictures in one surface. Use the first one for coloured glasses stereo and
 * the second for crossing eye stereoscopy.
 * 
 * Parameters:
 * left - the left surface in which will be drawn, too
 * right - the right surface, it will only be read
 * crossed - determines, whether the merge is or-based or two-images-based (See
 * above)*/
PREFIX void spStereoMergeSurfaces(SDL_Surface* left,SDL_Surface* right,int crossed);
#endif
