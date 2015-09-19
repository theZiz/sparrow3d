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
	if (n <= (1 << SP_SQRT_ACCURACY))
		return spSqrtvalue[n];
#ifdef BLACK_MAGIC
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
	return (Sint32)(spAtoFloat(buffer)*SP_ACCURACY_FACTOR);
}

Sint32 spLastRandValue = 0;

PREFIX void spSetRand( Sint32 seed )
{
	if (seed < 0)
		seed*=-1;
	spLastRandValue = seed;
}

PREFIX Sint32 spRand( void )
{
	//I stole the magic numbers from glibc...
	spLastRandValue = ((1103515245*spLastRandValue + 12345) >> 1)  & 0x7fffffff;
	return spLastRandValue;
}

#define DIGIT_CHECK(v) \
	((v) == '0' || (v) == '1' || \
	 (v) == '2' || (v) == '3' || \
	 (v) == '4' || (v) == '5' || \
	 (v) == '6' || (v) == '7' || \
	 (v) == '8' || (v) == '9')

PREFIX double spAtoFloat( char* buffer )
{
	int i = 0;
	while (buffer[i] == ' ' && buffer[i] != 0)
		i++;
	if (buffer[i] == 0)
		return 0.0f;
	double result = 0.0f;
	double sign = 1.0f;
	Sint64 left = 0;
	Sint64 middle = 0;
	Sint64 divisor = 1;
	Sint64 right = 0;
	//sign
	if (buffer[i] == '-')
	{
		sign = -1.0f;
		i++;
	}
	//left part
	while ( DIGIT_CHECK(buffer[i]) )
	{
		left *= 10;
		int digit = (int)buffer[i] - (int)'0';
		left += digit;
		i++;
	}
	//middle part
	if (buffer[i] == '.')
	{
		i++;
		while ( DIGIT_CHECK(buffer[i]) )
		{
			middle *= 10;
			int digit = (int)buffer[i] - (int)'0';
			middle += digit;
			divisor *= 10;
			i++;
		}
	}
	//right part
	if (buffer[i] == 'e' || buffer[i] == 'E')
	{
		i++;
		while ( DIGIT_CHECK(buffer[i]) )
		{
			right *= 10;
			int digit = (int)buffer[i] - (int)'0';
			right += digit;
			i++;
		}
	}
	if (right)
		return sign*((double)left + (double)middle/(double)divisor)*pow(10.0f,(double)right);
	else
		return sign*((double)left + (double)middle/(double)divisor);
}
