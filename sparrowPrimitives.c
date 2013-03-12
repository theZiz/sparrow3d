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
#include "sparrowPrimitives.h"
#include <stdlib.h>
#include <string.h>
SDL_Surface* spTarget = NULL;
Uint16* spTargetPixel = NULL;
SDL_Surface* spTexture = NULL;
Uint16* spTexturePixel = NULL;
Uint32 spZTest = 1;
Uint32 spZSet = 1;
Uint32 spAlphaTest = 1;
Uint32 spQuadQuali = 0;
Sint32* spZBuffer = NULL;
Sint32 spZFar = -6553600; //-100.0f
Sint32 spMaxWLogDiff = 3;
Sint32 spZNear = -7000; //-0.1f
Sint32 spTargetScanLine = 0; //if the surface is even, same as spTargetX, else +1.
Sint32 spTargetX = 0;
Sint32 spTargetY = 0;
Sint32 spTextureX = 0;
Sint32 spTextureScanLine = 0; //if the surface is even, same as spTextureX, else +1.
Sint32 spTextureY = 0;
Sint32 spTextureXY = 0;
Sint32 spOne_over_x_look_up[1 << SP_ACCURACY];
Sint32 spOne_over_x_look_up_fixed[1 << SP_ACCURACY];
Uint32 spZBufferCacheCount = 16;
Uint32 spZBufferCacheLast;
Sint32** spZBufferCache = NULL;
SDL_Surface** spTargetCache = NULL;
Uint32* spSizeCache = NULL;
char spCulling = 1;
Sint32 spVerticalOrigin = SP_CENTER;
Sint32 spHorizontalOrigin = SP_CENTER;
Uint8 spPattern[8] = {255,255,255,255,255,255,255,255}; //64 Bit, 8 Bytes, 4 Halfwords, 2 words, 1 long word
Sint32 spUsePattern = 0;
int spPrimitivesIsInitialized = 0;

PREFIX Sint32* spGetOne_over_x_pointer()
{
	return spOne_over_x_look_up;
}

PREFIX void spInitPrimitives()
{
	if (spPrimitivesIsInitialized)
		return;
	spPrimitivesIsInitialized = 1;
	int i;
	for ( i = 1; i < ( 1 << SP_ACCURACY ); i++ )
	{
		spOne_over_x_look_up      [i] = (SP_ONE+(i >> 1)) / i;
		spOne_over_x_look_up_fixed[i] = (((Sint64)SP_ONE)<<SP_HALF_ACCURACY)/(Sint64)i;
	}
	spOne_over_x_look_up[0] = 0;
	spOne_over_x_look_up_fixed[0] = 0;
	spSetZBufferCache( spZBufferCacheCount );
}

PREFIX void spQuitPrimitives()
{
	int cacheline;
	for ( cacheline = 0; cacheline < spZBufferCacheCount; cacheline++ )
		if ( spZBufferCache[cacheline] )
			free( spZBufferCache[cacheline] );
	if ( spZBufferCache )
		free( spZBufferCache );
	if ( spTargetCache )
		free( spTargetCache );
	if ( spSizeCache )
		free( spSizeCache );
}

PREFIX void spSelectRenderTarget( SDL_Surface* target )
{
	spTarget = target;
	spTargetScanLine = target->pitch/target->format->BytesPerPixel;
	spTargetX = target->w;
	spTargetY = target->h;
	spTargetPixel = ( Uint16* )target->pixels;
	spReAllocateZBuffer();
}

PREFIX SDL_Surface* spGetRenderTarget()
{
	return spTarget;
}

PREFIX Uint16* spLockRenderTarget()
{
	SDL_LockSurface( spTarget );
	return (Uint16*)(spTarget->pixels);
}

PREFIX void spUnlockRenderTarget()
{
	SDL_UnlockSurface( spTarget );
}

PREFIX Sint32* spGetRenderTargetZBuffer()
{
	return spZBuffer;
}

PREFIX void spBindTexture( SDL_Surface* texture )
{
	spTexture = texture;
	if ( texture == NULL )
	{
		spTextureScanLine = 0;
		spTextureX = 0;
		spTextureY = 0;
		spTextureXY = 0;
		spTexturePixel = NULL;
		return;
	}
	spTextureScanLine = texture->pitch/texture->format->BytesPerPixel;
	spTextureX = texture->w;
	spTextureY = texture->h;
	spTextureXY = spTextureScanLine * spTextureY;
	spTexturePixel = ( Uint16* )texture->pixels;
}

PREFIX void spClearTarget( Uint32 color )
{
	SDL_FillRect( spTarget, NULL, color );
	/*SDL_Surface(spTarget);
	spHorizentalLine(spTargetPixel,0,0,spTargetScanLine*spTargetY,color,0,0,0);
	SDL_UnlockSurface(spTarget);*/
}

#ifdef __GNUC__
inline Sint32 one_over_x( Sint32 x ) __attribute__((always_inline));
#endif
inline Sint32 one_over_x( Sint32 x )
{
	if ( x > 0 )
	{
		if ( x < ( 1 << SP_ACCURACY ) )
			return spOne_over_x_look_up[x];
		if ( x == ( 1 << SP_ACCURACY ) )
			return 1;
		return 0;
	}
	if ( x > ( -1 << SP_ACCURACY ) )
		return -spOne_over_x_look_up[-x];
	if ( x == ( -1 << SP_ACCURACY ) )
		return -1;
	return 0;
}

#ifdef __GNUC__
inline Sint32 z_div( Sint32 z, Sint32 d ) __attribute__((always_inline));
#endif
inline Sint32 z_div( Sint32 z, Sint32 d )
{
#ifdef REALGP2X
	return spMulHigh( z, one_over_x( d ));
#else
	if ( d == 0 )
		return 0;
	return ((z)+((d)>>1)) / (d);
#endif
}

/* ************* Include of the pixel functions / defines *********** */
#include "sparrowPrimitiveSetPixelInclude.c"

/* ************* Include of the triangle functions *********** */
#define __SPARROW_INTERNAL_ZBOTH__
#include "sparrowPrimitiveTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZBOTH__
#define __SPARROW_INTERNAL_ZTEST__
#include "sparrowPrimitiveTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZTEST__
#define __SPARROW_INTERNAL_ZSET__
#include "sparrowPrimitiveTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZSET__
#define __SPARROW_INTERNAL_ZNOTHING__
#include "sparrowPrimitiveTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZNOTHING__

// with pattern
#define __SPARROW_INTERNAL_PATTERN__
#define __SPARROW_INTERNAL_ZBOTH__
#include "sparrowPrimitiveTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZBOTH__
#define __SPARROW_INTERNAL_ZTEST__
#include "sparrowPrimitiveTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZTEST__
#define __SPARROW_INTERNAL_ZSET__
#include "sparrowPrimitiveTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZSET__
#define __SPARROW_INTERNAL_ZNOTHING__
#include "sparrowPrimitiveTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZNOTHING__
#undef __SPARROW_INTERNAL_PATTERN__


PREFIX int spTriangle( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
{
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return 0;
	if ( spCulling && ( x2 - x1 ) * ( y3 - y1 ) - ( y2 - y1 ) * ( x3 - x1 ) > 0 )
		return 0;
	if (spZTest && z1 < 0 && z2 < 0 && z3 < 0)
		return 0;
	if ( y1 > y2 )
	{
		Sint32 temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
		temp = z1;
		z1 = z2;
		z2 = temp;
	}
	if ( y1 > y3 )
	{
		Sint32 temp = x1;
		x1 = x3;
		x3 = temp;
		temp = y1;
		y1 = y3;
		y3 = temp;
		temp = z1;
		z1 = z3;
		z3 = temp;
	}
	if ( y2 < y3 )
	{
		Sint32 temp = x2;
		x2 = x3;
		x3 = temp;
		temp = y2;
		y2 = y3;
		y3 = temp;
		temp = z2;
		z2 = z3;
		z3 = temp;
	}
	int result = spGetPixelPosition( x1, y1 ) | spGetPixelPosition( x2, y2 ) | spGetPixelPosition( x3, y3 );
	if ( !result )
		return 0;
	if ( spUsePattern )
	{
		if ( spZSet )
		{
			if ( spZTest )
				sp_intern_Triangle_ztest_zset_pattern( x1, y1, z1, x2, y2, z2, x3, y3, z3, color );
			else
				sp_intern_Triangle_zset_pattern      ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color );
		}
		else
		{
			if ( spZTest )
				sp_intern_Triangle_ztest_pattern     ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color );
			else
				sp_intern_Triangle_pattern           ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color );
		}
	}
	else
	{
		if ( spZSet )
		{
			if ( spZTest )
				sp_intern_Triangle_ztest_zset( x1, y1, z1, x2, y2, z2, x3, y3, z3, color );
			else
				sp_intern_Triangle_zset      ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color );
		}
		else
		{
			if ( spZTest )
				sp_intern_Triangle_ztest     ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color );
			else
				sp_intern_Triangle           ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color );
		}
	}
	return result;
}

/* ************ Include of the texture triangle functions *********** */
#ifndef NO_PERSPECTIVE
#define __SPARROW_INTERNAL_PERSPECT__
	#define __SPARROW_INTERNAL_ALPHA___
		#define __SPARROW_INTERNAL_ZBOTH__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZBOTH__
		#define __SPARROW_INTERNAL_ZTEST__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZTEST__
		#define __SPARROW_INTERNAL_ZSET__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZSET__
		#define __SPARROW_INTERNAL_ZNOTHING__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZNOTHING__

	#undef __SPARROW_INTERNAL_ALPHA___
		#define __SPARROW_INTERNAL_ZBOTH__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZBOTH__
		#define __SPARROW_INTERNAL_ZTEST__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZTEST__
		#define __SPARROW_INTERNAL_ZSET__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZSET__
		#define __SPARROW_INTERNAL_ZNOTHING__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZNOTHING__

	// with pattern
	#define __SPARROW_INTERNAL_PATTERN__
	#define __SPARROW_INTERNAL_ALPHA___
		#define __SPARROW_INTERNAL_ZBOTH__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZBOTH__
		#define __SPARROW_INTERNAL_ZTEST__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZTEST__
		#define __SPARROW_INTERNAL_ZSET__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZSET__
		#define __SPARROW_INTERNAL_ZNOTHING__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZNOTHING__

	#undef __SPARROW_INTERNAL_ALPHA___
		#define __SPARROW_INTERNAL_ZBOTH__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZBOTH__
		#define __SPARROW_INTERNAL_ZTEST__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZTEST__
		#define __SPARROW_INTERNAL_ZSET__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZSET__
		#define __SPARROW_INTERNAL_ZNOTHING__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZNOTHING__
	#undef __SPARROW_INTERNAL_PATTERN__

