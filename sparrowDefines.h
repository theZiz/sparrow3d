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

/* Title: sparrowDefines
 * 
 * SparrowDefines contains many #defines used by the library or which
 * may be usefull for your application */
 
#ifndef _SPARROW_DEFINES_H
#define _SPARROW_DEFINES_H

/* These are defines for creating or using the library in C or C++ */
#ifdef _WIN32
	#include <windows.h>
	#if BUILDING_DLL
		# define DLLIMPORT __declspec (dllexport)
	#else /* Not BUILDING_DLL */
		# define DLLIMPORT __declspec (dllimport)
	#endif /* Not BUILDING_DLL */
	#ifdef __cplusplus
		#define PREFIX extern "C" DLLIMPORT
	#else
		#define PREFIX DLLIMPORT
	#endif
#else
	#ifdef __cplusplus
		#define PREFIX extern "C"
	#else
		#define PREFIX
	#endif
#endif

#ifndef SP_SURFACE_FLAGS
	#define SP_SURFACE_FLAGS SDL_HWSURFACE
#endif

/* Defines: Fixed Point Arithmetic Constants
 * 
 * The following #defines are for the fixed point arithmetic in
 * sparrow3d. Change the values only, if you know, what you are doing.
 * There should be optimal already.
 * 
 * SP_ACCURACY - Defines how many bits are used for the decimal place
 * SP_HALF_ACCURACY - The half of <SP_ACCURACY>
 * SP_ACCURACY_FACTOR - 2 ^ <SP_ACCURACY>. Needed for float conversation
 * SP_SQRT_ACCURACY - Accurary of the internal sqrt lookuptable
 * SP_PI - Fixed point Pi
 * SP_MATH_ACCURACY - (In)accuracy of the cos and sin lookup table
 * SP_ONE - 1.0f in fixed point*/

#define SP_ACCURACY 16
#define SP_HALF_ACCURACY 8
#define SP_ACCURACY_FACTOR 65536.0f
#define SP_SQRT_ACCURACY 16
#define SP_PI 205888
/* This is not really accuracy... More inaccuracy ^^ */
#define SP_MATH_ACCURACY 4
/* a fast define for the fixed point value 1.0 */
#define SP_ONE 65536

/* Defines: Fixed Point Arithmetics
 * 
 * These are some #defines for fast multiplication and division of
 * fixed point numbers. Use them like normal functions, e.g.
 * Sint32 a = spMul(b,c); instead of a = b * c (which will not work).
 * However multiplying a fixed point number with a "normal" integer just works
 * with the normal multiplication. For addition you have to convert!
 * 
 * spMul(a,b) - Multiplies fixed point number a with the fixed point number b
 * spMulHigh(a,b) - Does the same as <spMul(a,b)>, but with guaranteed high precision
 * spMulLow(a,b) - Does the same as <spMul(a,b)>, but maybe with lower precision
 * spDiv(a,b) - Divides fixed point number a with the fixed point number b
 * spDivHigh(a,b) - Does the same as <spDiv(a,b)>, but with guaranteed high precision
 * spDivLow(a,b) - Does the same as <spDiv(a,b)>, but maybe with lower precision
 * spSquare(a) - Squares the fixed point number a with the help of <spMul(a,b)>. */
#define spMulHigh(a,b) ((Sint64)(a)*(Sint64)(b)>>SP_ACCURACY)
#define spMulLow(a,b) (((a)>>SP_HALF_ACCURACY)*((b)>>SP_HALF_ACCURACY))

#ifdef FAST_MULTIPLICATION
	#define spMul(a,b) spMulLow(a,b)
#else
	#define spMul(a,b) spMulHigh(a,b)
#endif

#define spSquare(a) spMul(a,a)

#define spDivHigh(a,b) (((Sint64)(a)<<SP_ACCURACY)/(Sint64)(b))
/*#define spDivLow(a,b) ((b>=0 && b<(1<<SP_ACCURACY))? \
										 (a*spGetOne_over_x_pointer()[b]): \
										 ( \
											 (b <0 && b>(-1<<SP_ACCURACY))? \
											 (-a*spGetOne_over_x_pointer()[-b]): \
											 (((a<<SP_HALF_ACCURACY)/b)<<SP_HALF_ACCURACY) \
										 ))*/
#define spDivLow(a,b) (((a<<SP_HALF_ACCURACY)/b)<<SP_HALF_ACCURACY)
#ifdef FAST_DIVISION
	#define spDiv(a,b) spDivLow(a,b)
#else
	#define spDiv(a,b) spDivHigh(a,b)
