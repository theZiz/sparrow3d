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

/* This file is for fast functions for fixed point numbers like
 * trigonometric functions, square root, etc. */
#ifndef _SPARROW_MATH_H
#define _SPARROW_MATH_H

#include "sparrowDefines.h"
#include <SDL.h>

/* spInitMath initializes the fixed point look up tables for sin, cos, etc. */
PREFIX void spInitMath( void );

/* spSin calculates the sin of value*/
PREFIX Sint32 spSin( Sint32 value );

/* spCos calculates the cos of value*/
PREFIX Sint32 spCos( Sint32 value );

/* spTan calculates the cos of value*/
PREFIX Sint32 spTan( Sint32 value );

/* spAcos calculates the acos of value. For values smaller than -1 and bigger
 * than 1, it returns 0. */
PREFIX Sint32 spAcos(Sint32 value);

/* spAcos calculates the asin of value. For values smaller than -1 and bigger
 * than 1, it returns 0. */
PREFIX Sint32 spAsin(Sint32 value);

/* spSqrt fast square root*/
PREFIX Sint32 spSqrt ( Sint32 n );

/* This functions returns the square root of a number between 0 and
 * 1 << SP_SQRT_ACCURACY. If you give a number outside this range
 * IT WILL CRASH! So just use spSqrt to be safe. This function is
 * necessary for a good looking light calculation */
PREFIX Sint32 spUnsave_Small_Sqrt(Sint32 n);

/* spMin return the smallest of 2 numbers */
PREFIX Sint32 spMin( Sint32 a, Sint32 b );

/* spMax return the biggest of 2 numbers */
PREFIX Sint32 spMax( Sint32 a, Sint32 b );

/* Converts a string to a Sint32 fixed point number like atof. Be careful, it
 * is slow, especially on devices without FPU. But perfect for loading stuff. */
PREFIX Sint32 spAtof( char* buffer );

#endif