#undef __SPARROW_INTERNAL_PERSPECT__
#endif
	#define __SPARROW_INTERNAL_ALPHA___
		#define __SPARROW_INTERNAL_ZBOTH__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZBOTH__
		#define __SPARROW_INTERNAL_ZTEST__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZTEST__
		#define __SPARROW_INTERNAL_ZSET__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZSET__
		#define __SPARROW_INTERNAL_ZNOTHING__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZNOTHING__

	#undef __SPARROW_INTERNAL_ALPHA___
		#define __SPARROW_INTERNAL_ZBOTH__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZBOTH__
		#define __SPARROW_INTERNAL_ZTEST__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZTEST__
		#define __SPARROW_INTERNAL_ZSET__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZSET__
		#define __SPARROW_INTERNAL_ZNOTHING__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZNOTHING__

	// with pattern
	#define __SPARROW_INTERNAL_PATTERN__
	#define __SPARROW_INTERNAL_ALPHA___
		#define __SPARROW_INTERNAL_ZBOTH__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZBOTH__
		#define __SPARROW_INTERNAL_ZTEST__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZTEST__
		#define __SPARROW_INTERNAL_ZSET__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZSET__
		#define __SPARROW_INTERNAL_ZNOTHING__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZNOTHING__

	#undef __SPARROW_INTERNAL_ALPHA___
		#define __SPARROW_INTERNAL_ZBOTH__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZBOTH__
		#define __SPARROW_INTERNAL_ZTEST__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZTEST__
		#define __SPARROW_INTERNAL_ZSET__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZSET__
		#define __SPARROW_INTERNAL_ZNOTHING__
		#include "sparrowPrimitiveTexTriangleInclude.c"
		#undef __SPARROW_INTERNAL_ZNOTHING__
	#undef __SPARROW_INTERNAL_PATTERN__


PREFIX int spTriangle_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return 0;
	if ( spCulling && ( x2 - x1 ) * ( y3 - y1 ) - ( y2 - y1 ) * ( x3 - x1 ) > 0 )
		return 0;
	if (spZTest && z1 < 0 && z2 < 0 && z3 < 0)
		return 0;
	if ( y1 > y2 )
	{
		Sint32 temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
		temp = z1;
		z1 = z2;
		z2 = temp;
		temp = u1;
		u1 = u2;
		u2 = temp;
		temp = v1;
		v1 = v2;
		v2 = temp;
	}
	if ( y1 > y3 )
	{
		Sint32 temp = x1;
		x1 = x3;
		x3 = temp;
		temp = y1;
		y1 = y3;
		y3 = temp;
		temp = z1;
		z1 = z3;
		z3 = temp;
		temp = u1;
		u1 = u3;
		u3 = temp;
		temp = v1;
		v1 = v3;
		v3 = temp;
	}
	if ( y2 < y3 )
	{
		Sint32 temp = x2;
		x2 = x3;
		x3 = temp;
		temp = y2;
		y2 = y3;
		y3 = temp;
		temp = z2;
		z2 = z3;
		z3 = temp;
		temp = u2;
		u2 = u3;
		u3 = temp;
		temp = v2;
		v2 = v3;
		v3 = temp;
	}
	int result = spGetPixelPosition( x1, y1 ) | spGetPixelPosition( x2, y2 ) | spGetPixelPosition( x3, y3 );
	if ( !result )
		return 0;
	if ( spUsePattern )
	{
		if ( spAlphaTest )
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset_alpha_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
				else
					sp_intern_Triangle_tex_zset_alpha_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_alpha_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
				else
					sp_intern_Triangle_tex_alpha_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color );
			}
		}
		else
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
				else
					sp_intern_Triangle_tex_zset_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
				else
					sp_intern_Triangle_tex_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color );
			}
		}
	}
	else
	{
		if ( spAlphaTest )
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset_alpha( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
				else
					sp_intern_Triangle_tex_zset_alpha      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_alpha     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
				else
					sp_intern_Triangle_tex_alpha           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color );
			}
		}
		else
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
				else
					sp_intern_Triangle_tex_zset      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
				else
					sp_intern_Triangle_tex           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color );
			}
		}
	}
	return result;
}

PREFIX int spPerspectiveTriangle_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
{
	#ifdef NO_PERSPECTIVE
		return spTriangle_tex(x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
	#else
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return 0;
	if ( spCulling && ( x2 - x1 ) * ( y3 - y1 ) - ( y2 - y1 ) * ( x3 - x1 ) > 0 )
		return 0;
	if ( y1 > y2 )
	{
		Sint32 temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
		temp = z1;
		z1 = z2;
		z2 = temp;
		temp = u1;
		u1 = u2;
		u2 = temp;
		temp = v1;
		v1 = v2;
		v2 = temp;
		temp = w1;
		w1 = w2;
		w2 = temp;
	}
	if ( y1 > y3 )
	{
		Sint32 temp = x1;
		x1 = x3;
		x3 = temp;
		temp = y1;
		y1 = y3;
		y3 = temp;
		temp = z1;
		z1 = z3;
		z3 = temp;
		temp = u1;
		u1 = u3;
		u3 = temp;
		temp = v1;
		v1 = v3;
		v3 = temp;
		temp = w1;
		w1 = w3;
		w3 = temp;
	}
	if ( y2 < y3 )
	{
		Sint32 temp = x2;
		x2 = x3;
		x3 = temp;
		temp = y2;
		y2 = y3;
		y3 = temp;
		temp = z2;
		z2 = z3;
		z3 = temp;
		temp = u2;
		u2 = u3;
		u3 = temp;
		temp = v2;
		v2 = v3;
		v3 = temp;
		temp = w2;
		w2 = w3;
		w3 = temp;
	}
	//w clip of u and v:
	u1 = spDivHigh(u1 << SP_ACCURACY,w1);
	u2 = spDivHigh(u2 << SP_ACCURACY,w2);
	u3 = spDivHigh(u3 << SP_ACCURACY,w3);
	v1 = spDivHigh(v1 << SP_ACCURACY,w1);
	v2 = spDivHigh(v2 << SP_ACCURACY,w2);
	v3 = spDivHigh(v3 << SP_ACCURACY,w3);
	//reciprocal:
	w1 = spDivHigh(SP_ONE,w1);
	w2 = spDivHigh(SP_ONE,w2);
	w3 = spDivHigh(SP_ONE,w3);
	int result = spGetPixelPosition( x1, y1 ) | spGetPixelPosition( x2, y2 ) | spGetPixelPosition( x3, y3 );
	if ( !result )
		return 0;
	if ( spUsePattern )
	{
		if ( spAlphaTest )
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset_alpha_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
				else
					sp_intern_Triangle_tex_zset_alpha_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_alpha_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
				else
					sp_intern_Triangle_tex_alpha_pattern_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color );
			}
		}
		else
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
				else
					sp_intern_Triangle_tex_zset_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
				else
					sp_intern_Triangle_tex_pattern_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color );
			}
		}
	}
	else
	{
		if ( spAlphaTest )
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset_alpha_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
				else
					sp_intern_Triangle_tex_zset_alpha_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_alpha_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
				else
					sp_intern_Triangle_tex_alpha_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color );
			}
		}
		else
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
				else
					sp_intern_Triangle_tex_zset_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
				else
					sp_intern_Triangle_tex_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color );
			}
		}
	}
	return result;
	#endif
}

PREFIX int spQuad( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 x4, Sint32 y4, Sint32 z4, Uint32 color )
{
	int result = 0;
	if ( result = spTriangle( x1, y1, z1, x2, y2, z2, x3, y3, z3, color ) )
		return result | spTriangle( x1, y1, z1, x3, y3, z3, x4, y4, z4, color );
	return 0;
}

PREFIX int spQuad_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Uint32 color )
{
	int result = 0;
	switch (spQuadQuali)
	{
		case 2:
			{
				Sint32 mx = x1 + x2 + x3 + x4 >> 2;
				Sint32 my = y1 + y2 + y3 + y4 >> 2;
				Sint32 mu = u1 + u2 + u3 + u4 >> 2;
				Sint32 mv = v1 + v2 + v3 + v4 >> 2;
				Sint32 mz = ( z1 >> 2 ) + ( z2 >> 2 ) + ( z3 >> 2 ) + ( z4 >> 2 );

				Sint32 wx = x1 + x2 >> 1;
				Sint32 wy = y1 + y2 >> 1;
				Sint32 wu = u1 + u2 >> 1;
				Sint32 wv = v1 + v2 >> 1;
				Sint32 wz = ( z1 >> 1 ) + ( z2 >> 1 );
				if ( result = spTriangle_tex( mx, my, mz, mu, mv, x1, y1, z1, u1, v1, wx, wy, wz, wu, wv, color ) )
				{
					result |= spTriangle_tex( mx, my, mz, mu, mv, wx, wy, wz, wu, wv, x2, y2, z2, u2, v2, color );
					wx = x2 + x3 >> 1;
					wy = y2 + y3 >> 1;
					wu = u2 + u3 >> 1;
					wv = v2 + v3 >> 1;
					wz = ( z2 >> 1 ) + ( z3 >> 1 );
					result |= spTriangle_tex( mx, my, mz, mu, mv, x2, y2, z2, u2, v2, wx, wy, wz, wu, wv, color );
					result |= spTriangle_tex( mx, my, mz, mu, mv, wx, wy, wz, wu, wv, x3, y3, z3, u3, v3, color );
					wx = x3 + x4 >> 1;
					wy = y3 + y4 >> 1;
					wu = u3 + u4 >> 1;
					wv = v3 + v4 >> 1;
					wz = ( z3 >> 1 ) + ( z4 >> 1 );
					result |= spTriangle_tex( mx, my, mz, mu, mv, x3, y3, z3, u3, v3, wx, wy, wz, wu, wv, color );
					result |= spTriangle_tex( mx, my, mz, mu, mv, wx, wy, wz, wu, wv, x4, y4, z4, u4, v4, color );
					wx = x4 + x1 >> 1;
					wy = y4 + y1 >> 1;
					wu = u4 + u1 >> 1;
					wv = v4 + v1 >> 1;
					wz = ( z4 >> 1 ) + ( z1 >> 1 );
					result |= spTriangle_tex( mx, my, mz, mu, mv, x4, y4, z4, u4, v4, wx, wy, wz, wu, wv, color );
					return result | spTriangle_tex( mx, my, mz, mu, mv, wx, wy, wz, wu, wv, x1, y1, z1, u1, v1, color );
				}
			}
			return 0;
		case 1:
			{
				Sint32 mx = x1 + x2 + x3 + x4 >> 2;
				Sint32 my = y1 + y2 + y3 + y4 >> 2;
				Sint32 mu = u1 + u2 + u3 + u4 >> 2;
				Sint32 mv = v1 + v2 + v3 + v4 >> 2;
				Sint32 mz = ( z1 >> 2 ) + ( z2 >> 2 ) + ( z3 >> 2 ) + ( z4 >> 2 );
				if ( result = spTriangle_tex( mx, my, mz, mu, mv, x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, color ) )
				{
					result |= spTriangle_tex( mx, my, mz, mu, mv, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
					result |= spTriangle_tex( mx, my, mz, mu, mv, x3, y3, z3, u3, v3, x4, y4, z4, u4, v4, color );
					return result | spTriangle_tex( mx, my, mz, mu, mv, x4, y4, z4, u4, v4, x1, y1, z1, u1, v1, color );
				}
			}
			return 0;
		default:
			if ( result = spTriangle_tex( x1, y1, z1, u1, v1,
											x2, y2, z2, u2, v2,
											x3, y3, z3, u3, v3, color ) )
			return spTriangle_tex( x1, y1, z1, u1, v1,
									 x3, y3, z3, u3, v3,
									 x4, y4, z4, u4, v4, color ) | result;
	}
	return 0;
}

PREFIX int spPerspectiveQuad_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Sint32 w4, Uint32 color )
{
	int result = 0;
	if ( result = spPerspectiveTriangle_tex( x1, y1, z1, u1, v1, w1,
									x2, y2, z2, u2, v2, w2,
									x3, y3, z3, u3, v3, w3, color ) )
	return spPerspectiveTriangle_tex( x1, y1, z1, u1, v1, w1,
							 x3, y3, z3, u3, v3, w3,
							 x4, y4, z4, u4, v4, w4, color ) | result;
	return 0;
}

