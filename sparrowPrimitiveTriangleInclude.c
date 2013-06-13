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

/* This file is very bad style - and I know it! But this is the best
 * way to get the code fast and still understanable. The only
 * alternative I used before was massive copy and paste, which is
 * bollocks. */

#ifdef __SPARROW_INTERNAL_BLENDING__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#ifdef __GNUC__
			inline void draw_blending_line_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#ifdef __GNUC__
			inline void draw_blending_line_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZSET__
			#ifdef __GNUC__
			inline void draw_blending_line_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#else
			#ifdef __GNUC__
			inline void draw_blending_line_pattern( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_pattern( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#ifdef __GNUC__
			inline void draw_blending_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#ifdef __GNUC__
			inline void draw_blending_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZSET__
			#ifdef __GNUC__
			inline void draw_blending_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#else
			#ifdef __GNUC__
			inline void draw_blending_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color ) __attribute__((always_inline));
			#endif
			inline void draw_blending_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color )
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#ifdef __GNUC__
			inline void draw_line_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#ifdef __GNUC__
			inline void draw_line_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZSET__
			#ifdef __GNUC__
			inline void draw_line_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#else
			#ifdef __GNUC__
			inline void draw_line_pattern( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color ) __attribute__((always_inline));
			#endif
			inline void draw_line_pattern( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			#ifdef __GNUC__
			inline void draw_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			#ifdef __GNUC__
			inline void draw_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#elif defined __SPARROW_INTERNAL_ZSET__
			#ifdef __GNUC__
			inline void draw_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ ) __attribute__((always_inline));
			#endif
			inline void draw_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color, Sint32 sZ )
		#else
			#ifdef __GNUC__
			inline void draw_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color ) __attribute__((always_inline));
			#endif
			inline void draw_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color )
		#endif
	#endif
