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
#include "sparrowPrimitives.h"
#include <stdlib.h>

SDL_Surface* spTarget = NULL;
Uint16* spTargetPixel = NULL;
SDL_Surface* spTexture = NULL;
Uint16* spTexturePixel = NULL;
Uint32 spZTest = 1;
Uint32 spZSet = 1;
Uint32 spAlphaTest = 1;
Uint32 spQuadQuali = 1;
Sint32* spZBuffer = NULL;
Sint32 spTargetX = 0;
Sint32 spTargetY = 0;
Sint32 spTextureX = 0;
Sint32 spTextureY = 0;
Sint32 spTextureXY = 0;
Sint32 spOne_over_x_look_up[1 << SP_PRIM_ACCURACY];
Uint32 spZBufferCacheCount = 16;
Uint32 spZBufferCacheLast;
Sint32** spZBufferCache = NULL;
SDL_Surface** spTargetCache = NULL;
Uint32* spSizeCache = NULL;
char spCulling = 1;
Sint32 spVerticalOrigin = SP_CENTER;
Sint32 spHorizontalOrigin = SP_CENTER;

PREFIX Sint32* spGetOne_over_x_pointer()
{
	return spOne_over_x_look_up;
}

PREFIX void spInitPrimitives()
{
	int i;
	for ( i = 1; i < ( 1 << SP_PRIM_ACCURACY ); i++ )
		spOne_over_x_look_up[i] = ( 1 << SP_PRIM_ACCURACY ) / i;
	spOne_over_x_look_up[0] = 0;
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
	return spTarget->pixels;
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
	if ( texture == NULL )
		return;
	spTexture = texture;
	spTextureX = texture->w;
	spTextureY = texture->h;
	spTextureXY = spTextureX * spTextureY;
	spTexturePixel = ( Uint16* )texture->pixels;
}

PREFIX void spClearTarget( Uint32 color )
{
	SDL_FillRect( spTarget, NULL, color );
	/*SDL_Surface(spTarget);
	spHorizentalLine(spTargetPixel,0,0,spTargetX*spTargetY,color,0,0,0);
	SDL_UnlockSurface(spTarget);*/
}

inline Sint32 one_over_x( Sint32 x )
{
	if ( x > 0 )
	{
		if ( x < ( 1 << SP_PRIM_ACCURACY ) )
			return spOne_over_x_look_up[x];
		if ( x == ( 1 << SP_PRIM_ACCURACY ) )
			return 1;
		return 0;
	}
	if ( x > ( -1 << SP_PRIM_ACCURACY ) )
		return -spOne_over_x_look_up[-x];
	if ( x == ( -1 << SP_PRIM_ACCURACY ) )
		return -1;
	return 0;
}

inline Sint32 z_div( Sint32 z, Sint32 d )
{
#ifdef REALGP2X_TODO_FIX_ME
	return ( z >> SP_HALF_PRIM_ACCURACY ) * ( one_over_x( d ) >> SP_HALF_PRIM_ACCURACY );
#else
	if ( d == 0 )
		return 0;
	return z / d;
#endif
}

inline void draw_pixel_ztest_zset( Sint32 x, Sint32 y, Sint32 z, Uint32 color )
{
	if ( z < 0 && z > spZBuffer[x + y * spTargetX] )
	{
		spTargetPixel[x + y * spTargetX] = color;
		spZBuffer[x + y * spTargetX] = z;
	}
}

inline void draw_pixel_ztest( Sint32 x, Sint32 y, Sint32 z, Uint32 color )
{
	if ( z < 0 && z > spZBuffer[x + y * spTargetX] )
	{
		spTargetPixel[x + y * spTargetX] = color;
	}
}

inline void draw_pixel_zset( Sint32 x, Sint32 y, Sint32 z, Uint32 color )
{
	spTargetPixel[x + y * spTargetX] = color;
	spZBuffer[x + y * spTargetX] = z;
}

inline void draw_pixel( Sint32 x, Sint32 y, Uint32 color )
{
	spTargetPixel[x + y * spTargetX] = color;
}

inline void draw_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 z = z1;
	if ( x1 < 0 )
	{
		z -= x1 * sZ;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_ztest_zset( x, y, z, color );
		z += sZ;
	}
}

inline void draw_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 z = z1;
	if ( x1 < 0 )
	{
		z -= x1 * sZ;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_ztest( x, y, z, color );
		z += sZ;
	}
}

inline void draw_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 z = z1;
	if ( x1 < 0 )
	{
		z -= x1 * sZ;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_zset( x, y, z, color );
		z += sZ;
	}
}

inline void draw_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	if ( x1 < 0 )
		x1 = 0;
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	//TODO: Replace with hline!
	/*int x;
	for (x = x1; x<= x2; x++)
	{
	  draw_pixel(x,y,color);
	}*/
	spHorizentalLine( spTargetPixel, x1, y, x2 - x1, color, 1, spTargetX, spTargetY );
}