PREFIX void spReAllocateZBuffer()
{
	//in Cache?
	int cacheline;
	for ( cacheline = 0; cacheline < spZBufferCacheCount; cacheline++ )
		if ( spTargetCache[cacheline] == spTarget && spSizeCache[cacheline] == spTarget->w * spTarget->h )
			break;

	if ( cacheline == spZBufferCacheCount ) //not found
	{
		spZBufferCacheLast = ( spZBufferCacheLast + 1 ) % spZBufferCacheCount;
		if ( spZBufferCache[spZBufferCacheLast] )
			free( spZBufferCache[spZBufferCacheLast] );
		spZBuffer = ( Sint32* )malloc( spTargetScanLine * spTargetY * sizeof( Sint32 ) );
		spZBufferCache[spZBufferCacheLast] = spZBuffer;
		spTargetCache[spZBufferCacheLast] = spTarget;
		spSizeCache[spZBufferCacheLast] = spTarget->w * spTarget->h;
	}
	else
	{
		spZBuffer = spZBufferCache[cacheline];
	}
}

PREFIX void spResetZBuffer()
{
	int i;
	Sint32 s = spZFar-spZNear;
	if ( spZBuffer )
		for ( i = 0; i < spTargetScanLine * spTargetY; i++ )
			spZBuffer[i] = s;
}

PREFIX Sint32* spGetZBuffer()
{
	return spZBuffer;
}

PREFIX void spSetZTest( Uint32 test )
{
	spZTest = test;
}

PREFIX void spSetZSet( Uint32 test )
{
	spZSet = test;
}

PREFIX void spSetAlphaTest( Uint32 test )
{
	spAlphaTest = test;
}

PREFIX void spSetAffineTextureHack( Uint32 test )
{
	spQuadQuali = test;
}

#ifndef ARMCPU
#ifndef X86CPU
PREFIX void spHorizentalLine( Uint16* pixel, Sint32 x, Sint32 y, Sint32 l_, Uint32 color_, Uint32 check, Sint32 engineWindowX, Sint32 engineWindowY )
{
	//l_++;
	if ( check )
	{
		if ( y < 0 )
			return;
		if ( y >= engineWindowY ) //Their is a FUCKING Bug if I write in the last line on the gp2x. Dont know why...
			return;
		if ( x < 0 )
		{
			l_ = x + l_;
			x = 0;
		}
		if ( x + l_ >= engineWindowX )
			l_ = engineWindowX - x;
		if ( l_ <= 0 )
			return;
	}
	Uint32 pos = ( x + y * engineWindowX );
	int a;
	for ( a = pos; a < pos + l_; a++ )
		pixel[a] = color_;
}
#endif
#endif

#ifdef X86CPU
PREFIX void spHorizentalLine( Uint16* pixel, Sint32 x, Sint32 y, Sint32 l_, Uint32 color_, Uint32 check, Sint32 engineWindowX, Sint32 engineWindowY )
{
	//l_++;
	if ( check )
	{
		if ( y < 0 )
			return;
		if ( y >= engineWindowY ) //Their is a FUCKING Bug if I write in the last line on the gp2x. Dont know why...
			return;
		if ( x < 0 )
		{
			l_ = x + l_;
			x = 0;
		}
		if ( x + l_ >= engineWindowX )
			l_ = engineWindowX - x;
		if ( l_ <= 0 )
			return;
	}
	Uint32 pos = ( x + y * engineWindowX );
	if ( ( int )pixel + ( pos << 1 ) & 2 )
	{
		pixel[pos] = color_;
		pos++;
		l_--;
	}
	Uint32 color = color_ | ( color_ << 16 );
	int a;
	for ( a = pos >> 1; a < ( pos + l_ >> 1 ); a++ )
		( ( Uint32* )pixel )[a] = color;
	pixel[pos + l_ - 1] = color_;
}
#endif

PREFIX void spBlitSurface( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface )
{
	if ( spUsePattern )
	{
		if ( spAlphaTest )
		{
			if ( spZSet )
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					SDL_LockSurface( spTarget );
					Uint16 *pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ((spPattern[(y +v) & 7] >> ((x + u) & 7)) & 1)
							if ( pixel[( u + addu ) + ( v + addv )*(surface->pitch>>1)] != SP_ALPHA_COLOR && (Uint32)(z) < spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] )
							{
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
								spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] = z;
							}
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					Uint16 *pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ((spPattern[(y +v) & 7] >> ((x + u) & 7)) & 1)
							if ( pixel[( u + addu ) + ( v + addv )*(surface->pitch>>1)] != SP_ALPHA_COLOR)
							{
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
								spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] = z;
							}
				}
			}
			else
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					SDL_LockSurface( spTarget );
					Uint16 *pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ((spPattern[(y +v) & 7] >> ((x + u) & 7)) & 1)
							if ( pixel[( u + addu ) + ( v + addv )*(surface->pitch>>1)] != SP_ALPHA_COLOR && (Uint32)(z) < spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] )
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					SDL_LockSurface( spTarget );
					Uint16 *pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ((spPattern[(y +v) & 7] >> ((x + u) & 7)) & 1)
							if ( pixel[( u + addu ) + ( v + addv )*(surface->pitch>>1)] != SP_ALPHA_COLOR)
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
					SDL_UnlockSurface( spTarget );
				}
			}
		}
		else
		{
			if ( spZSet )
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					SDL_LockSurface( spTarget );
					Uint16 *pixel;
					pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ((spPattern[(y +v) & 7] >> ((x + u) & 7)) & 1)
							if ( (Uint32)(z) < spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] )
							{
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
								spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] = z;
							}
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					SDL_LockSurface( spTarget );
					Uint16 *pixel;
					pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ((spPattern[(y +v) & 7] >> ((x + u) & 7)) & 1)
							{
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
								spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] = z;
							}
					SDL_UnlockSurface( spTarget );
				}
			}
			else
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					SDL_LockSurface( spTarget );
					Uint16 *pixel;
					pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ((spPattern[(y +v) & 7] >> ((x + u) & 7)) & 1)
							if ( (Uint32)(z) < spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] )
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					SDL_LockSurface( spTarget );
					Uint16 *pixel;
					pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ((spPattern[(y +v) & 7] >> ((x + u) & 7)) & 1)
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
					SDL_UnlockSurface( spTarget );
				}
			}
		}
	}
	else
	{
		if ( spAlphaTest )
		{
			if ( spZSet )
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					SDL_LockSurface( spTarget );
					Uint16 *pixel;
					pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ( pixel[( u + addu ) + ( v + addv )*(surface->pitch>>1)] != SP_ALPHA_COLOR && (Uint32)(z) < spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] )
							{
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
								spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] = z;
							}
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] = z;
					SDL_Rect dest;
					dest.x = x - addu;
					dest.y = y - addv;
					dest.w = surface->w;
					dest.h = surface->h;
					SDL_SetColorKey( surface, SDL_SRCCOLORKEY, SP_ALPHA_COLOR );
					SDL_BlitSurface( surface, NULL, spTarget, &dest );
				}
			}
			else
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					SDL_LockSurface( spTarget );
					Uint16 *pixel;
					pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ( pixel[( u + addu ) + ( v + addv )*(surface->pitch>>1)] != SP_ALPHA_COLOR && (Uint32)(z) < spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] )
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					SDL_Rect dest;
					dest.w = surface->w;
					dest.h = surface->h;
					dest.x = x - addu;
					dest.y = y - addv;
					SDL_SetColorKey( surface, SDL_SRCCOLORKEY, SP_ALPHA_COLOR );
					SDL_BlitSurface( surface, NULL, spTarget, &dest );
				}
			}
		}
		else
		{
			if ( spZSet )
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					SDL_LockSurface( spTarget );
					Uint16 *pixel;
					pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ( (Uint32)(z) < spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] )
							{
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
								spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] = z;
							}
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] = z;
					SDL_Rect dest;
					dest.x = x - addu;
					dest.y = y - addv;
					dest.w = surface->w;
					dest.h = surface->h;
					SDL_SetColorKey( surface, 0, 0 );
					SDL_BlitSurface( surface, NULL, spTarget, &dest );
				}
			}
			else
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					SDL_LockSurface( spTarget );
					Uint16 *pixel;
					pixel = ( Uint16* )( surface->pixels );
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					int u = -addu;
					int maxu = u + surface->w;
					if ( x + u < 0 )
						u = -x;
					if ( x + maxu > spTargetX )
						maxu = spTargetX - x;
					int minv = -addv;
					int maxv = minv + surface->h;
					if ( y + minv < 0 )
						minv = -y;
					if ( y + maxv > spTargetY )
						maxv = spTargetY - y;
					int v;
					for ( ; u < maxu; u++ )
						for ( v = minv; v < maxv; v++ )
							if ( (Uint32)(z) < spZBuffer[( x + u ) + ( y + v )*spTargetScanLine] )
								spTargetPixel[( x + u ) + ( y + v )*spTargetScanLine] = pixel[( u + addu ) + ( v + addv ) * (surface->pitch>>1)];
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					int addu = surface->w >> 1;
					if ( spHorizontalOrigin == SP_LEFT )
						addu = 0;
					if ( spHorizontalOrigin == SP_RIGHT )
						addu = surface->w - 1;
					int addv = surface->h >> 1;
					if ( spVerticalOrigin == SP_TOP )
						addv = 0;
					if ( spVerticalOrigin == SP_BOTTOM )
						addv = surface->h - 1;
					SDL_Rect dest;
					dest.w = surface->w;
					dest.h = surface->h;
					dest.x = x - addu;
					dest.y = y - addv;
					SDL_SetColorKey( surface, 0, 0 );
					SDL_BlitSurface( surface, NULL, spTarget, &dest );
				}
			}
		}
	}
}