#endif
{
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
					draw_blending_pixel_ztest_zset_pattern( x, y, z, color, spBlending )
				#elif defined __SPARROW_INTERNAL_ZTEST__
					draw_blending_pixel_ztest_pattern( x, y, z, color, spBlending )
				#elif defined __SPARROW_INTERNAL_ZSET__
					draw_blending_pixel_zset_pattern( x, y, z, color, spBlending )
				#else
					draw_blending_pixel_pattern( x, y, color, spBlending )
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_ZBOTH__
					draw_pixel_ztest_zset_pattern( x, y, z, color )
				#elif defined __SPARROW_INTERNAL_ZTEST__
					draw_pixel_ztest_pattern( x, y, z, color )
				#elif defined __SPARROW_INTERNAL_ZSET__
					draw_pixel_zset_pattern( x, y, z, color )
				#else
					draw_pixel_pattern( x, y, color )
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
				draw_blending_pixel_ztest_zset( x, y, z, color, spBlending)
			#elif defined __SPARROW_INTERNAL_ZTEST__
				draw_blending_pixel_ztest( x, y, z, color, spBlending)
			#elif defined __SPARROW_INTERNAL_ZSET__
				draw_blending_pixel_zset( x, y, z, color, spBlending)
			#else
				draw_blending_pixel( x, y, color, spBlending )
			#endif
			#ifndef __SPARROW_INTERNAL_ZNOTHING__			
				z += sZ;
			#endif
			}
		#else
			#ifdef __SPARROW_INTERNAL_ZNOTHING__
				spHorizentalLine( spTargetPixel, x1, y, x2 - x1, color, 1, spTargetScanLine, spTargetY );
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
			inline void sp_intern_Triangle_blending_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			inline void sp_intern_Triangle_blending_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			inline void sp_intern_Triangle_blending_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
		#else
			inline void sp_intern_Triangle_blending_pattern( Sint32 x1, Sint32 y1,   Sint32 x2, Sint32 y2,   Sint32 x3, Sint32 y3,   Uint32 color )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			inline void sp_intern_Triangle_blending_ztest_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			inline void sp_intern_Triangle_blending_ztest( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			inline void sp_intern_Triangle_blending_zset( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
		#else
			inline void sp_intern_Triangle_blending( Sint32 x1, Sint32 y1,   Sint32 x2, Sint32 y2,   Sint32 x3, Sint32 y3,   Uint32 color )
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			inline void sp_intern_Triangle_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			inline void sp_intern_Triangle_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			inline void sp_intern_Triangle_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
		#else
			inline void sp_intern_Triangle_pattern( Sint32 x1, Sint32 y1,   Sint32 x2, Sint32 y2,   Sint32 x3, Sint32 y3,   Uint32 color )
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			inline void sp_intern_Triangle_ztest_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color )
		#elif defined __SPARROW_INTERNAL_ZTEST__
			inline void sp_intern_Triangle_ztest( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
		#elif defined __SPARROW_INTERNAL_ZSET__	
			inline void sp_intern_Triangle_zset( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
		#else
			inline void sp_intern_Triangle( Sint32 x1, Sint32 y1,   Sint32 x2, Sint32 y2,   Sint32 x3, Sint32 y3,   Uint32 color )
		#endif
	#endif
#endif
{
	int y;
	if ( y2 < 0 )
		return;
	if ( y1 >= spTargetY )
		return;
		
	SDL_LockSurface( spTarget );

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
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 zl = z1;
	#endif
	Sint32 sX_l = 0;
	Sint32 sZ_l = 0;
	if ( ( y1 - y2 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y2 );
		sX_l = ( x1 - x2 ) * mul;
		//sZ_l = (z1-z2)*mul;
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			sZ_l = z_div( z1 - z2, y1 - y2 );
		#endif
	}

	Sint32 xr = xl;
	Sint32 sX_r = 0;
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 zr = zl;
		Sint32 sZ_r = 0;
	#endif
	if ( ( y1 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y1 - y3 );
		sX_r = ( x1 - x3 ) * mul;
		//sZ_r = (z1-z3)*mul;
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			sZ_r = z_div( z1 - z3, y1 - y3 );
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
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				#ifdef __SPARROW_INTERNAL_BLENDING__
					#ifdef __SPARROW_INTERNAL_PATTERN__
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_blending_line_ztest_zset_pattern( xl >> SP_ACCURACY, zl,
													xr >> SP_ACCURACY, zr, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_blending_line_ztest_pattern( xl >> SP_ACCURACY, zl,
											 xr >> SP_ACCURACY, zr, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__	
							draw_blending_line_zset_pattern( xl >> SP_ACCURACY, zl,
											xr >> SP_ACCURACY, zr, y, color, sZ );
						#else
							draw_blending_line_pattern( xl >> SP_ACCURACY, 
										 xr >> SP_ACCURACY, y, color);
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_blending_line_ztest_zset( xl >> SP_ACCURACY, zl,
													xr >> SP_ACCURACY, zr, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_blending_line_ztest( xl >> SP_ACCURACY, zl,
											 xr >> SP_ACCURACY, zr, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__	
							draw_blending_line_zset( xl >> SP_ACCURACY, zl,
											xr >> SP_ACCURACY, zr, y, color, sZ );
						#else
							draw_blending_line( xl >> SP_ACCURACY, 
										 xr >> SP_ACCURACY, y, color);
						#endif
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_PATTERN__
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_ztest_zset_pattern( xl >> SP_ACCURACY, zl,
													xr >> SP_ACCURACY, zr, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_ztest_pattern( xl >> SP_ACCURACY, zl,
											 xr >> SP_ACCURACY, zr, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__	
							draw_line_zset_pattern( xl >> SP_ACCURACY, zl,
											xr >> SP_ACCURACY, zr, y, color, sZ );
						#else
							draw_line_pattern( xl >> SP_ACCURACY, 
										 xr >> SP_ACCURACY, y, color);
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_ztest_zset( xl >> SP_ACCURACY, zl,
													xr >> SP_ACCURACY, zr, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_ztest( xl >> SP_ACCURACY, zl,
											 xr >> SP_ACCURACY, zr, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__	
							draw_line_zset( xl >> SP_ACCURACY, zl,
											xr >> SP_ACCURACY, zr, y, color, sZ );
						#else
							draw_line( xl >> SP_ACCURACY, 
										 xr >> SP_ACCURACY, y, color);
						#endif
					#endif
				#endif
				xl += sX_l;
				xr += sX_r;
				#ifndef __SPARROW_INTERNAL_ZNOTHING__
					zl += sZ_l;
					zr += sZ_r;
				#endif
			}
		}
		else
		{
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				Sint32 sZ = 0;
				if ( ( x4 - x3 ) != 0 )
					sZ = z_div( z4 - z3, x4 - x3 );
			#endif
			for ( y = y1; y < y3; y++ )
			{
				#ifdef __SPARROW_INTERNAL_BLENDING__
					#ifdef __SPARROW_INTERNAL_PATTERN__
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_blending_line_ztest_zset_pattern( xr >> SP_ACCURACY, zr,
													xl >> SP_ACCURACY, zl, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_blending_line_ztest_pattern( xr >> SP_ACCURACY, zr,
											 xl >> SP_ACCURACY, zl, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__	
							draw_blending_line_zset_pattern( xr >> SP_ACCURACY, zr,
											xl >> SP_ACCURACY, zl, y, color, sZ );
						#else
							draw_blending_line_pattern( xr >> SP_ACCURACY, 
										 xl >> SP_ACCURACY, y, color);
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_blending_line_ztest_zset( xr >> SP_ACCURACY, zr,
													xl >> SP_ACCURACY, zl, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_blending_line_ztest( xr >> SP_ACCURACY, zr,
											 xl >> SP_ACCURACY, zl, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__	
							draw_blending_line_zset( xr >> SP_ACCURACY, zr,
											xl >> SP_ACCURACY, zl, y, color, sZ );
						#else
							draw_blending_line( xr >> SP_ACCURACY, 
										 xl >> SP_ACCURACY, y, color);
						#endif
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_PATTERN__
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_ztest_zset_pattern( xr >> SP_ACCURACY, zr,
													xl >> SP_ACCURACY, zl, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_ztest_pattern( xr >> SP_ACCURACY, zr,
											 xl >> SP_ACCURACY, zl, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__	
							draw_line_zset_pattern( xr >> SP_ACCURACY, zr,
											xl >> SP_ACCURACY, zl, y, color, sZ );
						#else
							draw_line_pattern( xr >> SP_ACCURACY, 
										 xl >> SP_ACCURACY, y, color);
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_ztest_zset( xr >> SP_ACCURACY, zr,
													xl >> SP_ACCURACY, zl, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_ztest( xr >> SP_ACCURACY, zr,
											 xl >> SP_ACCURACY, zl, y, color, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__	
							draw_line_zset( xr >> SP_ACCURACY, zr,
											xl >> SP_ACCURACY, zl, y, color, sZ );
						#else
							draw_line( xr >> SP_ACCURACY, 
										 xl >> SP_ACCURACY, y, color);
						#endif
					#endif
				#endif
				xl += sX_l;
				xr += sX_r;
				#ifndef __SPARROW_INTERNAL_ZNOTHING__
					zl += sZ_l;
					zr += sZ_r;
				#endif
			}
		}
	}
	
	xr = x3 << SP_ACCURACY;
	sX_r = 0;
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		zr = z3;
		sZ_r = 0;
	#endif
	if ( ( y2 - y3 ) != 0 )
	{
		Sint32 mul = one_over_x( y2 - y3 );
		sX_r = ( x2 - x3 ) * mul;
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			sZ_r = z_div( z2 - z3, y2 - y3 );
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
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			#ifdef __SPARROW_INTERNAL_BLENDING__
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_blending_line_ztest_zset_pattern( xl >> SP_ACCURACY, zl,
												xr >> SP_ACCURACY, zr, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_blending_line_ztest_pattern( xl >> SP_ACCURACY, zl,
										 xr >> SP_ACCURACY, zr, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_blending_line_zset_pattern( xl >> SP_ACCURACY, zl,
										xr >> SP_ACCURACY, zr, y, color, sZ );
					#else
						draw_blending_line_pattern( xl >> SP_ACCURACY, 
									 xr >> SP_ACCURACY, y, color);
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_blending_line_ztest_zset( xl >> SP_ACCURACY, zl,
												xr >> SP_ACCURACY, zr, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_blending_line_ztest( xl >> SP_ACCURACY, zl,
										 xr >> SP_ACCURACY, zr, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_blending_line_zset( xl >> SP_ACCURACY, zl,
										xr >> SP_ACCURACY, zr, y, color, sZ );
					#else
						draw_blending_line( xl >> SP_ACCURACY, 
									 xr >> SP_ACCURACY, y, color);
					#endif
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_line_ztest_zset_pattern( xl >> SP_ACCURACY, zl,
												xr >> SP_ACCURACY, zr, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_line_ztest_pattern( xl >> SP_ACCURACY, zl,
										 xr >> SP_ACCURACY, zr, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_line_zset_pattern( xl >> SP_ACCURACY, zl,
										xr >> SP_ACCURACY, zr, y, color, sZ );
					#else
						draw_line_pattern( xl >> SP_ACCURACY, 
									 xr >> SP_ACCURACY, y, color);
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_line_ztest_zset( xl >> SP_ACCURACY, zl,
												xr >> SP_ACCURACY, zr, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_line_ztest( xl >> SP_ACCURACY, zl,
										 xr >> SP_ACCURACY, zr, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_line_zset( xl >> SP_ACCURACY, zl,
										xr >> SP_ACCURACY, zr, y, color, sZ );
					#else
						draw_line( xl >> SP_ACCURACY, 
									 xr >> SP_ACCURACY, y, color);
					#endif
				#endif
			#endif
			xl += sX_l;
			xr += sX_r;
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				zl += sZ_l;
				zr += sZ_r;
			#endif
		}
	}
	else
	{
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			Sint32 sZ = 0;
			if ( ( x4 - x3 ) != 0 )
				sZ = z_div( z4 - z3, x4 - x3 );
		#endif
		for ( y = y3; y <= y2; y++ )
		{
			#ifdef __SPARROW_INTERNAL_BLENDING__
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_blending_line_ztest_zset_pattern( xr >> SP_ACCURACY, zr,
												xl >> SP_ACCURACY, zl, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_blending_line_ztest_pattern( xr >> SP_ACCURACY, zr,
										 xl >> SP_ACCURACY, zl, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_blending_line_zset_pattern( xr >> SP_ACCURACY, zr,
										xl >> SP_ACCURACY, zl, y, color, sZ );
					#else
						draw_blending_line_pattern( xr >> SP_ACCURACY, 
									 xl >> SP_ACCURACY, y, color);
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_blending_line_ztest_zset( xr >> SP_ACCURACY, zr,
												xl >> SP_ACCURACY, zl, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_blending_line_ztest( xr >> SP_ACCURACY, zr,
										 xl >> SP_ACCURACY, zl, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_blending_line_zset( xr >> SP_ACCURACY, zr,
										xl >> SP_ACCURACY, zl, y, color, sZ );
					#else
						draw_blending_line( xr >> SP_ACCURACY, 
									 xl >> SP_ACCURACY, y, color);
					#endif
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_line_ztest_zset_pattern( xr >> SP_ACCURACY, zr,
												xl >> SP_ACCURACY, zl, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_line_ztest_pattern( xr >> SP_ACCURACY, zr,
										 xl >> SP_ACCURACY, zl, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_line_zset_pattern( xr >> SP_ACCURACY, zr,
										xl >> SP_ACCURACY, zl, y, color, sZ );
					#else
						draw_line_pattern( xr >> SP_ACCURACY, 
									 xl >> SP_ACCURACY, y, color);
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_line_ztest_zset( xr >> SP_ACCURACY, zr,
												xl >> SP_ACCURACY, zl, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_line_ztest( xr >> SP_ACCURACY, zr,
										 xl >> SP_ACCURACY, zl, y, color, sZ );
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_line_zset( xr >> SP_ACCURACY, zr,
										xl >> SP_ACCURACY, zl, y, color, sZ );
					#else
						draw_line( xr >> SP_ACCURACY, 
									 xl >> SP_ACCURACY, y, color);
					#endif
				#endif
			#endif
			xl += sX_l;
			xr += sX_r;
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				zl += sZ_l;
				zr += sZ_r;
			#endif
		}
	}
	SDL_UnlockSurface( spTarget );
}

#ifdef __SPARROW_INTERNAL_BLENDING__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			 int sp_intern_Triangle_thread_blending_ztest_zset_pattern(void* reserved)
		#elif defined __SPARROW_INTERNAL_ZTEST__
			 int sp_intern_Triangle_thread_blending_ztest_pattern(void* reserved)
		#elif defined __SPARROW_INTERNAL_ZSET__	
			 int sp_intern_Triangle_thread_blending_zset_pattern(void* reserved)
		#else
			 int sp_intern_Triangle_thread_blending_pattern(void* reserved)
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			 int sp_intern_Triangle_thread_blending_ztest_zset(void* reserved)
		#elif defined __SPARROW_INTERNAL_ZTEST__
			 int sp_intern_Triangle_thread_blending_ztest(void* reserved)
		#elif defined __SPARROW_INTERNAL_ZSET__	
			 int sp_intern_Triangle_thread_blending_zset(void* reserved)
		#else
			 int sp_intern_Triangle_thread_blending(void* reserved)
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			 int sp_intern_Triangle_thread_ztest_zset_pattern(void* reserved)
		#elif defined __SPARROW_INTERNAL_ZTEST__
			 int sp_intern_Triangle_thread_ztest_pattern(void* reserved)
		#elif defined __SPARROW_INTERNAL_ZSET__	
			 int sp_intern_Triangle_thread_zset_pattern(void* reserved)
		#else
			 int sp_intern_Triangle_thread_pattern(void* reserved)
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ZBOTH__
			 int sp_intern_Triangle_thread_ztest_zset(void* reserved)
		#elif defined __SPARROW_INTERNAL_ZTEST__
			 int sp_intern_Triangle_thread_ztest(void* reserved)
		#elif defined __SPARROW_INTERNAL_ZSET__	
			 int sp_intern_Triangle_thread_zset(void* reserved)
		#else
			 int sp_intern_Triangle_thread(void* reserved)
		#endif
	#endif
#endif
{
	//endless drawing loop
	while (spScanLineMessage)
	{
		//something in the stack?
		SDL_mutexP(spScanLineMutex);
		if (spScanLineBegin != spScanLineEnd)
		{
			SDL_mutexV(spScanLineMutex);
			#ifdef __SPARROW_INTERNAL_BLENDING__
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						sp_intern_Triangle_blending_ztest_zset_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#elif defined __SPARROW_INTERNAL_ZTEST__
						sp_intern_Triangle_blending_ztest_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#elif defined __SPARROW_INTERNAL_ZSET__	
						sp_intern_Triangle_blending_zset_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#else
						sp_intern_Triangle_blending_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].color);
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						sp_intern_Triangle_blending_ztest_zset(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#elif defined __SPARROW_INTERNAL_ZTEST__
						sp_intern_Triangle_blending_ztest(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#elif defined __SPARROW_INTERNAL_ZSET__	
						sp_intern_Triangle_blending_zset(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#else
						sp_intern_Triangle_blending(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].color);
					#endif
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						sp_intern_Triangle_ztest_zset_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#elif defined __SPARROW_INTERNAL_ZTEST__
						sp_intern_Triangle_ztest_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#elif defined __SPARROW_INTERNAL_ZSET__	
						sp_intern_Triangle_zset_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#else
						sp_intern_Triangle_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].color);
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						sp_intern_Triangle_ztest_zset(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#elif defined __SPARROW_INTERNAL_ZTEST__
						sp_intern_Triangle_ztest(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#elif defined __SPARROW_INTERNAL_ZSET__	
						sp_intern_Triangle_zset(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].z3,
							spScanLineCache[spScanLineBegin].color);
					#else
						sp_intern_Triangle(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].y1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y2,
							spScanLineCache[spScanLineBegin].x3,
							spScanLineCache[spScanLineBegin].y3,
							spScanLineCache[spScanLineBegin].color);
					#endif
				#endif
			#endif
			SDL_mutexP(spScanLineMutex);
			spScanLineBegin = (spScanLineBegin+1) & SP_MAX_SCANLINES_MOD;
			SDL_mutexV(spScanLineMutex);
		}
		else
		{
			SDL_mutexV(spScanLineMutex);
			usleep(200);
		}
	}
}
