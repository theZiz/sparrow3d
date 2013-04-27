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

/* File: sparrowMath
 * 
 * This file is for fast functions for fixed point numbers like
 * trigonometric functions, square root, etc. */
#ifndef _SPARROW_MATH_H
#define _SPARROW_MATH_H

#include "sparrowDefines.h"
#include <SDL.h>

/* Function: spInitMath
 * 
 * Initializes the fixed point look up tables for sin, cos, etc. Is called by
 * <spInitCore>.*/
PREFIX void spInitMath( void );

/* Function: spSin
 * 
 * Calculates the sinus of a fixed point value
 * 
 * Parameters:
 * value - fixed point radian value
 * 
 * Returns:
 * Sint32 - fixed point result
 * 
 * See Also:
 * <spCos>, <spTan>*/
PREFIX Sint32 spSin( Sint32 value );

/* Function: spCos
 * 
 * Calculates the cosinus of a fixed point value
 * 
 * Parameters:
 * value - fixed point radian value
 * 
 * Returns:
 * Sint32 - fixed point result
 * 
 * See Also:
 * <spSin>, <spTan>*/
PREFIX Sint32 spCos( Sint32 value );

/* Function: spTan
 * 
 * Calculates the tangent of a fixed point value
 * 
 * Parameters:
 * value - fixed point radian value
 * 
 * Returns:
 * Sint32 - fixed point result
 * 
 * See Also:
 * <spSin>, <spCos>*/
PREFIX Sint32 spTan( Sint32 value );

/* Function: spAsin
 * 
 * Calculates the arcus sinus of a fixed point value
 * 
 * Parameters:
 * value - fixed point radian value
 * 
 * Returns:
 * Sint32 - fixed point result. If the value out of the range (not between
 * -<SP_ONE> and <SP_ONE>) zero is returned. If you want some error handling, do
 * it yourself.
 * 
 * See Also:
 * <spAcos>*/
PREFIX Sint32 spAsin(Sint32 value);

/* Function: spAcos
 * 
 * Calculates the arcus cosinus of a fixed point value
 * 
 * Parameters:
 * value - fixed point radian value
 * 
 * Returns:
 * Sint32 - fixed point result. If the value out of the range (not between
 * -<SP_ONE> and <SP_ONE>) zero is returned. If you want some error handling, do
 * it yourself.
 * 
 * See Also:
 * <spAsin>*/
PREFIX Sint32 spAcos(Sint32 value);

/* Function: spSqrt
 * 
 * Fast square root for fixed point numbers
 * 
 * Parameters:
 * n - the fixed point value to be square rooted
 * 
 * Returns:
 * Sint32 - square root of n*/
PREFIX Sint32 spSqrt ( Sint32 n );

/* Function: spUnsave_Small_Sqrt
 * 
 * This functions returns the square root of a number between 0 and
 * 1 << <SP_SQRT_ACCURACY>. If you give a number outside this range
 * IT WILL CRASH! So use spSqrt if you are unsure. The light calculation uses
 * this function for fast rendering.
 * 
 * Parameters:
 * n - as said a fixed point value between 0 and 1 << <SP_SQRT_ACCURACY>
 * 
 * Returns:
 * Sint32 - the square root of n. Always keep in mind, that the normal fixed
 * point numbers with shift of <SP_ACCURACY> may not be compatible to this fixed
 * point numbers with shift of <SP_SQRT_ACCURACY>!*/
PREFIX Sint32 spUnsave_Small_Sqrt(Sint32 n);

/* Function: spMin
 * 
 * Returns the smallest of 2 numbers. To get the smallest of three and more
 * numbers, use a tree like function call like: spMin(spMin(a,b),c) or
 * spMin(spMin(a,b),spMin(c,d)).
 * 
 * Parameters:
 * a,b - numbers to compare
 * 
 * Returns:
 * Sint32 - the smaller number. If the numbers are even, it returns the bigger
 * on.
 * 
 * See Also:
 * <spMax>*/
PREFIX Sint32 spMin( Sint32 a, Sint32 b );

/* Function: spMax
 * 
 * Returns the biggest of 2 numbers. To get the biggest of three and more
 * numbers, use a tree like function call like: spMax(spMax(a,b),c) or
 * spMax(spMax(a,b),spMax(c,d)).
 * 
 * Parameters:
 * a,b - numbers to compare
 * 
 * Returns:
 * Sint32 - the bigger number. If the numbers are even, it returns the smaller
 * on.
 * 
 * See Also:
 * <spMin>*/
PREFIX Sint32 spMax( Sint32 a, Sint32 b );

/* Function: spAtof
 * 
 * Converts a string to a Sint32 fixed point number like atof. Be careful, it
 * is slow, especially on devices without FPU. But perfect for loading stuff.
 * 
 * Parameters:
 * buffer - string to convert
 * 
 * Returns:
 * Sint32 - the fixed point value stored in buffer. That means, if buffer is
 * "12.3" the fixed point conversion of 12.3 will be saved, which is 806092 and
 * not 12. ;)*/
PREFIX Sint32 spAtof( char* buffer );

#endif
