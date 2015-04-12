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

/* This file is very bad style - and I know it! But this is the best
 * way to get the code fast and still understanable. The only
 * alternative I used before was massive copy and paste, which is
 * bollocks. */
 
#ifdef __SPARROW_INTERNAL_BLENDING__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#ifdef __GNUC__
			inline void draw_blending_line_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#ifdef __GNUC__
			inline void draw_blending_line_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZSET__
			#ifdef __GNUC__
			inline void draw_blending_line_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#else
			#ifdef __GNUC__
			inline void draw_blending_line_pattern( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_pattern( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#ifdef __GNUC__
			inline void draw_blending_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#ifdef __GNUC__
			inline void draw_blending_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZSET__
			#ifdef __GNUC__
			inline void draw_blending_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#else
			#ifdef __GNUC__
			inline void draw_blending_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#ifdef __GNUC__
			inline void draw_line_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#ifdef __GNUC__
			inline void draw_line_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZSET__
			#ifdef __GNUC__
			inline void draw_line_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#else
			#ifdef __GNUC__
			inline void draw_line_pattern( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending ) __attribute__((always_inline));
			#endif
			inline void draw_line_pattern( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#ifdef __GNUC__
			inline void draw_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#ifdef __GNUC__
			inline void draw_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZSET__
			#ifdef __GNUC__
			inline void draw_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending, Sint32 sZ )
		#else
			#ifdef __GNUC__
			inline void draw_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending ) __attribute__((always_inline));
			#endif
			inline void draw_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#endif
{
	if ( x2 < x1)
		return;
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 z = z1;
	#endif
	if ( x1 < 0 )
	{
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			z -= x1 * sZ;
		#endif
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;

	#ifdef __SPARROW_INTERNAL_PATTERN__
		int x;
		for ( x = x1; x <= x2; x++ )
		{
			#ifdef __SPARROW_INTERNAL_BLENDING__
				#ifdef __SPARROW_INTERNAL_ZBOTH__
					draw_pixel_blending_ztest_zset_pattern( x, y, z, color,spPattern, spBlending )
				#elif defined __SPARROW_INTERNAL_ZTEST__
					draw_pixel_blending_ztest_pattern( x, y, z, color,spPattern, spBlending )
				#elif defined __SPARROW_INTERNAL_ZSET__
					draw_pixel_blending_zset_pattern( x, y, z, color,spPattern, spBlending )
				#else
					draw_pixel_blending_pattern( x, y, color,spPattern, spBlending )
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_ZBOTH__
					draw_pixel_ztest_zset_pattern( x, y, z, color,spPattern )
				#elif defined __SPARROW_INTERNAL_ZTEST__
					draw_pixel_ztest_pattern( x, y, z, color,spPattern )
				#elif defined __SPARROW_INTERNAL_ZSET__
					draw_pixel_zset_pattern( x, y, z, color,spPattern )
				#else
					draw_pixel_pattern( x, y, color,spPattern )
				#endif
			#endif
			#ifndef __SPARROW_INTERNAL_ZNOTHING__			
				z += sZ;
			#endif
		}
	#else
		#ifdef __SPARROW_INTERNAL_BLENDING__
			int x;
			for ( x = x1; x <= x2; x++ )
			{
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				draw_pixel_blending_ztest_zset( x, y, z, color, spBlending)
			#elif defined __SPARROW_INTERNAL_ZTEST__
				draw_pixel_blending_ztest( x, y, z, color, spBlending)
			#elif defined __SPARROW_INTERNAL_ZSET__
				draw_pixel_blending_zset( x, y, z, color, spBlending)
			#else
				draw_pixel_blending( x, y, color, spBlending )
			#endif
			#ifndef __SPARROW_INTERNAL_ZNOTHING__			
				z += sZ;
			#endif
			}
		#else
			#ifdef __SPARROW_INTERNAL_ZNOTHING__
				spHorizentalLine( spTargetPixel, x1, y, x2 - x1 + 1, color, 1, spTargetScanLine, spTargetY );
			#else
				int x;
				for ( x = x1; x <= x2; x++ )
				{
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_pixel_ztest_zset( x, y, z, color )
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_pixel_ztest( x, y, z, color )
					#elif defined __SPARROW_INTERNAL_ZSET__
						draw_pixel_zset( x, y, z, color )
					#endif
						z += sZ;
				}
			#endif
		#endif
	#endif
}

#ifdef __SPARROW_INTERNAL_BLENDING__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Triangle_blending_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Triangle_blending_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Triangle_blending_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Triangle_blending_pattern( Sint32 x1, Sint32 y1,   Sint32 x2, Sint32 y2,   Sint32 x3, Sint32 y3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Triangle_blending_ztest_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Triangle_blending_ztest( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Triangle_blending_zset( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Triangle_blending( Sint32 x1, Sint32 y1,   Sint32 x2, Sint32 y2,   Sint32 x3, Sint32 y3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Triangle_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Triangle_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Triangle_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Triangle_pattern( Sint32 x1, Sint32 y1,   Sint32 x2, Sint32 y2,   Sint32 x3, Sint32 y3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Triangle_ztest_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Triangle_ztest( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Triangle_zset( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Triangle( Sint32 x1, Sint32 y1,   Sint32 x2, Sint32 y2,   Sint32 x3, Sint32 y3,   Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#endif
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
	Sint32 x4 = x1;
	Sint32 y4 = y1;
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 z4 = z1;
	#endif
	int div = y2 - y1;
	if ( div != 0 )
	{
		int mul = y3 - y1;
		Sint32 mul32 = mul * one_over_x( div );
		x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_ACCURACY );
		y4 = y3;
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			z4 = z1 + mul * z_div( z2 - z1, div );
		#endif
	}
	Sint32 xl = x1 << SP_ACCURACY;
	Sint32 sX_l = 0;
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 zl = z1;
		Sint32 sZ_l = 0;
	#endif
	Sint32 xr = xl;
	Sint32 sX_r = 0;
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 zr = zl;
		Sint32 sZ_r = 0;
	#endif
	if ( ( y1 - y2 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y2 );
		if ( x4 < x3 )
			sX_l = ( x1 - x2 ) * mul;
		else
			sX_r = ( x1 - x2 ) * mul;
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			if ( x4 < x3 )
				sZ_l = z_div( z1 - z2, y1 - y2 );
			else
				sZ_r = z_div( z1 - z2, y1 - y2 );
		#endif
	}	
	if ( ( y1 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y3 );
		if ( x4 < x3 )
			sX_r = ( x1 - x3 ) * mul;
		else
			sX_l = ( x1 - x3 ) * mul;
		//sZ_r = (z1-z3)*mul;
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			if ( x4 < x3 )
				sZ_r = z_div( z1 - z3, y1 - y3 );
			else
				sZ_l = z_div( z1 - z3, y1 - y3 );
		#endif
	}

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		xr += sX_r * diff;
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			zl += sZ_l * diff;
			zr += sZ_r * diff;
		#endif
	}
	else
	{
		if ( y1 < 0 )
		{
			int diff = -y1;
			y1 = 0;
			xl += sX_l * diff;
			xr += sX_r * diff;
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				zl += sZ_l * diff;
				zr += sZ_r * diff;
			#endif
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			Sint32 sZ = 0;		
			if ( ( x4 - x3 ) != 0 )
				sZ = z_div( z4 - z3, x4 - x3 );
		#endif
		for ( y = y1; y < y3; y++ )
		{
			if (sX_r < 0)
				xl += sX_l;
			else
				xr += sX_r;
			#ifdef __SPARROW_INTERNAL_BLENDING__
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_blending_line_ztest_zset_pattern( xl >> SP_ACCURACY, zl,
												xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_blending_line_ztest_pattern( xl >> SP_ACCURACY, zl,
										 xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_blending_line_zset_pattern( xl >> SP_ACCURACY, zl,
										xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#else
						draw_blending_line_pattern( xl >> SP_ACCURACY, 
									 xr >> SP_ACCURACY, y, color, spPattern, spBlending);
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_blending_line_ztest_zset( xl >> SP_ACCURACY, zl,
												xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_blending_line_ztest( xl >> SP_ACCURACY, zl,
										 xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_blending_line_zset( xl >> SP_ACCURACY, zl,
										xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#else
						draw_blending_line( xl >> SP_ACCURACY, 
									 xr >> SP_ACCURACY, y, color, spPattern, spBlending);
					#endif
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_line_ztest_zset_pattern( xl >> SP_ACCURACY, zl,
												xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_line_ztest_pattern( xl >> SP_ACCURACY, zl,
										 xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_line_zset_pattern( xl >> SP_ACCURACY, zl,
										xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#else
						draw_line_pattern( xl >> SP_ACCURACY, 
									 xr >> SP_ACCURACY, y, color, spPattern, spBlending);
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_line_ztest_zset( xl >> SP_ACCURACY, zl,
												xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_line_ztest( xl >> SP_ACCURACY, zl,
										 xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_line_zset( xl >> SP_ACCURACY, zl,
										xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
					#else
						draw_line( xl >> SP_ACCURACY, 
									 xr >> SP_ACCURACY, y, color, spPattern, spBlending);
					#endif
				#endif
			#endif
			if (sX_r < 0)
				xr += sX_r;
			else
				xl += sX_l;
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				zl += sZ_l;
				zr += sZ_r;
			#endif
		}
	}
	
	if ( x4 < x3 )
	{
		xr = x3 << SP_ACCURACY;
		sX_r = 0;
	}
	else
	{
		xl = x3 << SP_ACCURACY;
		sX_l = 0;
	}
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		if ( x4 < x3 )
		{
			zr = z3;
			sZ_r = 0;
		}
		else
		{
			zl = z3;
			sZ_l = 0;
		}
	#endif
	if ( ( y2 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y2 - y3 );
		if ( x4 < x3 )
			sX_r = ( x2 - x3 ) * mul;
		else
			sX_l = ( x2 - x3 ) * mul;
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			if ( x4 < x3 )
				sZ_r = z_div( z2 - z3, y2 - y3 );
			else
				sZ_l = z_div( z2 - z3, y2 - y3 );
		#endif
	}

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		xr += sX_r * diff;
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			zl += sZ_l * diff;
			zr += sZ_r * diff;
		#endif
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 sZ = 0;
		if ( ( x4 - x3 ) != 0 )
			sZ = z_div( z4 - z3, x4 - x3 );
	#endif
	for ( y = y3; y <= y2; y++ )
	{
		if (sX_r < 0)
		{
			if (y == y2)
				xl == x2 << SP_ACCURACY;
			else
				xl += sX_l;
		}
		else
		{
			if (y == y2)
				xr == x2 << SP_ACCURACY;
			else
				xr += sX_r;
		}
		#ifdef __SPARROW_INTERNAL_BLENDING__
			#ifdef __SPARROW_INTERNAL_PATTERN__
				#ifdef __SPARROW_INTERNAL_ZBOTH__
					draw_blending_line_ztest_zset_pattern( xl >> SP_ACCURACY, zl,
											xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#elif defined __SPARROW_INTERNAL_ZTEST__
					draw_blending_line_ztest_pattern( xl >> SP_ACCURACY, zl,
									 xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#elif defined __SPARROW_INTERNAL_ZSET__	
					draw_blending_line_zset_pattern( xl >> SP_ACCURACY, zl,
									xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#else
					draw_blending_line_pattern( xl >> SP_ACCURACY, 
								 xr >> SP_ACCURACY, y, color, spPattern, spBlending);
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_ZBOTH__
					draw_blending_line_ztest_zset( xl >> SP_ACCURACY, zl,
											xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#elif defined __SPARROW_INTERNAL_ZTEST__
					draw_blending_line_ztest( xl >> SP_ACCURACY, zl,
									 xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#elif defined __SPARROW_INTERNAL_ZSET__	
					draw_blending_line_zset( xl >> SP_ACCURACY, zl,
									xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#else
					draw_blending_line( xl >> SP_ACCURACY, 
								 xr >> SP_ACCURACY, y, color, spPattern, spBlending);
				#endif
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_PATTERN__
				#ifdef __SPARROW_INTERNAL_ZBOTH__
					draw_line_ztest_zset_pattern( xl >> SP_ACCURACY, zl,
											xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#elif defined __SPARROW_INTERNAL_ZTEST__
					draw_line_ztest_pattern( xl >> SP_ACCURACY, zl,
									 xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#elif defined __SPARROW_INTERNAL_ZSET__	
					draw_line_zset_pattern( xl >> SP_ACCURACY, zl,
									xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#else
					draw_line_pattern( xl >> SP_ACCURACY, 
								 xr >> SP_ACCURACY, y, color, spPattern, spBlending);
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_ZBOTH__
					draw_line_ztest_zset( xl >> SP_ACCURACY, zl,
											xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#elif defined __SPARROW_INTERNAL_ZTEST__
					draw_line_ztest( xl >> SP_ACCURACY, zl,
									 xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#elif defined __SPARROW_INTERNAL_ZSET__	
					draw_line_zset( xl >> SP_ACCURACY, zl,
									xr >> SP_ACCURACY, zr, y, color, spPattern, spBlending, sZ );
				#else
					draw_line( xl >> SP_ACCURACY, 
								 xr >> SP_ACCURACY, y, color, spPattern, spBlending);
				#endif
			#endif
		#endif
		if (sX_r < 0)
		{
			if (y == y2)
				xr == x2 << SP_ACCURACY;
			else
				xr += sX_r;
		}
		else
		{
			if (y == y2)
				xl == x2 << SP_ACCURACY;
			else
				xl += sX_l;
		}
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			zl += sZ_l;
			zr += sZ_r;
		#endif
	}
}

#ifdef __SPARROW_INTERNAL_BLENDING__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#define pixel_macro(x1,y,z,color) draw_pixel_blending_ztest_zset_pattern( x1, y, z, color, spPattern, spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#define pixel_macro(x1,y,z,color) draw_pixel_blending_ztest_pattern( x1, y, z, color, spPattern, spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			#define pixel_macro(x1,y,z,color) draw_pixel_blending_zset_pattern( x1, y, z, color, spPattern, spBlending )
		#else
			#define pixel_macro(x1,y,z,color) draw_pixel_blending_pattern( x1, y, color, spPattern, spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#define pixel_macro(x1,y,z,color) draw_pixel_blending_ztest_zset( x1, y, z, color, spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#define pixel_macro(x1,y,z,color) draw_pixel_blending_ztest( x1, y, z, color, spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			#define pixel_macro(x1,y,z,color) draw_pixel_blending_zset( x1, y, z, color, spBlending )
		#else
			#define pixel_macro(x1,y,z,color) draw_pixel_blending( x1, y, color, spBlending )
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#define pixel_macro(x1,y,z,color) draw_pixel_ztest_zset_pattern( x1, y, z, color, spPattern )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#define pixel_macro(x1,y,z,color) draw_pixel_ztest_pattern( x1, y, z, color, spPattern )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			#define pixel_macro(x1,y,z,color) draw_pixel_zset_pattern( x1, y, z, color, spPattern )
		#else
			#define pixel_macro(x1,y,z,color) draw_pixel_pattern( x1, y, color, spPattern )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#define pixel_macro(x1,y,z,color) draw_pixel_ztest_zset( x1, y, z, color )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#define pixel_macro(x1,y,z,color) draw_pixel_ztest( x1, y, z, color )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			#define pixel_macro(x1,y,z,color) draw_pixel_zset( x1, y, z, color )
		#else
			#define pixel_macro(x1,y,z,color) draw_pixel( x1, y, color )
		#endif
	#endif
#endif

#ifdef __SPARROW_INTERNAL_BLENDING__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Rectangle_blending_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Rectangle_blending_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Rectangle_blending_zset_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Rectangle_blending_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Rectangle_blending_ztest_zset( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Rectangle_blending_ztest( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Rectangle_blending_zset( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Rectangle_blending( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Rectangle_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Rectangle_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Rectangle_zset_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Rectangle_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Rectangle_ztest_zset( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Rectangle_ztest( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Rectangle_zset( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Rectangle( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#endif
{
	int y;
	for ( ; x1 <= x2; x1++ )
		for ( y = y1; y <= y2; y++ )
			pixel_macro(x1,y,z,color)
}

#ifdef __SPARROW_INTERNAL_BLENDING__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_RectangleBorder_blending_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_RectangleBorder_blending_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_RectangleBorder_blending_zset_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_RectangleBorder_blending_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_RectangleBorder_blending_ztest_zset( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_RectangleBorder_blending_ztest( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_RectangleBorder_blending_zset( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_RectangleBorder_blending( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_RectangleBorder_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_RectangleBorder_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_RectangleBorder_zset_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_RectangleBorder_pattern( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_RectangleBorder_ztest_zset( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_RectangleBorder_ztest( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_RectangleBorder_zset( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_RectangleBorder( Sint32 x1, Sint32 y1, Sint32 x2, Sint32 y2, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#endif
{
	int cx,cy;
	//top border
	for ( cy = y1; cy < y1 + by && cy <= y2; cy++ )
		for ( cx = x1; cx <= x2; cx++ )
			pixel_macro( cx, cy, z, color )
	//left & right border
	for ( ; cy < y2 - by && cy <= y2; cy++ )
	{
		for ( cx = x1; cx <= x1 + bx && cx <= x2; cx++ )
			pixel_macro( cx, cy, z, color )
		for ( cx = x2 - bx; cx <= x2; cx++ )
			pixel_macro( cx, cy, z, color )
	}
	//bottom border
	for ( ; cy <= y2; cy++ )
		for ( cx = x1; cx <= x2; cx++ )
			pixel_macro( cx, cy, z, color )
}

#ifdef __SPARROW_INTERNAL_BLENDING__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Ellipse_blending_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Ellipse_blending_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Ellipse_blending_zset_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Ellipse_blending_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Ellipse_blending_ztest_zset( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Ellipse_blending_ztest( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Ellipse_blending_zset( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Ellipse_blending( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Ellipse_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Ellipse_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Ellipse_zset_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Ellipse_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_Ellipse_ztest_zset( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_Ellipse_ztest( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_Ellipse_zset( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_Ellipse( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#endif
{
	int x,y;
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
			pixel_macro( x1 + x, y1 + y, z, color )
	}
}

#ifdef __SPARROW_INTERNAL_BLENDING__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_EllipseBorder_blending_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_EllipseBorder_blending_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_EllipseBorder_blending_zset_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_EllipseBorder_blending_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_EllipseBorder_blending_ztest_zset( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_EllipseBorder_blending_ztest( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_EllipseBorder_blending_zset( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_EllipseBorder_blending( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_EllipseBorder_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_EllipseBorder_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_EllipseBorder_zset_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_EllipseBorder_pattern( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			static void sp_intern_EllipseBorder_ztest_zset( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			static void sp_intern_EllipseBorder_ztest( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			static void sp_intern_EllipseBorder_zset( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#else
			static void sp_intern_EllipseBorder( Sint32 x1, Sint32 y1, Sint32 rxl, Sint32 rxr, Sint32 rx, Sint32 ryl, Sint32 ryr, Sint32 ry, Sint32 z, Sint32 bx, Sint32 by, Uint32 color, Uint8* spPattern, Sint32 spBlending )
		#endif
	#endif
#endif
{
	int x,y;
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
			pixel_macro( x1 + x, y1 + y, z, color)
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
			pixel_macro( x1 + x, y1 + y, z, color)
		for (x = RX_in;x < RX_out; x++)
			pixel_macro( x1 + x, y1 + y, z, color)
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
			pixel_macro( x1 + x, y1 + y, z, color)
	}
}

#undef pixel_macro