#endif

/* Defines: Fixed Point Conversion
 * 
 * These conversions are no function calls, but defines. That means *before* the
 * compiler compiles the code, it is replaced by the defined rule. E.g., you
 * want to convert the float a to a fixed point number using spFloatToFixed. You
 * call
 * > Sint32 foo = spFloatToFixed(bar);
 * Internal the compiler will make this:
 * > Sint32 foo = ((Sint32)((float)(bar)*SP_ACCURACY_FACTOR));
 * This saves you a function call. But it gets MUCH cheaper, if you don't have
 * a float variable as parameter, but a number like 1.5f. Example:
 * > Sint32 foo = spFloatToFixed(1.5);
 * will become to:
 * > Sint32 foo = ((Sint32)((float)(1.5f)*SP_ACCURACY_FACTOR));
 * As no unknown parameter is in this equation the compiler will calculate it
 * at *compile time*, which means: In you final binary, the (binary) code will
 * just be:
 * > Sint32 foo = 98304;
 * which is obviouly *incredible* fast.
 * 
 * spFixedToFloat(x) - converts the fixed point number a to a float
 * spFloatToFixed(x) - converts the float a to a fixed point number
 * spIntToFixed(x) - converts the int x to a fixed point number
 * spFixedToInt(x) - converts the fixed point number x to an int with cutting of
 * the decimal place.
 * spFixedRoundInt(x) - converts the fixed point number x with round to an int */

#define spFixedToFloat(x) ((float)((Sint32)(x))/SP_ACCURACY_FACTOR)
#define spFloatToFixed(x) ((Sint32)((float)(x)*SP_ACCURACY_FACTOR))
#define spIntToFixed(x) ((Sint32)(((int)(x))<<SP_ACCURACY))
#define spFixedToInt(x) ((int)(((Sint32)(x))>>SP_ACCURACY))

#define spFixedRoundInt(x) ((int)(((Sint32)(x)+(1 << SP_ACCURACY-1))>>SP_ACCURACY))

/* Defines: Generic Buttons
 * 
 * The following (long) list of #defines gives you the numbers and names
 * of the generic device buttons or of the buttons on the explicit
 * device. The Dingoo e.g. has the A button on the right, but GP2X and
 * others on the left. That means: If you want the button with the
 * CAPTION "A" use SP_BUTTON_A, but if you want the LEFT Button on every
 * device use SP_BUTTON_LEFT, which will be SP_BUTTON_A on the gp2x or
 * SP_BUTTON_Y on the Dingoo. The same for the names. Buttons like
 * SP_BUTTON_L or similar are defined just one time - because they are
 * the same on every device.
 * 
 * SP_BUTTON_START - number of the "Start" or "Menu" button
 * SP_BUTTON_SELECT - number of the "Select", "||" or similar button
 * SP_BUTTON_L - number of the left shoulder button
 * SP_BUTTON_R - number of the right shoulder button
 * SP_BUTTON_A - number of the A button. On every device at another position!
 * SP_BUTTON_B - number of the B button. On every device at another position!
 * SP_BUTTON_X - number of the X button. On every device at another position!
 * SP_BUTTON_Y - number of the Y button. On every device at another position!
 * SP_BUTTON_LEFT - number of the left button. On every device at the same position!
 * SP_BUTTON_RIGHT - number of the right button. On every device at the same position!
 * SP_BUTTON_UP - number of the up button. On every device at the same position!
 * SP_BUTTON_DOWN - number of the down button. On every device at the same position!
 * SP_PRACTICE_OK - number of the button, which is on the specific device the
 * standard button for "ok".
 * SP_PRACTICE_CANCEL - number of the button, which is on the specific device
 * SP_PRACTICE_3 - number of the button, which is on the specific device the
 * neither ok nor cancel.
 * SP_PRACTICE_4 - number of the other button, which is on the specific device
 * neither ok nor cancel. */
 
