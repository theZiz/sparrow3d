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
#ifndef _SPARROW_MATH_H
#define _SPARROW_MATH_H

#include "sparrowDefines.h"
#include <SDL.h>

/* spInitMath initializes the fixed point look up tables for sin, cos, etc. */
PREFIX void spInitMath(void);

/* spSin calculates the sin of value*/
PREFIX Sint32 spSin(Sint32 value);

/* spCos calculates the cos of value*/
PREFIX Sint32 spCos(Sint32 value);

/* spFPdiv divides very fast*/
#ifdef ARMCPU
  Sint32 spFPdiv(register Sint32 numerator, register Sint32 denominator);
#else
  Sint32 spFPdiv(Sint32 numerator,Sint32 denominator);
#endif

PREFIX Sint32 spSqrt (Sint32 n);

/* spMin return the smallest of 2 numbers */
PREFIX Sint32 spMin(Sint32 a,Sint32 b);

/* spMin3 return the smallest of 3 numbers */
PREFIX Sint32 spMin3(Sint32 a,Sint32 b, Sint32 c);

/* spMin4 return the smallest of 4 numbers */
PREFIX Sint32 spMin4(Sint32 a,Sint32 b, Sint32 c, Sint32 d);

/* spMax return the biggest of 2 numbers */
PREFIX Sint32 spMax(Sint32 a,Sint32 b);

/* spMax3 return the biggest of 2 numbers */
PREFIX Sint32 spMax3(Sint32 a,Sint32 b, Sint32 c);

/* spMax4 return the biggest of 2 numbers */
PREFIX Sint32 spMax4(Sint32 a,Sint32 b, Sint32 c, Sint32 d);

#endif
