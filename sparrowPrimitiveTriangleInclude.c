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

//#define SP_TRY
#ifndef SP_TRY

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
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				draw_pixel_ztest_zset_pattern( x, y, z, color )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				draw_pixel_ztest_pattern( x, y, z, color )
			#elif defined __SPARROW_INTERNAL_ZSET__
				draw_pixel_zset_pattern( x, y, z, color )
			#else
				draw_pixel_pattern( x, y, color )
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
}

#ifdef __SPARROW_INTERNAL_PATTERN__
	#ifdef __SPARROW_INTERNAL_ZBOTH__
		inline void sp_intern_Triangle_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color )
	#elif defined __SPARROW_INTERNAL_ZTEST__
		inline void sp_intern_Triangle_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#elif defined __SPARROW_INTERNAL_ZSET__	
		inline void sp_intern_Triangle_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#else
		inline void sp_intern_Triangle_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#endif
#else
	#ifdef __SPARROW_INTERNAL_ZBOTH__
		inline void sp_intern_Triangle_ztest_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color )
	#elif defined __SPARROW_INTERNAL_ZTEST__
		inline void sp_intern_Triangle_ztest( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#elif defined __SPARROW_INTERNAL_ZSET__	
		inline void sp_intern_Triangle_zset( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#else
		inline void sp_intern_Triangle( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
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

#else

#ifdef __SPARROW_INTERNAL_PATTERN__
	#ifdef __SPARROW_INTERNAL_ZBOTH__
		#ifdef __GNUC__
		inline void draw_line_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color) __attribute__((always_inline));
		#endif
		inline void draw_line_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color)
	#elif defined __SPARROW_INTERNAL_ZTEST__
		#ifdef __GNUC__
		inline void draw_line_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color) __attribute__((always_inline));
		#endif
		inline void draw_line_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color)
	#elif defined __SPARROW_INTERNAL_ZSET__
		#ifdef __GNUC__
		inline void draw_line_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color) __attribute__((always_inline));
		#endif
		inline void draw_line_zset_pattern( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color)
	#else
		#ifdef __GNUC__
		inline void draw_line_pattern( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color ) __attribute__((always_inline));
		#endif
		inline void draw_line_pattern( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color )
	#endif
#else
	#ifdef __SPARROW_INTERNAL_ZBOTH__
		#ifdef __GNUC__
		inline void draw_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color) __attribute__((always_inline));
		#endif
		inline void draw_line_ztest_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color)
	#elif defined __SPARROW_INTERNAL_ZTEST__
		#ifdef __GNUC__
		inline void draw_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color) __attribute__((always_inline));
		#endif
		inline void draw_line_ztest( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color)
	#elif defined __SPARROW_INTERNAL_ZSET__
		#ifdef __GNUC__
		inline void draw_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color) __attribute__((always_inline));
		#endif
		inline void draw_line_zset( Sint32 x1, Sint32 z1, Sint32 x2, Sint32 z2, Sint32 y, Uint32 color)
	#else
		#ifdef __GNUC__
		inline void draw_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color ) __attribute__((always_inline));
		#endif
		inline void draw_line( Sint32 x1, Sint32 x2, Sint32 y, Uint32 color )
	#endif