/* Defines: Generic Buttons Names
 * 
 * SP_BUTTON_START_NAME - name of the "Start" or "Menu" button
 * SP_BUTTON_SELECT_NAME - name of the "Select", "||" or similar button
 * SP_BUTTON_L_NAME - name of the left shoulder button
 * SP_BUTTON_R_NAME - name of the right shoulder button
 * SP_BUTTON_A_NAME - name of the A button.
 * SP_BUTTON_B_NAME - name of the B button.
 * SP_BUTTON_X_NAME - name of the X button.
 * SP_BUTTON_Y_NAME - name of the Y button.
 * SP_BUTTON_LEFT_NAME - name of the left button.
 * SP_BUTTON_RIGHT_NAME - name of the right button.
 * SP_BUTTON_UP_NAME - name of the up button.
 * SP_BUTTON_DOWN_NAME - name of the down button.
 * SP_PAD_NAME - name of the pad (e.g. "Arrows keys" on the PC)
 * SP_PRACTICE_OK_NAME - name of the button, which is on the specific device the
 * standard button for "ok".
 * SP_PRACTICE_CANCEL_NAME - name of the button, which is on the specific device
 * the standard button for canceling an action.
 * SP_PRACTICE_3_NAME - name of the button, which is on the specific device the
 * neither ok nor cancel.
 * SP_PRACTICE_4_NAME - name of the other button, which is on the specific device
 * neither ok nor cancel*/
#ifdef GP2X
	#define SP_AXIS_UP 0
	#define SP_AXIS_LEFTUP 1
	#define SP_AXIS_LEFT 2
	#define SP_AXIS_LEFTDOWN 3
	#define SP_AXIS_DOWN 4
	#define SP_AXIS_RIGHTDOWN 5
	#define SP_AXIS_RIGHT 6
	#define SP_AXIS_RIGHTUP 7
	#define SP_BUTTON_START 8
	#define SP_BUTTON_SELECT 9
	#define SP_BUTTON_L 10
	#define SP_BUTTON_R 11
	#define SP_BUTTON_A 12
	#define SP_BUTTON_B 13
	#define SP_BUTTON_X 14
	#define SP_BUTTON_Y 15
	#define SP_BUTTON_LEFT SP_BUTTON_A
	#define SP_BUTTON_UP SP_BUTTON_Y
	#define SP_BUTTON_RIGHT SP_BUTTON_B
	#define SP_BUTTON_DOWN SP_BUTTON_X
	#define SP_BUTTON_GP2X_ONLY_VOLPLUS 16
	#define SP_BUTTON_GP2X_ONLY_VOLMINUS 17
	#define SP_BUTTON_GP2X_ONLY_TIP 18
	//just for different practices
	#define SP_PRACTICE_OK SP_BUTTON_B
	#define SP_PRACTICE_CANCEL SP_BUTTON_X
	#define SP_PRACTICE_3 SP_BUTTON_A
	#define SP_PRACTICE_4 SP_BUTTON_Y
#elif defined CAANOO
	#define SP_BUTTON_A 0
	#define SP_BUTTON_X 1
	#define SP_BUTTON_B 2
	#define SP_BUTTON_Y 3
	#define SP_BUTTON_LEFT SP_BUTTON_A
	#define SP_BUTTON_UP SP_BUTTON_Y
	#define SP_BUTTON_RIGHT SP_BUTTON_B
	#define SP_BUTTON_DOWN SP_BUTTON_X
	#define SP_BUTTON_L 4
	#define SP_BUTTON_R 5
	#define SP_BUTTON_START 6
	//SELECT == HELP I
	#define SP_BUTTON_SELECT 8
	//the Caanoo has a hardware volume control...
	#define SP_PRACTICE_OK SP_BUTTON_B
	#define SP_PRACTICE_CANCEL SP_BUTTON_X
	#define SP_PRACTICE_3 SP_BUTTON_A
	#define SP_PRACTICE_4 SP_BUTTON_Y
#elif defined DINGUX
	#define SP_BUTTON_START 8
	#define SP_BUTTON_SELECT 9
	#define SP_BUTTON_L 10
	#define SP_BUTTON_R 11
	#define SP_BUTTON_A 12
	#define SP_BUTTON_B 13
	#define SP_BUTTON_X 14
	#define SP_BUTTON_Y 15
	#define SP_BUTTON_LEFT SP_BUTTON_Y
	#define SP_BUTTON_UP SP_BUTTON_X
	#define SP_BUTTON_RIGHT SP_BUTTON_A
	#define SP_BUTTON_DOWN SP_BUTTON_B
	#define SP_PRACTICE_OK SP_BUTTON_A
	#define SP_PRACTICE_CANCEL SP_BUTTON_X
	#define SP_PRACTICE_3 SP_BUTTON_B
	#define SP_PRACTICE_4 SP_BUTTON_Y
