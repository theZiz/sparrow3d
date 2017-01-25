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

#include "sparrowPrimitives.h"
#ifdef PANDORA
	#include <arm_neon.h>
#endif
#include <stdlib.h>
#include <string.h>
SDL_Surface* spTarget = NULL;
//Uint16* spTargetPixel = NULL; GP2X
SDL_Surface* spTexture = NULL;
Uint16* spTexturePixel = NULL;
Uint32 spZTest = 1;
Uint32 spZSet = 1;
Uint32 spAlphaTest = 1;
Uint32 spQuadQuali = 0;
//Sint32* spZBuffer = NULL; GP2X
Sint32 spZFar = -6553600; //-100.0f
//Sint32 spMaxWLogDiff = 3; GP2X
Sint32 spZNear = -7000; //-0.1f
/*Sint32 spTargetScanLine = 0; //if the surface is even, same as spTargetX, else +1.
Sint32 spTargetX = 0;
Sint32 spTargetY = 0; GP2X*/
Sint32 spTextureX = 0;
Sint32 spTextureScanLine = 0; //if the surface is even, same as spTextureX, else +1.
Sint32 spTextureY = 0;
Sint32 spOne_over_x_look_up[1 << SP_ACCURACY];
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
Sint32 spBlending = SP_ONE;
Sint32 spUseParallelProcess = 0;
Sint32 spLineWidth = 1;
Sint32 spBlendingPatternEmulation = 1;
int spFixedX = 0, spFixedY = 0;
//Sint32* spOne_over_x_look_up_fixed; GP2X

#include "sparrowPrimitiveDrawingThread.c"

#define setup_stack_struct(pos_,mode_) \
	spScanLineCache[(pos_)].mode = (mode_); \
	spScanLineCache[(pos_)].texturePixel = spTexturePixel; \
	spScanLineCache[(pos_)].textureScanLine = spTextureScanLine; \
	spScanLineCache[(pos_)].textureX = spTextureX; \
	spScanLineCache[(pos_)].textureY = spTextureY; \
	spScanLineCache[(pos_)].zTest = spZTest; \
	spScanLineCache[(pos_)].zSet = spZSet; \
	spScanLineCache[(pos_)].alphaTest = spAlphaTest; \
	spScanLineCache[(pos_)].usePattern = spUsePattern; \
	{ \
		int i; \
		for (i = 0; i < 8; i++) \
			spScanLineCache[(pos_)].pattern[i] = spPattern[i]; \
	} \
	spScanLineCache[(pos_)].blending = spBlending;

#ifndef SP_MAX_SCANLINE_NO_CHECK
#define CIRCLE_ON_FULL_STACK \
	while (1) \
	{ \
		SDL_mutexP(spScanLineMutex); \
		if (((spScanLineEnd+1) & SP_MAX_SCANLINES_MOD) != spScanLineBegin) \
			break; \
		SDL_mutexV(spScanLineMutex); \
		spSleep(SP_MAX_SCANLINES_WAIT_TIME); \
	} \
	SDL_mutexV(spScanLineMutex);
#else
#define CIRCLE_ON_FULL_STACK {}
#endif

inline void sp_intern_Triangle_overlord( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color )
{
	//Adding to stack if not full!
	CIRCLE_ON_FULL_STACK
	setup_stack_struct(spScanLineEnd,0)
	spScanLineCache[spScanLineEnd].primitive.triangle.x1 = x1;
	spScanLineCache[spScanLineEnd].primitive.triangle.y1 = y1;
	spScanLineCache[spScanLineEnd].primitive.triangle.z1 = z1;
	spScanLineCache[spScanLineEnd].primitive.triangle.x2 = x2;
	spScanLineCache[spScanLineEnd].primitive.triangle.y2 = y2;
	spScanLineCache[spScanLineEnd].primitive.triangle.z2 = z2;
	spScanLineCache[spScanLineEnd].primitive.triangle.x3 = x3;
	spScanLineCache[spScanLineEnd].primitive.triangle.y3 = y3;
	spScanLineCache[spScanLineEnd].primitive.triangle.z3 = z3;
	spScanLineCache[spScanLineEnd].primitive.triangle.color = color;
	SDL_mutexP(spScanLineMutex);
	spScanLineEnd = (spScanLineEnd+1) & SP_MAX_SCANLINES_MOD;
	SDL_mutexV(spScanLineMutex);
}

inline void sp_intern_Triangle_tex_overlord( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	//Adding to stack if not full!
	CIRCLE_ON_FULL_STACK
	setup_stack_struct(spScanLineEnd,1)
	spScanLineCache[spScanLineEnd].primitive.texTriangle.x1 = x1;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.y1 = y1;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.z1 = z1;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.u1 = u1;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.v1 = v1;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.x2 = x2;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.y2 = y2;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.z2 = z2;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.u2 = u2;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.v2 = v2;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.x3 = x3;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.y3 = y3;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.z3 = z3;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.u3 = u3;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.v3 = v3;
	spScanLineCache[spScanLineEnd].primitive.texTriangle.color = color;
	SDL_mutexP(spScanLineMutex);
	spScanLineEnd = (spScanLineEnd+1) & SP_MAX_SCANLINES_MOD;
	SDL_mutexV(spScanLineMutex);
}

#ifndef NO_PERSPECTIVE
inline void sp_intern_Triangle_tex_overlord_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
{
	//Adding to stack if not full!
	CIRCLE_ON_FULL_STACK
	setup_stack_struct(spScanLineEnd,2)
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.x1 = x1;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.y1 = y1;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.z1 = z1;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.u1 = u1;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.w1 = w1;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.v1 = v1;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.x2 = x2;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.y2 = y2;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.z2 = z2;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.u2 = u2;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.v2 = v2;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.w2 = w2;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.x3 = x3;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.y3 = y3;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.z3 = z3;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.u3 = u3;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.v3 = v3;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.w3 = w3;
	spScanLineCache[spScanLineEnd].primitive.perspectiveTexTriangle.color = color;
	SDL_mutexP(spScanLineMutex);
	spScanLineEnd = (spScanLineEnd+1) & SP_MAX_SCANLINES_MOD;
	SDL_mutexV(spScanLineMutex);
}
#endif

inline void sp_intern_Rectangle_overlord( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color)
{
	//Adding to stack if not full!
	CIRCLE_ON_FULL_STACK
	setup_stack_struct(spScanLineEnd,3)
	spScanLineCache[spScanLineEnd].primitive.rectangle.x1 = x1;
	spScanLineCache[spScanLineEnd].primitive.rectangle.y1 = y1;
	spScanLineCache[spScanLineEnd].primitive.rectangle.x2 = x2;
	spScanLineCache[spScanLineEnd].primitive.rectangle.y2 = y2;
	spScanLineCache[spScanLineEnd].primitive.rectangle.z = z;
	spScanLineCache[spScanLineEnd].primitive.rectangle.color = color;
	SDL_mutexP(spScanLineMutex);
	spScanLineEnd = (spScanLineEnd+1) & SP_MAX_SCANLINES_MOD;
	SDL_mutexV(spScanLineMutex);
}