#endif
{
/*	if (x1 == x2 || x1+1 == x2)
		return;
#ifndef __SPARROW_INTERNAL_ZNOTHING__
	Sint32 xm = x1+x2 >> 1;
	Sint32 zm = z1+z2 >> 1;
	draw_pixel_ztest_zset(xm,y,zm,color);
	draw_line_ztest_zset(x1,z1,xm,zm,y,color);
	draw_line_ztest_zset(xm,zm,x2,z2,y,color);
#endif*/
#ifdef __SPARROW_INTERNAL_ZNOTHING__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		if (x1 < 0)
			x1 = 0;
		if (x2 >= spTargetX)
			x2 = spTargetX-1;
		for (;x1 <= x2; x1++)
			draw_pixel_pattern(x1,y,color);
	#else
		if (x1 < 0)
			x1 = 0;
		if (x2 >= spTargetX)
			x2 = spTargetX-1;
		spHorizentalLine( spTargetPixel, x1, y, x2 - x1, color, 1, spTargetScanLine, spTargetY );
	#endif
#else
	Sint32 stack[x2-x1+1 << 2];
	
	int stack_counter = -1;
	//Init line
	if (x2-x1 != 0)
	{
		stack[++stack_counter] = x1;
		stack[++stack_counter] = z1;
		stack[++stack_counter] = x2;
		stack[++stack_counter] = z2;
	}
	while (stack_counter >= 0)
	{
		//stack pop backwards!
		Sint32 zr = stack[stack_counter--];
		Sint32 xr = stack[stack_counter--];
		Sint32 zl = stack[stack_counter--];
		Sint32 xl = stack[stack_counter--];
		if (xl+1 == xr)
			continue;
		//Calculating the middle
		Sint32 xm = xl + xr >> 1; //(l + r) / 2
		Sint32 zm = zl + zr >> 1; //(z[l] + z[r]) / 2
		//putpixel
		if (xm >= 0 && xm < spTargetX)
		#ifdef __SPARROW_INTERNAL_PATTERN__
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				draw_pixel_ztest_zset_pattern(xm,y,zm,color);
			#elif defined __SPARROW_INTERNAL_ZTEST__
				draw_pixel_ztest_pattern(xm,y,zm,color);
			#elif defined __SPARROW_INTERNAL_ZSET__
				draw_pixel_zset_pattern(xm,y,zm,color);
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				draw_pixel_ztest_zset(xm,y,zm,color);
			#elif defined __SPARROW_INTERNAL_ZTEST__
				draw_pixel_ztest(xm,y,zm,color);
			#elif defined __SPARROW_INTERNAL_ZSET__
				draw_pixel_zset(xm,y,zm,color);
			#endif
		#endif

		if (xl+2 == xr)
			continue;
		//left stack push
		//stack[++stack_counter] = xl;
		//stack[++stack_counter] = zl;
		stack_counter+=2;
		stack[++stack_counter] = xm;
		stack[++stack_counter] = zm;
		//right stack push
		stack[++stack_counter] = xm;
		stack[++stack_counter] = zm;
		stack[++stack_counter] = xr;
		stack[++stack_counter] = zr;
	}	
#endif
}