#elif defined GCW
	#define SP_BUTTON_START 8
	#define SP_BUTTON_SELECT 9
	#define SP_BUTTON_L 10
	#define SP_BUTTON_R 11
	#define SP_BUTTON_A 12
	#define SP_BUTTON_B 13
	#define SP_BUTTON_X 14
	#define SP_BUTTON_Y 15
	#define SP_BUTTON_LEFT SP_BUTTON_X
	#define SP_BUTTON_UP SP_BUTTON_Y
	#define SP_BUTTON_RIGHT SP_BUTTON_A
	#define SP_BUTTON_DOWN SP_BUTTON_B
	#define SP_PRACTICE_OK SP_BUTTON_A
	#define SP_PRACTICE_CANCEL SP_BUTTON_B
	#define SP_PRACTICE_3 SP_BUTTON_X
	#define SP_PRACTICE_4 SP_BUTTON_Y
#elif defined PANDORA
	#define SP_AXIS_LEFTRIGHT 0
	#define SP_AXIS_UPDOWN 1
	#define SP_BUTTON_START 8
	#define SP_BUTTON_SELECT 9
	#define SP_BUTTON_L 10
	#define SP_BUTTON_R 11
	#define SP_BUTTON_A 12
	#define SP_BUTTON_B 13
	#define SP_BUTTON_X 14
	#define SP_BUTTON_Y 15
	#define SP_BUTTON_LEFT SP_BUTTON_A
	#define SP_BUTTON_UP SP_BUTTON_Y
	#define SP_BUTTON_RIGHT SP_BUTTON_B
	#define SP_BUTTON_DOWN SP_BUTTON_X
	#define SP_PRACTICE_OK SP_BUTTON_A
	#define SP_PRACTICE_CANCEL SP_BUTTON_X
	#define SP_PRACTICE_3 SP_BUTTON_B
	#define SP_PRACTICE_4 SP_BUTTON_Y
#else //PC
	#define SP_AXIS_LEFTRIGHT 0
	#define SP_AXIS_UPDOWN 1
	#define SP_BUTTON_SELECT_NOWASD 1
	#define SP_BUTTON_L_NOWASD 2
	#define SP_BUTTON_R_NOWASD 3
	#define SP_BUTTON_A_NOWASD 4
	#define SP_BUTTON_B_NOWASD 5
	#define SP_BUTTON_X_NOWASD 6
	#define SP_BUTTON_Y_NOWASD 7
	#define SP_BUTTON_START 8
	#define SP_BUTTON_SELECT 9
	#define SP_BUTTON_L 10
	#define SP_BUTTON_R 11
	#define SP_BUTTON_A 12
	#define SP_BUTTON_B 13
	#define SP_BUTTON_X 14
	#define SP_BUTTON_Y 15
	#define SP_BUTTON_LEFT SP_BUTTON_A
	#define SP_BUTTON_UP SP_BUTTON_Y
	#define SP_BUTTON_RIGHT SP_BUTTON_B
	#define SP_BUTTON_DOWN SP_BUTTON_X
	#define SP_BUTTON_LEFT_NOWASD SP_BUTTON_A_NOWASD
	#define SP_BUTTON_UP_NOWASD SP_BUTTON_Y_NOWASD
	#define SP_BUTTON_RIGHT_NOWASD SP_BUTTON_B_NOWASD
	#define SP_BUTTON_DOWN_NOWASD SP_BUTTON_X_NOWASD
	#define SP_PRACTICE_OK SP_BUTTON_A
	#define SP_PRACTICE_CANCEL SP_BUTTON_B
	#define SP_PRACTICE_3 SP_BUTTON_X
	#define SP_PRACTICE_4 SP_BUTTON_Y
	#define SP_PRACTICE_OK_NOWASD SP_BUTTON_A_NOWASD
	#define SP_PRACTICE_CANCEL_NOWASD SP_BUTTON_B_NOWASD
	#define SP_PRACTICE_3_NOWASD SP_BUTTON_X_NOWASD
	#define SP_PRACTICE_4_NOWASD SP_BUTTON_Y_NOWASD
#endif

#define SP_BUTTON_START_NOWASD SP_BUTTON_START
#ifndef DESKTOP
	#define SP_BUTTON_SELECT_NOWASD SP_BUTTON_SELECT
	#define SP_BUTTON_L_NOWASD SP_BUTTON_L
	#define SP_BUTTON_R_NOWASD SP_BUTTON_R
	#define SP_BUTTON_A_NOWASD SP_BUTTON_A
	#define SP_BUTTON_B_NOWASD SP_BUTTON_B
	#define SP_BUTTON_X_NOWASD SP_BUTTON_X
	#define SP_BUTTON_Y_NOWASD SP_BUTTON_Y
	#define SP_PRACTICE_OK_NOWASD SP_PRACTICE_OK
	#define SP_PRACTICE_CANCEL_NOWASD SP_PRACTICE_CANCEL
	#define SP_PRACTICE_3_NOWASD SP_PRACTICE_3
	#define SP_PRACTICE_4_NOWASD SP_PRACTICE_4
	#define SP_BUTTON_LEFT_NOWASD SP_BUTTON_LEFT
	#define SP_BUTTON_UP_NOWASD SP_BUTTON_UP
	#define SP_BUTTON_RIGHT_NOWASD SP_BUTTON_RIGHT
	#define SP_BUTTON_DOWN_NOWASD SP_BUTTON_DOWN