inline void sp_intern_RectangleBorder_overlord( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color)
{
	//Adding to stack if not full!
	CIRCLE_ON_FULL_STACK
	setup_stack_struct(spScanLineEnd,4)
	spScanLineCache[spScanLineEnd].primitive.rectangleBorder.x1 = x1;
	spScanLineCache[spScanLineEnd].primitive.rectangleBorder.y1 = y1;
	spScanLineCache[spScanLineEnd].primitive.rectangleBorder.x2 = x2;
	spScanLineCache[spScanLineEnd].primitive.rectangleBorder.y2 = y2;
	spScanLineCache[spScanLineEnd].primitive.rectangleBorder.z = z;
	spScanLineCache[spScanLineEnd].primitive.rectangleBorder.bx = bx;
	spScanLineCache[spScanLineEnd].primitive.rectangleBorder.by = by;
	spScanLineCache[spScanLineEnd].primitive.rectangleBorder.color = color;
	SDL_mutexP(spScanLineMutex);
	spScanLineEnd = (spScanLineEnd+1) & SP_MAX_SCANLINES_MOD;
	SDL_mutexV(spScanLineMutex);
}

inline void sp_intern_Ellipse_overlord( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color)
{
	//Adding to stack if not full!
	CIRCLE_ON_FULL_STACK
	setup_stack_struct(spScanLineEnd,5)
	spScanLineCache[spScanLineEnd].primitive.ellipse.x1 = x1;
	spScanLineCache[spScanLineEnd].primitive.ellipse.y1 = y1;
	spScanLineCache[spScanLineEnd].primitive.ellipse.rxl = rxl;
	spScanLineCache[spScanLineEnd].primitive.ellipse.rxr = rxr;
	spScanLineCache[spScanLineEnd].primitive.ellipse.rx = rx;
	spScanLineCache[spScanLineEnd].primitive.ellipse.ryl = ryl;
	spScanLineCache[spScanLineEnd].primitive.ellipse.ryr = ryr;
	spScanLineCache[spScanLineEnd].primitive.ellipse.ry = ry;
	spScanLineCache[spScanLineEnd].primitive.ellipse.z = z;
	spScanLineCache[spScanLineEnd].primitive.ellipse.color = color;
	SDL_mutexP(spScanLineMutex);
	spScanLineEnd = (spScanLineEnd+1) & SP_MAX_SCANLINES_MOD;
	SDL_mutexV(spScanLineMutex);
}

inline void sp_intern_EllipseBorder_overlord( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color)
{
	//Adding to stack if not full!
	CIRCLE_ON_FULL_STACK
	setup_stack_struct(spScanLineEnd,6)
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.x1 = x1;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.y1 = y1;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.rxl = rxl;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.rxr = rxr;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.rx = rx;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.ryl = ryl;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.ryr = ryr;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.ry = ry;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.z = z;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.bx = bx;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.by = by;
	spScanLineCache[spScanLineEnd].primitive.ellipseBorder.color = color;
	SDL_mutexP(spScanLineMutex);
	spScanLineEnd = (spScanLineEnd+1) & SP_MAX_SCANLINES_MOD;
	SDL_mutexV(spScanLineMutex);
}

PREFIX Sint32* spGetOne_over_x_pointer( void )
{
	return spOne_over_x_look_up;
}

void spRestartDrawingThread( void );
void spStopDrawingThread( void );
void spStartDrawingThread( void );

PREFIX void spInitPrimitives( void )
{
	if (spPrimitivesIsInitialized)
		return;
	spPrimitivesIsInitialized = 1;
	#if defined(PANDORA_NEON) && defined(PANDORA)
		static const unsigned int x = 0x04086060;
		static const unsigned int y = 0x03000000;
		int r;
		asm volatile (
			"fmrx	%0, fpscr			\n\t"	//r0 = FPSCR
			"and	%0, %0, %1			\n\t"	//r0 = r0 & 0x04086060
			"orr	%0, %0, %2			\n\t"	//r0 = r0 | 0x03000000
			"fmxr	fpscr, %0			\n\t"	//FPSCR = r0
			: "=r"(r)
			: "r"(x), "r"(y)
		);
	#endif
	int i;
	for ( i = 1; i < ( 1 << SP_ACCURACY ); i++ )
	{
		spOne_over_x_look_up      [i] = (SP_ONE+(i >> 1)) / i;
		spOne_over_x_look_up_fixed[i] = (((Sint64)SP_ONE)<<SP_HALF_ACCURACY)/(Sint64)i;
	}
	spOne_over_x_look_up[0] = 0;
	spOne_over_x_look_up_fixed[0] = 0;
	spSetZBufferCache( spZBufferCacheCount );
	//Preparing the second processor of the gp2x:
	//TODO
	spScanLineMutex = SDL_CreateMutex();
}

PREFIX void spQuitPrimitives( void )
{
	int cacheline;
	for ( cacheline = 0; cacheline < spZBufferCacheCount; cacheline++ )
		if ( spZBufferCache[cacheline] )
		{
			free( spZBufferCache[cacheline] );
			spZBufferCache[cacheline] = NULL;
		}
	if ( spZBufferCache )
		free( spZBufferCache );
	spZBufferCache = NULL;
	if ( spTargetCache )
		free( spTargetCache );
	spTargetCache = NULL;
	if ( spSizeCache )
		free( spSizeCache );
	spSizeCache = NULL;
	if ( spUseParallelProcess )
		spStopDrawingThread();
	SDL_DestroyMutex( spScanLineMutex );
	spScanLineMutex = NULL;
	if ( spScanLineCache ) //TODO: Not on the gp2x!
		free( spScanLineCache );
	spScanLineCache = NULL;
	spPrimitivesIsInitialized = 0;
}

PREFIX void spSelectRenderTarget( SDL_Surface* target )
{
	if (spTarget)
		SDL_UnlockSurface( spTarget );
	spTarget = target;
	if (target)
	{
		spTargetScanLine = target->pitch/target->format->BytesPerPixel;
		spTargetX = target->w;
		spTargetY = target->h;
		spTargetPixel = ( Uint16* )target->pixels;
		spReAllocateZBuffer();
		SDL_LockSurface( spTarget );
	}
}

PREFIX SDL_Surface* spGetRenderTarget( void )
{
	return spTarget;
}

PREFIX void spLockRenderTarget( void )
{
	if (spTarget)
		SDL_LockSurface( spTarget );
}

PREFIX Uint16* spGetTargetPixel( void )
{
	if (spTarget)
		return (Uint16*)(spTarget->pixels);
	return NULL;
}

PREFIX void spUnlockRenderTarget( void )
{
	if (spTarget)
		SDL_UnlockSurface( spTarget );
}