PREFIX void spBlitSurfacePart( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h )
{
	int addu;
	switch (spHorizontalOrigin)
	{
		case SP_LEFT: addu = 0; break;
		case SP_RIGHT: addu = w - 1; break;
		default: addu = w >> 1;
	}
	int addv;
	switch (spVerticalOrigin)
	{
		case SP_TOP: addv = 0; break;
		case SP_BOTTOM: addv = h - 1; break;
		default: addv = h >> 1;
	}
	if ( spUsePattern )
	{
		if ( spAlphaTest )
		{
			if ( spZSet )
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
	
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_ztest_zset_alpha_pattern( x, y, z, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_zset_alpha_pattern( x, y, z, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
			}
			else
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_ztest_alpha_pattern( x, y, z, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_alpha_pattern( x, y, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
			}
		}
		else
		{
			if ( spZSet )
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_ztest_zset_pattern( x, y, z, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_zset_pattern( x, y, z, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
			}
			else
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_ztest_pattern( x, y, z, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_pattern( x, y, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
			}
		}
	}
	else
	{
		if ( spAlphaTest )
		{
			if ( spZSet )
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_ztest_zset_alpha( x, y, z, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					int u = sx;
					for ( x = x1; x < x2; x++ )
						for ( y = y1; y < y2; y++ )
							spZBuffer[x + y * spTargetScanLine] = z;
					SDL_Rect dest;
					dest.w = w;
					dest.h = h;
					dest.x = x1;
					dest.y = y1;
					SDL_Rect src;
					src.x = sx;
					src.y = sy;
					src.w = w;
					src.h = h;
					SDL_SetColorKey( surface, SDL_SRCCOLORKEY, SP_ALPHA_COLOR );
					SDL_BlitSurface( surface, &src, spTarget, &dest );
				}
			}
			else
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_ztest_alpha( x, y, z, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					SDL_Rect dest;
					dest.w = w;
					dest.h = h;
					dest.x = x - addu;
					dest.y = y - addv;
					SDL_Rect src;
					src.x = sx;
					src.y = sy;
					src.w = w;
					src.h = h;
					SDL_SetColorKey( surface, SDL_SRCCOLORKEY, SP_ALPHA_COLOR );
					SDL_BlitSurface( surface, &src, spTarget, &dest );
				}
			}
		}
		else
		{
			if ( spZSet )
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_ztest_zset( x, y, z, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					int u = sx;
					for ( x = x1; x < x2; x++ )
						for ( y = y1; y < y2; y++ )
							spZBuffer[x + y * spTargetScanLine] = z;
					SDL_Rect dest;
					dest.w = w;
					dest.h = h;
					dest.x = x1;
					dest.y = y1;
					SDL_Rect src;
					src.x = sx;
					src.y = sy;
					src.w = w;
					src.h = h;
					SDL_SetColorKey( surface, 0, 0 );
					SDL_BlitSurface( surface, &src, spTarget, &dest );
				}
			}
			else
			{
				if ( spZTest )
				{
					if ( z < 0 )
						return;
					int x1 = x - addu;
					if ( x1 >= spTargetX )
						return;
					if ( x1 < 0 )
					{
						sx -= x1;
						w += x1;
						x1 = 0;
					}
					int x2 = x1 + w;
					if ( x2 <= 0 )
						return;
					if ( x2 > spTargetX )
					{
						w -= x2 - spTargetX;
						x2 = spTargetX;
					}
					int y1 = y - addv;
					if ( y1 >= spTargetY )
						return;
					if ( y1 < 0 )
					{
						sy -= y1;
						h += y1;
						y1 = 0;
					}
					int y2 = y1 + h;
					if ( y2 <= 0 )
						return;
					if ( y2 > spTargetY )
					{
						h -= y2 - spTargetY;
						y2 = spTargetY;
					}
					SDL_Surface* oldTexture = spTexture;
					Sint32 oldTextureScanLine = spTextureScanLine;
					Sint32 oldTextureX = spTextureY;
					Sint32 oldTextureY = spTextureX;
					Sint32 oldTextureXY = spTextureXY;
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					SDL_LockSurface( spTarget );
					int u = sx;
					for ( x = x1; x < x2; x++ )
					{
						int v = sy;
						for ( y = y1; y < y2; y++ )
						{
							draw_pixel_tex_ztest( x, y, z, u, v, 65535 );
							v++;
						}
						u++;
					}
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTextureXY = oldTextureXY;
					spTexturePixel = oldTexturePixel;
					SDL_UnlockSurface( spTarget );
				}
				else
				{
					SDL_Rect dest;
					dest.w = w;
					dest.h = h;
					dest.x = x - addu;
					dest.y = y - addv;
					SDL_Rect src;
					src.x = sx;
					src.y = sy;
					src.w = w;
					src.h = h;
					SDL_SetColorKey( surface, 0, 0 );
					SDL_BlitSurface( surface, &src, spTarget, &dest );
				}
			}
		}
	}
}

PREFIX void spSetZBufferCache( Uint32 value )
{
	if ( spZBufferCache )
		free( spZBufferCache );
	if ( spTargetCache )
		free( spTargetCache );
	if ( spSizeCache )
		free( spSizeCache );
	spZBufferCacheCount = value;
	spZBufferCache = ( Sint32** )malloc( sizeof( Sint32* )*spZBufferCacheCount );
	spTargetCache = ( SDL_Surface** )malloc( sizeof( SDL_Surface* )*spZBufferCacheCount );
	spSizeCache = ( Uint32* )malloc( sizeof( Uint32 ) * spZBufferCacheCount );
	memset( spZBufferCache, 0, sizeof( Sint32* )*spZBufferCacheCount );
	memset( spTargetCache, 0, sizeof( SDL_Surface* )*spZBufferCacheCount );
	memset( spSizeCache, 0, sizeof( Uint32 )*spZBufferCacheCount );
	spZBufferCacheLast = -1;
}

PREFIX void spSetCulling( char value )
{
	spCulling = value;
}

#define spLineSetPixel(x, y, z, color) \
{ \
	if ( spZSet ) \
	{ \
		if ( spZTest ) \
			draw_pixel_ztest_zset( x, y, z, color ) \
		else \
			draw_pixel_zset( x, y, z, color ) \
	} \
	else \
	{ \
		if ( spZTest ) \
			draw_pixel_ztest( x, y, z, color ) \
		else \
			draw_pixel( x, y, color ) \
	} \
}

typedef struct {Sint32 x,y,z;} spInternalLinePoint;
typedef struct {Sint32 x,z;} spInternalLineRedPoint;

inline void spInternalHorizentalLine(Sint32 xl,Sint32 zl,Sint32 xr,Sint32 zr,Sint32 y,Sint32 color)
{
	spInternalLineRedPoint stack[(xr-xl)*2+2];
	stack[0].x = xl;
	stack[0].z = zl;
	stack[1].x = xr;
	stack[1].z = zr;
	int stack_counter = 1;
	while (stack_counter >= 0)
	{
		spInternalLineRedPoint left,right;
		right = stack[stack_counter--];
		left = stack[stack_counter--];
		if (right.x == left.x)
		{}
		else
		if (right.x == left.x + 1)
		{
			if (left.x >= 0 && left.x < spTargetX)
				spLineSetPixel(left.x,y,left.z,color);
		}
		else
		{
			spInternalLineRedPoint point;
			point.x = left.x + right.x >> 1;
			point.z = left.z + right.z >> 1;
			stack[++stack_counter] = left;
			stack[++stack_counter] = point;
			stack[++stack_counter] = point;
			stack[++stack_counter] = right;
		}
	}
}

inline void spInternalLine( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Uint32 color )
{
	spInternalLinePoint stack[abs(y1-y2)*2+2];
	if (y1 < y2)
	{
		stack[0].x = x1;
		stack[0].y = y1;
		stack[0].z = z1;
		stack[1].x = x2;
		stack[1].y = y2;
		stack[1].z = z2;
	}
	else
	{
		stack[0].x = x2;
		stack[0].y = y2;
		stack[0].z = z2;
		stack[1].x = x1;
		stack[1].y = y1;
		stack[1].z = z1;
	}
	int stack_counter = 1;
	//x1 < x2:
	if (stack[0].x < stack[1].x)
		while (stack_counter >= 0)
		{
			spInternalLinePoint left,right;
			right = stack[stack_counter--];
			left = stack[stack_counter--];
			if (right.y == left.y + 1 || right.y == left.y)
			{
				if (left.y >= 0 && left.y < spTargetY)
				{
					if (left.x == right.x)
						spLineSetPixel(left.x,left.y,left.z,color)
					else
						spInternalHorizentalLine(left.x,left.z,right.x,right.z,left.y,color);
				}
			}
			else
			{
				spInternalLinePoint point;
				point.x = left.x + right.x >> 1;
				point.y = left.y + right.y >> 1;
				point.z = left.z + right.z >> 1;
				stack[++stack_counter] = left;
				stack[++stack_counter] = point;
				stack[++stack_counter] = point;
				stack[++stack_counter] = right;
			}
		}
	else
		while (stack_counter >= 0)
		{
			spInternalLinePoint left,right;
			right = stack[stack_counter--];
			left = stack[stack_counter--];
			if (right.y == left.y + 1 || right.y == left.y)
			{
				if (left.y >= 0 && left.y < spTargetY)
				{
					if (left.x == right.x)
						spLineSetPixel(left.x,left.y,left.z,color)
					else
						spInternalHorizentalLine(right.x,right.z,left.x,left.z,left.y,color);
				}
			}
			else
			{
				spInternalLinePoint point;
				point.x = left.x + right.x >> 1;
				point.y = left.y + right.y >> 1;
				point.z = left.z + right.z >> 1;
				stack[++stack_counter] = left;
				stack[++stack_counter] = point;
				stack[++stack_counter] = point;
				stack[++stack_counter] = right;
			}
		}
}

PREFIX void spLine( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Uint32 color )
{
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return;
	if (spZTest && z1 < 0 && z2 < 0)
		return;
#ifndef NEVER_DEFINED
	if ( x1 > x2 )
	{
		Sint32 t = x1;
		x1 = x2;
		x2 = t;
		t = y1;
		y1 = y2;
		y2 = t;
		t = z1;
		z1 = z2;
		z2 = t;
	}

	SDL_LockSurface( spTarget );
	Sint32 dx = abs( x2 - x1 );
	Sint32 dy = abs( y2 - y1 );
	Sint32 div = dx+dy;
	if ( div == 0 ) //x1==x2 and y1==y2
	{
		if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
			draw_pixel_ztest_zset( x1, y1, z1, color )
		SDL_UnlockSurface( spTarget );
		return;
	}
	if ( spUsePattern )
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				//ZSet, ZTest
				Sint32 dz = ( z2 - z1 ) / div;
				Sint32 sx = -1;
				Sint32 sy = -1;
				if ( x1 < x2 )
					sx = 1;
				if ( y1 < y2 )
					sy = 1;
				int err = dx - dy;
				while ( 1 )
				{
					if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
						draw_pixel_ztest_zset_pattern( x1, y1, z1, color )
					if ( x1 == x2 && y1 == y2 )
						break;
					Sint32 e2 = 2 * err;
					if ( e2 > -dy )
					{
						err = err - dy;
						x1 = x1 + sx;
					}
					if ( e2 < dx )
					{
						err = err + dx;
						y1 = y1 + sy;
					}
					z1 += dz;
				}
			}
			else
			{
				//ZSet, No ZTest
				Sint32 dz = ( z2 - z1 ) / div;
				Sint32 sx = -1;
				Sint32 sy = -1;
				if ( x1 < x2 )
					sx = 1;
				if ( y1 < y2 )
					sy = 1;
				int err = dx - dy;
				while ( 1 )
				{
					if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
						draw_pixel_zset_pattern( x1, y1, z1, color )
					if ( x1 == x2 && y1 == y2 )
						break;
					Sint32 e2 = 2 * err;
					if ( e2 > -dy )
					{
						err = err - dy;
						x1 = x1 + sx;
					}
					if ( e2 < dx )
					{
						err = err + dx;
						y1 = y1 + sy;
					}
					z1 += dz;
				}
			}
		}
		else
		{
			if ( spZTest )
			{
				//No ZSet, ZTest
				Sint32 dz = ( z2 - z1 ) / div;
				Sint32 sx = -1;
				Sint32 sy = -1;
				if ( x1 < x2 )
					sx = 1;
				if ( y1 < y2 )
					sy = 1;
				int err = dx - dy;
				while ( 1 )
				{
					if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
						draw_pixel_ztest_pattern( x1, y1, z1, color )
					if ( x1 == x2 && y1 == y2 )
						break;
					Sint32 e2 = 2 * err;
					if ( e2 > -dy )
					{
						err = err - dy;
						x1 = x1 + sx;
					}
					if ( e2 < dx )
					{
						err = err + dx;
						y1 = y1 + sy;
					}
					z1 += dz;
				}
			}
			else
			{
				//No ZSet, No ZTest
				Sint32 sx = -1;
				Sint32 sy = -1;
				if ( x1 < x2 )
					sx = 1;
				if ( y1 < y2 )
					sy = 1;
				int err = dx - dy;
				while ( 1 )
				{
					if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
						draw_pixel_pattern( x1, y1, color )
					if ( x1 == x2 && y1 == y2 )
						break;
					Sint32 e2 = 2 * err;
					if ( e2 > -dy )
					{
						err = err - dy;
						x1 = x1 + sx;
					}
					if ( e2 < dx )
					{
						err = err + dx;
						y1 = y1 + sy;
					}
				}
			}
		}
	}
	else
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				//ZSet, ZTest
				Sint32 dz = ( z2 - z1 ) / div;
				Sint32 sx = -1;
				Sint32 sy = -1;
				if ( x1 < x2 )
					sx = 1;
				if ( y1 < y2 )
					sy = 1;
				int err = dx - dy;
				while ( 1 )
				{
					if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
						draw_pixel_ztest_zset( x1, y1, z1, color )
					if ( x1 == x2 && y1 == y2 )
						break;
					Sint32 e2 = 2 * err;
					if ( e2 > -dy )
					{
						err = err - dy;
						x1 = x1 + sx;
					}
					if ( e2 < dx )
					{
						err = err + dx;
						y1 = y1 + sy;
					}
					z1 += dz;
				}
			}
			else
			{
				//ZSet, No ZTest
				Sint32 dz = ( z2 - z1 ) / div;
				Sint32 sx = -1;
				Sint32 sy = -1;
				if ( x1 < x2 )
					sx = 1;
				if ( y1 < y2 )
					sy = 1;
				int err = dx - dy;
				while ( 1 )
				{
					if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
						draw_pixel_zset( x1, y1, z1, color )
					if ( x1 == x2 && y1 == y2 )
						break;
					Sint32 e2 = 2 * err;
					if ( e2 > -dy )
					{
						err = err - dy;
						x1 = x1 + sx;
					}
					if ( e2 < dx )
					{
						err = err + dx;
						y1 = y1 + sy;
					}
					z1 += dz;
				}
			}
		}
		else
		{
			if ( spZTest )
			{
				//No ZSet, ZTest
				Sint32 dz = ( z2 - z1 ) / div;
				Sint32 sx = -1;
				Sint32 sy = -1;
				if ( x1 < x2 )
					sx = 1;
				if ( y1 < y2 )
					sy = 1;
				int err = dx - dy;
				while ( 1 )
				{
					if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
						draw_pixel_ztest( x1, y1, z1, color )
					if ( x1 == x2 && y1 == y2 )
						break;
					Sint32 e2 = 2 * err;
					if ( e2 > -dy )
					{
						err = err - dy;
						x1 = x1 + sx;
					}
					if ( e2 < dx )
					{
						err = err + dx;
						y1 = y1 + sy;
					}
					z1 += dz;
				}
			}
			else
			{
				//No ZSet, No ZTest
				Sint32 sx = -1;
				Sint32 sy = -1;
				if ( x1 < x2 )
					sx = 1;
				if ( y1 < y2 )
					sy = 1;
				int err = dx - dy;
				while ( 1 )
				{
					if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
						draw_pixel( x1, y1, color )
					if ( x1 == x2 && y1 == y2 )
						break;
					Sint32 e2 = 2 * err;
					if ( e2 > -dy )
					{
						err = err - dy;
						x1 = x1 + sx;
					}
					if ( e2 < dx )
					{
						err = err + dx;
						y1 = y1 + sy;
					}
				}
			}
		}
	}
	SDL_UnlockSurface( spTarget );
	#else
		spInternalLine(x1,y1,z1,x2,y2,z2,color);
	#endif
}