#endif

#ifdef F100
	#define SP_BUTTON_START_NAME "Start"
	#define SP_BUTTON_SELECT_NAME "Select"
	#define SP_BUTTON_L_NAME "L"
	#define SP_BUTTON_R_NAME "R"
	#define SP_BUTTON_A_NAME "A"
	#define SP_BUTTON_B_NAME "B"
	#define SP_BUTTON_X_NAME "X"
	#define SP_BUTTON_Y_NAME "Y"
	#define SP_BUTTON_LEFT_NAME SP_BUTTON_A_NAME
	#define SP_BUTTON_UP_NAME SP_BUTTON_Y_NAME
	#define SP_BUTTON_RIGHT_NAME SP_BUTTON_B_NAME
	#define SP_BUTTON_DOWN_NAME SP_BUTTON_X_NAME
	#define SP_PAD_NAME "D-Pad"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_B_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
	#define SP_PRACTICE_3_NAME SP_BUTTON_A_NAME
	#define SP_PRACTICE_4_NAME SP_BUTTON_Y_NAME
#elif defined WIZ
	#define SP_BUTTON_START_NAME "Menu"
	#define SP_BUTTON_SELECT_NAME "Select"
	#define SP_BUTTON_L_NAME "L"
	#define SP_BUTTON_R_NAME "R"
	#define SP_BUTTON_A_NAME "A"
	#define SP_BUTTON_B_NAME "B"
	#define SP_BUTTON_X_NAME "X"
	#define SP_BUTTON_Y_NAME "Y"
	#define SP_BUTTON_LEFT_NAME SP_BUTTON_A_NAME
	#define SP_BUTTON_UP_NAME SP_BUTTON_Y_NAME
	#define SP_BUTTON_RIGHT_NAME SP_BUTTON_B_NAME
	#define SP_BUTTON_DOWN_NAME SP_BUTTON_X_NAME
	#define SP_PAD_NAME "D-Pad"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_B_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
	#define SP_PRACTICE_3_NAME SP_BUTTON_A_NAME
	#define SP_PRACTICE_4_NAME SP_BUTTON_Y_NAME
#elif defined CAANOO
	#define SP_BUTTON_START_NAME "Home"
	#define SP_BUTTON_SELECT_NAME "Help I"
	#define SP_BUTTON_L_NAME "L"
	#define SP_BUTTON_R_NAME "R"
	#define SP_BUTTON_A_NAME "A"
	#define SP_BUTTON_B_NAME "B"
	#define SP_BUTTON_X_NAME "X"
	#define SP_BUTTON_Y_NAME "Y"
	#define SP_BUTTON_LEFT_NAME SP_BUTTON_A_NAME
	#define SP_BUTTON_UP_NAME SP_BUTTON_Y_NAME
	#define SP_BUTTON_RIGHT_NAME SP_BUTTON_B_NAME
	#define SP_BUTTON_DOWN_NAME SP_BUTTON_X_NAME
	#define SP_PAD_NAME "Stick"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_B_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
	#define SP_PRACTICE_3_NAME SP_BUTTON_A_NAME
	#define SP_PRACTICE_4_NAME SP_BUTTON_Y_NAME
#elif defined DINGUX
	#define SP_BUTTON_START_NAME "Start"
	#define SP_BUTTON_SELECT_NAME "Select"
	#define SP_BUTTON_L_NAME "L"
	#define SP_BUTTON_R_NAME "R"
	#define SP_BUTTON_A_NAME "A"
	#define SP_BUTTON_B_NAME "B"
	#define SP_BUTTON_X_NAME "X"
	#define SP_BUTTON_Y_NAME "Y"
	#define SP_BUTTON_LEFT_NAME SP_BUTTON_Y_NAME
	#define SP_BUTTON_UP_NAME SP_BUTTON_X_NAME
	#define SP_BUTTON_RIGHT_NAME SP_BUTTON_A_NAME
	#define SP_BUTTON_DOWN_NAME SP_BUTTON_B_NAME
	#define SP_PAD_NAME "D-Pad"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_A_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
	#define SP_PRACTICE_3_NAME SP_BUTTON_B_NAME
	#define SP_PRACTICE_4_NAME SP_BUTTON_Y_NAME