#ifdef __SPARROW_INTERNAL_PATTERN__
	#ifdef __SPARROW_INTERNAL_ZBOTH__
		inline void sp_intern_Triangle_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color )
	#elif defined __SPARROW_INTERNAL_ZTEST__
		inline void sp_intern_Triangle_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#elif defined __SPARROW_INTERNAL_ZSET__	
		inline void sp_intern_Triangle_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#else
		inline void sp_intern_Triangle_pattern( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#endif
#else
	#ifdef __SPARROW_INTERNAL_ZBOTH__
		inline void sp_intern_Triangle_ztest_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color )
	#elif defined __SPARROW_INTERNAL_ZTEST__
		inline void sp_intern_Triangle_ztest( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#elif defined __SPARROW_INTERNAL_ZSET__	
		inline void sp_intern_Triangle_zset( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#else
		inline void sp_intern_Triangle( Sint32 x1, Sint32 y1, Sint32 z1,   Sint32 x2, Sint32 y2, Sint32 z2,   Sint32 x3, Sint32 y3, Sint32 z3,   Uint32 color )
	#endif
#endif
{
	//Locking the Target
	SDL_LockSurface( spTarget );

	Sint32  long_line_x[y2-y1+1]; //x
	Sint32 short_line_x[y2-y1+1]; //x
#ifndef __SPARROW_INTERNAL_ZNOTHING__
	Sint32  long_line_z[y2-y1+1]; //z
	Sint32 short_line_z[y2-y1+1]; //z
#endif
	Sint32 stack_l[y2-y1+1];
	Sint32 stack_r[y2-y1+1];
	
	int stack_counter = -1;
	//Init long line
	long_line_x[0] = x1;
	long_line_x[y2-y1] = x2;
#ifndef __SPARROW_INTERNAL_ZNOTHING__
	long_line_z[0] = z1;
	long_line_z[y2-y1] = z2;
#endif

	if (y2 != y1)
	{
		stack_counter++;
		stack_l[stack_counter] = 0;
		stack_r[stack_counter] = y2-y1;
	}
	while (stack_counter >= 0)
	{
		//stack pop
		Sint32 l = stack_l[stack_counter];
		Sint32 r = stack_r[stack_counter];
		stack_counter--;
		if (l+1 == r)
			continue;
		//Calculating the middle
		Sint32 m = l + r >> 1; //(l + r) / 2
		long_line_x[m] = long_line_x[l] + long_line_x[r] >> 1; //(x[l] + x[r]) / 2
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		long_line_z[m] = long_line_z[l] + long_line_z[r] >> 1; //(z[l] + z[r]) / 2
	#endif
		if (l+2 == r)
			continue;
		//left stack push
		stack_counter++;
		stack_l[stack_counter] = l;
		stack_r[stack_counter] = m;
		//right stack push
		stack_counter++;
		stack_l[stack_counter] = m;
		stack_r[stack_counter] = r;
	}
	
	//Init short lines
	short_line_x[0] = x1;
	short_line_x[y3-y1] = x3;
#ifndef __SPARROW_INTERNAL_ZNOTHING__	
	short_line_z[0]= z1;
	short_line_z[y3-y1] = z3;
#endif
	stack_counter = -1;
	if (y3 != y1)
	{
		stack_counter++;
		stack_l[stack_counter] = 0;
		stack_r[stack_counter] = y3-y1;
	}
	if (y2 != y3)
	{
		stack_counter++;
		stack_l[stack_counter] = y3-y1;
		stack_r[stack_counter] = y2-y1;
		short_line_x[y2-y1] = x2;
	#ifndef __SPARROW_INTERNAL_ZNOTHING__	
		short_line_z[y2-y1] = z2;
	#endif
	}
	while (stack_counter >= 0)
	{
		//stack pop
		Sint32 l = stack_l[stack_counter];
		Sint32 r = stack_r[stack_counter];
		stack_counter--;
		if (l+1 == r)
			continue;
		//Calculating the middle
		Sint32 m = l + r >> 1; //(l + r) / 2
		short_line_x[m] = short_line_x[l] + short_line_x[r] >> 1; //(x[l] + x[r]) / 2
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		short_line_z[m] = short_line_z[l] + short_line_z[r] >> 1; //(x[l] + x[r]) / 2
	#endif
		if (l+2 == r)
			continue;
		//left stack push
		stack_counter++;
		stack_l[stack_counter] = l;
		stack_r[stack_counter] = m;
		//right stack push
		stack_counter++;
		stack_l[stack_counter] = m;
		stack_r[stack_counter] = r;
	}
	
	if (y1 < 0)
		stack_counter = -y1;
	else
		stack_counter = 0;
	Sint32 end = y2-y1;
	if (y1 + end >= spTargetY)
		end = spTargetY-y1-1;
	for (; stack_counter <= end; stack_counter++)
	{
		Sint32 y = y1+stack_counter;
		Sint32 xl = long_line_x[stack_counter];
		Sint32 xr = short_line_x[stack_counter];
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 zl = long_line_z[stack_counter];
		Sint32 zr = short_line_z[stack_counter];
	#endif
		#ifdef __SPARROW_INTERNAL_PATTERN__
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				if (xl < xr)
					draw_line_ztest_zset_pattern(xl,zl,xr,zr,y,color);
				else
					draw_line_ztest_zset_pattern(xr,zr,xl,zl,y,color);
			#elif defined __SPARROW_INTERNAL_ZTEST__
				if (xl < xr)
					draw_line_ztest_pattern(xl,zl,xr,zr,y,color);
				else
					draw_line_ztest_pattern(xr,zr,xl,zl,y,color);
			#elif defined __SPARROW_INTERNAL_ZSET__	
				if (xl < xr)
					draw_line_zset_pattern(xl,zl,xr,zr,y,color);
				else
					draw_line_zset_pattern(xr,zr,xl,zl,y,color);
			#else
				if (xl < xr)
					draw_line_pattern(xl,xr,y,color);
				else
					draw_line_pattern(xr,xl,y,color);
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				if (xl < xr)
					draw_line_ztest_zset(xl,zl,xr,zr,y,color);
				else
					draw_line_ztest_zset(xr,zr,xl,zl,y,color);
			#elif defined __SPARROW_INTERNAL_ZTEST__
				if (xl < xr)
					draw_line_ztest(xl,zl,xr,zr,y,color);
				else
					draw_line_ztest(xr,zr,xl,zl,y,color);
			#elif defined __SPARROW_INTERNAL_ZSET__	
				if (xl < xr)
					draw_line_zset(xl,zl,xr,zr,y,color);
				else
					draw_line_zset(xr,zr,xl,zl,y,color);
			#else
				if (xl < xr)
					draw_line(xl,xr,y,color);
				else
					draw_line(xr,xl,y,color);
			#endif
		#endif
	}
	SDL_UnlockSurface( spTarget );
}
#endif