PREFIX void spRectangle( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Uint32 color )
{
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return;
	if (spZTest && z < 0)
		return;
	int addu = w >> 1;
	if ( spHorizontalOrigin == SP_LEFT )
		addu = 0;
	if ( spHorizontalOrigin == SP_RIGHT )
		addu = w - 1;
	int addv = h >> 1;
	if ( spVerticalOrigin == SP_TOP )
		addv = 0;
	if ( spVerticalOrigin == SP_BOTTOM )
		addv = h - 1;
	Sint32 x1 = x - addu;
	Sint32 x2 = x1 + w;
	Sint32 y1 = y - addv;
	Sint32 y2 = y1 + h;
	y = y1;
	if ( spUsePattern )
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				if ( x1 >= spTargetX ) return;
				if ( y1 >= spTargetY ) return;
				if ( x2 < 0 )          return;
				if ( y2 < 0 )          return;
				if ( x2 >= spTargetX ) x2 = spTargetX - 1;
				if ( y2 >= spTargetY ) y2 = spTargetY - 1;
				if ( x1 < 0 )          x1 = 0;
				if ( y1 < 0 )          y1 = 0;
				SDL_LockSurface( spTarget );
				for ( ; x1 <= x2; x1++ )
					for ( y = y1; y <= y2; y++ )
						draw_pixel_ztest_zset_pattern( x1, y, z, color )
			}
			else
			{
				if ( x1 >= spTargetX ) return;
				if ( y1 >= spTargetY ) return;
				if ( x2 < 0 )          return;
				if ( y2 < 0 )          return;
				if ( x2 >= spTargetX ) x2 = spTargetX - 1;
				if ( y2 >= spTargetY ) y2 = spTargetY - 1;
				if ( x1 < 0 )          x1 = 0;
				if ( y1 < 0 )          y1 = 0;
				SDL_LockSurface( spTarget );
				for ( ; x1 <= x2; x1++ )
					for ( y = y1; y <= y2; y++ )
						draw_pixel_zset_pattern( x1, y, z, color )
			}
		}
		else
		{
			if ( spZTest )
			{
				if ( x1 >= spTargetX ) return;
				if ( y1 >= spTargetY ) return;
				if ( x2 < 0 )          return;
				if ( y2 < 0 )          return;
				if ( x2 >= spTargetX ) x2 = spTargetX - 1;
				if ( y2 >= spTargetY ) y2 = spTargetY - 1;
				if ( x1 < 0 )          x1 = 0;
				if ( y1 < 0 )          y1 = 0;
				SDL_LockSurface( spTarget );
				for ( ; x1 <= x2; x1++ )
					for ( y = y1; y <= y2; y++ )
						draw_pixel_ztest_pattern( x1, y, z, color )
			}
			else
			{
				if ( x1 >= spTargetX ) return;
				if ( y1 >= spTargetY ) return;
				if ( x2 < 0 )          return;
				if ( y2 < 0 )          return;
				if ( x2 >= spTargetX ) x2 = spTargetX - 1;
				if ( y2 >= spTargetY ) y2 = spTargetY - 1;
				if ( x1 < 0 )          x1 = 0;
				if ( y1 < 0 )          y1 = 0;
				SDL_LockSurface( spTarget );
				for ( ; x1 <= x2; x1++ )
					for ( y = y1; y <= y2; y++ )
						draw_pixel_pattern( x1, y, color )
			}
		}
	}
	else
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				if ( x1 >= spTargetX ) return;
				if ( y1 >= spTargetY ) return;
				if ( x2 < 0 )          return;
				if ( y2 < 0 )          return;
				if ( x2 >= spTargetX ) x2 = spTargetX - 1;
				if ( y2 >= spTargetY ) y2 = spTargetY - 1;
				if ( x1 < 0 )          x1 = 0;
				if ( y1 < 0 )          y1 = 0;
				SDL_LockSurface( spTarget );
				for ( ; x1 <= x2; x1++ )
					for ( y = y1; y <= y2; y++ )
						draw_pixel_ztest_zset( x1, y, z, color )
			}
			else
			{
				if ( x1 >= spTargetX ) return;
				if ( y1 >= spTargetY ) return;
				if ( x2 < 0 )          return;
				if ( y2 < 0 )          return;
				if ( x2 >= spTargetX ) x2 = spTargetX - 1;
				if ( y2 >= spTargetY ) y2 = spTargetY - 1;
				if ( x1 < 0 )          x1 = 0;
				if ( y1 < 0 )          y1 = 0;
				SDL_LockSurface( spTarget );
				for ( ; x1 <= x2; x1++ )
					for ( y = y1; y <= y2; y++ )
						draw_pixel_zset( x1, y, z, color )
			}
		}
		else
		{
			if ( spZTest )
			{
				if ( x1 >= spTargetX ) return;
				if ( y1 >= spTargetY ) return;
				if ( x2 < 0 )          return;
				if ( y2 < 0 )          return;
				if ( x2 >= spTargetX ) x2 = spTargetX - 1;
				if ( y2 >= spTargetY ) y2 = spTargetY - 1;
				if ( x1 < 0 )          x1 = 0;
				if ( y1 < 0 )          y1 = 0;
				SDL_LockSurface( spTarget );
				for ( ; x1 <= x2; x1++ )
					for ( y = y1; y <= y2; y++ )
						draw_pixel_ztest( x1, y, z, color )
			}
			else
			{
				if ( x1 >= spTargetX ) return;
				if ( y1 >= spTargetY ) return;
				if ( x2 < 0 )          return;
				if ( y2 < 0 )          return;
				if ( x2 >= spTargetX ) x2 = spTargetX - 1;
				if ( y2 >= spTargetY ) y2 = spTargetY - 1;
				if ( x1 < 0 )          x1 = 0;
				if ( y1 < 0 )          y1 = 0;
				SDL_LockSurface( spTarget );
				for ( ; x1 <= x2; x1++ )
					for ( y = y1; y <= y2; y++ )
						draw_pixel( x1, y, color )
			}
		}
	}
	SDL_UnlockSurface( spTarget );
}