inline void sp_intern_Triangle_ztest_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	Sint32 x4 = x1;
	Sint32 y4 = y1;
	Sint32 z4 = z1;
	int div = y2 - y1;
	if ( div != 0 )
	{
		int mul = y3 - y1;
		Sint32 mul32 = mul * one_over_x( div ); //(mul<<SP_PRIM_ACCURACY)/div;
		x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
		y4 = y3;
		//z4 = z1+((z2-z1)*mul32>>SP_PRIM_ACCURACY-SP_ACCURACY);
		z4 = z1 + mul * z_div( z2 - z1, div );
	}
	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 zl = z1;
	Sint32 sX_l = 0;
	Sint32 sZ_l = 0;
	if ( ( y1 - y2 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y2 ); //(1<<SP_PRIM_ACCURACY)/(y1-y2);
		sX_l = ( x1 - x2 ) * mul;
		//sZ_l = (z1-z2)*mul;
		sZ_l = z_div( z1 - z2, y1 - y2 );
	}

	Sint32 xr = xl;
	Sint32 zr = zl;
	Sint32 sX_r = 0;
	Sint32 sZ_r = 0;
	if ( ( y1 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y3 ); //(1<<SP_PRIM_ACCURACY)/(y1-y3);
		sX_r = ( x1 - x3 ) * mul;
		//sZ_r = (z1-z3)*mul;
		sZ_r = z_div( z1 - z3, y1 - y3 );
	}

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		zr += sZ_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			zr += sZ_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 sZ = 0;
		if ( ( x4 - x3 ) != 0 )
		{
			//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
			//sZ = (z4-z3)*mul;
			sZ = z_div( z4 - z3, x4 - x3 );
		}
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
				//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
				draw_line_ztest_zset( xl >> SP_PRIM_ACCURACY, zl,
									  xr >> SP_PRIM_ACCURACY, zr, y, color, sZ );
				xl += sX_l;
				zl += sZ_l;
				xr += sX_r;
				zr += sZ_r;
			}
		}
		else
		{
			Sint32 sZ = 0;
			if ( ( x4 - x3 ) != 0 )
			{
				//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
				//sZ = (z4-z3)*mul;
				sZ = z_div( z4 - z3, x4 - x3 );
			}
			for ( y = y1; y < y3; y++ )
			{
				//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
				//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
				draw_line_ztest_zset( xr >> SP_PRIM_ACCURACY, zr,
									  xl >> SP_PRIM_ACCURACY, zl, y, color, sZ );
				xl += sX_l;
				zl += sZ_l;
				xr += sX_r;
				zr += sZ_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	zr = z3;
	sX_r = 0;
	sZ_r = 0;
	if ( ( y2 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y2 - y3 ); //(1<<SP_PRIM_ACCURACY)/(y2-y3);
		sX_r = ( x2 - x3 ) * mul;
		//sZ_r = (z2-z3)*mul;
		sZ_r = z_div( z2 - z3, y2 - y3 );
	}

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		zr += sZ_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	Sint32 sZ = 0;
	if ( ( x4 - x3 ) != 0 )
	{
		//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
		//sZ = (z4-z3)*mul;
		sZ = z_div( z4 - z3, x4 - x3 );
	}
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
			//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
			draw_line_ztest_zset( xl >> SP_PRIM_ACCURACY, zl,
								  xr >> SP_PRIM_ACCURACY, zr, y, color, sZ );
			xl += sX_l;
			zl += sZ_l;
			xr += sX_r;
			zr += sZ_r;
		}
	}
	else
	{
		Sint32 sZ = 0;
		if ( ( x4 - x3 ) != 0 )
		{
			//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
			//sZ = (z4-z3)*mul;
			sZ = z_div( z4 - z3, x4 - x3 );
		}
		for ( y = y3; y <= y2; y++ )
		{
			//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
			//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
			draw_line_ztest_zset( xr >> SP_PRIM_ACCURACY, zr,
								  xl >> SP_PRIM_ACCURACY, zl, y, color, sZ );
			xl += sX_l;
			zl += sZ_l;
			xr += sX_r;
			zr += sZ_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

inline void sp_intern_Triangle_ztest( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	Sint32 x4 = x1;
	Sint32 y4 = y1;
	Sint32 z4 = z1;
	int div = y2 - y1;
	if ( div != 0 )
	{
		int mul = y3 - y1;
		Sint32 mul32 = mul * one_over_x( div ); //(mul<<SP_PRIM_ACCURACY)/div;
		x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
		y4 = y3;
		//z4 = z1+((z2-z1)*mul32>>SP_PRIM_ACCURACY-SP_ACCURACY);
		z4 = z1 + mul * z_div( z2 - z1, div );
	}
	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 zl = z1;
	Sint32 sX_l = 0;
	Sint32 sZ_l = 0;
	if ( ( y1 - y2 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y2 ); //(1<<SP_PRIM_ACCURACY)/(y1-y2);
		sX_l = ( x1 - x2 ) * mul;
		//sZ_l = (z1-z2)*mul;
		sZ_l = z_div( z1 - z2, y1 - y2 );
	}

	Sint32 xr = xl;
	Sint32 zr = zl;
	Sint32 sX_r = 0;
	Sint32 sZ_r = 0;
	if ( ( y1 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y3 ); //(1<<SP_PRIM_ACCURACY)/(y1-y3);
		sX_r = ( x1 - x3 ) * mul;
		//sZ_r = (z1-z3)*mul;
		sZ_r = z_div( z1 - z3, y1 - y3 );
	}

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		zr += sZ_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			zr += sZ_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 sZ = 0;
		if ( ( x4 - x3 ) != 0 )
		{
			//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
			//sZ = (z4-z3)*mul;
			sZ = z_div( z4 - z3, x4 - x3 );
		}
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
				//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
				draw_line_ztest( xl >> SP_PRIM_ACCURACY, zl,
								 xr >> SP_PRIM_ACCURACY, zr, y, color, sZ );
				xl += sX_l;
				zl += sZ_l;
				xr += sX_r;
				zr += sZ_r;
			}
		}
		else
		{
			Sint32 sZ = 0;
			if ( ( x4 - x3 ) != 0 )
			{
				//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
				//sZ = (z4-z3)*mul;
				sZ = z_div( z4 - z3, x4 - x3 );
			}
			for ( y = y1; y < y3; y++ )
			{
				//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
				//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
				draw_line_ztest( xr >> SP_PRIM_ACCURACY, zr,
								 xl >> SP_PRIM_ACCURACY, zl, y, color, sZ );
				xl += sX_l;
				zl += sZ_l;
				xr += sX_r;
				zr += sZ_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	zr = z3;
	sX_r = 0;
	sZ_r = 0;
	if ( ( y2 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y2 - y3 ); //(1<<SP_PRIM_ACCURACY)/(y2-y3);
		sX_r = ( x2 - x3 ) * mul;
		//sZ_r = (z2-z3)*mul;
		sZ_r = z_div( z2 - z3, y2 - y3 );
	}

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		zr += sZ_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	Sint32 sZ = 0;
	if ( ( x4 - x3 ) != 0 )
	{
		//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
		//sZ = (z4-z3)*mul;
		sZ = z_div( z4 - z3, x4 - x3 );
	}
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
			//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
			draw_line_ztest( xl >> SP_PRIM_ACCURACY, zl,
							 xr >> SP_PRIM_ACCURACY, zr, y, color, sZ );
			xl += sX_l;
			zl += sZ_l;
			xr += sX_r;
			zr += sZ_r;
		}
	}
	else
	{
		Sint32 sZ = 0;
		if ( ( x4 - x3 ) != 0 )
		{
			//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
			//sZ = (z4-z3)*mul;
			sZ = z_div( z4 - z3, x4 - x3 );
		}
		for ( y = y3; y <= y2; y++ )
		{
			//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
			//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
			draw_line_ztest( xr >> SP_PRIM_ACCURACY, zr,
							 xl >> SP_PRIM_ACCURACY, zl, y, color, sZ );
			xl += sX_l;
			zl += sZ_l;
			xr += sX_r;
			zr += sZ_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

inline void sp_intern_Triangle_zset( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	Sint32 x4 = x1;
	Sint32 y4 = y1;
	Sint32 z4 = z1;
	int div = y2 - y1;
	if ( div != 0 )
	{
		int mul = y3 - y1;
		Sint32 mul32 = mul * one_over_x( div ); //(mul<<SP_PRIM_ACCURACY)/div;
		x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
		y4 = y3;
		//z4 = z1+((z2-z1)*mul32>>SP_PRIM_ACCURACY-SP_ACCURACY);
		z4 = z1 + mul * z_div( z2 - z1, div );
	}
	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 zl = z1;
	Sint32 sX_l = 0;
	Sint32 sZ_l = 0;
	if ( ( y1 - y2 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y2 ); //(1<<SP_PRIM_ACCURACY)/(y1-y2);
		sX_l = ( x1 - x2 ) * mul;
		//sZ_l = (z1-z2)*mul;
		sZ_l = z_div( z1 - z2, y1 - y2 );
	}

	Sint32 xr = xl;
	Sint32 zr = zl;
	Sint32 sX_r = 0;
	Sint32 sZ_r = 0;
	if ( ( y1 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y3 ); //(1<<SP_PRIM_ACCURACY)/(y1-y3);
		sX_r = ( x1 - x3 ) * mul;
		//sZ_r = (z1-z3)*mul;
		sZ_r = z_div( z1 - z3, y1 - y3 );
	}

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		zr += sZ_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			zr += sZ_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 sZ = 0;
		if ( ( x4 - x3 ) != 0 )
		{
			//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
			//sZ = (z4-z3)*mul;
			sZ = z_div( z4 - z3, x4 - x3 );
		}
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
				//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
				draw_line_zset( xl >> SP_PRIM_ACCURACY, zl,
								xr >> SP_PRIM_ACCURACY, zr, y, color, sZ );
				xl += sX_l;
				zl += sZ_l;
				xr += sX_r;
				zr += sZ_r;
			}
		}
		else
		{
			Sint32 sZ = 0;
			if ( ( x4 - x3 ) != 0 )
			{
				//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
				//sZ = (z4-z3)*mul;
				sZ = z_div( z4 - z3, x4 - x3 );
			}
			for ( y = y1; y < y3; y++ )
			{
				//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
				//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
				draw_line_zset( xr >> SP_PRIM_ACCURACY, zr,
								xl >> SP_PRIM_ACCURACY, zl, y, color, sZ );
				xl += sX_l;
				zl += sZ_l;
				xr += sX_r;
				zr += sZ_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	zr = z3;
	sX_r = 0;
	sZ_r = 0;
	if ( ( y2 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y2 - y3 ); //(1<<SP_PRIM_ACCURACY)/(y2-y3);
		sX_r = ( x2 - x3 ) * mul;
		//sZ_r = (z2-z3)*mul;
		sZ_r = z_div( z2 - z3, y2 - y3 );
	}

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		zr += sZ_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	Sint32 sZ = 0;
	if ( ( x4 - x3 ) != 0 )
	{
		//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
		//sZ = (z4-z3)*mul;
		sZ = z_div( z4 - z3, x4 - x3 );
	}
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
			//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
			draw_line_zset( xl >> SP_PRIM_ACCURACY, zl,
							xr >> SP_PRIM_ACCURACY, zr, y, color, sZ );
			xl += sX_l;
			zl += sZ_l;
			xr += sX_r;
			zr += sZ_r;
		}
	}
	else
	{
		Sint32 sZ = 0;
		if ( ( x4 - x3 ) != 0 )
		{
			//Sint32 mul = one_over_x(x4-x3);//(1<<SP_PRIM_ACCURACY)/(x4-x3);
			//sZ = (z4-z3)*mul;
			sZ = z_div( z4 - z3, x4 - x3 );
		}
		for ( y = y3; y <= y2; y++ )
		{
			//if ((xl-xr>>SP_PRIM_ACCURACY)!=0)
			//  sZ = z_div(zl-zr,xl-xr>>SP_PRIM_ACCURACY);
			draw_line_zset( xr >> SP_PRIM_ACCURACY, zr,
							xl >> SP_PRIM_ACCURACY, zl, y, color, sZ );
			xl += sX_l;
			zl += sZ_l;
			xr += sX_r;
			zr += sZ_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

inline void sp_intern_Triangle( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	Sint32 x4 = x1;
	Sint32 y4 = y1;
	int div = y2 - y1;
	if ( div != 0 )
	{
		if ( div < 0 )
			div = -div;
		int mul = y3 - y1;
		if ( mul < 0 )
			mul = -mul;
		Sint32 mul32 = mul * one_over_x( div ); //(mul<<SP_PRIM_ACCURACY)/div;
		x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
		y4 = y3;
	}
	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 sX_l = 0;
	if ( ( y1 - y2 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y2 ); //(1<<SP_PRIM_ACCURACY)/(y1-y2);
		sX_l = ( x1 - x2 ) * mul;
	}

	Sint32 xr = xl;
	Sint32 sX_r = 0;
	if ( ( y1 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y3 ); //(1<<SP_PRIM_ACCURACY)/(y1-y3);
		sX_r = ( x1 - x3 ) * mul;
	}

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		xr += sX_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			xr += sX_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line( xl >> SP_PRIM_ACCURACY, xr >> SP_PRIM_ACCURACY, y, color );
				xl += sX_l;
				xr += sX_r;
			}
		}
		else
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line( xr >> SP_PRIM_ACCURACY, xl >> SP_PRIM_ACCURACY, y, color );
				xl += sX_l;
				xr += sX_r;
			}
		}
	}
	xr = x3 << SP_PRIM_ACCURACY;
	sX_r = 0;
	if ( ( y2 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y2 - y3 ); //(1<<SP_PRIM_ACCURACY)/(y2-y3);
		sX_r = ( x2 - x3 ) * mul;
	}
	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		xr += sX_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line( xl >> SP_PRIM_ACCURACY, xr >> SP_PRIM_ACCURACY, y, color );
			xl += sX_l;
			xr += sX_r;
		}
	}
	else
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line( xr >> SP_PRIM_ACCURACY, xl >> SP_PRIM_ACCURACY, y, color );
			xl += sX_l;
			xr += sX_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

