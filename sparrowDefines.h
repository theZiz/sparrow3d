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
 of the GNU Lesser General Public license (the	"LGPL License"), in which case the
 provisions of LGPL License are applicable instead of those
 above.

 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

/* sparrowDefines contains many #defines used by the library or which
 * may be usefull four your application */
 
#ifndef _SPARROW_DEFINES_H
#define _SPARROW_DEFINES_H

/* These are defines for creating or using the library in C or C++ */
#ifdef WIN32
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

/* The following #defines are for the fixed point arithmetic in
 * sparrow3d. Change the values only, if you know, what you are doing.
 * There should be optimal already.*/

#define SP_ACCURACY 16
#define SP_HALF_ACCURACY 8
#define SP_ACCURACY_FACTOR 65536.0f
#define SP_PI 205888
/* This is not really accuracy... More inaccuracy ^^ */
#define SP_MATH_ACCURACY 4
/* a fast define for the fixed point value 1.0 */
#define SP_ONE 65536

/* if a REAL input device (not the sparrow3d generic input device!) has
 * a analog stick, these are the limits, from which the generic axis are
 * uneven 0 or reseted to 0 again*/
#define SP_JOYSTICK_MIN_TRIGGER_ON -24576
#define SP_JOYSTICK_MIN_TRIGGER_OFF -8192
#define SP_JOYSTICK_MAX_TRIGGER_ON 24576
#define SP_JOYSTICK_MAX_TRIGGER_OFF	8192

/* These are some #defines e.g. for sparrowPrimitives to determine,
 * where the base of the drawn primitive is. */
#define SP_CENTER 0
#define SP_LEFT 1
#define SP_TOP 2
#define SP_RIGHT 3
#define SP_BOTTOM 4

/* TODO: Hm, what did I used this for? ^^ */
#define SP_NO_TOUCHSCREEN_EMULATION -1

/* These are some #defines for fast multiplication and division of
 * fixed point numbers. Use them like normal functions, e.g.
 * Sint32 a = spMul(b,c); //a = b * c */
#ifdef FAST_MULTIPLICATION
	#define spMul(a,b) (((a)>>SP_HALF_ACCURACY)*((b)>>SP_HALF_ACCURACY))
#else
	#define spMul(a,b) ((Sint64)(a)*(Sint64)(b)>>SP_ACCURACY)
#endif
#define spMulHigh(a,b) ((Sint64)(a)*(Sint64)(b)>>SP_ACCURACY)

#ifdef FAST_DIVISION
	#define spDiv(a,b) ((b>=0 && b<(1<<SP_PRIM_ACCURACY))? \
											 (a*spGetOne_over_x_pointer()[b]>>SP_PRIM_ACCURACY-SP_ACCURACY): \
											 ( \
												 (b <0 && b>(-1<<SP_PRIM_ACCURACY))? \
												 (-a*spGetOne_over_x_pointer()[-b]>>SP_PRIM_ACCURACY-SP_ACCURACY): \
												 (((a<<SP_HALF_ACCURACY)/b)<<SP_HALF_ACCURACY) \
											 ))
#else
	//#define spDiv(a,b) ((((a)<<SP_HALF_ACCURACY)/(b))<<SP_HALF_ACCURACY)
	#define spDiv(a,b) (((Sint64)(a)<<SP_ACCURACY)/(Sint64)(b))
#endif
#define spDivHigh(a,b) (((Sint64)(a)<<SP_ACCURACY)/(Sint64)(b))

/* The following (long) list of #defines gives you the numbers and names
 * of the generic device buttons. */
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
	#define SP_BUTTON_VOLPLUS 16
	#define SP_BUTTON_VOLMINUS 17
	#define SP_BUTTON_TIP 18
	//just for different SP_PRActices
	#define SP_PRACTICE_OK SP_BUTTON_B
	#define SP_PRACTICE_CANCEL SP_BUTTON_X