void update_lazy_zBuffer( void )
{
	spZBufferCacheLast = ( spZBufferCacheLast + 1 ) % spZBufferCacheCount;
	if ( spZBufferCache[spZBufferCacheLast] )
		free( spZBufferCache[spZBufferCacheLast] );
	spZBuffer = ( Sint32* )malloc( spTargetScanLine * spTargetY * sizeof( Sint32 ) );
	spZBufferCache[spZBufferCacheLast] = spZBuffer;
	spTargetCache[spZBufferCacheLast] = spTarget;
	spSizeCache[spZBufferCacheLast] = spTarget->w * spTarget->h;
}

PREFIX Sint32* spGetRenderTargetZBuffer( void )
{
	if (!spZBuffer)
		update_lazy_zBuffer();
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
		spTexturePixel = NULL;
		return;
	}
	spTextureScanLine = texture->pitch/texture->format->BytesPerPixel;
	spTextureX = texture->w;
	spTextureY = texture->h;
	spTexturePixel = ( Uint16* )texture->pixels;
}

PREFIX void spClearTarget( Uint32 color )
{
	spWaitForDrawingThread();
	//testfill with CLEAR_PER_FRAME = 64 with SDL_FillRect: 27 fps
	//testfill with CLEAR_PER_FRAME = 64 assembler spHorizentalLine: 32 fps
	//testfill with CLEAR_PER_FRAME = 128 with SDL_FillRect: 14 fps
	//testfill with CLEAR_PER_FRAME = 128 assembler spHorizentalLine: 17 fps
	#ifdef __arm__
		spHorizentalLine(spTargetPixel,0,0,spTargetScanLine*spTargetY,color,0,0,0);
	#else
		spUnlockRenderTarget();
		SDL_FillRect( spTarget, NULL, color );
		spLockRenderTarget();
	#endif
}



PREFIX int spTriangle( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
{
	if ( spBlending == 0)
		return 0;
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

	if (spUseParallelProcess)
		sp_intern_Triangle_overlord( x1, y1, z1, x2, y2, z2, x3, y3, z3, color );
	else
	{
		#ifndef NO_BLENDING
		if ( spBlending == SP_ONE )
		{
		#endif
			#ifndef NO_PATTERN
			if ( spUsePattern )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_ztest_zset_pattern( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
					else
						sp_intern_Triangle_zset_pattern      ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_ztest_pattern     ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
					else
						sp_intern_Triangle_pattern           ( x1, y1, x2, y2, x3, y3, color, spPattern, spBlending );
				}
			}
			else
			#endif
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_ztest_zset( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
					else
						sp_intern_Triangle_zset      ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_ztest     ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
					else
						sp_intern_Triangle           ( x1, y1, x2, y2, x3, y3, color, spPattern, spBlending );
				}
			}
		#ifndef NO_BLENDING
		}
		else
		{
			#ifndef NO_PATTERN
			if ( spUsePattern )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_ztest_zset_pattern( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_zset_pattern      ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_ztest_pattern     ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_pattern           ( x1, y1, x2, y2, x3, y3, color, spPattern, spBlending );
				}
			}
			else
			#endif
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_ztest_zset( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_zset      ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_ztest     ( x1, y1, z1, x2, y2, z2, x3, y3, z3, color, spPattern, spBlending );
					else
						sp_intern_Triangle_blending           ( x1, y1, x2, y2, x3, y3, color, spPattern, spBlending );
				}
			}
		}
		#endif
	}
	return result;
}