#elif defined GCW
	#define SP_BUTTON_START_NAME "Start"
	#define SP_BUTTON_SELECT_NAME "Select"
	#define SP_BUTTON_L_NAME "L"
	#define SP_BUTTON_R_NAME "R"
	#define SP_BUTTON_A_NAME "A"
	#define SP_BUTTON_B_NAME "B"
	#define SP_BUTTON_X_NAME "X"
	#define SP_BUTTON_Y_NAME "Y"
	#define SP_BUTTON_LEFT_NAME SP_BUTTON_X_NAME
	#define SP_BUTTON_UP_NAME SP_BUTTON_Y_NAME
	#define SP_BUTTON_RIGHT_NAME SP_BUTTON_A_NAME
	#define SP_BUTTON_DOWN_NAME SP_BUTTON_B_NAME
	#define SP_PAD_NAME "D-Pad"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_A_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_B_NAME
	#define SP_PRACTICE_3_NAME SP_BUTTON_X_NAME
	#define SP_PRACTICE_4_NAME SP_BUTTON_Y_NAME
#elif defined PANDORA
	#define SP_BUTTON_START_NAME "Start"
	#define SP_BUTTON_SELECT_NAME "Select"
	#define SP_BUTTON_L_NAME "L"
	#define SP_BUTTON_R_NAME "R"
	#define SP_BUTTON_A_NAME "A"
	#define SP_BUTTON_B_NAME "B"
	#define SP_BUTTON_X_NAME "X"
	#define SP_BUTTON_Y_NAME "Y"
	#define SP_BUTTON_LEFT_NAME SP_BUTTON_A_NAME
	#define SP_BUTTON_UP_NAME SP_BUTTON_Y_NAME
	#define SP_BUTTON_RIGHT_NAME SP_BUTTON_B_NAME
	#define SP_BUTTON_DOWN_NAME SP_BUTTON_X_NAME
	#define SP_PAD_NAME "D-Pad"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_A_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
	#define SP_PRACTICE_3_NAME SP_BUTTON_B_NAME
	#define SP_PRACTICE_4_NAME SP_BUTTON_Y_NAME
#else
	#define SP_BUTTON_START_NAME "Return"
	#define SP_BUTTON_START_NOWASD_NAME "Return"
	#define SP_BUTTON_SELECT_NAME "Backspace"
	#define SP_BUTTON_SELECT_NOWASD_NAME "Esc"
	#define SP_BUTTON_L_NAME "q"
	#define SP_BUTTON_L_NOWASD_NAME "PgUp"
	#define SP_BUTTON_R_NAME "e"
	#define SP_BUTTON_R_NOWASD_NAME "PgDn"
	#define SP_BUTTON_A_NAME "a"
	#define SP_BUTTON_A_NOWASD_NAME "Ctrl"
	#define SP_BUTTON_B_NAME "d"
	#define SP_BUTTON_B_NOWASD_NAME "Alt"
	#define SP_BUTTON_X_NAME "s"
	#define SP_BUTTON_X_NOWASD_NAME "Menu"
	#define SP_BUTTON_Y_NAME "w"	
	#define SP_BUTTON_Y_NOWASD_NAME "Shift"
	#define SP_BUTTON_LEFT_NAME         SP_BUTTON_A_NAME
	#define SP_BUTTON_LEFT_NOWASD_NAME  SP_BUTTON_A_NOWASD_NAME
	#define SP_BUTTON_UP_NAME           SP_BUTTON_Y_NAME
	#define SP_BUTTON_UP_NOWASD_NAME    SP_BUTTON_Y_NOWASD_NAME
	#define SP_BUTTON_RIGHT_NAME        SP_BUTTON_B_NAME
	#define SP_BUTTON_RIGHT_NOWASD_NAME SP_BUTTON_B_NOWASD_NAME
	#define SP_BUTTON_DOWN_NAME         SP_BUTTON_X_NAME
	#define SP_BUTTON_DOWN_NOWASD_NAME  SP_BUTTON_X_NOWASD_NAME
	#define SP_PAD_NAME "Arrow Keys"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_A_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_B_NAME
	#define SP_PRACTICE_3_NAME SP_BUTTON_X_NAME
	#define SP_PRACTICE_4_NAME SP_BUTTON_Y_NAME
	#define SP_PRACTICE_OK_NOWASD_NAME SP_BUTTON_A_NOWASD_NAME
	#define SP_PRACTICE_CANCEL_NOWASD_NAME SP_BUTTON_B_NOWASD_NAME
	#define SP_PRACTICE_3_NOWASD_NAME SP_BUTTON_X_NOWASD_NAME
	#define SP_PRACTICE_4_NOWASD_NAME SP_BUTTON_Y_NOWASD_NAME