PREFIX int spTriangle( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
{
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
	return result;
}

inline void draw_pixel_tex_ztest_zset( Sint32 x, Sint32 y, Sint32 z, Sint32 u, Sint32 v, Uint32 color )
{
	if ( z < 0 && z > spZBuffer[x + y * spTargetX] )
	{
#ifdef FAST_BUT_UGLY
		//if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
		//  spTargetPixel[x+y*spTargetX] = color;
		//else
#else
		if ( u < 0 )
			u = 0;
		if ( v < 0 )
			v = 0;
		if ( u >= spTextureX )
			u = spTextureX - 1;
		if ( v >= spTextureY )
			v = spTextureY - 1;
#endif
		spTargetPixel[x + y * spTargetX] = ( ( spTexturePixel[u + v * spTextureX] * color >> 16 ) & 63488 )
										   + ( ( ( spTexturePixel[u + v * spTextureX] & 2047 ) * ( color & 2047 ) >> 11 ) & 2016 )
										   + ( ( spTexturePixel[u + v * spTextureX] & 31 ) * ( color & 31 ) >> 5 );
		spZBuffer[x + y * spTargetX] = z;
	}
}

inline void draw_pixel_tex_ztest( Sint32 x, Sint32 y, Sint32 z, Sint32 u, Sint32 v, Uint32 color )
{
	if ( z < 0 && z > spZBuffer[x + y * spTargetX] )
	{
#ifdef FAST_BUT_UGLY
		//if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
		//  spTargetPixel[x+y*spTargetX] = color;
		//else
#else
		if ( u < 0 )
			u = 0;
		if ( v < 0 )
			v = 0;
		if ( u >= spTextureX )
			u = spTextureX - 1;
		if ( v >= spTextureY )
			v = spTextureY - 1;
#endif
		spTargetPixel[x + y * spTargetX] = ( ( spTexturePixel[u + v * spTextureX] * color >> 16 ) & 63488 )
										   + ( ( ( spTexturePixel[u + v * spTextureX] & 2047 ) * ( color & 2047 ) >> 11 ) & 2016 )
										   + ( ( spTexturePixel[u + v * spTextureX] & 31 ) * ( color & 31 ) >> 5 );
	}
}

inline void draw_pixel_tex_zset( Sint32 x, Sint32 y, Sint32 z, Sint32 u, Sint32 v, Uint32 color )
{
#ifdef FAST_BUT_UGLY
	//if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
	//  spTargetPixel[x+y*spTargetX] = color;
	//else
#else
	if ( u < 0 )
		u = 0;
	if ( v < 0 )
		v = 0;
	if ( u >= spTextureX )
		u = spTextureX - 1;
	if ( v >= spTextureY )
		v = spTextureY - 1;
#endif
	spTargetPixel[x + y * spTargetX] = ( ( spTexturePixel[u + v * spTextureX] * color >> 16 ) & 63488 )
									   + ( ( ( spTexturePixel[u + v * spTextureX] & 2047 ) * ( color & 2047 ) >> 11 ) & 2016 )
									   + ( ( spTexturePixel[u + v * spTextureX] & 31 ) * ( color & 31 ) >> 5 );
	spZBuffer[x + y * spTargetX] = z;
}

inline void draw_pixel_tex( Sint32 x, Sint32 y, Sint32 u, Sint32 v, Uint32 color )
{
#ifdef FAST_BUT_UGLY
	//if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
	//  spTargetPixel[x+y*spTargetX] = color;
	//else
#else
	if ( u < 0 )
		u = 0;
	if ( v < 0 )
		v = 0;
	if ( u >= spTextureX )
		u = spTextureX - 1;
	if ( v >= spTextureY )
		v = spTextureY - 1;
#endif
	spTargetPixel[x + y * spTargetX] = ( ( spTexturePixel[u + v * spTextureX] * color >> 16 ) & 63488 )
									   + ( ( ( spTexturePixel[u + v * spTextureX] & 2047 ) * ( color & 2047 ) >> 11 ) & 2016 )
									   + ( ( spTexturePixel[u + v * spTextureX] & 31 ) * ( color & 31 ) >> 5 );
}

inline void draw_line_tex_ztest_zset( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 u = u1;
	Sint32 v = v1;
	Sint32 z = z1;
	if ( x1 < 0 )
	{
		z -= x1 * sZ;
		u -= x1 * sU;
		v -= x1 * sV;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_tex_ztest_zset( x, y, z, u >> SP_PRIM_ACCURACY, v >> SP_PRIM_ACCURACY, color );
		u += sU;
		v += sV;
		z += sZ;
	}
}

inline void draw_line_tex_ztest( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 u = u1;
	Sint32 v = v1;
	Sint32 z = z1;
	if ( x1 < 0 )
	{
		z -= x1 * sZ;
		u -= x1 * sU;
		v -= x1 * sV;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_tex_ztest( x, y, z, u >> SP_PRIM_ACCURACY, v >> SP_PRIM_ACCURACY, color );
		u += sU;
		v += sV;
		z += sZ;
	}
}

inline void draw_line_tex_zset( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 u = u1;
	Sint32 v = v1;
	Sint32 z = z1;
	if ( x1 < 0 )
	{
		z -= x1 * sZ;
		u -= x1 * sU;
		v -= x1 * sV;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_tex_zset( x, y, z, u >> SP_PRIM_ACCURACY, v >> SP_PRIM_ACCURACY, color );
		u += sU;
		v += sV;
		z += sZ;
	}
}

inline void draw_line_tex( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 u = u1;
	Sint32 v = v1;
	if ( x1 < 0 )
	{
		u -= x1 * sU;
		v -= x1 * sV;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_tex( x, y, u >> SP_PRIM_ACCURACY, v >> SP_PRIM_ACCURACY, color );
		u += sU;
		v += sV;
	}
}

inline void sp_intern_Triangle_tex_ztest_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	int div = y2 - y1;
	Sint32 mul = y3 - y1;
	Sint32 mul32 = mul * one_over_x( div );
	Sint32 x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 z4 = z1 + mul * z_div( z2 - z1, div );
	Sint32 u4 = u1 + ( ( u2 - u1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 v4 = v1 + ( ( v2 - v1 ) * mul32 >> SP_PRIM_ACCURACY );

	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 ul = u1 << SP_PRIM_ACCURACY;
	Sint32 vl = v1 << SP_PRIM_ACCURACY;
	Sint32 zl = z1;

	mul = one_over_x( y1 - y2 );
	Sint32 sX_l = ( x1 - x2 ) * mul;
	Sint32 sU_l = ( u1 - u2 ) * mul;
	Sint32 sV_l = ( v1 - v2 ) * mul;
	Sint32 sZ_l = z_div( z1 - z2, y1 - y2 );

	Sint32 xr = xl;
	Sint32 ur = ul;
	Sint32 vr = vl;
	Sint32 zr = zl;
	mul = one_over_x( y1 - y3 );
	Sint32 sX_r = ( x1 - x3 ) * mul;
	Sint32 sU_r = ( u1 - u3 ) * mul;
	Sint32 sV_r = ( v1 - v3 ) * mul;
	Sint32 sZ_r = z_div( z1 - z3, y1 - y3 );

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			ul += sU_l * diff;
			vl += sV_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
			zr += sZ_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 mul = one_over_x( x4 - x3 );
		Sint32 sU = ( u4 - u3 ) * mul;
		Sint32 sV = ( v4 - v3 ) * mul;
		Sint32 sZ = z_div( z4 - z3, x4 - x3 );
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_ztest_zset( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
										  xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
		else
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_ztest_zset( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
										  xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	ur = u3 << SP_PRIM_ACCURACY;
	vr = v3 << SP_PRIM_ACCURACY;
	zr = z3;
	mul = one_over_x( y2 - y3 );
	sX_r = ( x2 - x3 ) * mul;
	sU_r = ( u2 - u3 ) * mul;
	sV_r = ( v2 - v3 ) * mul;
	sZ_r = z_div( z2 - z3, y2 - y3 );

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	mul = one_over_x( x4 - x3 );
	Sint32 sU = ( u4 - u3 ) * mul;
	Sint32 sV = ( v4 - v3 ) * mul;
	Sint32 sZ = z_div( z4 - z3, x4 - x3 );
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_ztest_zset( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
									  xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	else
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_ztest_zset( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
									  xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

inline void sp_intern_Triangle_tex_ztest( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	int div = y2 - y1;
	Sint32 mul = y3 - y1;
	Sint32 mul32 = mul * one_over_x( div );
	Sint32 x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 z4 = z1 + mul * z_div( z2 - z1, div );
	Sint32 u4 = u1 + ( ( u2 - u1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 v4 = v1 + ( ( v2 - v1 ) * mul32 >> SP_PRIM_ACCURACY );

	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 ul = u1 << SP_PRIM_ACCURACY;
	Sint32 vl = v1 << SP_PRIM_ACCURACY;
	Sint32 zl = z1;

	mul = one_over_x( y1 - y2 );
	Sint32 sX_l = ( x1 - x2 ) * mul;
	Sint32 sU_l = ( u1 - u2 ) * mul;
	Sint32 sV_l = ( v1 - v2 ) * mul;
	Sint32 sZ_l = z_div( z1 - z2, y1 - y2 );

	Sint32 xr = xl;
	Sint32 ur = ul;
	Sint32 vr = vl;
	Sint32 zr = zl;
	mul = one_over_x( y1 - y3 );
	Sint32 sX_r = ( x1 - x3 ) * mul;
	Sint32 sU_r = ( u1 - u3 ) * mul;
	Sint32 sV_r = ( v1 - v3 ) * mul;
	Sint32 sZ_r = z_div( z1 - z3, y1 - y3 );

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			ul += sU_l * diff;
			vl += sV_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
			zr += sZ_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 mul = one_over_x( x4 - x3 );
		Sint32 sU = ( u4 - u3 ) * mul;
		Sint32 sV = ( v4 - v3 ) * mul;
		Sint32 sZ = z_div( z4 - z3, x4 - x3 );
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_ztest( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
									 xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
		else
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_ztest( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
									 xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	ur = u3 << SP_PRIM_ACCURACY;
	vr = v3 << SP_PRIM_ACCURACY;
	zr = z3;
	mul = one_over_x( y2 - y3 );
	sX_r = ( x2 - x3 ) * mul;
	sU_r = ( u2 - u3 ) * mul;
	sV_r = ( v2 - v3 ) * mul;
	sZ_r = z_div( z2 - z3, y2 - y3 );

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	mul = one_over_x( x4 - x3 );
	Sint32 sU = ( u4 - u3 ) * mul;
	Sint32 sV = ( v4 - v3 ) * mul;
	Sint32 sZ = z_div( z4 - z3, x4 - x3 );
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_ztest( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
								 xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	else
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_ztest( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
								 xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

inline void sp_intern_Triangle_tex_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	int div = y2 - y1;
	Sint32 mul = y3 - y1;
	Sint32 mul32 = mul * one_over_x( div );
	Sint32 x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 z4 = z1 + mul * z_div( z2 - z1, div );
	Sint32 u4 = u1 + ( ( u2 - u1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 v4 = v1 + ( ( v2 - v1 ) * mul32 >> SP_PRIM_ACCURACY );

	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 ul = u1 << SP_PRIM_ACCURACY;
	Sint32 vl = v1 << SP_PRIM_ACCURACY;
	Sint32 zl = z1;

	mul = one_over_x( y1 - y2 );
	Sint32 sX_l = ( x1 - x2 ) * mul;
	Sint32 sU_l = ( u1 - u2 ) * mul;
	Sint32 sV_l = ( v1 - v2 ) * mul;
	Sint32 sZ_l = z_div( z1 - z2, y1 - y2 );

	Sint32 xr = xl;
	Sint32 ur = ul;
	Sint32 vr = vl;
	Sint32 zr = zl;
	mul = one_over_x( y1 - y3 );
	Sint32 sX_r = ( x1 - x3 ) * mul;
	Sint32 sU_r = ( u1 - u3 ) * mul;
	Sint32 sV_r = ( v1 - v3 ) * mul;
	Sint32 sZ_r = z_div( z1 - z3, y1 - y3 );

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			ul += sU_l * diff;
			vl += sV_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
			zr += sZ_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 mul = one_over_x( x4 - x3 );
		Sint32 sU = ( u4 - u3 ) * mul;
		Sint32 sV = ( v4 - v3 ) * mul;
		Sint32 sZ = z_div( z4 - z3, x4 - x3 );
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_zset( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
									xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
		else
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_zset( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
									xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	ur = u3 << SP_PRIM_ACCURACY;
	vr = v3 << SP_PRIM_ACCURACY;
	zr = z3;
	mul = one_over_x( y2 - y3 );
	sX_r = ( x2 - x3 ) * mul;
	sU_r = ( u2 - u3 ) * mul;
	sV_r = ( v2 - v3 ) * mul;
	sZ_r = z_div( z2 - z3, y2 - y3 );

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	mul = one_over_x( x4 - x3 );
	Sint32 sU = ( u4 - u3 ) * mul;
	Sint32 sV = ( v4 - v3 ) * mul;
	Sint32 sZ = z_div( z4 - z3, x4 - x3 );
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_zset( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
								xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	else
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_zset( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
								xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

inline void sp_intern_Triangle_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	int div = y2 - y1;
	Sint32 mul = y3 - y1;
	Sint32 mul32 = mul * one_over_x( div );
	Sint32 x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 u4 = u1 + ( ( u2 - u1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 v4 = v1 + ( ( v2 - v1 ) * mul32 >> SP_PRIM_ACCURACY );

	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 ul = u1 << SP_PRIM_ACCURACY;
	Sint32 vl = v1 << SP_PRIM_ACCURACY;

	mul = one_over_x( y1 - y2 );
	Sint32 sX_l = ( x1 - x2 ) * mul;
	Sint32 sU_l = ( u1 - u2 ) * mul;
	Sint32 sV_l = ( v1 - v2 ) * mul;

	Sint32 xr = xl;
	Sint32 ur = ul;
	Sint32 vr = vl;
	mul = one_over_x( y1 - y3 );
	Sint32 sX_r = ( x1 - x3 ) * mul;
	Sint32 sU_r = ( u1 - u3 ) * mul;
	Sint32 sV_r = ( v1 - v3 ) * mul;

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			ul += sU_l * diff;
			vl += sV_l * diff;
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 mul = one_over_x( x4 - x3 );
		Sint32 sU = ( u4 - u3 ) * mul;
		Sint32 sV = ( v4 - v3 ) * mul;
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex( xl >> SP_PRIM_ACCURACY, ul, vl,
							   xr >> SP_PRIM_ACCURACY, ur, vr, y, color, sU, sV );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
			}
		}
		else
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex( xr >> SP_PRIM_ACCURACY, ur, vr,
							   xl >> SP_PRIM_ACCURACY, ul, vl, y, color, sU, sV );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	ur = u3 << SP_PRIM_ACCURACY;
	vr = v3 << SP_PRIM_ACCURACY;
	mul = one_over_x( y2 - y3 );
	sX_r = ( x2 - x3 ) * mul;
	sU_r = ( u2 - u3 ) * mul;
	sV_r = ( v2 - v3 ) * mul;

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	mul = one_over_x( x4 - x3 );
	Sint32 sU = ( u4 - u3 ) * mul;
	Sint32 sV = ( v4 - v3 ) * mul;
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex( xl >> SP_PRIM_ACCURACY, ul, vl,
						   xr >> SP_PRIM_ACCURACY, ur, vr, y, color, sU, sV );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
		}
	}
	else
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex( xr >> SP_PRIM_ACCURACY, ur, vr,
						   xl >> SP_PRIM_ACCURACY, ul, vl, y, color, sU, sV );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

inline void draw_pixel_tex_ztest_zset_alpha( Sint32 x, Sint32 y, Sint32 z, Sint32 u, Sint32 v, Uint32 color )
{
	if ( z < 0 && z > spZBuffer[x + y * spTargetX] )
	{
#ifdef FAST_BUT_UGLY
		//if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
		//  spTargetPixel[x+y*spTargetX] = color;
		//else
#else
		if ( u < 0 )
			u = 0;
		if ( v < 0 )
			v = 0;
		if ( u >= spTextureX )
			u = spTextureX - 1;
		if ( v >= spTextureY )
			v = spTextureY - 1;
#endif
		if ( spTexturePixel[u + v * spTextureX] != SP_ALPHA_COLOR )
		{
			spTargetPixel[x + y * spTargetX] = ( ( spTexturePixel[u + v * spTextureX] * color >> 16 ) & 63488 )
											   + ( ( ( spTexturePixel[u + v * spTextureX] & 2047 ) * ( color & 2047 ) >> 11 ) & 2016 )
											   + ( ( spTexturePixel[u + v * spTextureX] & 31 ) * ( color & 31 ) >> 5 );
			spZBuffer[x + y * spTargetX] = z;
		}
	}
}

inline void draw_pixel_tex_ztest_alpha( Sint32 x, Sint32 y, Sint32 z, Sint32 u, Sint32 v, Uint32 color )
{
	if ( z < 0 && z > spZBuffer[x + y * spTargetX] )
	{
#ifdef FAST_BUT_UGLY
		//if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
		//  spTargetPixel[x+y*spTargetX] = color;
		//else
#else
		if ( u < 0 )
			u = 0;
		if ( v < 0 )
			v = 0;
		if ( u >= spTextureX )
			u = spTextureX - 1;
		if ( v >= spTextureY )
			v = spTextureY - 1;
#endif
		if ( spTexturePixel[u + v * spTextureX] != SP_ALPHA_COLOR )
			spTargetPixel[x + y * spTargetX] = ( ( spTexturePixel[u + v * spTextureX] * color >> 16 ) & 63488 )
											   + ( ( ( spTexturePixel[u + v * spTextureX] & 2047 ) * ( color & 2047 ) >> 11 ) & 2016 )
											   + ( ( spTexturePixel[u + v * spTextureX] & 31 ) * ( color & 31 ) >> 5 );
	}
}

inline void draw_pixel_tex_zset_alpha( Sint32 x, Sint32 y, Sint32 z, Sint32 u, Sint32 v, Uint32 color )
{
#ifdef FAST_BUT_UGLY
	//if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
	//  spTargetPixel[x+y*spTargetX] = color;
	//else
#else
	if ( u < 0 )
		u = 0;
	if ( v < 0 )
		v = 0;
	if ( u >= spTextureX )
		u = spTextureX - 1;
	if ( v >= spTextureY )
		v = spTextureY - 1;
#endif
	if ( spTexturePixel[u + v * spTextureX] != SP_ALPHA_COLOR )
	{
		spTargetPixel[x + y * spTargetX] = ( ( spTexturePixel[u + v * spTextureX] * color >> 16 ) & 63488 )
										   + ( ( ( spTexturePixel[u + v * spTextureX] & 2047 ) * ( color & 2047 ) >> 11 ) & 2016 )
										   + ( ( spTexturePixel[u + v * spTextureX] & 31 ) * ( color & 31 ) >> 5 );
		spZBuffer[x + y * spTargetX] = z;
	}
}

inline void draw_pixel_tex_alpha( Sint32 x, Sint32 y, Sint32 u, Sint32 v, Uint32 color )
{
#ifdef FAST_BUT_UGLY
	//if (u+v*spTextureX < 0 || u+v*spTextureX >= spTextureXY)
	//  spTargetPixel[x+y*spTargetX] = color;
	//else
#else
	if ( u < 0 )
		u = 0;
	if ( v < 0 )
		v = 0;
	if ( u >= spTextureX )
		u = spTextureX - 1;
	if ( v >= spTextureY )
		v = spTextureY - 1;
#endif
	if ( spTexturePixel[u + v * spTextureX] != SP_ALPHA_COLOR )
	{
		spTargetPixel[x + y * spTargetX] = ( ( spTexturePixel[u + v * spTextureX] * color >> 16 ) & 63488 )
										   + ( ( ( spTexturePixel[u + v * spTextureX] & 2047 ) * ( color & 2047 ) >> 11 ) & 2016 )
										   + ( ( spTexturePixel[u + v * spTextureX] & 31 ) * ( color & 31 ) >> 5 );
	}
}

inline void draw_line_tex_ztest_zset_alpha( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 u = u1;
	Sint32 v = v1;
	Sint32 z = z1;
	if ( x1 < 0 )
	{
		z -= x1 * sZ;
		u -= x1 * sU;
		v -= x1 * sV;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_tex_ztest_zset_alpha( x, y, z, u >> SP_PRIM_ACCURACY, v >> SP_PRIM_ACCURACY, color );
		u += sU;
		v += sV;
		z += sZ;
	}
}

inline void draw_line_tex_ztest_alpha( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 u = u1;
	Sint32 v = v1;
	Sint32 z = z1;
	if ( x1 < 0 )
	{
		z -= x1 * sZ;
		u -= x1 * sU;
		v -= x1 * sV;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_tex_ztest_alpha( x, y, z, u >> SP_PRIM_ACCURACY, v >> SP_PRIM_ACCURACY, color );
		u += sU;
		v += sV;
		z += sZ;
	}
}

inline void draw_line_tex_zset_alpha( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 u = u1;
	Sint32 v = v1;
	Sint32 z = z1;
	if ( x1 < 0 )
	{
		z -= x1 * sZ;
		u -= x1 * sU;
		v -= x1 * sV;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_tex_zset_alpha( x, y, z, u >> SP_PRIM_ACCURACY, v >> SP_PRIM_ACCURACY, color );
		u += sU;
		v += sV;
		z += sZ;
	}
}

inline void draw_line_tex_alpha( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV )
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 u = u1;
	Sint32 v = v1;
	if ( x1 < 0 )
	{
		u -= x1 * sU;
		v -= x1 * sV;
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		draw_pixel_tex_alpha( x, y, u >> SP_PRIM_ACCURACY, v >> SP_PRIM_ACCURACY, color );
		u += sU;
		v += sV;
	}
}

/*inline void horz_tex_zset_zset_alpha(Sint32 xl,Sint32 xr,Sint32 y,
                              Sint32 zl,Sint32 zr,
                              Sint32 ul,Sint32 ur,
                              Sint32 vl,Sint32 vr,Uint32 color)
{
  if (xl+1 >= xr)
    return;
  Sint32 xm = xl+xr>>1;
  Sint32 zm = zl+zr>>1;
  Sint32 um = ul+ur>>1;
  Sint32 vm = vl+vr>>1;
  draw_pixel_tex_ztest_zset_alpha(xm,y,zm,um,vm,color);
  horz_tex_zset_zset_alpha(xl,xm,y,zl,zm,ul,um,vl,vm,color);
  horz_tex_zset_zset_alpha(xm,xr,y,zm,zr,um,ur,vm,vr,color);
}

inline void vert_tex_ztest_zset_alpha(Sint32 xtl,Sint32 xtr,Sint32 yt,
                                       Sint32 ztl,        Sint32 ztr,
                               Sint32 utl,Sint32 utr,
                               Sint32 vtl,Sint32 vtr,
                               Sint32 xbl,Sint32 xbr,Sint32 yb,
                                       Sint32 zbl,        Sint32 zbr,
                               Sint32 ubl,Sint32 ubr,
                               Sint32 vbl,Sint32 vbr,Uint32 color)
{
  if (yt+1 >= yb)
    return;
  Sint32 xml = xtl + xbl >>1;
  Sint32 xmr = xtr + xbr >>1;
  Sint32 ym  =  yt +  yb >>1;
  Sint32 zml = ztl + zbl >>1;
  Sint32 zmr = ztr + zbr >>1;
  Sint32 uml = utl + ubl >>1;
  Sint32 umr = utr + ubr >>1;
  Sint32 vml = vtl + vbl >>1;
  Sint32 vmr = vtr + vbr >>1;
  vert_tex_ztest_zset_alpha(xtl,xtr,yt,ztl,ztr,utl,utr,vtl,vtr,
                            xml,xmr,ym,zml,zmr,uml,umr,vml,vmr,color);
  draw_pixel_tex_ztest_zset_alpha(xml,ym,zml,uml,vml,color);
  if (xml < xmr)
    horz_tex_zset_zset_alpha(xml,xmr,ym,zml,zmr,uml,umr,vml,vmr,color);
  else
    horz_tex_zset_zset_alpha(xmr,xml,ym,zmr,zml,umr,uml,vmr,vml,color);
  draw_pixel_tex_ztest_zset_alpha(xmr,ym,zmr,umr,vmr,color);
  vert_tex_ztest_zset_alpha(xml,xmr,ym,zml,zmr,uml,umr,vml,vmr,
                            xbl,xbr,yb,zbl,zbr,ubl,ubr,vbl,vbr,color);
}*/

/*inline void sp_intern_recursive_Triangle_tex_ztest_zset_alpha(
  Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1,
  Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2,
  Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color)
{
  Sint32 nx = (x1+x2+x3)/3;
  Sint32 ny = (y1+y2+y3)/3;
  if (((x1>>SP_ACCURACY) == (nx>>SP_ACCURACY) && (y1>>SP_ACCURACY) == (ny>>SP_ACCURACY)) ||
      ((x2>>SP_ACCURACY) == (nx>>SP_ACCURACY) && (y2>>SP_ACCURACY) == (ny>>SP_ACCURACY)) ||
      ((x3>>SP_ACCURACY) == (nx>>SP_ACCURACY) && (y3>>SP_ACCURACY) == (ny>>SP_ACCURACY)))
    return;
  Sint32 nz = (z1+z2+z3)/3;
  Sint32 nu = (u1+u2+u3)/3;
  Sint32 nv = (v1+v2+v3)/3;
  draw_pixel_tex_ztest_zset_alpha(nx>>SP_ACCURACY,ny>>SP_ACCURACY,nz,
                                  nu>>SP_ACCURACY,nv>>SP_ACCURACY,color);
  sp_intern_recursive_Triangle_tex_ztest_zset_alpha(x1,y1,z1,u1,v1,
                                                    x2,y2,z2,u2,v2,
                                                    nx,ny,nz,nu,nv,color);
  sp_intern_recursive_Triangle_tex_ztest_zset_alpha(x2,y2,z2,u2,v2,
                                                    x3,y3,z3,u3,v3,
                                                    nx,ny,nz,nu,nv,color);
  sp_intern_recursive_Triangle_tex_ztest_zset_alpha(x1,y1,z1,u1,v1,
                                                    x3,y3,z3,u3,v3,
                                                    nx,ny,nz,nu,nv,color);
}


void sp_intern_Triangle_tex_ztest_zset_alpha(Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color)
{
  sp_intern_recursive_Triangle_tex_ztest_zset_alpha(x1<<SP_ACCURACY,y1<<SP_ACCURACY,z1,u1<<SP_ACCURACY,v1<<SP_ACCURACY,
                                                    x2<<SP_ACCURACY,y2<<SP_ACCURACY,z2,u2<<SP_ACCURACY,v2<<SP_ACCURACY,
                                                    x3<<SP_ACCURACY,y3<<SP_ACCURACY,z3,u3<<SP_ACCURACY,v3<<SP_ACCURACY,color);
}*/

inline void sp_intern_Triangle_tex_ztest_zset_alpha( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	int div = y2 - y1;
	Sint32 mul = y3 - y1;
	Sint32 mul32 = mul * one_over_x( div );
	Sint32 x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 z4 = z1 + mul * z_div( z2 - z1, div );
	Sint32 u4 = u1 + ( ( u2 - u1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 v4 = v1 + ( ( v2 - v1 ) * mul32 >> SP_PRIM_ACCURACY );

	/*draw_pixel_tex_ztest_zset_alpha(x1,y1,z1,u1,v1,color);
	vert_tex_ztest_zset_alpha(x1,x1,y1,z1,z1,u1,u1,v1,v1,
	                          x3,x4,y3,z3,z4,u3,u4,v3,v4,color);
	if (x3 < x4)
	  horz_tex_zset_zset_alpha(x3,x4,y3,z3,z4,u3,u4,v3,v4,color);
	else
	  horz_tex_zset_zset_alpha(x4,x3,y3,z4,z3,u4,u3,v4,v3,color);
	vert_tex_ztest_zset_alpha(x3,x4,y3,z3,z4,u3,u4,v3,v4,
	                          x2,x2,y2,z2,z2,u2,u2,v2,v2,color);
	draw_pixel_tex_ztest_zset_alpha(x2,y2,z2,u2,v2,color);
	*/


	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 ul = u1 << SP_PRIM_ACCURACY;
	Sint32 vl = v1 << SP_PRIM_ACCURACY;
	Sint32 zl = z1;

	mul = one_over_x( y1 - y2 );
	Sint32 sX_l = ( x1 - x2 ) * mul;
	Sint32 sU_l = ( u1 - u2 ) * mul;
	Sint32 sV_l = ( v1 - v2 ) * mul;
	Sint32 sZ_l = z_div( z1 - z2, y1 - y2 );

	Sint32 xr = xl;
	Sint32 ur = ul;
	Sint32 vr = vl;
	Sint32 zr = zl;
	mul = one_over_x( y1 - y3 );
	Sint32 sX_r = ( x1 - x3 ) * mul;
	Sint32 sU_r = ( u1 - u3 ) * mul;
	Sint32 sV_r = ( v1 - v3 ) * mul;
	Sint32 sZ_r = z_div( z1 - z3, y1 - y3 );

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			ul += sU_l * diff;
			vl += sV_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
			zr += sZ_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 mul = one_over_x( x4 - x3 );
		Sint32 sU = ( u4 - u3 ) * mul;
		Sint32 sV = ( v4 - v3 ) * mul;
		Sint32 sZ = z_div( z4 - z3, x4 - x3 );
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_ztest_zset_alpha( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
												xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
		else
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_ztest_zset_alpha( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
												xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	ur = u3 << SP_PRIM_ACCURACY;
	vr = v3 << SP_PRIM_ACCURACY;
	zr = z3;
	mul = one_over_x( y2 - y3 );
	sX_r = ( x2 - x3 ) * mul;
	sU_r = ( u2 - u3 ) * mul;
	sV_r = ( v2 - v3 ) * mul;
	sZ_r = z_div( z2 - z3, y2 - y3 );

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	mul = one_over_x( x4 - x3 );
	Sint32 sU = ( u4 - u3 ) * mul;
	Sint32 sV = ( v4 - v3 ) * mul;
	Sint32 sZ = z_div( z4 - z3, x4 - x3 );
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_ztest_zset_alpha( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
											xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	else
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_ztest_zset_alpha( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
											xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

inline void sp_intern_Triangle_tex_ztest_alpha( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	int div = y2 - y1;
	Sint32 mul = y3 - y1;
	Sint32 mul32 = mul * one_over_x( div );
	Sint32 x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 z4 = z1 + mul * z_div( z2 - z1, div );
	Sint32 u4 = u1 + ( ( u2 - u1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 v4 = v1 + ( ( v2 - v1 ) * mul32 >> SP_PRIM_ACCURACY );

	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 ul = u1 << SP_PRIM_ACCURACY;
	Sint32 vl = v1 << SP_PRIM_ACCURACY;
	Sint32 zl = z1;

	mul = one_over_x( y1 - y2 );
	Sint32 sX_l = ( x1 - x2 ) * mul;
	Sint32 sU_l = ( u1 - u2 ) * mul;
	Sint32 sV_l = ( v1 - v2 ) * mul;
	Sint32 sZ_l = z_div( z1 - z2, y1 - y2 );

	Sint32 xr = xl;
	Sint32 ur = ul;
	Sint32 vr = vl;
	Sint32 zr = zl;
	mul = one_over_x( y1 - y3 );
	Sint32 sX_r = ( x1 - x3 ) * mul;
	Sint32 sU_r = ( u1 - u3 ) * mul;
	Sint32 sV_r = ( v1 - v3 ) * mul;
	Sint32 sZ_r = z_div( z1 - z3, y1 - y3 );

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			ul += sU_l * diff;
			vl += sV_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
			zr += sZ_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 mul = one_over_x( x4 - x3 );
		Sint32 sU = ( u4 - u3 ) * mul;
		Sint32 sV = ( v4 - v3 ) * mul;
		Sint32 sZ = z_div( z4 - z3, x4 - x3 );
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_ztest_alpha( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
										   xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
		else
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_ztest_alpha( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
										   xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	ur = u3 << SP_PRIM_ACCURACY;
	vr = v3 << SP_PRIM_ACCURACY;
	zr = z3;
	mul = one_over_x( y2 - y3 );
	sX_r = ( x2 - x3 ) * mul;
	sU_r = ( u2 - u3 ) * mul;
	sV_r = ( v2 - v3 ) * mul;
	sZ_r = z_div( z2 - z3, y2 - y3 );

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	mul = one_over_x( x4 - x3 );
	Sint32 sU = ( u4 - u3 ) * mul;
	Sint32 sV = ( v4 - v3 ) * mul;
	Sint32 sZ = z_div( z4 - z3, x4 - x3 );
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_ztest_alpha( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
									   xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	else
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_ztest_alpha( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
									   xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

inline void sp_intern_Triangle_tex_zset_alpha( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	int div = y2 - y1;
	Sint32 mul = y3 - y1;
	Sint32 mul32 = mul * one_over_x( div );
	Sint32 x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 z4 = z1 + mul * z_div( z2 - z1, div );
	Sint32 u4 = u1 + ( ( u2 - u1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 v4 = v1 + ( ( v2 - v1 ) * mul32 >> SP_PRIM_ACCURACY );

	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 ul = u1 << SP_PRIM_ACCURACY;
	Sint32 vl = v1 << SP_PRIM_ACCURACY;
	Sint32 zl = z1;

	mul = one_over_x( y1 - y2 );
	Sint32 sX_l = ( x1 - x2 ) * mul;
	Sint32 sU_l = ( u1 - u2 ) * mul;
	Sint32 sV_l = ( v1 - v2 ) * mul;
	Sint32 sZ_l = z_div( z1 - z2, y1 - y2 );

	Sint32 xr = xl;
	Sint32 ur = ul;
	Sint32 vr = vl;
	Sint32 zr = zl;
	mul = one_over_x( y1 - y3 );
	Sint32 sX_r = ( x1 - x3 ) * mul;
	Sint32 sU_r = ( u1 - u3 ) * mul;
	Sint32 sV_r = ( v1 - v3 ) * mul;
	Sint32 sZ_r = z_div( z1 - z3, y1 - y3 );

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			ul += sU_l * diff;
			vl += sV_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
			zr += sZ_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 mul = one_over_x( x4 - x3 );
		Sint32 sU = ( u4 - u3 ) * mul;
		Sint32 sV = ( v4 - v3 ) * mul;
		Sint32 sZ = z_div( z4 - z3, x4 - x3 );
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_zset_alpha( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
										  xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
		else
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_zset_alpha( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
										  xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	ur = u3 << SP_PRIM_ACCURACY;
	vr = v3 << SP_PRIM_ACCURACY;
	zr = z3;
	mul = one_over_x( y2 - y3 );
	sX_r = ( x2 - x3 ) * mul;
	sU_r = ( u2 - u3 ) * mul;
	sV_r = ( v2 - v3 ) * mul;
	sZ_r = z_div( z2 - z3, y2 - y3 );

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	mul = one_over_x( x4 - x3 );
	Sint32 sU = ( u4 - u3 ) * mul;
	Sint32 sV = ( v4 - v3 ) * mul;
	Sint32 sZ = z_div( z4 - z3, x4 - x3 );
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_zset_alpha( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
									  xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	else
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_zset_alpha( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
									  xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			zl += sZ_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			zr += sZ_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

inline void sp_intern_Triangle_tex_alpha( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	int div = y2 - y1;
	Sint32 mul = y3 - y1;
	Sint32 mul32 = mul * one_over_x( div );
	Sint32 x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 u4 = u1 + ( ( u2 - u1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 v4 = v1 + ( ( v2 - v1 ) * mul32 >> SP_PRIM_ACCURACY );

	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 ul = u1 << SP_PRIM_ACCURACY;
	Sint32 vl = v1 << SP_PRIM_ACCURACY;

	mul = one_over_x( y1 - y2 );
	Sint32 sX_l = ( x1 - x2 ) * mul;
	Sint32 sU_l = ( u1 - u2 ) * mul;
	Sint32 sV_l = ( v1 - v2 ) * mul;

	Sint32 xr = xl;
	Sint32 ur = ul;
	Sint32 vr = vl;
	mul = one_over_x( y1 - y3 );
	Sint32 sX_r = ( x1 - x3 ) * mul;
	Sint32 sU_r = ( u1 - u3 ) * mul;
	Sint32 sV_r = ( v1 - v3 ) * mul;

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			ul += sU_l * diff;
			vl += sV_l * diff;
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 mul = one_over_x( x4 - x3 );
		Sint32 sU = ( u4 - u3 ) * mul;
		Sint32 sV = ( v4 - v3 ) * mul;
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_alpha( xl >> SP_PRIM_ACCURACY, ul, vl,
									 xr >> SP_PRIM_ACCURACY, ur, vr, y, color, sU, sV );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
			}
		}
		else
		{
			for ( y = y1; y < y3; y++ )
			{
				draw_line_tex_alpha( xr >> SP_PRIM_ACCURACY, ur, vr,
									 xl >> SP_PRIM_ACCURACY, ul, vl, y, color, sU, sV );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
			}
		}
	}

	xr = x3 << SP_PRIM_ACCURACY;
	ur = u3 << SP_PRIM_ACCURACY;
	vr = v3 << SP_PRIM_ACCURACY;
	mul = one_over_x( y2 - y3 );
	sX_r = ( x2 - x3 ) * mul;
	sU_r = ( u2 - u3 ) * mul;
	sV_r = ( v2 - v3 ) * mul;

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	mul = one_over_x( x4 - x3 );
	Sint32 sU = ( u4 - u3 ) * mul;
	Sint32 sV = ( v4 - v3 ) * mul;
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_alpha( xl >> SP_PRIM_ACCURACY, ul, vl,
								 xr >> SP_PRIM_ACCURACY, ur, vr, y, color, sU, sV );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
		}
	}
	else
	{
		for ( y = y3; y <= y2; y++ )
		{
			draw_line_tex_alpha( xr >> SP_PRIM_ACCURACY, ur, vr,
								 xl >> SP_PRIM_ACCURACY, ul, vl, y, color, sU, sV );
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
		}
	}
	SDL_UnlockSurface( spTarget );
}

PREFIX int spTriangle_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
{
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
				sp_intern_Triangle_tex_alpha           ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
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
				sp_intern_Triangle_tex           ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, color );
		}
	}
	return result;
}

PREFIX int spQuad( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 x4, Sint32 y4, Sint32 z4, Uint32 color )
{
	int result = 0;
	if ( result = spTriangle( x1, y1, z1, x2, y2, z2, x3, y3, z3, color ) )
		return result | spTriangle( x1, y1, z1, x3, y3, z3, x4, y4, z4, color );
	return 0;
}

inline void sp_intern_Quad_tex_ztest_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Uint32 color )
{
	int y;
	if ( y3 > y4 )
	{
		if ( y3 < 0 )
			return;
	}
	else if ( y4 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	SDL_LockSurface( spTarget );

	Sint32 div = y4 - y1;
	Sint32 mul = y2 - y1;
	Sint32 mul32 = mul * one_over_x( div );
	Sint32 xt = x1 + ( ( x4 - x1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 zt = z1 + mul * z_div( z4 - z1, div );
	Sint32 ut = u1 + ( ( u4 - u1 ) * mul32 >> SP_PRIM_ACCURACY );
	Sint32 vt = v1 + ( ( v4 - v1 ) * mul32 >> SP_PRIM_ACCURACY );

	Sint32 xl = x1 << SP_PRIM_ACCURACY;
	Sint32 ul = u1 << SP_PRIM_ACCURACY;
	Sint32 vl = v1 << SP_PRIM_ACCURACY;
	Sint32 zl = z1;

	mul = one_over_x( y1 - y4 );
	Sint32 sX_l = ( x1 - x4 ) * mul;
	Sint32 sU_l = ( u1 - u4 ) * mul;
	Sint32 sV_l = ( v1 - v4 ) * mul;
	Sint32 sZ_l = z_div( z1 - z4, y1 - y4 );

	Sint32 xr = xl;
	Sint32 ur = ul;
	Sint32 vr = vl;
	Sint32 zr = zl;
	mul = one_over_x( y1 - y2 );
	Sint32 sX_r = ( x1 - x2 ) * mul;
	Sint32 sU_r = ( u1 - u2 ) * mul;
	Sint32 sV_r = ( v1 - v2 ) * mul;
	Sint32 sZ_r = z_div( z1 - z2, y1 - y2 );

	if ( y2 < 0 )
	{
		int diff = y2 - y1;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		zl += sZ_l * diff;
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		zr += sZ_r * diff;
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			ul += sU_l * diff;
			vl += sV_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
			zr += sZ_r * diff;
		}
		if ( y2 >= spTargetY )
			y2 = spTargetY - 1;

		Sint32 mul = one_over_x( xt - x2 );
		Sint32 sU = ( ut - u2 ) * mul;
		Sint32 sV = ( vt - v2 ) * mul;
		Sint32 sZ = z_div( zt - z2, xt - x2 );
		if ( xt < x2 )
		{
			for ( y = y1; y < y2; y++ )
			{
				draw_line_tex_ztest_zset( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
										  xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
		else
		{
			for ( y = y1; y < y2; y++ )
			{
				draw_line_tex_ztest_zset( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
										  xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
	}

	xr = x2 << SP_PRIM_ACCURACY;
	ur = u2 << SP_PRIM_ACCURACY;
	vr = v2 << SP_PRIM_ACCURACY;
	zr = z2;
	mul = one_over_x( y3 - y2 );
	sX_r = ( x3 - x2 ) * mul;
	sU_r = ( u3 - u2 ) * mul;
	sV_r = ( v3 - v2 ) * mul;
	sZ_r = z_div( z3 - z2, y3 - y2 );

	if ( y4 < y3 )
	{
		if ( y4 < 0 )
		{
			int diff = -y4;
			y4 = 0;
			xl += sX_l * diff;
			ul += sU_l * diff;
			vl += sV_l * diff;
			zl += sZ_l * diff;
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
			zr += sZ_r * diff;
		}

		if ( y4 >= spTargetY )
			y4 = spTargetY - 1;

		mul = y4 - y2;
		div = y3 - y2;
		mul32 = mul * one_over_x( div );
		xt = x2 + ( ( x3 - x2 ) * mul32 >> SP_PRIM_ACCURACY );
		zt = z2 + mul * z_div( z3 - z2, div );
		ut = u2 + ( ( u3 - u2 ) * mul32 >> SP_PRIM_ACCURACY );
		vt = v2 + ( ( v3 - v2 ) * mul32 >> SP_PRIM_ACCURACY );

		if ( xt > x4 )
		{
			for ( y = y2; y <= y4; y++ )
			{
				mul = one_over_x( xl - xr >> SP_PRIM_ACCURACY );
				Sint32 sU = ( ul - ur >> SP_PRIM_ACCURACY ) * mul;
				Sint32 sV = ( vl - vr >> SP_PRIM_ACCURACY ) * mul;
				Sint32 sZ = z_div( zl - zr >> SP_PRIM_ACCURACY, xl - xr >> SP_PRIM_ACCURACY );
				draw_line_tex_ztest_zset( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
										  xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
		else
		{
			for ( y = y2; y <= y4; y++ )
			{
				mul = one_over_x( xl - xr >> SP_PRIM_ACCURACY );
				Sint32 sU = ( ul - ur >> SP_PRIM_ACCURACY ) * mul;
				Sint32 sV = ( vl - vr >> SP_PRIM_ACCURACY ) * mul;
				Sint32 sZ = z_div( zl - zr >> SP_PRIM_ACCURACY, xl - xr >> SP_PRIM_ACCURACY );
				draw_line_tex_ztest_zset( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
										  xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}

		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;


		xl = x4 << SP_PRIM_ACCURACY;
		ul = u4 << SP_PRIM_ACCURACY;
		vl = v4 << SP_PRIM_ACCURACY;
		zl = z4;
		mul = one_over_x( y3 - y4 );
		sX_l = ( x3 - x4 ) * mul;
		sU_l = ( u3 - u4 ) * mul;
		sV_l = ( v3 - v4 ) * mul;
		sZ_l = z_div( z3 - z4, y3 - y4 );

		/*mul = one_over_x(xt-x4);
		Sint32 sU = (ut-u4)*mul;
		Sint32 sV = (vt-v4)*mul;
		Sint32 sZ = z_div(zt-z4,xt-x4);*/
		if ( xt > x4 )
		{
			for ( y = y4; y <= y3; y++ )
			{
				mul = one_over_x( xl - xr >> SP_PRIM_ACCURACY );
				Sint32 sU = ( ul - ur >> SP_PRIM_ACCURACY ) * mul;
				Sint32 sV = ( vl - vr >> SP_PRIM_ACCURACY ) * mul;
				Sint32 sZ = z_div( zl - zr >> SP_PRIM_ACCURACY, xl - xr >> SP_PRIM_ACCURACY );
				draw_line_tex_ztest_zset( xl >> SP_PRIM_ACCURACY, zl, ul, vl,
										  xr >> SP_PRIM_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
		else
		{
			for ( y = y4; y <= y3; y++ )
			{
				mul = one_over_x( xl - xr >> SP_PRIM_ACCURACY );
				Sint32 sU = ( ul - ur >> SP_PRIM_ACCURACY ) * mul;
				Sint32 sV = ( vl - vr >> SP_PRIM_ACCURACY ) * mul;
				Sint32 sZ = z_div( zl - zr >> SP_PRIM_ACCURACY, xl - xr >> SP_PRIM_ACCURACY );
				draw_line_tex_ztest_zset( xr >> SP_PRIM_ACCURACY, zr, ur, vr,
										  xl >> SP_PRIM_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				zl += sZ_l;
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				zr += sZ_r;
			}
		}
	}
	SDL_UnlockSurface( spTarget );
}


PREFIX int spQuad_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Uint32 color )
{
	int result = 0;
	if ( spQuadQuali )
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
		return 0;
	}
	if ( result = spTriangle_tex( x1, y1, z1, u1, v1,
								  x2, y2, z2, u2, v2,
								  x3, y3, z3, u3, v3, color ) )
		return spTriangle_tex( x1, y1, z1, u1, v1,
							   x3, y3, z3, u3, v3,
							   x4, y4, z4, u4, v4, color ) | result;
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
		spZBuffer = ( Sint32* )malloc( spTargetX * spTargetY * sizeof( Sint32 ) );
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
	if ( spZBuffer )
		for ( i = 0; i < spTargetX * spTargetY; i++ )
			spZBuffer[i] = SP_MAX_NEGATIVE;
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
	if ( z >= 0 )
		return;
	if ( spAlphaTest )
	{
		if ( spZSet )
		{
			if ( spZTest )
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
						if ( pixel[( u + addu ) + ( v + addv )*surface->w] != SP_ALPHA_COLOR && z > spZBuffer[( x + u ) + ( y + v )*spTargetX] )
						{
							spTargetPixel[( x + u ) + ( y + v )*spTargetX] = pixel[( u + addu ) + ( v + addv ) * surface->w];
							spZBuffer[( x + u ) + ( y + v )*spTargetX] = z;
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
						spZBuffer[( x + u ) + ( y + v )*spTargetX] = z;
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
						if ( pixel[( u + addu ) + ( v + addv )*surface->w] != SP_ALPHA_COLOR && z > spZBuffer[( x + u ) + ( y + v )*spTargetX] )
							spTargetPixel[( x + u ) + ( y + v )*spTargetX] = pixel[( u + addu ) + ( v + addv ) * surface->w];
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
						if ( z > spZBuffer[( x + u ) + ( y + v )*spTargetX] )
						{
							spTargetPixel[( x + u ) + ( y + v )*spTargetX] = pixel[( u + addu ) + ( v + addv ) * surface->w];
							spZBuffer[( x + u ) + ( y + v )*spTargetX] = z;
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
						spZBuffer[( x + u ) + ( y + v )*spTargetX] = z;
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
						if ( z > spZBuffer[( x + u ) + ( y + v )*spTargetX] )
							spTargetPixel[( x + u ) + ( y + v )*spTargetX] = pixel[( u + addu ) + ( v + addv ) * surface->w];
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

PREFIX void spBlitSurfacePart( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h )
{
	if ( z >= 0 )
		return;
	if ( spAlphaTest )
	{
		if ( spZSet )
		{
			if ( spZTest )
			{
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
				if ( x2 < 0 )
					return;
				if ( x2 >= spTargetX )
				{
					w -= x2 - spTargetX + 1;
					x2 = spTargetX - 1;
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
				if ( y2 < 0 )
					return;
				if ( y2 >= spTargetY )
				{
					h -= y2 - spTargetY + 1;
					y2 = spTargetY - 1;
				}
				SDL_Surface* oldTexture = spTexture;
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
				spBindTexture( oldTexture );
				SDL_UnlockSurface( spTarget );
			}
			else
			{
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
				if ( x2 < 0 )
					return;
				if ( x2 >= spTargetX )
				{
					w -= x2 - spTargetX + 1;
					x2 = spTargetX - 1;
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
				if ( y2 < 0 )
					return;
				if ( y2 >= spTargetY )
				{
					h -= y2 - spTargetY + 1;
					y2 = spTargetY - 1;
				}
				int u = sx;
				for ( x = x1; x < x2; x++ )
					for ( y = y1; y < y2; y++ )
						spZBuffer[x + y * spTargetX] = z;
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
				if ( x2 < 0 )
					return;
				if ( x2 >= spTargetX )
				{
					w -= x2 - spTargetX + 1;
					x2 = spTargetX - 1;
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
				if ( y2 < 0 )
					return;
				if ( y2 >= spTargetY )
				{
					h -= y2 - spTargetY + 1;
					y2 = spTargetY - 1;
				}
				SDL_Surface* oldTexture = spTexture;
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
				spBindTexture( oldTexture );
				SDL_UnlockSurface( spTarget );
			}
			else
			{
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
				if ( x2 < 0 )
					return;
				if ( x2 >= spTargetX )
				{
					w -= x2 - spTargetX + 1;
					x2 = spTargetX - 1;
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
				if ( y2 < 0 )
					return;
				if ( y2 >= spTargetY )
				{
					h -= y2 - spTargetY + 1;
					y2 = spTargetY - 1;
				}
				SDL_Surface* oldTexture = spTexture;
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
				spBindTexture( oldTexture );
				SDL_UnlockSurface( spTarget );
			}
			else
			{
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
				if ( x2 < 0 )
					return;
				if ( x2 >= spTargetX )
				{
					w -= x2 - spTargetX + 1;
					x2 = spTargetX - 1;
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
				if ( y2 < 0 )
					return;
				if ( y2 >= spTargetY )
				{
					h -= y2 - spTargetY + 1;
					y2 = spTargetY - 1;
				}
				int u = sx;
				for ( x = x1; x < x2; x++ )
					for ( y = y1; y < y2; y++ )
						spZBuffer[x + y * spTargetX] = z;
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
				if ( x2 < 0 )
					return;
				if ( x2 >= spTargetX )
				{
					w -= x2 - spTargetX + 1;
					x2 = spTargetX - 1;
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
				if ( y2 < 0 )
					return;
				if ( y2 >= spTargetY )
				{
					h -= y2 - spTargetY + 1;
					y2 = spTargetY - 1;
				}
				SDL_Surface* oldTexture = spTexture;
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
				spBindTexture( oldTexture );
				SDL_UnlockSurface( spTarget );
			}
			else
			{
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

PREFIX void spLine( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Uint32 color )
{
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return;
	//Dirty bugfix...
	if ( x1 == x2 )
	{
		if ( x1 < 0 || x1 >= spTargetX )
			return;
		if ( y1 == y2 )
		{
			if ( y1 < 0 || y1 >= spTargetY )
				return;
			if ( spZSet )
			{
				if ( spZTest )
					draw_pixel_ztest_zset( x1, y1, z1, color );
				else
					draw_pixel_zset( x1, y1, z1, color );
			}
			else
			{
				if ( spZTest )
					draw_pixel_ztest( x1, y1, z1, color );
				else
					draw_pixel( x1, y1, color );
			}
		}
		else if ( y1 < y2 )
		{
			if ( y2 < 0 || y1 >= spTargetY )
				return;
			if ( y1 < 0 )
			{
				z1 += spDiv( z1 - z2, y2 - y1 ) * y1;
				y1 = 0;
			}
			if ( y2 >= spTargetY )
				y2 = spTargetY - 1;
			if ( spZSet )
			{
				if ( spZTest )
				{
					Sint32 dz = spDiv( z1 - z2, y1 - y2 );
					for ( ; y1 <= y2; y1++ )
					{
						draw_pixel_ztest_zset( x1, y1, z1, color );
						z1 += dz;
					}
				}
				else
				{
					Sint32 dz = spDiv( z1 - z2, y1 - y2 );
					for ( ; y1 <= y2; y1++ )
					{
						draw_pixel_zset( x1, y1, z1, color );
						z1 += dz;
					}
				}
			}
			else
			{
				if ( spZTest )
				{
					Sint32 dz = spDiv( z1 - z2, y1 - y2 );
					for ( ; y1 <= y2; y1++ )
					{
						draw_pixel_ztest( x1, y1, z1, color );
						z1 += dz;
					}
				}
				else
					for ( ; y1 <= y2; y1++ )
						draw_pixel( x1, y1, color );
			}
		}
		else
		{
			if ( y1 < 0 || y2 >= spTargetY )
				return;
			if ( y2 < 0 )
			{
				z2 += spDiv( z2 - z1, y1 - y2 ) * y2;
				y2 = 0;
			}
			if ( y1 >= spTargetY )
				y1 = spTargetY - 1;
			if ( spZSet )
			{
				if ( spZTest )
				{
					Sint32 dz = spDiv( z1 - z2, y1 - y2 );
					for ( ; y2 <= y1; y2++ )
					{
						draw_pixel_ztest_zset( x1, y2, z2, color );
						z2 += dz;
					}
				}
				else
				{
					Sint32 dz = spDiv( z1 - z2, y1 - y2 );
					for ( ; y2 <= y1; y2++ )
					{
						draw_pixel_zset( x1, y2, z2, color );
						z2 += dz;
					}
				}
			}
			else
			{
				if ( spZTest )
				{
					Sint32 dz = spDiv( z1 - z2, y1 - y2 );
					for ( ; y2 <= y1; y2++ )
					{
						draw_pixel_ztest( x1, y2, z2, color );
						z2 += dz;
					}
				}
				else
					for ( ; y2 <= y1; y2++ )
						draw_pixel( x1, y2, color );
			}
		}
		return;
	}
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
	if ( spZSet )
	{
		if ( spZTest )
		{
			//ZSet, ZTest
			Sint32 dx = abs( x2 - x1 );
			Sint32 dy = abs( y2 - y1 );
			Sint32 div = spMax( x2 - x1, y2 - y1 );
			if ( div == 0 ) //x1==x2 and y1==y2
			{
				if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
					draw_pixel_ztest_zset( x1, y1, z1, color );
				return;
			}
			Sint32 dz = ( z2 - z1 ) / div;
			Sint32 sx = -1;
			Sint32 sy = -1;
			if ( x1 < x2 )
				sx = 1;
			if ( y1 < y2 )
				sy = 1;
			int err = dx - dy;
			SDL_LockSurface( spTarget );
			while ( 1 )
			{
				if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
					draw_pixel_ztest_zset( x1, y1, z1, color );
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
			SDL_UnlockSurface( spTarget );
		}
		else
		{
			//ZSet, No ZTest
			Sint32 dx = abs( x2 - x1 );
			Sint32 dy = abs( y2 - y1 );
			Sint32 div = spMax( x2 - x1, y2 - y1 );
			if ( div == 0 ) //x1==x2 and y1==y2
			{
				if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
					draw_pixel_zset( x1, y1, z1, color );
				return;
			}
			Sint32 dz = ( z2 - z1 ) / div;
			Sint32 sx = -1;
			Sint32 sy = -1;
			if ( x1 < x2 )
				sx = 1;
			if ( y1 < y2 )
				sy = 1;
			int err = dx - dy;
			SDL_LockSurface( spTarget );
			while ( 1 )
			{
				if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
					draw_pixel_zset( x1, y1, z1, color );
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
			SDL_UnlockSurface( spTarget );
		}
	}
	else
	{
		if ( spZTest )
		{
			//No ZSet, ZTest
			Sint32 dx = abs( x2 - x1 );
			Sint32 dy = abs( y2 - y1 );
			Sint32 div = spMax( x2 - x1, y2 - y1 );
			if ( div == 0 ) //x1==x2 and y1==y2
			{
				if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
					draw_pixel_ztest( x1, y1, z1, color );
				return;
			}
			Sint32 dz = ( z2 - z1 ) / div;
			Sint32 sx = -1;
			Sint32 sy = -1;
			if ( x1 < x2 )
				sx = 1;
			if ( y1 < y2 )
				sy = 1;
			int err = dx - dy;
			SDL_LockSurface( spTarget );
			while ( 1 )
			{
				if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
					draw_pixel_ztest( x1, y1, z1, color );
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
			SDL_UnlockSurface( spTarget );
		}
		else
		{
			//No ZSet, No ZTest
			Sint32 dx = abs( x2 - x1 );
			Sint32 dy = abs( y2 - y1 );
			Sint32 sx = -1;
			Sint32 sy = -1;
			if ( x1 < x2 )
				sx = 1;
			if ( y1 < y2 )
				sy = 1;
			int err = dx - dy;
			SDL_LockSurface( spTarget );
			while ( 1 )
			{
				if ( x1 >= 0 && x1 < spTargetX && y1 >= 0 && y1 < spTargetY )
					draw_pixel( x1, y1, color );
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
			SDL_UnlockSurface( spTarget );
		}
	}
}

PREFIX void spRectangle( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Uint32 color )
{
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
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
					draw_pixel_ztest_zset( x1, y, z, color );
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
					draw_pixel_zset( x1, y, z, color );
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
					draw_pixel_ztest( x1, y, z, color );
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
					draw_pixel( x1, y, color );
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
	if ( spZSet )
	{
		if ( spZTest )
		{
			//top border
			for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
				for ( cx = x1; cx <= x2; cx++ )
					draw_pixel_ztest_zset( cx, cy, z, color );
			//left & right border
			for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
			{
				for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
					draw_pixel_ztest_zset( cx, cy, z, color );
				for ( cx = x - addu + w - bx; cx <= x2; cx++ )
					draw_pixel_ztest_zset( cx, cy, z, color );
			}
			//bottom border
			for ( ; cy <= y2; cy++ )
				for ( cx = x1; cx <= x2; cx++ )
					draw_pixel_ztest_zset( cx, cy, z, color );
		}
		else
		{
			//top border
			for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
				for ( cx = x1; cx <= x2; cx++ )
					draw_pixel_zset( cx, cy, z, color );
			//left & right border
			for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
			{
				for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
					draw_pixel_zset( cx, cy, z, color );
				for ( cx = x - addu + w - bx; cx <= x2; cx++ )
					draw_pixel_zset( cx, cy, z, color );
			}
			//bottom border
			for ( ; cy <= y2; cy++ )
				for ( cx = x1; cx <= x2; cx++ )
					draw_pixel_zset( cx, cy, z, color );
		}
	}
	else
	{
		if ( spZTest )
		{
			//top border
			for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
				for ( cx = x1; cx <= x2; cx++ )
					draw_pixel_ztest( cx, cy, z, color );
			//left & right border
			for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
			{
				for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
					draw_pixel_ztest( cx, cy, z, color );
				for ( cx = x - addu + w - bx; cx <= x2; cx++ )
					draw_pixel_ztest( cx, cy, z, color );
			}
			//bottom border
			for ( ; cy <= y2; cy++ )
				for ( cx = x1; cx <= x2; cx++ )
					draw_pixel_ztest( cx, cy, z, color );
		}
		else
		{
			//top border
			for ( cy = y1; cy < y - addv + by && cy <= y2; cy++ )
				for ( cx = x1; cx <= x2; cx++ )
					draw_pixel( cx, cy, color );
			//left & right border
			for ( ; cy < y - addv + h - by && cy <= y2; cy++ )
			{
				for ( cx = x1; cx <= x - addu + bx && cx <= x2; cx++ )
					draw_pixel( cx, cy, color );
				for ( cx = x - addu + w - bx; cx <= x2; cx++ )
					draw_pixel( cx, cy, color );
			}
			//bottom border
			for ( ; cy <= y2; cy++ )
				for ( cx = x1; cx <= x2; cx++ )
					draw_pixel( cx, cy, color );
		}
	}
	SDL_UnlockSurface( spTarget );
}

PREFIX void spEllipse( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Uint32 color )

{
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
		return;
	if ( x - rx >= spTargetX ) return;
	if ( y - ry >= spTargetY ) return;
	if ( x + rx < 0 )          return;
	if ( y + ry < 0 )          return;
	Sint32 rxl = -rx;
	Sint32 rxr = +rx;
	Sint32 ryl = -ry;
	Sint32 ryr = +ry;
	Sint32 RX = rx * rx;
	Sint32 RY = ry * ry;
	Sint32 RR = RX * RY;
	if ( x + rxr >= spTargetX ) rxr = spTargetX - 1 - x;
	if ( y + ryr >= spTargetY ) ryr = spTargetY - 1 - y;
	if ( x + rxl < 0 )          rxl = -x;
	if ( y + ryl < 0 )          ryl = -y;
	Sint32 x1 = x;
	Sint32 y1 = y;
	Sint32 XX_mul = spDiv( rx * rx << SP_ACCURACY, ry * ry << SP_ACCURACY );
	SDL_LockSurface( spTarget );
	if ( spZSet )
	{
		if ( spZTest )
		{
			//x*x*ry*ry+y*y*rx*rx = rx*rx*ry*ry
			for ( y = ryl; y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_ztest_zset( x1 + x, y1 + y, z, color );
			}
		}
		else
		{
			//x*x*ry*ry+y*y*rx*rx = rx*rx*ry*ry
			for ( y = ryl; y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_zset( x1 + x, y1 + y, z, color );
			}
		}
	}
	else
	{
		if ( spZTest )
		{
			//x*x*ry*ry+y*y*rx*rx = rx*rx*ry*ry
			for ( y = ryl; y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_ztest( x1 + x, y1 + y, z, color );
			}
		}
		else
		{
			//x*x*ry*ry+y*y*rx*rx = rx*rx*ry*ry
			for ( y = ryl; y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel( x1 + x, y1 + y, color );
			}
		}
	}
	SDL_UnlockSurface( spTarget );
}

PREFIX void spEllipseBorder( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Sint32 bx, Sint32 by, Uint32 color )
{
	if ( ( bx >= rx ) || ( by >= ry ) )
	{
		spEllipse( x, y, z, rx, ry, color );
		return;
	}
	if ( spAlphaTest && color == SP_ALPHA_COLOR )
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
	if ( spZSet )
	{
		if ( spZTest )
		{
			//x*x*ry*ry+y*y*rx*rx = rx*rx*ry*ry
			//up
			for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_ztest_zset( x1 + x, y1 + y, z, color );
			}
			//middle
			for ( ; y <= ry - by && y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				Sint32 XXB = ( rx - bx ) * ( rx - bx ) - spMul( y * y, XXB_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x >= XXB && x <= rxr; x++ )
					draw_pixel_ztest_zset( x1 + x, y1 + y, z, color );
				for ( ; x * x <= XXB && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_ztest_zset( x1 + x, y1 + y, z, color );
			}
			//down
			for ( ; y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_ztest_zset( x1 + x, y1 + y, z, color );
			}
		}
		else
		{
			//x*x*ry*ry+y*y*rx*rx = rx*rx*ry*ry
			//up
			for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_zset( x1 + x, y1 + y, z, color );
			}
			//middle
			for ( ; y <= ry - by && y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				Sint32 XXB = ( rx - bx ) * ( rx - bx ) - spMul( y * y, XXB_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x >= XXB && x <= rxr; x++ )
					draw_pixel_zset( x1 + x, y1 + y, z, color );
				for ( ; x * x <= XXB && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_zset( x1 + x, y1 + y, z, color );
			}
			//down
			for ( ; y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_zset( x1 + x, y1 + y, z, color );
			}
		}
	}
	else
	{
		if ( spZTest )
		{
			//x*x*ry*ry+y*y*rx*rx = rx*rx*ry*ry
			//up
			for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_ztest( x1 + x, y1 + y, z, color );
			}
			//middle
			for ( ; y <= ry - by && y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				Sint32 XXB = ( rx - bx ) * ( rx - bx ) - spMul( y * y, XXB_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x >= XXB && x <= rxr; x++ )
					draw_pixel_ztest( x1 + x, y1 + y, z, color );
				for ( ; x * x <= XXB && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_ztest( x1 + x, y1 + y, z, color );
			}
			//down
			for ( ; y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel_ztest( x1 + x, y1 + y, z, color );
			}
		}
		else
		{
			//x*x*ry*ry+y*y*rx*rx = rx*rx*ry*ry
			//up
			for ( y = ryl; y <= -ry + by && y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel( x1 + x, y1 + y, color );
			}
			//middle
			for ( ; y <= ry - by && y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				Sint32 XXB = ( rx - bx ) * ( rx - bx ) - spMul( y * y, XXB_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x >= XXB && x <= rxr; x++ )
					draw_pixel( x1 + x, y1 + y, color );
				for ( ; x * x <= XXB && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel( x1 + x, y1 + y, color );
			}
			//down
			for ( ; y <= ryr; y++ )
			{
				Sint32 XX = rx * rx - spMul( y * y, XX_mul );
				for ( x = rxl; x * x >= XX && x <= rxr; x++ );
				for ( ; x * x <= XX && x <= rxr; x++ )
					draw_pixel( x1 + x, y1 + y, color );
			}
		}
	}
	SDL_UnlockSurface( spTarget );

	/*if (x-rx >= spTargetX) return;
	if (y-ry >= spTargetY) return;
	if (x+rx < 0)          return;
	if (y+ry < 0)          return;
	Sint32 rxl = -rx;
	Sint32 rxr = +rx;
	Sint32 ryl = -ry;
	Sint32 ryr = +ry;
	Sint32 rxlb = -rx+bx;
	Sint32 rxrb = +rx-bx;
	Sint32 rylb = -ry+by;
	Sint32 ryrb = +ry-by;
	Sint32 RX = rx*rx;
	Sint32 RY = ry*ry;
	Sint32 RR = RX*RY;
	Sint32 RXB = (rx-bx)*(rx-bx);
	Sint32 RYB = (ry-by)*(ry-by);
	Sint32 RRB = RXB*RYB;
	if (x+rxr >= spTargetX) rxr = spTargetX-1-x;
	if (y+ryr >= spTargetY) ryr = spTargetY-1-y;
	if (x+rxl < 0)          rxl = -x;
	if (y+ryl < 0)          ryl = -y;
	if (x+rxrb >= spTargetX) rxrb = spTargetX-1-x;
	if (y+ryrb >= spTargetY) ryrb = spTargetY-1-y;
	if (x+rxlb < 0)          rxlb = -x;
	if (y+rylb < 0)          rylb = -y;
	Sint32 x1 = x;
	Sint32 y1 = y;
	SDL_LockSurface(spTarget);
	if (spZSet)
	{
	  if (spZTest)
	  {
	    for (y = ryl;y<=ryr;y++)
	    {
	      for (x = rxl;x<=rxr;x++)
	        if (x*x*RY+y*y*RX <= RR &&
	            x*x*RYB+y*y*RXB >= RRB)
	          draw_pixel_ztest_zset(x1+x,y1+y,z,color);
	    }
	  }
	  else
	    for (x = rxl;x<=rxr;x++)
	      for (y = ryl;y<=ryr;y++)
	        if (x*x*RY+y*y*RX <= RR &&
	            x*x*RYB+y*y*RXB >= RRB)
	          draw_pixel_zset(x1+x,y1+y,z,color);
	}
	else
	{
	  if (spZTest)
	  {
	    for (x = rxl;x<=rxr;x++)
	      for (y = ryl;y<=ryr;y++)
	        if (x*x*RY+y*y*RX <= RR &&
	            x*x*RYB+y*y*RXB >= RRB)
	          draw_pixel_ztest(x1+x,y1+y,z,color);
	  }
	  else
	    for (x = rxl;x<=rxr;x++)
	      for (y = ryl;y<=ryr;y++)
	        if (x*x*RY+y*y*RX <= RR &&
	            x*x*RYB+y*y*RXB >= RRB)
	          draw_pixel(x1+x,y1+y,color);
	}
	SDL_UnlockSurface(spTarget);*/
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
				sp_intern_Triangle_tex_alpha           ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
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
				sp_intern_Triangle_tex           ( x1, y1, z1, u1, v1, x2, y2, z2, u2, v2, x3, y3, z3, u3, v3, 65535 );
		}
	}
}
PREFIX void spRotozoomSurfacePart( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, Sint32 zoomX, Sint32 zoomY, Sint32 angle )
{
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
	spBindTexture( oldTexture );
}

PREFIX void spSetHorizontalOrigin( Sint32 origin )
{
	spHorizontalOrigin = origin;
}

PREFIX void spSetVerticalOrigin( Sint32 origin )
{
	spVerticalOrigin = origin;
}