PREFIX void spRectangleBorder( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Sint32 bx, Sint32 by, Uint32 color )
{
	if ( ( bx * 2 >= w ) || ( by * 2 >= h ) )
	{
		spRectangle( x, y, z, w, h, color );
		return;
	}
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return;
	if (spZTest && z < 0)
		return;
	int addu = w >> 1;
	if ( spHorizontalOrigin == SP_LEFT )
		addu = 0;
	if ( spHorizontalOrigin == SP_RIGHT )
		addu = w - 1;
	int addv = h >> 1;
	if ( spVerticalOrigin == SP_TOP )
		addv = 0;
	if ( spVerticalOrigin == SP_BOTTOM )
		addv = h - 1;
	Sint32 x1 = x - addu;
	Sint32 x2 = x1 + w;
	Sint32 y1 = y - addv;
	Sint32 y2 = y1 + h;
	Sint32 cy; //counter variable for Y
	Sint32 cx; //counter variable for X
	if ( x1 >= spTargetX ) return;
	if ( y1 >= spTargetY ) return;
	if ( x2 < 0 )          return;
	if ( y2 < 0 )          return;
	if ( x2 >= spTargetX ) x2 = spTargetX - 1;
	if ( y2 >= spTargetY ) y2 = spTargetY - 1;
	if ( x1 < 0 )          x1 = 0;
	if ( y1 < 0 )          y1 = 0;
	SDL_LockSurface( spTarget );
	if ( spUsePattern )
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				//top border
				for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_ztest_zset_pattern( cx, cy, z, color )
				//left & right border
				for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
				{
					for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
						draw_pixel_ztest_zset_pattern( cx, cy, z, color )
					for ( cx = x - addu + w - bx; cx <= x2; cx++ )
						draw_pixel_ztest_zset_pattern( cx, cy, z, color )
				}
				//bottom border
				for ( ; cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_ztest_zset_pattern( cx, cy, z, color )
			}
			else
			{
				//top border
				for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_zset_pattern( cx, cy, z, color )
				//left & right border
				for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
				{
					for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
						draw_pixel_zset_pattern( cx, cy, z, color )
					for ( cx = x - addu + w - bx; cx <= x2; cx++ )
						draw_pixel_zset_pattern( cx, cy, z, color )
				}
				//bottom border
				for ( ; cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_zset_pattern( cx, cy, z, color )
			}
		}
		else
		{
			if ( spZTest )
			{
				//top border
				for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_ztest_pattern( cx, cy, z, color )
				//left & right border
				for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
				{
					for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
						draw_pixel_ztest_pattern( cx, cy, z, color )
					for ( cx = x - addu + w - bx; cx <= x2; cx++ )
						draw_pixel_ztest_pattern( cx, cy, z, color )
				}
				//bottom border
				for ( ; cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_ztest_pattern( cx, cy, z, color )
			}
			else
			{
				//top border
				for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_pattern( cx, cy, color )
				//left & right border
				for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
				{
					for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
						draw_pixel_pattern( cx, cy, color )
					for ( cx = x - addu + w - bx; cx <= x2; cx++ )
						draw_pixel_pattern( cx, cy, color )
				}
				//bottom border
				for ( ; cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_pattern( cx, cy, color )
			}
		}
	}
	else
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				//top border
				for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_ztest_zset( cx, cy, z, color )
				//left & right border
				for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
				{
					for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
						draw_pixel_ztest_zset( cx, cy, z, color )
					for ( cx = x - addu + w - bx; cx <= x2; cx++ )
						draw_pixel_ztest_zset( cx, cy, z, color )
				}
				//bottom border
				for ( ; cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_ztest_zset( cx, cy, z, color )
			}
			else
			{
				//top border
				for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_zset( cx, cy, z, color )
				//left & right border
				for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
				{
					for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
						draw_pixel_zset( cx, cy, z, color )
					for ( cx = x - addu + w - bx; cx <= x2; cx++ )
						draw_pixel_zset( cx, cy, z, color )
				}
				//bottom border
				for ( ; cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_zset( cx, cy, z, color )
			}
		}
		else
		{
			if ( spZTest )
			{
				//top border
				for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_ztest( cx, cy, z, color )
				//left & right border
				for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
				{
					for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
						draw_pixel_ztest( cx, cy, z, color )
					for ( cx = x - addu + w - bx; cx <= x2; cx++ )
						draw_pixel_ztest( cx, cy, z, color )
				}
				//bottom border
				for ( ; cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel_ztest( cx, cy, z, color )
			}
			else
			{
				//top border
				for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel( cx, cy, color )
				//left & right border
				for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
				{
					for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
						draw_pixel( cx, cy, color )
					for ( cx = x - addu + w - bx; cx <= x2; cx++ )
						draw_pixel( cx, cy, color )
				}
				//bottom border
				for ( ; cy <= y2; cy++ )
					for ( cx = x1; cx <= x2; cx++ )
						draw_pixel( cx, cy, color )
			}
		}
	}
	SDL_UnlockSurface( spTarget );
}

PREFIX void spEllipse( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Uint32 color )

{
	if (rx <= 0)
	  return;
	if (ry <= 0)
	  return;
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return;
	if (spZTest && z < 0)
		return;
	if ( x - rx >= spTargetX ) return;
	if ( y - ry >= spTargetY ) return;
	if ( x + rx < 0 )          return;
	if ( y + ry < 0 )          return;
	Sint32 rxl = -rx;
	Sint32 rxr = +rx;
	Sint32 ryl = -ry;
	Sint32 ryr = +ry;
	if ( x + rxr >= spTargetX ) rxr = spTargetX - 1 - x;
	if ( y + ryr >= spTargetY ) ryr = spTargetY - 1 - y;
	if ( x + rxl < 0 )          rxl = -x;
	if ( y + ryl < 0 )          ryl = -y;
	Sint32 x1 = x;
	Sint32 y1 = y;
	SDL_LockSurface( spTarget );
	if ( spUsePattern )
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				Sint32 factor = one_over_x(ry);
				for ( y = ryl; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest_zset_pattern( x1 + x, y1 + y, z, color )
				}
			}
			else
			{
				Sint32 factor = one_over_x(ry);
				for ( y = ryl; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_zset_pattern( x1 + x, y1 + y, z, color )
				}
			}
		}
		else
		{
			if ( spZTest )
			{
				Sint32 factor = one_over_x(ry);
				for ( y = ryl; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest_pattern( x1 + x, y1 + y, z, color )
				}
			}
			else
			{
				Sint32 factor = one_over_x(ry);
				for ( y = ryl; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_pattern( x1 + x, y1 + y, color )
				}
			}
		}
	}
	else
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				Sint32 factor = one_over_x(ry);
				for ( y = ryl; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest_zset( x1 + x, y1 + y, z, color )
				}
			}
			else
			{
				Sint32 factor = one_over_x(ry);
				for ( y = ryl; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_zset( x1 + x, y1 + y, z, color )
				}
			}
		}
		else
		{
			if ( spZTest )
			{
				Sint32 factor = one_over_x(ry);
				for ( y = ryl; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest( x1 + x, y1 + y, z, color )
				}
			}
			else
			{
				Sint32 factor = one_over_x(ry);
				for ( y = ryl; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel( x1 + x, y1 + y, color )
				}
			}
		}
	}
	SDL_UnlockSurface( spTarget );
}

