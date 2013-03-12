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
#include "sparrowMath.h"
#include <stdlib.h>
#include <math.h>

int spMathIsInitialized = 0;
Sint32 spCosvalue[(2 * SP_PI) >> SP_MATH_ACCURACY];
Sint32 spTanvalue[(    SP_PI) >> SP_MATH_ACCURACY];
Sint32 spAcosvalue[(2 << SP_ACCURACY)+1];
Sint32 spSqrtvalue[(1 << SP_SQRT_ACCURACY)+1];

PREFIX void spInitMath(void)
{
	if (spMathIsInitialized)
		return;
	spMathIsInitialized = 1;
	int a;
	for (a = 0; a < ((SP_PI << 1) >> SP_MATH_ACCURACY); a++)
		spCosvalue[a] = (Sint32)(cos((float)(a << SP_MATH_ACCURACY) / SP_ACCURACY_FACTOR) * SP_ACCURACY_FACTOR);
	for (a = 0; a < ((SP_PI     ) >> SP_MATH_ACCURACY); a++)
		spTanvalue[a] = (Sint32)(tan((float)(a << SP_MATH_ACCURACY) / SP_ACCURACY_FACTOR) * SP_ACCURACY_FACTOR);
	for (a = 0; a < (2 << SP_ACCURACY) +1; a++)
		spAcosvalue[a] = (Sint32)(acos((float)(a-(SP_ONE)) / SP_ACCURACY_FACTOR) * SP_ACCURACY_FACTOR);
	for (a = 0; a < (1 << SP_SQRT_ACCURACY) +1; a++)
		spSqrtvalue[a] = (Sint32)(sqrt((float)a / SP_ACCURACY_FACTOR) * SP_ACCURACY_FACTOR);
}

PREFIX Sint32 spSin(Sint32 value)
{
	return spCos(value + ((3 * SP_PI) >> 1));
}

PREFIX Sint32 spCos(Sint32 value)
{
	return spCosvalue[(abs(value) % (SP_PI << 1)) >> SP_MATH_ACCURACY];
}

PREFIX Sint32 spTan(Sint32 value)
{
	return spTanvalue[(abs(value) % (SP_PI)) >> SP_MATH_ACCURACY];
}

PREFIX Sint32 spAcos(Sint32 value)
{
	if (value<(-SP_ONE))
	  return 0;
	if (value>( SP_ONE))
	  return 0;
	return spAcosvalue[value+(SP_ONE)];
}

PREFIX Sint32 spAsin(Sint32 value)
{
	return SP_PI/2-spAcos(value);
}


//senquack - credit for this fast sqrt goes to Wilco Dijkstra http://www.finesse.demon.co.uk/steven/sqrt.html
#define iter1(N) \
    try_ = root + (1 << (N)); \
    if (n >= try_ << (N))   \
    {   n -= try_ << (N);   \
        root |= 2 << (N); \
    }

PREFIX Sint32 spUnsave_Small_Sqrt(Sint32 n)
{
	return spSqrtvalue[n];	
}

PREFIX Sint32 spSqrt (Sint32 n)
{
	if (n <= 0)
		return 0;
	if (n <= (1 << SP_SQRT_ACCURACY)+1)
		return spSqrtvalue[n];
#ifdef FAST_BUT_UGLY
	int bit_count = 30;
	Sint32 x = 1 << bit_count;
	while (x > n)
	{
		bit_count-=2;
		x >>= 2;
	}
	return spSqrtvalue[n >> bit_count - SP_SQRT_ACCURACY + 2] << (bit_count - SP_SQRT_ACCURACY >> 1) + 1;
#else
	Sint32 root = 0, try_;
	iter1 (15);
	iter1 (14);
	iter1 (13);
	iter1 (12);
	iter1 (11);
	iter1 (10);
	iter1 (9);
	iter1 (8);
	iter1 (7);
	iter1 (6);
	iter1 (5);
	iter1 (4);
	iter1 (3);
	iter1 (2);
	iter1 (1);
	iter1 (0);
	return root << (SP_HALF_ACCURACY - 1);
#endif
}


PREFIX Sint32 spMin(Sint32 a, Sint32 b)
{
	return (a < b) ? a : b;
}

PREFIX Sint32 spMax(Sint32 a, Sint32 b)
{
	return (a > b) ? a : b;
}

PREFIX Sint32 spAtof( char* buffer )
{
	return (Sint32)(atof(buffer)*SP_ACCURACY_FACTOR);
}
