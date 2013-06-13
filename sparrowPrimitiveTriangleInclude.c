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
						draw_blending_line_ztest_zset_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_blending_line_ztest_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_blending_line_zset_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#else
						draw_blending_line_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color);
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_blending_line_ztest_zset(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_blending_line_ztest(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_blending_line_zset(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#else
						draw_blending_line(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color);
					#endif
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_line_ztest_zset_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_line_ztest_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_line_zset_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#else
						draw_line_pattern(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color);
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_line_ztest_zset(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_line_ztest(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#elif defined __SPARROW_INTERNAL_ZSET__	
						draw_line_zset(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].z1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].z2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color,
							spScanLineCache[spScanLineBegin].sZ);
					#else
						draw_line(
							spScanLineCache[spScanLineBegin].x1,
							spScanLineCache[spScanLineBegin].x2,
							spScanLineCache[spScanLineBegin].y,
							spScanLineCache[spScanLineBegin].color);
					#endif
				#endif
			#endif
			SDL_mutexP(spScanLineMutex);
			spScanLineBegin = (spScanLineBegin+1) & SP_MAX_SCANLINES_MOD;
			SDL_mutexV(spScanLineMutex);
		}
		#ifndef DO_NOT_USE_DELAY
		else
		{
			SDL_mutexV(spScanLineMutex);
			SDL_Delay(1);
		}
		#endif
	}
}