PREFIX int spTriangle_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	if ( spBlending == 0)
		return 0;
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
	if (spUseParallelProcess)
		sp_intern_Triangle_tex_overlord( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
	else
	#ifndef NO_BLENDING
	if ( spBlending == SP_ONE )
	{
	#endif
		#ifndef NO_PATTERN
		if ( spUsePattern )
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset_alpha_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset_alpha_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_alpha_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_alpha_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
		else
		#endif
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset_alpha( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset_alpha      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_alpha     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_alpha           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
	#ifndef NO_BLENDING
	}
	else
	{
		#ifndef NO_PATTERN
		if ( spUsePattern )
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset_alpha_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset_alpha_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_alpha_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_alpha_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
		else
		#endif
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset_alpha( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset_alpha      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_alpha     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_alpha           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
	}
	#endif
	return result;
}

PREFIX int spPerspectiveTriangle_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
{
	#ifdef NO_PERSPECTIVE
		return spTriangle_tex(x1,y1,z1,u1,v1,x2,y2,z2,u2,v2,x3,y3,z3,u3,v3,color);
	#else
	if ( spBlending == 0)
		return 0;
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
	if (spUseParallelProcess)
		sp_intern_Triangle_tex_overlord_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color );
	else
	#ifndef NO_BLENDING
	if ( spBlending == SP_ONE )
	{
	#endif
		#ifndef NO_PATTERN
		if ( spUsePattern )
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset_alpha_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset_alpha_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_alpha_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_alpha_pattern_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_pattern_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
		else
		#endif
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset_alpha_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset_alpha_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_alpha_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_alpha_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
	#ifndef NO_BLENDING
	}
	else
	{
		#ifndef NO_PATTERN
		if ( spUsePattern )
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset_alpha_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset_alpha_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_alpha_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_alpha_pattern_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_pattern_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_pattern_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
		else
		#endif
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset_alpha_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset_alpha_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_alpha_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_alpha_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_perspect( x1, y1, z1, u1, v1, w1, x2, y2, z2, u2, v2, w2, x3, y3, z3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_perspect( x1, y1, u1, v1, w1, x2, y2, u2, v2, w2, x3, y3, u3, v3, w3, color, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
	}
	#endif
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

PREFIX void spReAllocateZBuffer( void )
{
	spWaitForDrawingThread();
	//in Cache?
	int cacheline;
	for ( cacheline = 0; cacheline < spZBufferCacheCount; cacheline++ )
		if ( spTargetCache[cacheline] == spTarget && spSizeCache[cacheline] == spTarget->w * spTarget->h )
			break;

	if ( cacheline == spZBufferCacheCount ) //not found
	{
		if (spZSet || spZTest)
			update_lazy_zBuffer();
		else
			spZBuffer = NULL;
	}
	else
	{
		spZBuffer = spZBufferCache[cacheline];
	}
}

PREFIX void spResetZBuffer( void )
{
	spWaitForDrawingThread();
	int i;
	Sint32 s = spZFar-spZNear;
	if ( spZBuffer )
		for ( i = 0; i < spTargetScanLine * spTargetY; i++ )
			spZBuffer[i] = s;
}

PREFIX void spSetZTest( Uint32 test )
{
	spZTest = test;
	if (!spZBuffer && spZTest)
		update_lazy_zBuffer();
}

PREFIX void spSetZSet( Uint32 test )
{
	spZSet = test;
	if (!spZBuffer && spZSet)
		update_lazy_zBuffer();
}

PREFIX void spSetAlphaTest( Uint32 test )
{
	spAlphaTest = test;
}

PREFIX void spSetBlending( Sint32 value )
{
	#ifndef NO_BLENDING
	#ifndef NO_PERSPECTIVE
//#ifdef GP2X
//	if (spBlendingPatternEmulation == 2 || spBlendingPatternEmulation == 1)
//#else
	if (spBlendingPatternEmulation == 2)
//#endif
	{
		spBlending = SP_ONE;
		if (value <= 0)
			spSetAlphaPattern4x4(0,0);
		else
		if (value >= SP_ONE)
			spDeactivatePattern();
		else
			spSetAlphaPattern4x4(spFixedToInt(value*255),0);
		return;
	}
	#endif
	if (value <= 0)
		spBlending = 0;
	else
	if (value >= SP_ONE)
		spBlending = SP_ONE;
	else
		spBlending = value;
	#endif
}

PREFIX void spSetAffineTextureHack( Uint32 test )
{
	spQuadQuali = test;
}

#ifndef __arm__
#ifndef DESKTOP
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

#else

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
#endif

PREFIX void spBlitSurface( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface )
{
	spBlitSurfacePart(x,y,z,surface,0,0,surface->w,surface->h);
}

static Sint32 get_horizontal_add(Sint32 w)
{
	switch (spHorizontalOrigin)
	{
		case SP_LEFT: return 0;
		case SP_RIGHT: return w - 1;
		case SP_FIXED: return spFixedX;
	}
	return w >> 1;
}

static Sint32 get_vertical_add(Sint32 h)
{
	switch (spVerticalOrigin)
	{
		case SP_TOP: return 0;
		case SP_BOTTOM: return h - 1;
		case SP_FIXED: return spFixedY;
	}
	return h >> 1;
}

PREFIX void spBlitSurfacePart( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h )
{
	if (spBlending == 0)
		return;
	spWaitForDrawingThread();
	int addu = get_horizontal_add(w);
	int addv = get_vertical_add(h);
	#ifndef NO_PATTERN
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
					Uint16* oldTexturePixel = spTexturePixel;

					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_ztest_zset_alpha_pattern( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern);
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_ztest_zset_alpha_pattern( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_zset_alpha_pattern( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern );
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_zset_alpha_pattern( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_ztest_alpha_pattern( x, y, z, u, v,65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern );
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_ztest_alpha_pattern( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_alpha_pattern( x, y, u, v,65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern);
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_alpha_pattern( x, y, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_ztest_zset_pattern( x, y, z, u, v,65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern);
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_ztest_zset_pattern( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_zset_pattern( x, y, z, u, v,65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern);
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_zset_pattern( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_ztest_pattern( x, y, z, u, v,65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern);
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_ztest_pattern( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_pattern( x, y, u, v,65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern);
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_pattern( x, y, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY,spPattern, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
				}
			}
		}
	}
	else
	#endif
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_ztest_zset_alpha( x, y, z, u, v,65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY);
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_ztest_zset_alpha( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					#ifndef NO_BLENDING
					if (spBlending != SP_ONE)
						SDL_SetAlpha( surface, SDL_SRCALPHA, spBlending * SDL_ALPHA_OPAQUE >> SP_ACCURACY);
					#endif
					spUnlockRenderTarget();
					SDL_BlitSurface( surface, &src, spTarget, &dest );
					spLockRenderTarget();
					#ifndef NO_BLENDING
					if (spBlending != SP_ONE)
						SDL_SetAlpha( surface, 0, SDL_ALPHA_OPAQUE );
					#endif
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_ztest_alpha( x, y, z, u, v,65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY);
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_ztest_alpha( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					#ifndef NO_BLENDING
					if (spBlending != SP_ONE)
						SDL_SetAlpha( surface, SDL_SRCALPHA, spBlending * SDL_ALPHA_OPAQUE >> SP_ACCURACY);
					#endif
					spUnlockRenderTarget();
					SDL_BlitSurface( surface, &src, spTarget, &dest );
					spLockRenderTarget();
					#ifndef NO_BLENDING
					if (spBlending != SP_ONE)
						SDL_SetAlpha( surface, 0, SDL_ALPHA_OPAQUE );
					#endif
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_ztest_zset( x, y, z, u, v,65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY);
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_ztest_zset( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					#ifndef NO_BLENDING
					if (spBlending != SP_ONE)
						SDL_SetAlpha( surface, SDL_SRCALPHA, spBlending * SDL_ALPHA_OPAQUE >> SP_ACCURACY);
					#endif
					spUnlockRenderTarget();
					SDL_BlitSurface( surface, &src, spTarget, &dest );
					spLockRenderTarget();
					#ifndef NO_BLENDING
					if (spBlending != SP_ONE)
						SDL_SetAlpha( surface, 0, SDL_ALPHA_OPAQUE );
					#endif
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
					Uint16* oldTexturePixel = spTexturePixel;
					spBindTexture( surface );
					int u = sx;
					#ifndef NO_BLENDING
					if (spBlending == SP_ONE)
					#endif
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_tex_ztest( x, y, z, u, v,65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY);
								v++;
							}
							u++;
						}
					#ifndef NO_BLENDING
					else
						for ( x = x1; x < x2; x++ )
						{
							int v = sy;
							for ( y = y1; y < y2; y++ )
							{
								draw_pixel_blending_tex_ztest( x, y, z, u, v, 65535,spTexturePixel,spTextureScanLine,spTextureX,spTextureY, spBlending );
								v++;
							}
							u++;
						}
					#endif
					spTexture = oldTexture;
					spTextureScanLine = oldTextureScanLine;
					spTextureX = oldTextureY;
					spTextureY = oldTextureX;
					spTexturePixel = oldTexturePixel;
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
					#ifndef NO_BLENDING
					if (spBlending != SP_ONE)
						SDL_SetAlpha( surface, SDL_SRCALPHA, spBlending * SDL_ALPHA_OPAQUE >> SP_ACCURACY);
					#endif
					spUnlockRenderTarget();
					SDL_BlitSurface( surface, &src, spTarget, &dest );
					spLockRenderTarget();
					#ifndef NO_BLENDING
					if (spBlending != SP_ONE)
						SDL_SetAlpha( surface, 0, SDL_ALPHA_OPAQUE );
					#endif
				}
			}
		}
	}
}

PREFIX void spSetZBufferCache( Uint32 value )
{
	spWaitForDrawingThread();
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

PREFIX void spSetLineWidth(Sint32 width)
{
	if (width > 0)
		spLineWidth = width;
}

PREFIX void spLine( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Uint32 color )
{
	if (spLineWidth == 1)
		spTriangle(x1,y1,z1,x1,y1,z1,x2,y2,z2,color);
	else
	{
		Sint32 l = abs(x2-x1)+abs(y2-y1);
		if (l > 0)
		{
			Sint32 x = (x2-x1)*spLineWidth / l;
			Sint32 y = (y2-y1)*spLineWidth / l;
			spQuad(x1+y,y1-x,z1,
			       x1-y,y1+x,z1,
			       x2-y,y2+x,z2,
			       x2+y,y2-x,z2,color);
		}
	}
}

PREFIX void spRectangle( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Uint32 color )
{
	if ( spBlending == 0)
		return;
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return;
	if (spZTest && z < 0)
		return;
	int addu = get_horizontal_add(w);
	int addv = get_vertical_add(h);
	Sint32 x1 = x - addu;
	Sint32 x2 = x1 + w;
	Sint32 y1 = y - addv;
	Sint32 y2 = y1 + h;
	y = y1;
	if ( x1 >= spTargetX ) return;
	if ( y1 >= spTargetY ) return;
	if ( x2 < 0 )          return;
	if ( y2 < 0 )          return;
	if ( x2 >= spTargetX ) x2 = spTargetX - 1;
	if ( y2 >= spTargetY ) y2 = spTargetY - 1;
	if ( x1 < 0 )          x1 = 0;
	if ( y1 < 0 )          y1 = 0;
	if ( spUseParallelProcess )
		sp_intern_Rectangle_overlord(x1,y1,x2,y2,z,color);
	else
	#ifndef NO_PATTERN
	if ( spUsePattern )
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Rectangle_ztest_zset_pattern( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Rectangle_blending_ztest_zset_pattern( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Rectangle_zset_pattern( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Rectangle_blending_zset_pattern( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#endif
			}
		}
		else
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Rectangle_ztest_pattern( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Rectangle_blending_ztest_pattern( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Rectangle_pattern( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Rectangle_blending_pattern( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#endif
			}
		}
	}
	else
	#endif
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Rectangle_ztest_zset( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Rectangle_blending_ztest_zset( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Rectangle_zset( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Rectangle_blending_zset( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#endif
			}
		}
		else
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Rectangle_ztest( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Rectangle_blending_ztest( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Rectangle( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Rectangle_blending( x1, y1, x2, y2, z, color, spPattern, spBlending);
				#endif
			}
		}
	}
}

PREFIX void spRectangleBorder( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Sint32 bx, Sint32 by, Uint32 color )
{
	if (spBlending == 0)
		return;
	if ( ( bx * 2 >= w ) || ( by * 2 >= h ) )
	{
		spRectangle( x, y, z, w, h, color );
		return;
	}
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return;
	if (spZTest && z < 0)
		return;

	int addv = h-by >> 1;
	if ( spVerticalOrigin == SP_TOP )
		addv = 0;
	if ( spVerticalOrigin == SP_BOTTOM )
		addv = h-by - 1;

	spRectangle(x,y-addv     ,z,w,by,color);
	spRectangle(x,y-addv+h-by,z,w,by,color);

	addv = 0;
	if ( spVerticalOrigin == SP_TOP )
		addv = -by;
	if ( spVerticalOrigin == SP_BOTTOM )
		addv = by;

	int addu = w-bx >> 1;
	if ( spHorizontalOrigin == SP_LEFT )
		addu = 0;
	if ( spHorizontalOrigin == SP_RIGHT )
		addu = w-bx - 1;

	spRectangle(x-addu     ,y-addv,z,bx,h-2*by,color);
	spRectangle(x-addu+w-bx,y-addv,z,bx,h-2*by,color);
}

PREFIX void spEllipse( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Uint32 color )
{
	if (spBlending == 0)
		return;
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
	if ( spUseParallelProcess )
		sp_intern_Ellipse_overlord(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color);
	else
	#ifndef NO_PATTERN
	if ( spUsePattern )
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Ellipse_ztest_zset_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Ellipse_blending_ztest_zset_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Ellipse_zset_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Ellipse_blending_zset_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#endif
			}
		}
		else
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Ellipse_ztest_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Ellipse_blending_ztest_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Ellipse_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Ellipse_blending_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#endif
			}
		}
	}
	else
	#endif
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Ellipse_ztest_zset(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Ellipse_blending_ztest_zset(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Ellipse_zset(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Ellipse_blending_zset(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#endif
			}
		}
		else
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Ellipse_ztest(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Ellipse_blending_ztest(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_Ellipse(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_Ellipse_blending(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,color,spPattern,spBlending);
				#endif
			}
		}
	}
}

PREFIX void spEllipseBorder( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Sint32 bx, Sint32 by, Uint32 color )
{
	if (spBlending == 0)
		return;
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
	if ( spUseParallelProcess )
		sp_intern_EllipseBorder_overlord(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color);
	else
	#ifndef NO_PATTERN
	if ( spUsePattern )
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_EllipseBorder_ztest_zset_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_EllipseBorder_blending_ztest_zset_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_EllipseBorder_zset_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_EllipseBorder_blending_zset_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#endif
			}
		}
		else
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_EllipseBorder_ztest_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_EllipseBorder_blending_ztest_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#endif

			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_EllipseBorder_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_EllipseBorder_blending_pattern(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#endif
			}
		}
	}
	else
	#endif
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_EllipseBorder_ztest_zset(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_EllipseBorder_blending_ztest_zset(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_EllipseBorder_zset(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_EllipseBorder_blending_zset(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#endif
			}
		}
		else
		{
			if ( spZTest )
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_EllipseBorder_ztest(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_EllipseBorder_blending_ztest(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#endif
			}
			else
			{
				#ifndef NO_BLENDING
				if ( spBlending == SP_ONE)
				#endif
					sp_intern_EllipseBorder(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#ifndef NO_BLENDING
				else
					sp_intern_EllipseBorder_blending(x1,y1,rxl,rxr,rx,ryl,ryr,ry,z,bx,by,color,spPattern,spBlending);
				#endif
			}
		}
	}
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

inline static void sp_intern_Triangle_tex_inter( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3 )
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
	#ifndef NO_BLENDING
	if ( spBlending == SP_ONE )
	{
	#endif
		#ifndef NO_PATTERN
		if ( spUsePattern )
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset_alpha_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset_alpha_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_alpha_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_alpha_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
		else
		#endif
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset_alpha( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset_alpha      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_alpha     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_alpha           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest_zset( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex_zset      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_tex_ztest     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_tex           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
	#ifndef NO_BLENDING
	}
	else
	{
		#ifndef NO_PATTERN
		if ( spUsePattern )
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset_alpha_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset_alpha_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_alpha_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_alpha_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset_pattern( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset_pattern      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_pattern     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_pattern           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
		else
		#endif
		{
			if ( spAlphaTest )
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset_alpha( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset_alpha      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_alpha     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_alpha           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
			else
			{
				if ( spZSet )
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest_zset( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex_zset      ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
				else
				{
					if ( spZTest )
						sp_intern_Triangle_blending_tex_ztest     ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
					else
						sp_intern_Triangle_blending_tex           ( x1, y1, u1, v1, x2, y2, u2, v2, x3, y3, u3, v3, 65535, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending );
				}
			}
		}
	}
	#endif
}

static void draw_tiny_quad(Sint32 nx1,Sint32 ny1, Sint32 sx1, Sint32 sy1,
                     Sint32 nx2,Sint32 ny2, Sint32 sx2, Sint32 sy2,
                     Sint32 nx3,Sint32 ny3, Sint32 sx3, Sint32 sy3,
                     Sint32 nx4,Sint32 ny4, Sint32 sx4, Sint32 sy4,Sint32 z)
{
		sp_intern_Triangle_tex_inter( nx1, ny1, z, sx1 , sy1 ,
									  nx2, ny2, z, sx2 , sy2 ,
									  nx3, ny3, z, sx3 , sy3 );
		sp_intern_Triangle_tex_inter( nx1, ny1, z, sx1 , sy1 ,
									  nx3, ny3, z, sx3 , sy3 ,
									  nx4, ny4, z, sx4 , sy4 );
}

#define MAX_QUAD_SIZE 256

static void draw_recursive_rotoquad(Sint32 nx1,Sint32 ny1, Sint32 sx1, Sint32 sy1,
                             Sint32 nx2, Sint32 ny2, Sint32 sx2, Sint32 sy2,
                             Sint32 nx3, Sint32 ny3, Sint32 sx3, Sint32 sy3,
                             Sint32 nx4, Sint32 ny4, Sint32 sx4, Sint32 sy4,
                             Sint32 z, Sint32 maxX, Sint32 maxY)
{
	if (nx1 < 0 && nx2 < 0 && nx3 < 0 && nx4 < 0)
		return;
	if (nx1 >= spTargetX && nx2 >= spTargetX && nx3 >= spTargetX && nx4 >= spTargetX)
		return;
	if (ny1 < 0 && ny2 < 0 && ny3 < 0 && ny4 < 0)
		return;
	if (ny1 >= spTargetY && ny2 >= spTargetY && ny3 >= spTargetY && ny4 >= spTargetY)
		return;
	if (abs(sx1-sx4) > maxY)
	{
		Sint32 nx14 = nx1 + nx4 >> 1;
		Sint32 ny14 = ny1 + ny4 >> 1;
		Sint32 sx14 = sx1 + sx4 >> 1;
		Sint32 sy14 = sy1 + sy4 >> 1;
		Sint32 nx23 = nx2 + nx3 >> 1;
		Sint32 ny23 = ny2 + ny3 >> 1;
		Sint32 sx23 = sx2 + sx3 >> 1;
		Sint32 sy23 = sy2 + sy3 >> 1;
		draw_recursive_rotoquad(nx1 ,ny1 ,sx1 ,sy1 ,
		                        nx2 ,ny2 ,sx2 ,sy2 ,
		                        nx23,ny23,sx23,sy23,
		                        nx14,ny14,sx14,sy14,z,maxX,maxY);
		draw_recursive_rotoquad(nx14,ny14,sx14,sy14,
		                        nx23,ny23,sx23,sy23,
		                        nx3 ,ny3 ,sx3 ,sy3 ,
		                        nx4 ,ny4 ,sx4 ,sy4 ,z,maxX,maxY);
		return;
	}
	if (abs(sy1 - sy2) > maxY)
	{
		Sint32 nx12 = nx1 + nx2 >> 1;
		Sint32 ny12 = ny1 + ny2 >> 1;
		Sint32 sx12 = sx1 + sx2 >> 1;
		Sint32 sy12 = sy1 + sy2 >> 1;
		Sint32 nx34 = nx3 + nx4 >> 1;
		Sint32 ny34 = ny3 + ny4 >> 1;
		Sint32 sx34 = sx3 + sx4 >> 1;
		Sint32 sy34 = sy3 + sy4 >> 1;
		draw_recursive_rotoquad(nx1 ,ny1 ,sx1 ,sy1 ,
		                        nx12,ny12,sx12,sy12,
		                        nx34,ny34,sx34,sy34,
		                        nx4 ,ny4 ,sx4 ,sy4 ,z,maxX,maxY);
		draw_recursive_rotoquad(nx12,ny12,sx12,sy12,
		                        nx2 ,ny2 ,sx2 ,sy2 ,
		                        nx3 ,ny3 ,sx3 ,sy3 ,
		                        nx34,ny34,sx34,sy34,z,maxX,maxY);
		return;
	}
	draw_tiny_quad(nx1,ny1,sx1,sy1,nx2,ny2,sx2,sy2,nx3,ny3,sx3,sy3,nx4,ny4,sx4,sy4,z);
}

PREFIX void spRotozoomSurfacePart( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, Sint32 zoomX, Sint32 zoomY, Sint32 angle )
{
	if (spBlending == 0)
		return;
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

	x1 = - get_horizontal_add(w)*abs(zoomX) >> SP_ACCURACY;
	x2 = x1;
	x3 = (w - get_horizontal_add(w))*abs(zoomX) >> SP_ACCURACY;
	x4 = x3;
	y1 = - get_vertical_add(h)*abs(zoomY) >> SP_ACCURACY;
	y2 = (h - get_vertical_add(h))*abs(zoomY) >> SP_ACCURACY;
	y3 = y2;
	y4 = y1;

	SDL_Surface* oldTexture = spTexture;
	Sint32 oldTextureScanLine = spTextureScanLine;
	Sint32 oldTextureX = spTextureY;
	Sint32 oldTextureY = spTextureX;
	Uint16* oldTexturePixel = spTexturePixel;
	spBindTexture( surface );

	if (zoomX < 0)
	{
		sx+=w-1;
		w = -w;
	}
	if (zoomY < 0)
	{
		sy+=h-1;
		h = -h;
	}

	if (angle == 0)
	{
		x1 += x;
		x3 += x;
		y1 += y;
		y3 += y;

		#ifndef NO_BLENDING
		if ( spBlending == SP_ONE)
		{
		#endif
			#ifndef NO_PATTERN
			if ( spUsePattern )
			{
				if ( spAlphaTest )
				{
					if ( spZSet )
					{
						if ( spZTest )
							draw_zoom_ztest_zset_alpha_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_zoom_zset_alpha_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
					else
					{
						if ( spZTest )
							draw_zoom_ztest_alpha_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_zoom_alpha_pattern(x1,x3,y1,y3,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
				}
				else
				{
					if ( spZSet )
					{
						if ( spZTest )
							draw_zoom_ztest_zset_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_zoom_zset_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
					else
					{
						if ( spZTest )
							draw_zoom_ztest_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_zoom_pattern(x1,x3,y1,y3,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
				}
			}
			else
			#endif
			{
				if ( spAlphaTest )
				{
					if ( spZSet )
					{
						if ( spZTest )
							draw_zoom_ztest_zset_alpha(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_zoom_zset_alpha(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
					else
					{
						if ( spZTest )
							draw_zoom_ztest_alpha(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_zoom_alpha(x1,x3,y1,y3,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
				}
				else
				{
					if ( spZSet )
					{
						if ( spZTest )
							draw_zoom_ztest_zset(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_zoom_zset(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
					else
					{
						if ( spZTest )
							draw_zoom_ztest(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_zoom(x1,x3,y1,y3,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
				}
			}
		#ifndef NO_BLENDING
		}
		else
		{
			#ifndef NO_PATTERN
			if ( spUsePattern )
			{
				if ( spAlphaTest )
				{
					if ( spZSet )
					{
						if ( spZTest )
							draw_blending_zoom_ztest_zset_alpha_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_blending_zoom_zset_alpha_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
					else
					{
						if ( spZTest )
							draw_blending_zoom_ztest_alpha_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_blending_zoom_alpha_pattern(x1,x3,y1,y3,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
				}
				else
				{
					if ( spZSet )
					{
						if ( spZTest )
							draw_blending_zoom_ztest_zset_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_blending_zoom_zset_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
					else
					{
						if ( spZTest )
							draw_blending_zoom_ztest_pattern(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_blending_zoom_pattern(x1,x3,y1,y3,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
				}
			}
			else
			#endif
			{
				if ( spAlphaTest )
				{
					if ( spZSet )
					{
						if ( spZTest )
							draw_blending_zoom_ztest_zset_alpha(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_blending_zoom_zset_alpha(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
					else
					{
						if ( spZTest )
							draw_blending_zoom_ztest_alpha(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_blending_zoom_alpha(x1,x3,y1,y3,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
				}
				else
				{
					if ( spZSet )
					{
						if ( spZTest )
							draw_blending_zoom_ztest_zset(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_blending_zoom_zset(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
					else
					{
						if ( spZTest )
							draw_blending_zoom_ztest(x1,x3,y1,y3,z,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
						else
							draw_blending_zoom(x1,x3,y1,y3,sx,sy,w,h, spTexturePixel, spTextureScanLine, spTextureX, spTextureY, spPattern, spBlending);
					}
				}
			}
		}
		#endif
	}
	else
	{
		Sint32 nx1 = x + ( x1 * spCos( angle ) - y1 * spSin( angle ) >> SP_ACCURACY );
		Sint32 ny1 = y + ( y1 * spCos( angle ) + x1 * spSin( angle ) >> SP_ACCURACY );
		Sint32 nx2 = x + ( x2 * spCos( angle ) - y2 * spSin( angle ) >> SP_ACCURACY );
		Sint32 ny2 = y + ( y2 * spCos( angle ) + x2 * spSin( angle ) >> SP_ACCURACY );
		Sint32 nx3 = x + ( x3 * spCos( angle ) - y3 * spSin( angle ) >> SP_ACCURACY );
		Sint32 ny3 = y + ( y3 * spCos( angle ) + x3 * spSin( angle ) >> SP_ACCURACY );
		Sint32 nx4 = x + ( x4 * spCos( angle ) - y4 * spSin( angle ) >> SP_ACCURACY );
		Sint32 ny4 = y + ( y4 * spCos( angle ) + x4 * spSin( angle ) >> SP_ACCURACY );

		draw_recursive_rotoquad(nx1,ny1,sx    ,sy    ,
		                        nx2,ny2,sx    ,sy + h,
		                        nx3,ny3,sx + w,sy + h,
		                        nx4,ny4,sx + w,sy    ,z,
		                        spFixedToInt(spDivHigh(spIntToFixed(MAX_QUAD_SIZE),spSqrt(zoomX))),
		                        spFixedToInt(spDivHigh(spIntToFixed(MAX_QUAD_SIZE),spSqrt(zoomY))));
	}

	spTexture = oldTexture;
	spTextureScanLine = oldTextureScanLine;
	spTextureX = oldTextureY;
	spTextureY = oldTextureX;
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

PREFIX Sint32 spGetHorizontalOrigin( void )
{
	return spHorizontalOrigin;
}

PREFIX Sint32 spGetVerticalOrigin( void )
{
	return spVerticalOrigin;
}

static int log_2(int x)
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
	spWaitForDrawingThread();
	spZFar = zfar;
	//For division optimization we need the logarithm of the biggest w component,
	//that will happen while rendering
	Sint32 x,y,z,w;
	spProjectPoint3D(0,0,-zfar,&x,&y,&z,&w,1);
	int l = log_2(w)+1;
	spMaxWLogDiff = l - SP_ACCURACY;
}

PREFIX Sint32 spGetZFar( void )
{
	return spZFar;
}

PREFIX void spSetZNear(Sint32 znear)
{
	spWaitForDrawingThread();
	spZNear = znear;
}

PREFIX Sint32 spGetZNear( void )
{
	return spZNear;
}

PREFIX void spAddColorToTarget(Uint16 destColor,Sint32 interpolation)
{
	spWaitForDrawingThread();
  int i;
  int goal = spTargetScanLine*spTargetY;
  for (i=0;i<goal;i++)
    spTargetPixel[i]=spAddColor(spTargetPixel[i],destColor,interpolation);
}

PREFIX void spInterpolateTargetToColor(Uint16 destColor,Sint32 interpolation)
{
	spWaitForDrawingThread();
  int i;
  int goal = spTargetScanLine*spTargetY;
  for (i=0;i<goal;i++)
    spTargetPixel[i]=spInterpolateColor(spTargetPixel[i],destColor,interpolation);
}

PREFIX void spSetPattern32(Uint32 first_32_bit,Uint32 last_32_bit)
{
	#ifndef NO_PATTERN
	spPattern[0] = first_32_bit >> 24;
	spPattern[1] = first_32_bit >> 16;
	spPattern[2] = first_32_bit >>  8;
	spPattern[3] = first_32_bit      ;
	spPattern[4] = last_32_bit >> 24;
	spPattern[5] = last_32_bit >> 16;
	spPattern[6] = last_32_bit >>  8;
	spPattern[7] = last_32_bit      ;
	spUsePattern = (first_32_bit != ((Uint32)0xFFFFFFFF)) || (last_32_bit != ((Uint32)0xFFFFFFFF));
	#endif
}

PREFIX void spSetPattern64(Uint64 pattern)
{
	#ifndef NO_PATTERN
	spPattern[0] = pattern >> 56;
	spPattern[1] = pattern >> 48;
	spPattern[2] = pattern >> 40;
	spPattern[3] = pattern >> 32;
	spPattern[4] = pattern >> 24;
	spPattern[5] = pattern >> 16;
	spPattern[6] = pattern >>  8;
	spPattern[7] = pattern      ;
	spUsePattern = pattern != ((Uint64)0xFFFFFFFFFFFFFFFFLL);
	#endif
}

PREFIX void spSetPattern8(Uint8 line1,Uint8 line2,Uint8 line3,Uint8 line4,Uint8 line5,Uint8 line6,Uint8 line7,Uint8 line8)
{
	#ifndef NO_PATTERN
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
	#endif
}

PREFIX void spDeactivatePattern( void )
{
	#ifndef NO_PATTERN
	spUsePattern = 0;
	#endif
}

PREFIX void spSetAlphaPattern(int alpha,int shift)
{
	#ifndef NO_PATTERN
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
	spUsePattern = spPattern[1] != 255 || spPattern[2] != 255 || spPattern[3] != 255 ||
	               spPattern[4] != 255 || spPattern[5] != 255 || spPattern[6] != 255 ||
	               spPattern[7] != 255 || spPattern[8] != 255;
	#endif
}

#define ringshift(left,shift) ((left << shift) | (left >> 32-shift))

PREFIX void spSetAlphaPattern4x4(int alpha,int shift)
{
	#ifndef NO_PATTERN
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
			left = 4456448LL;
			break;
		case 2:
			/* 0000 0000
			 * 0100 0100
			 * 0000 0000
			 * 0001 0001 */
			left = 4456465LL;
			break;
		case 3:
			/* 0000 0000
			 * 0100 0100
			 * 0000 0000
			 * 0101 0101 */
			left = 4456533LL;
			break;
		case 4:
			/* 0000 0000
			 * 0101 0101
			 * 0000 0000
			 * 0101 0101 */
			left = 5570645LL;
			break;
		case 5:
			/* 1000 1000
			 * 0101 0101
			 * 0000 0000
			 * 0101 0101 */
			left = 2287272021LL;
			break;
		case 6:
			/* 1000 1000
			 * 0101 0101
			 * 0010 0010
			 * 0101 0101 */
			left = 2287280725LL;
			break;
		case 7:
			/* 1000 1000
			 * 0101 0101
			 * 1010 1010
			 * 0101 0101 */
			left = 2287315541LL;
			break;
		case 8:
			/* 1010 1010
			 * 0101 0101
			 * 1010 1010
			 * 0101 0101 */
			left = 2857740885LL;
			break;
		case 9:
			/* 1010 1010
			 * 0101 0101
			 * 1110 1110
			 * 0101 0101 */
			left = 2857758293LL;
			break;
		case 10:
			/* 1011 1011
			 * 0101 0101
			 * 1110 1110
			 * 0101 0101 */
			left = 3142970965LL;
			break;
		case 11:
			/* 1011 1011
			 * 0101 0101
			 * 1111 1111
			 * 0101 0101 */
			left = 3142975317LL;
			break;
		case 12:
			/* 1111 1111
			 * 0101 0101
			 * 1111 1111
			 * 0101 0101 */
			left = 4283826005LL;
			break;
		case 13:
			/* 1111 1111
			 * 0101 0101
			 * 1111 1111
			 * 1101 1101 */
			left = 4283826141LL;
			break;
		case 14:
			/* 1111 1111
			 * 0111 0111
			 * 1111 1111
			 * 1101 1101 */
			left = 4286054365LL;
			break;
		case 15:
			/* 1111 1111
			 * 0111 0111
			 * 1111 1111
			 * 1111 1111 */
			left = 4286054399LL;
		case 16:
			/* 1111 1111
			 * 1111 1111
			 * 1111 1111
			 * 1111 1111 */
			left = 4294967295LL;
			break;
	}
	shift = shift & 15;
	left = ringshift(left,shift);
	spSetPattern32(left,left);
	#endif
}

void spRestartDrawingThread( void )
{
	if (!spUseParallelProcess)
		return;
	spWaitForDrawingThread();
	spStopDrawingThread();
	spStartDrawingThread();
}

void spStopDrawingThread( void )
{
	if (spScanLineThread)
	{
		spScanLineMessage = 0; //Terminate!
		SDL_WaitThread(spScanLineThread,NULL);
		spScanLineThread = NULL;
	}
}

PREFIX void spWaitForDrawingThread( void )
{
	if (!spUseParallelProcess)
		return;
	while (1)
	{
		SDL_mutexP(spScanLineMutex);
		if (spScanLineBegin == spScanLineEnd)
			break;
		SDL_mutexV(spScanLineMutex);
		spSleep(SP_MAX_SCANLINES_WAIT_TIME);
	}
	SDL_mutexV(spScanLineMutex);
}

void spStartDrawingThread( void )
{
	if (!spUseParallelProcess)
		return;
	if (spScanLineThread)
		return;

	if ( spScanLineCache == NULL ) //TODO: The gp2x just gets an adress of the upper memory
		spScanLineCache = (type_spScanLineCache*)malloc(sizeof(type_spScanLineCache)*SP_MAX_SCANLINES);

	spScanLineBegin = 0;
	spScanLineEnd = 0;
	spScanLineMessage = 1;
	spScanLineThread = SDL_CreateThread(sp_intern_drawing_thread, NULL);
}

PREFIX void spDrawInExtraThread(int value)
{
	spWaitForDrawingThread();
	spUseParallelProcess = value;
	if (value == 0)
		spStopDrawingThread();
	else
		spStartDrawingThread();
}


PREFIX void spUpdateTargetPixels( void )
{
	if (spTarget)
		spTargetPixel = ( Uint16* )spTarget->pixels;
}

#define FLOOD_PIXEL(x,y) spTargetPixel[(x) + (y) * spTargetScanLine]

struct spFloodFillPixel
{
	int x;
	int y;
	struct spFloodFillPixel *next;
};

struct spFloodFillPixel* spFloodFillStack = NULL;

#ifdef __GNUC__
inline void flood_fill_stack_test_and_push(int x, int y,Uint16 oldColor) __attribute__((always_inline));
#endif
inline void flood_fill_stack_test_and_push(int x, int y,Uint16 oldColor)
{
	if (x >= 0 && y >= 0 && x < spTargetX && y < spTargetY && FLOOD_PIXEL(x,y) == oldColor)
	{
		struct spFloodFillPixel *add = malloc(sizeof(struct spFloodFillPixel));
		add->x = x;
		add->y = y;
		add->next = spFloodFillStack;
		spFloodFillStack = add;
	}
}

#ifdef __GNUC__
inline struct spFloodFillPixel* flood_fill_stack_pop( void ) __attribute__((always_inline));
#endif
inline struct spFloodFillPixel* flood_fill_stack_pop( void )
{
	//if (spFloodFillStack == NULL) //This SHOULD never happen...
	//	return NULL;
	struct spFloodFillPixel *temp = spFloodFillStack;
	spFloodFillStack = spFloodFillStack->next;
	return temp;
}

void floodFill(int x,int y,Uint16 newColor,Uint16 oldColor)
{
	flood_fill_stack_test_and_push(x,y,oldColor);
	while (spFloodFillStack)
	{
		struct spFloodFillPixel *curr = flood_fill_stack_pop();
		FLOOD_PIXEL(curr->x,curr->y) = newColor;
		flood_fill_stack_test_and_push(curr->x+1,curr->y,oldColor);
		flood_fill_stack_test_and_push(curr->x,curr->y+1,oldColor);
		flood_fill_stack_test_and_push(curr->x-1,curr->y,oldColor);
		flood_fill_stack_test_and_push(curr->x,curr->y-1,oldColor);
		free(curr);
	}
}

PREFIX void spFloodFill(int x,int y,Uint16 color)
{
	if (FLOOD_PIXEL(x,y) == color)
		return;
	floodFill(x,y,color,FLOOD_PIXEL(x,y));
}

PREFIX void spEmulateBlendingWithPattern( Sint32 value)
{
	spBlendingPatternEmulation = value;
}

PREFIX void spSetFixedOrign(int x,int y)
{
	spFixedX = x;
	spFixedY = y;
}