#endif

#ifndef DESKTOP
	#define SP_BUTTON_START_NOWASD_NAME SP_BUTTON_START_NAME
	#define SP_BUTTON_SELECT_NOWASD_NAME SP_BUTTON_SELECT_NAME
	#define SP_BUTTON_L_NOWASD_NAME SP_BUTTON_L_NAME
	#define SP_BUTTON_R_NOWASD_NAME SP_BUTTON_R_NAME
	#define SP_BUTTON_A_NOWASD_NAME SP_BUTTON_A_NAME
	#define SP_BUTTON_B_NOWASD_NAME SP_BUTTON_B_NAME
	#define SP_BUTTON_X_NOWASD_NAME SP_BUTTON_X_NAME
	#define SP_BUTTON_Y_NOWASD_NAME SP_BUTTON_Y_NAME
	#define SP_PRACTICE_OK_NOWASD_NAME     SP_PRACTICE_OK_NAME
	#define SP_PRACTICE_CANCEL_NOWASD_NAME SP_PRACTICE_CANCEL_NAME
	#define SP_PRACTICE_3_NOWASD_NAME      SP_PRACTICE_3_NAME
	#define SP_PRACTICE_4_NOWASD_NAME	     SP_PRACTICE_4_NAME
	#define SP_BUTTON_LEFT_NOWASD_NAME     SP_BUTTON_LEFT_NAME
	#define SP_BUTTON_UP_NOWASD_NAME       SP_BUTTON_UP_NAME
	#define SP_BUTTON_RIGHT_NOWASD_NAME    SP_BUTTON_RIGHT_NAME
	#define SP_BUTTON_DOWN_NOWASD_NAME     SP_BUTTON_DOWN_NAME
#endif

/* Defines: Generic Buttons Masks
 * 
 * Mask of the buttons, Useful for <spPollKeyboardInput>s last parameter.
 * 
 * SP_BUTTON_START_MASK - mask of the "Start" or "Menu" button
 * SP_BUTTON_SELECT_MASK - mask of the "Select", "||" or similar button
 * SP_BUTTON_L_MASK - mask of the left shoulder button
 * SP_BUTTON_R_MASK - mask of the right shoulder button
 * SP_BUTTON_A_MASK - mask of the A button.
 * SP_BUTTON_B_MASK - mask of the B button.
 * SP_BUTTON_X_MASK - mask of the X button.
 * SP_BUTTON_Y_MASK - mask of the Y button.
 * SP_BUTTON_LEFT_MASK - mask of the left button.
 * SP_BUTTON_RIGHT_MASK - mask of the right button.
 * SP_BUTTON_UP_MASK - mask of the up button.
 * SP_BUTTON_DOWN_MASK - mask of the down button.
 * SP_PRACTICE_OK_MASK - mask of the button, which is on the specific device the
 * standard button for "ok".
 * SP_PRACTICE_CANCEL_MASK - mask of the button, which is on the specific device
 * the standard button for canceling an action.
 * SP_PRACTICE_3_MASK - mask of the button, which is on the specific device the
 * neither ok nor cancel.
 * SP_PRACTICE_4_MASK - mask of the other button, which is on the specific device
 * neither ok nor cancel. */