#elif defined CAANOO
	#define SP_BUTTON_A 0
	#define SP_BUTTON_X 1
	#define SP_BUTTON_B 2
	#define SP_BUTTON_Y 3
	#define SP_BUTTON_L 4
	#define SP_BUTTON_R 5
	#define SP_BUTTON_START 6
	#define SP_BUTTON_TIP 7
	//SELECT == HELP I
	#define SP_BUTTON_SELECT 8
	//the Caanoo has a hardware volume control...
	#define SP_BUTTON_VOLPLUS 18
	#define SP_BUTTON_VOLMINUS 19
	#define SP_PRACTICE_OK SP_BUTTON_B
	#define SP_PRACTICE_CANCEL SP_BUTTON_X
#else //PC and DINGUX and PANDORA
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
	#define SP_BUTTON_VOLPLUS 16
	#define SP_BUTTON_VOLMINUS 17
	#define SP_BUTTON_TIP 18
	#define SP_PRACTICE_OK SP_BUTTON_A
	#define SP_PRACTICE_CANCEL SP_BUTTON_X
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
	#define SP_PAD_NAME "D-Pad"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_B_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
#elif defined WIZ
	#define SP_BUTTON_START_NAME "Menu"
	#define SP_BUTTON_SELECT_NAME "Select"
	#define SP_BUTTON_L_NAME "L"
	#define SP_BUTTON_R_NAME "R"
	#define SP_BUTTON_A_NAME "A"
	#define SP_BUTTON_B_NAME "B"
	#define SP_BUTTON_X_NAME "X"
	#define SP_BUTTON_Y_NAME "Y"
	#define SP_PAD_NAME "D-Pad"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_B_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
#elif defined CAANOO
	#define SP_BUTTON_START_NAME "Home"
	#define SP_BUTTON_SELECT_NAME "Help I"
	#define SP_BUTTON_L_NAME "L"
	#define SP_BUTTON_R_NAME "R"
	#define SP_BUTTON_A_NAME "A"
	#define SP_BUTTON_B_NAME "B"
	#define SP_BUTTON_X_NAME "X"
	#define SP_BUTTON_Y_NAME "Y"
	#define SP_PAD_NAME "Stick"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_B_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
#elif defined DINGUX
	#define SP_BUTTON_START_NAME "Start"
	#define SP_BUTTON_SELECT_NAME "Select"
	#define SP_BUTTON_L_NAME "L"
	#define SP_BUTTON_R_NAME "R"
	#define SP_BUTTON_A_NAME "A"
	#define SP_BUTTON_B_NAME "B"
	#define SP_BUTTON_X_NAME "X"
	#define SP_BUTTON_Y_NAME "Y"
	#define SP_PAD_NAME "D-Pad"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_A_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
#elif defined PANDORA
	#define SP_BUTTON_START_NAME "Start"
	#define SP_BUTTON_SELECT_NAME "Select"
	#define SP_BUTTON_L_NAME "L"
	#define SP_BUTTON_R_NAME "R"
	#define SP_BUTTON_A_NAME "A"
	#define SP_BUTTON_B_NAME "B"
	#define SP_BUTTON_X_NAME "X"
	#define SP_BUTTON_Y_NAME "Y"
	#define SP_PAD_NAME "D-Pad"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_A_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
#else
	#define SP_BUTTON_START_NAME "Return"
	/*#define SP_BUTTON_SELECT_NAME "Esc"
	#define SP_BUTTON_L_NAME "PgUp"
	#define SP_BUTTON_R_NAME "PgDn"
	#define SP_BUTTON_A_NAME "AltGr"
	#define SP_BUTTON_B_NAME "Ctrl"
	#define SP_BUTTON_X_NAME "Super"
	#define SP_BUTTON_Y_NAME "Alt"*/
	#define SP_BUTTON_SELECT_NAME "Backspace"
	#define SP_BUTTON_L_NAME "q"
	#define SP_BUTTON_R_NAME "e"
	#define SP_BUTTON_A_NAME "a"
	#define SP_BUTTON_B_NAME "d"
	#define SP_BUTTON_X_NAME "s"
	#define SP_BUTTON_Y_NAME "w"	
	#define SP_PAD_NAME "Arrow Keys"
	#define SP_PRACTICE_OK_NAME SP_BUTTON_A_NAME
	#define SP_PRACTICE_CANCEL_NAME SP_BUTTON_X_NAME
#endif

#endif