PREFIX void spEllipseBorder( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Sint32 bx, Sint32 by, Uint32 color )
{
	if (bx <= 0)
	  return;
	if (by <= 0)
	  return;
	if ( ( bx >= rx ) || ( by >= ry ) )
	{
		spEllipse( x, y, z, rx, ry, color );
		return;
	}
	if (rx <= 0)
	  return;
	if (ry <= 0)
	  return;
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return;
	if (spZTest && z < 0)
		return;

	if ( x - rx >= spTargetX ) return;
	if ( y - ry >= spTargetY ) return;
	if ( x + rx < 0 )          return;
	if ( y + ry < 0 )          return;
	Sint32 rxl = -rx;
	Sint32 rxr = +rx;
	Sint32 ryl = -ry;
	Sint32 ryr = +ry;
	Sint32 rxlb = -rx + bx;
	Sint32 rxrb = +rx - bx;
	Sint32 rylb = -ry + by;
	Sint32 ryrb = +ry - by;
	Sint32 RX = rx * rx;
	Sint32 RY = ry * ry;
	Sint32 RR = RX * RY;
	Sint32 RXB = ( rx - bx ) * ( rx - bx );
	Sint32 RYB = ( ry - by ) * ( ry - by );
	Sint32 RRB = RXB * RYB;
	if ( x + rxr >= spTargetX ) rxr = spTargetX - 1 - x;
	if ( y + ryr >= spTargetY ) ryr = spTargetY - 1 - y;
	if ( x + rxl < 0 )          rxl = -x;
	if ( y + ryl < 0 )          ryl = -y;
	if ( x + rxrb >= spTargetX ) rxrb = spTargetX - 1 - x;
	if ( y + ryrb >= spTargetY ) ryrb = spTargetY - 1 - y;
	if ( x + rxlb < 0 )          rxlb = -x;
	if ( y + rylb < 0 )          rylb = -y;
	Sint32 x1 = x;
	Sint32 y1 = y;
	Sint32 XX_mul = spDiv( rx * rx << SP_ACCURACY, ry * ry << SP_ACCURACY );
	Sint32 XXB_mul = spDiv( ( rx - bx ) * ( rx - bx ) << SP_ACCURACY, ( ry - by ) * ( ry - by ) << SP_ACCURACY );
	SDL_LockSurface( spTarget );
	if ( spUsePattern )
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				Sint32 factor_out = one_over_x(ry);
				Sint32 factor_in  = one_over_x(ry-by);
				//up
				for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest_zset_pattern( x1 + x, y1 + y, z, color )
				}
				//middle
				for (; y < ry - by && y <= ryr; y++ )
				{
					Sint32 angel_out = spAsin(y*factor_out);
					Sint32 angel_in  = spAsin(y*factor_in);
					Sint32 RX_in  = spCos(angel_in)*(rx-bx) >> SP_ACCURACY;
					Sint32 RX_out = spCos(angel_out)*rx >> SP_ACCURACY;
					Sint32 LX_in  = -RX_in;
					Sint32 LX_out = -RX_out;
					if (LX_in < rxl)
						LX_in = rxl;
					if (LX_out < rxl)
						LX_out = rxl;
					if (RX_in > rxr)
						RX_in = rxr;
					if (RX_out > rxr)
						RX_out = rxr;
					for (x = LX_out;x < LX_in; x++)
						draw_pixel_ztest_zset_pattern( x1 + x, y1 + y, z, color )
					for (x = RX_in;x < RX_out; x++)
						draw_pixel_ztest_zset_pattern( x1 + x, y1 + y, z, color )
				}
				//down
				for (; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest_zset_pattern( x1 + x, y1 + y, z, color )
				}
			}
			else
			{
				Sint32 factor_out = one_over_x(ry);
				Sint32 factor_in  = one_over_x(ry-by);
				//up
				for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_zset_pattern( x1 + x, y1 + y, z, color )
				}
				//middle
				for (; y < ry - by && y <= ryr; y++ )
				{
					Sint32 angel_out = spAsin(y*factor_out);
					Sint32 angel_in  = spAsin(y*factor_in);
					Sint32 RX_in  = spCos(angel_in)*(rx-bx) >> SP_ACCURACY;
					Sint32 RX_out = spCos(angel_out)*rx >> SP_ACCURACY;
					Sint32 LX_in  = -RX_in;
					Sint32 LX_out = -RX_out;
					if (LX_in < rxl)
						LX_in = rxl;
					if (LX_out < rxl)
						LX_out = rxl;
					if (RX_in > rxr)
						RX_in = rxr;
					if (RX_out > rxr)
						RX_out = rxr;
					for (x = LX_out;x < LX_in; x++)
						draw_pixel_zset_pattern( x1 + x, y1 + y, z, color )
					for (x = RX_in;x < RX_out; x++)
						draw_pixel_zset_pattern( x1 + x, y1 + y, z, color )
				}
				//down
				for (; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_zset_pattern( x1 + x, y1 + y, z, color )
				}
			}
		}
		else
		{
			if ( spZTest )
			{
				Sint32 factor_out = one_over_x(ry);
				Sint32 factor_in  = one_over_x(ry-by);
				//up
				for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest_pattern( x1 + x, y1 + y, z, color )
				}
				//middle
				for (; y < ry - by && y <= ryr; y++ )
				{
					Sint32 angel_out = spAsin(y*factor_out);
					Sint32 angel_in  = spAsin(y*factor_in);
					Sint32 RX_in  = spCos(angel_in)*(rx-bx) >> SP_ACCURACY;
					Sint32 RX_out = spCos(angel_out)*rx >> SP_ACCURACY;
					Sint32 LX_in  = -RX_in;
					Sint32 LX_out = -RX_out;
					if (LX_in < rxl)
						LX_in = rxl;
					if (LX_out < rxl)
						LX_out = rxl;
					if (RX_in > rxr)
						RX_in = rxr;
					if (RX_out > rxr)
						RX_out = rxr;
					for (x = LX_out;x < LX_in; x++)
						draw_pixel_ztest_pattern( x1 + x, y1 + y, z, color )
					for (x = RX_in;x < RX_out; x++)
						draw_pixel_ztest_pattern( x1 + x, y1 + y, z, color )
				}
				//down
				for (; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest_pattern( x1 + x, y1 + y, z, color )
				}
			}
			else
			{
				Sint32 factor_out = one_over_x(ry);
				Sint32 factor_in  = one_over_x(ry-by);
				//up
				for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_pattern( x1 + x, y1 + y, color )
				}
				//middle
				for (; y < ry - by && y <= ryr; y++ )
				{
					Sint32 angel_out = spAsin(y*factor_out);
					Sint32 angel_in  = spAsin(y*factor_in);
					Sint32 RX_in  = spCos(angel_in)*(rx-bx) >> SP_ACCURACY;
					Sint32 RX_out = spCos(angel_out)*rx >> SP_ACCURACY;
					Sint32 LX_in  = -RX_in;
					Sint32 LX_out = -RX_out;
					if (LX_in < rxl)
						LX_in = rxl;
					if (LX_out < rxl)
						LX_out = rxl;
					if (RX_in > rxr)
						RX_in = rxr;
					if (RX_out > rxr)
						RX_out = rxr;
					for (x = LX_out;x < LX_in; x++)
						draw_pixel_pattern( x1 + x, y1 + y, color )
					for (x = RX_in;x < RX_out; x++)
						draw_pixel_pattern( x1 + x, y1 + y, color )
				}
				//down
				for (; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_pattern( x1 + x, y1 + y, color )
				}
			}
		}
	}
	else
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				Sint32 factor_out = one_over_x(ry);
				Sint32 factor_in  = one_over_x(ry-by);
				//up
				for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest_zset( x1 + x, y1 + y, z, color )
				}
				//middle
				for (; y < ry - by && y <= ryr; y++ )
				{
					Sint32 angel_out = spAsin(y*factor_out);
					Sint32 angel_in  = spAsin(y*factor_in);
					Sint32 RX_in  = spCos(angel_in)*(rx-bx) >> SP_ACCURACY;
					Sint32 RX_out = spCos(angel_out)*rx >> SP_ACCURACY;
					Sint32 LX_in  = -RX_in;
					Sint32 LX_out = -RX_out;
					if (LX_in < rxl)
						LX_in = rxl;
					if (LX_out < rxl)
						LX_out = rxl;
					if (RX_in > rxr)
						RX_in = rxr;
					if (RX_out > rxr)
						RX_out = rxr;
					for (x = LX_out;x < LX_in; x++)
						draw_pixel_ztest_zset( x1 + x, y1 + y, z, color )
					for (x = RX_in;x < RX_out; x++)
						draw_pixel_ztest_zset( x1 + x, y1 + y, z, color )
				}
				//down
				for (; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest_zset( x1 + x, y1 + y, z, color )
				}
			}
			else
			{
				Sint32 factor_out = one_over_x(ry);
				Sint32 factor_in  = one_over_x(ry-by);
				//up
				for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_zset( x1 + x, y1 + y, z, color )
				}
				//middle
				for (; y < ry - by && y <= ryr; y++ )
				{
					Sint32 angel_out = spAsin(y*factor_out);
					Sint32 angel_in  = spAsin(y*factor_in);
					Sint32 RX_in  = spCos(angel_in)*(rx-bx) >> SP_ACCURACY;
					Sint32 RX_out = spCos(angel_out)*rx >> SP_ACCURACY;
					Sint32 LX_in  = -RX_in;
					Sint32 LX_out = -RX_out;
					if (LX_in < rxl)
						LX_in = rxl;
					if (LX_out < rxl)
						LX_out = rxl;
					if (RX_in > rxr)
						RX_in = rxr;
					if (RX_out > rxr)
						RX_out = rxr;
					for (x = LX_out;x < LX_in; x++)
						draw_pixel_zset( x1 + x, y1 + y, z, color )
					for (x = RX_in;x < RX_out; x++)
						draw_pixel_zset( x1 + x, y1 + y, z, color )
				}
				//down
				for (; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_zset( x1 + x, y1 + y, z, color )
				}
			}
		}
		else
		{
			if ( spZTest )
			{
				Sint32 factor_out = one_over_x(ry);
				Sint32 factor_in  = one_over_x(ry-by);
				//up
				for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest( x1 + x, y1 + y, z, color )
				}
				//middle
				for (; y < ry - by && y <= ryr; y++ )
				{
					Sint32 angel_out = spAsin(y*factor_out);
					Sint32 angel_in  = spAsin(y*factor_in);
					Sint32 RX_in  = spCos(angel_in)*(rx-bx) >> SP_ACCURACY;
					Sint32 RX_out = spCos(angel_out)*rx >> SP_ACCURACY;
					Sint32 LX_in  = -RX_in;
					Sint32 LX_out = -RX_out;
					if (LX_in < rxl)
						LX_in = rxl;
					if (LX_out < rxl)
						LX_out = rxl;
					if (RX_in > rxr)
						RX_in = rxr;
					if (RX_out > rxr)
						RX_out = rxr;
					for (x = LX_out;x < LX_in; x++)
						draw_pixel_ztest( x1 + x, y1 + y, z, color )
					for (x = RX_in;x < RX_out; x++)
						draw_pixel_ztest( x1 + x, y1 + y, z, color )
				}
				//down
				for (; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel_ztest( x1 + x, y1 + y, z, color )
				}
			}
			else
			{
				Sint32 factor_out = one_over_x(ry);
				Sint32 factor_in  = one_over_x(ry-by);
				//up
				for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel( x1 + x, y1 + y, color )
				}
				//middle
				for (; y < ry - by && y <= ryr; y++ )
				{
					Sint32 angel_out = spAsin(y*factor_out);
					Sint32 angel_in  = spAsin(y*factor_in);
					Sint32 RX_in  = spCos(angel_in)*(rx-bx) >> SP_ACCURACY;
					Sint32 RX_out = spCos(angel_out)*rx >> SP_ACCURACY;
					Sint32 LX_in  = -RX_in;
					Sint32 LX_out = -RX_out;
					if (LX_in < rxl)
						LX_in = rxl;
					if (LX_out < rxl)
						LX_out = rxl;
					if (RX_in > rxr)
						RX_in = rxr;
					if (RX_out > rxr)
						RX_out = rxr;
					for (x = LX_out;x < LX_in; x++)
						draw_pixel( x1 + x, y1 + y, color )
					for (x = RX_in;x < RX_out; x++)
						draw_pixel( x1 + x, y1 + y, color )
				}
				//down
				for (; y <= ryr; y++ )
				{
					Sint32 angel = spAsin(y*factor_out);
					Sint32 RX = spCos(angel)*rx >> SP_ACCURACY;
					Sint32 LX = -RX;
					if (LX < rxl)
						LX = rxl;
					if (RX > rxr)
						RX = rxr;
					for (x = LX;x <= RX; x++)
						draw_pixel( x1 + x, y1 + y, color )
				}
			}
		}
	}
	SDL_UnlockSurface( spTarget );

}

PREFIX int spGetPixelPosition( Sint32 x, Sint32 y )
{
	if ( x < 0 )
	{
		if ( y < 0 )
			return 4;
		if ( y >= spTargetY )
			return 256;
		return 2;
	}
	if ( x >= spTargetX )
	{
		if ( y < 0 )
			return 16;
		if ( y >= spTargetY )
			return 64;
		return 32;
	}
	if ( y < 0 )
		return 8;
	if ( y >= spTargetY )
		return 128;
	return 1;
}

PREFIX void spRotozoomSurface( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 zoomX, Sint32 zoomY, Sint32 angle )
{
	spRotozoomSurfacePart( x, y, z, surface, 0, 0, surface->w, surface->h, zoomX, zoomY, angle );
}

inline void sp_intern_Triangle_tex_inter( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3 )
{
	if ( y1 > y2 )
	{
		Sint32 temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
		temp = z1;
		z1 = z2;
		z2 = temp;
		temp = u1;
		u1 = u2;
		u2 = temp;
		temp = v1;
		v1 = v2;
		v2 = temp;
	}
	if ( y1 > y3 )
	{
		Sint32 temp = x1;
		x1 = x3;
		x3 = temp;
		temp = y1;
		y1 = y3;
		y3 = temp;
		temp = z1;
		z1 = z3;
		z3 = temp;
		temp = u1;
		u1 = u3;
		u3 = temp;
		temp = v1;
		v1 = v3;
		v3 = temp;
	}
	if ( y2 < y3 )
	{
		Sint32 temp = x2;
		x2 = x3;
		x3 = temp;
		temp = y2;
		y2 = y3;
		y3 = temp;
		temp = z2;
		z2 = z3;
		z3 = temp;
		temp = u2;
		u2 = u3;
		u3 = temp;
		temp = v2;
		v2 = v3;
		v3 = temp;
	}
	if ( spUsePattern )
	{
		if ( spAlphaTest )
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset_alpha_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
				else
					sp_intern_Triangle_tex_zset_alpha_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_alpha_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
				else
					sp_intern_Triangle_tex_alpha_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535 );
			}
		}
		else
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
				else
					sp_intern_Triangle_tex_zset_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
				else
					sp_intern_Triangle_tex_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535 );
			}
		}
	}
	else
	{
		if ( spAlphaTest )
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset_alpha( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
				else
					sp_intern_Triangle_tex_zset_alpha      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_alpha     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
				else
					sp_intern_Triangle_tex_alpha           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535 );
			}
		}
		else
		{
			if ( spZSet )
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest_zset( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
				else
					sp_intern_Triangle_tex_zset      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
			}
			else
			{
				if ( spZTest )
					sp_intern_Triangle_tex_ztest     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
				else
					sp_intern_Triangle_tex           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535 );
			}
		}
	}
}