//Bit masks for every button
#define SP_BUTTON_START_MASK (1 << SP_BUTTON_START)
#define SP_BUTTON_SELECT_MASK (1 << SP_BUTTON_SELECT)
#define SP_BUTTON_L_MASK (1 << SP_BUTTON_L)
#define SP_BUTTON_R_MASK (1 << SP_BUTTON_R)
#define SP_BUTTON_A_MASK (1 << SP_BUTTON_A)
#define SP_BUTTON_B_MASK (1 << SP_BUTTON_B)
#define SP_BUTTON_X_MASK (1 << SP_BUTTON_X)
#define SP_BUTTON_Y_MASK (1 << SP_BUTTON_Y)
#define SP_BUTTON_LEFT_MASK (1 << SP_BUTTON_LEFT)
#define SP_BUTTON_UP_MASK (1 << SP_BUTTON_UP)
#define SP_BUTTON_RIGHT_MASK (1 << SP_BUTTON_RIGHT)
#define SP_BUTTON_DOWN_MASK (1 << SP_BUTTON_DOWN)
#define SP_PRACTICE_OK_MASK (1 << SP_PRACTICE_OK)
#define SP_PRACTICE_CANCEL_MASK (1 << SP_PRACTICE_CANCEL)
#define SP_PRACTICE_3_MASK (1 << SP_PRACTICE_3)
#define SP_PRACTICE_4_MASK (1 << SP_PRACTICE_4)
#define SP_BUTTON_START_NOWASD_MASK (1 << SP_BUTTON_START_NOWASD)
#define SP_BUTTON_SELECT_NOWASD_MASK (1 << SP_BUTTON_SELECT_NOWASD)
#define SP_BUTTON_L_NOWASD_MASK (1 << SP_BUTTON_L_NOWASD)
#define SP_BUTTON_R_NOWASD_MASK (1 << SP_BUTTON_R_NOWASD)
#define SP_BUTTON_A_NOWASD_MASK (1 << SP_BUTTON_A_NOWASD)
#define SP_BUTTON_B_NOWASD_MASK (1 << SP_BUTTON_B_NOWASD)
#define SP_BUTTON_X_NOWASD_MASK (1 << SP_BUTTON_X_NOWASD)
#define SP_BUTTON_Y_NOWASD_MASK (1 << SP_BUTTON_Y_NOWASD)
#define SP_BUTTON_LEFT_NOWASD_MASK (1 << SP_BUTTON_LEFT_NOWASD)
#define SP_BUTTON_UP_NOWASD_MASK (1 << SP_BUTTON_UP_NOWASD)
#define SP_BUTTON_RIGHT_NOWASD_MASK (1 << SP_BUTTON_RIGHT_NOWASD)
#define SP_BUTTON_DOWN_NOWASD_MASK (1 << SP_BUTTON_DOWN_NOWASD)
#define SP_PRACTICE_OK_NOWASD_MASK (1 << SP_PRACTICE_OK_NOWASD)
#define SP_PRACTICE_CANCEL_NOWASD_MASK (1 << SP_PRACTICE_CANCEL_NOWASD)
#define SP_PRACTICE_3_NOWASD_MASK (1 << SP_PRACTICE_3_NOWASD)
#define SP_PRACTICE_4_NOWASD_MASK (1 << SP_PRACTICE_4_NOWASD)
#ifdef GP2X
	#define SP_BUTTON_GP2X_ONLY_VOLPLUS_MASK (1 << SP_BUTTON_GP2X_ONLY_VOLPLUS)
	#define SP_BUTTON_GP2X_ONLY_VOLMINUS_MASK (1 << SP_BUTTON_GP2X_ONLY_VOLMINUS)
	#define SP_BUTTON_GP2X_ONLY_TIP_MASK (1 << SP_BUTTON_GP2X_ONLY_TIP)
#endif

/* Defines: Directions
 * 
 * These are some #defines e.g. for sparrowPrimitives to determine,
 * where the base of the drawn primitive is.
 * 
 * SP_CENTER - center
 * SP_LEFT - left
 * SP_TOP - top
 * SP_RIGHT - right
 * SP_BOTTOM - bottom
 * SP_FIXED - sets the base to a fixed position*/
#define SP_CENTER 0
#define SP_LEFT 1
#define SP_TOP 2
#define SP_RIGHT 3
#define SP_BOTTOM 4
#define SP_FIXED 5

// Define: SP_NO_TOUCHSCREEN_EMULATION
// Used by <spSetTouchscreenEmulationButtons>
#define SP_NO_TOUCHSCREEN_EMULATION -1

// Define: SP_ALPHA_COLOR
// If alphatest is enabled, this (pink) color will not be drawn, it is "transparent".
#define SP_ALPHA_COLOR 63519

// Define: SP_DEVICE_STRING
// Gives you the name of the device, e.g. "GP2X F100" or "Personal Computer"
#ifdef F100
	#define SP_DEVICE_STRING "GP2X F100"
#elif defined WIZ
	#define SP_DEVICE_STRING "GP2X Wiz"
#elif defined CAANOO
	#define SP_DEVICE_STRING "GP2X Caanoo"
#elif defined DINGUX
	#define SP_DEVICE_STRING "Dingoo"
#elif defined GCW
	#define SP_DEVICE_STRING "GCW Zero"
#elif defined PANDORA
	#define SP_DEVICE_STRING "Open Pandora"
#elif defined _WIN32
	#define SP_DEVICE_STRING "Windows PC"
#else
	#define SP_DEVICE_STRING "Linux PC"
#endif

#endif