PREFIX void spRotozoomSurfacePart( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, Sint32 zoomX, Sint32 zoomY, Sint32 angle )
{
	if (zoomX == SP_ONE && zoomY == SP_ONE && angle == 0)
	{
		spBlitSurfacePart(x,y,z,surface,sx,sy,w,h);
		return;
	}
	Sint32 x1;
	Sint32 x2;
	Sint32 x3;
	Sint32 x4;
	Sint32 y1;
	Sint32 y2;
	Sint32 y3;
	Sint32 y4;

	switch ( spHorizontalOrigin )
	{
	case SP_CENTER:
		x1 = -( w * zoomX >> SP_ACCURACY + 1 );
		x2 = -( w * zoomX >> SP_ACCURACY + 1 );
		x3 = +( w * zoomX >> SP_ACCURACY + 1 );
		x4 = +( w * zoomX >> SP_ACCURACY + 1 );
		break;
	case SP_LEFT:
		x1 = -0;
		x2 = -0;
		x3 = +( w * zoomX >> SP_ACCURACY );
		x4 = +( w * zoomX >> SP_ACCURACY );
		break;
	case SP_RIGHT:
		x1 = -( w * zoomX >> SP_ACCURACY );
		x2 = -( w * zoomX >> SP_ACCURACY );
		x3 = +0;
		x4 = +0;
		break;
	}
	switch ( spVerticalOrigin )
	{
	case SP_CENTER:
		y1 = -( h * zoomY >> SP_ACCURACY + 1 );
		y2 = +( h * zoomY >> SP_ACCURACY + 1 );
		y3 = +( h * zoomY >> SP_ACCURACY + 1 );
		y4 = -( h * zoomY >> SP_ACCURACY + 1 );
		break;
	case SP_TOP:
		y1 = -0;
		y2 = +( h * zoomY >> SP_ACCURACY );
		y3 = +( h * zoomY >> SP_ACCURACY );
		y4 = -0;
		break;
	case SP_BOTTOM:
		y1 = -( h * zoomY >> SP_ACCURACY );
		y2 = +0;
		y3 = +0;
		y4 = -( h * zoomY >> SP_ACCURACY );
		break;
	}

	Sint32 nx1 = x + ( x1 * spCos( angle ) - y1 * spSin( angle ) >> SP_ACCURACY );
	Sint32 ny1 = y + ( y1 * spCos( angle ) + x1 * spSin( angle ) >> SP_ACCURACY );
	Sint32 nx2 = x + ( x2 * spCos( angle ) - y2 * spSin( angle ) >> SP_ACCURACY );
	Sint32 ny2 = y + ( y2 * spCos( angle ) + x2 * spSin( angle ) >> SP_ACCURACY );
	Sint32 nx3 = x + ( x3 * spCos( angle ) - y3 * spSin( angle ) >> SP_ACCURACY );
	Sint32 ny3 = y + ( y3 * spCos( angle ) + x3 * spSin( angle ) >> SP_ACCURACY );
	Sint32 nx4 = x + ( x4 * spCos( angle ) - y4 * spSin( angle ) >> SP_ACCURACY );
	Sint32 ny4 = y + ( y4 * spCos( angle ) + x4 * spSin( angle ) >> SP_ACCURACY );

	SDL_Surface* oldTexture = spTexture;
	Sint32 oldTextureScanLine = spTextureScanLine;
	Sint32 oldTextureX = spTextureY;
	Sint32 oldTextureY = spTextureX;
	Sint32 oldTextureXY = spTextureXY;
	Uint16* oldTexturePixel = spTexturePixel;
	spBindTexture( surface );
	Sint32 mx = nx1 + nx2 + nx3 + nx4 >> 2;
	Sint32 my = ny1 + ny2 + ny3 + ny4 >> 2;
	Sint32 mu = sx + ( w >> 1 );
	Sint32 mv = sy + ( h >> 1 );
	sp_intern_Triangle_tex_inter( nx1, ny1, z, sx  , sy  ,
								  nx2, ny2, z, sx  , sy + h,
								  mx , my , z, mu  , mv  );
	sp_intern_Triangle_tex_inter( nx2, ny2, z, sx  , sy + h,
								  nx3, ny3, z, sx + w, sy + h,
								  mx , my , z, mu  , mv  );
	sp_intern_Triangle_tex_inter( nx3, ny3, z, sx + w, sy + h,
								  nx4, ny4, z, sx + w, sy  ,
								  mx , my , z, mu  , mv  );
	sp_intern_Triangle_tex_inter( nx4, ny4, z, sx + w, sy  ,
								  nx1, ny1, z, sx  , sy  ,
								  mx , my , z, mu  , mv  );
	spTexture = oldTexture;
	spTextureScanLine = oldTextureScanLine;
	spTextureX = oldTextureY;
	spTextureY = oldTextureX;
	spTextureXY = oldTextureXY;
	spTexturePixel = oldTexturePixel;
}

PREFIX void spSetHorizontalOrigin( Sint32 origin )
{
	spHorizontalOrigin = origin;
}

PREFIX void spSetVerticalOrigin( Sint32 origin )
{
	spVerticalOrigin = origin;
}

PREFIX Sint32 spGetHorizontalOrigin()
{
	return spHorizontalOrigin;
}

PREFIX Sint32 spGetVerticalOrigin()
{
	return spVerticalOrigin;
}

int log_2(int x)
{
	int mom = 1 << 31;
	int l = 31;
	while ((x & mom) == 0)
	{
		mom >>= 1;
		l--;
	}
	return l;
}

PREFIX void spSetZFar(Sint32 zfar)
{
	spZFar = zfar;
	Sint32 x,y,z,w;
	spProjectPoint3D(0,0,-zfar,&x,&y,&z,&w,1);
	int l = log_2(w)+1;
	spMaxWLogDiff = l - SP_ACCURACY;
}

PREFIX Sint32 spGetZFar()
{
	return spZFar;
}

PREFIX void spSetZNear(Sint32 znear)
{
	spZNear = znear;
}

PREFIX Sint32 spGetZNear()
{
	return spZNear;
}

PREFIX void spAddWhiteLayer(int alpha)
{
  int i;
  int goal = spTargetScanLine*spTargetY;
  for (i=0;i<goal;i++)
  {
    int r=( spTargetPixel[i]>>11     )*8+alpha;
    if (r>255)
      r=255;
    if (r<0)
      r=0;
    int g=((spTargetPixel[i]>>5) & 63)*4+alpha;
    if (g>255)
      g=255;
    if (g<0)
      g=0;
    int b=( spTargetPixel[i]     & 31)*8+alpha;
    if (b>255)
      b=255;
    if (b<0)
      b=0;
    spTargetPixel[i]=spGetFastRGB(r,g,b);
  }

}

PREFIX void spAddBlackLayer(int alpha)
{
  int i;
  int goal = spTargetScanLine*spTargetY;
  for (i=0;i<goal;i++)
  {
    int r=( spTargetPixel[i]>>11     )*8-alpha;
    if (r>255)
      r=255;
    if (r<0)
      r=0;
    int g=((spTargetPixel[i]>>5) & 63)*4-alpha;
    if (g>255)
      g=255;
    if (g<0)
      g=0;
    int b=( spTargetPixel[i]     & 31)*8-alpha;
    if (b>255)
      b=255;
    if (b<0)
      b=0;
    spTargetPixel[i]=spGetFastRGB(r,g,b);
  }

}

PREFIX void spSetPattern32(Uint32 first_32_bit,Uint32 last_32_bit)
{
	spPattern[0] = first_32_bit >> 24;
	spPattern[1] = first_32_bit >> 16;
	spPattern[2] = first_32_bit >>  8;
	spPattern[3] = first_32_bit      ;
	spPattern[4] = last_32_bit >> 24;
	spPattern[5] = last_32_bit >> 16;
	spPattern[6] = last_32_bit >>  8;
	spPattern[7] = last_32_bit      ;
	spUsePattern = (first_32_bit != ((Uint32)0xFFFFFFFF)) || (last_32_bit != ((Uint32)0xFFFFFFFF));
}

PREFIX void spSetPattern64(Uint64 pattern)
{
	spPattern[0] = pattern >> 56;
	spPattern[1] = pattern >> 48;
	spPattern[2] = pattern >> 40;
	spPattern[3] = pattern >> 32;
	spPattern[4] = pattern >> 24;
	spPattern[5] = pattern >> 16;
	spPattern[6] = pattern >>  8;
	spPattern[7] = pattern      ;
	spUsePattern = pattern != ((Uint64)0xFFFFFFFFFFFFFFFF);
}

PREFIX void spSetPattern8(Uint8 line1,Uint8 line2,Uint8 line3,Uint8 line4,Uint8 line5,Uint8 line6,Uint8 line7,Uint8 line8)
{
	spPattern[0] = line1;
	spPattern[1] = line2;
	spPattern[2] = line3;
	spPattern[3] = line4;
	spPattern[4] = line5;
	spPattern[5] = line6;
	spPattern[6] = line7;
	spPattern[7] = line8;
	spUsePattern = line1 != 255 || line2 != 255 || line3 != 255 ||
	               line4 != 255 || line5 != 255 || line6 != 255 ||
	               line7 != 255 || line8 != 255;

}

PREFIX void spDeactivatePattern()
{
	spUsePattern = 0;
}

PREFIX void spSetAlphaPattern(int alpha,int shift)
{
	alpha = alpha + 3 >> 2; //alpha = (alpha+3) / 4;
	//now alpha is the count of bits, that should be set.
	int pos = shift & 63; //pos = shift % 64;
	int x,y;
	int lastPos = -1;
	int addend = alpha;
	for (y = 0; y < 8; y++)
	{
		if (lastPos == pos)
		{
			pos+=addend;
			addend = -addend;
		}
		lastPos = pos;
		spPattern[y] = 0;
		for (x = 0; x < 8; x++)
		{
			pos += alpha;
			if (pos >= 64)
			{
				pos &= 63; //pos = pos % 64;
				spPattern[y] |= 1 << x; //Setting bit
			}
		}
	}
}

#define ringshift(left,shift) ((left << shift) | (left >> 32-shift))

PREFIX void spSetAlphaPattern4x4(int alpha,int shift)
{
	alpha = alpha + 3 >> 4; //alpha = (alpha+3) / 16;
	//now alpha is the count of bits, that should be set.
	Uint32 left;
	switch (alpha)
	{
		case 0:
			left = 0;
			break;
		case 1:
			/* 0000 0000
			 * 0100 0100
			 * 0000 0000
			 * 0000 0000 */
			left = 4456448;
			break;
		case 2:
			/* 0000 0000
			 * 0100 0100
			 * 0000 0000
			 * 0001 0001 */
			left = 4456465;
			break;
		case 3:
			/* 0000 0000
			 * 0100 0100
			 * 0000 0000
			 * 0101 0101 */
			left = 4456533;
			break;
		case 4:
			/* 0000 0000
			 * 0101 0101
			 * 0000 0000
			 * 0101 0101 */
			left = 5570645;
			break;
		case 5:
			/* 1000 1000
			 * 0101 0101
			 * 0000 0000
			 * 0101 0101 */
			left = 2287272021;
			break;
		case 6:
			/* 1000 1000
			 * 0101 0101
			 * 0010 0010
			 * 0101 0101 */
			left = 2287280725;
			break;
		case 7:
			/* 1000 1000
			 * 0101 0101
			 * 1010 1010
			 * 0101 0101 */
			left = 2287315541;
			break;
		case 8:
			/* 1010 1010
			 * 0101 0101
			 * 1010 1010
			 * 0101 0101 */
			left = 2857740885;
			break;
		case 9:
			/* 1010 1010
			 * 0101 0101
			 * 1110 1110
			 * 0101 0101 */
			left = 2857758293;
			break;
		case 10:
			/* 1011 1011
			 * 0101 0101
			 * 1110 1110
			 * 0101 0101 */
			left = 3142970965;
			break;
		case 11:
			/* 1011 1011
			 * 0101 0101
			 * 1111 1111
			 * 0101 0101 */
			left = 3142975317;
			break;
		case 12:
			/* 1111 1111
			 * 0101 0101
			 * 1111 1111
			 * 0101 0101 */
			left = 4283826005;
			break;
		case 13:
			/* 1111 1111
			 * 0101 0101
			 * 1111 1111
			 * 1101 1101 */
			left = 4283826141;
			break;
		case 14:
			/* 1111 1111
			 * 0111 0111
			 * 1111 1111
			 * 1101 1101 */
			left = 4286054365;
			break;
		case 15:
			/* 1111 1111
			 * 0111 0111
			 * 1111 1111
			 * 1111 1111 */
			left = 4286054399;
		case 16:
			/* 1111 1111
			 * 1111 1111
			 * 1111 1111
			 * 1111 1111 */
			left = 4294967295;
			break;
	}
	shift = shift & 15;
	left = ringshift(left,shift);
	spSetPattern32(left,left);
}
