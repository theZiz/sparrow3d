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

#ifdef __SPARROW_INTERNAL_PERSPECT__
	#ifdef __SPARROW_INTERNAL_PATTERN__ 
		#ifdef __SPARROW_INTERNAL_ALPHA___
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_zset_alpha_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_zset_alpha_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_alpha_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_alpha_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZSET__
				#ifdef __GNUC__
				inline void draw_line_tex_zset_alpha_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_zset_alpha_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#else
				#ifdef __GNUC__
				inline void draw_line_tex_alpha_pattern_perspect( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_alpha_pattern_perspect( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW  )
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_zset_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_zset_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZSET__
				#ifdef __GNUC__
				inline void draw_line_tex_zset_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_zset_pattern_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#else
				#ifdef __GNUC__
				inline void draw_line_tex_pattern_perspect( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_pattern_perspect( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW  )
			#endif
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ALPHA___
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_zset_alpha_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_zset_alpha_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_alpha_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_alpha_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZSET__
				#ifdef __GNUC__
				inline void draw_line_tex_zset_alpha_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_zset_alpha_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#else
				#ifdef __GNUC__
				inline void draw_line_tex_alpha_perspect( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_alpha_perspect( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW  )
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_zset_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_zset_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZSET__
				#ifdef __GNUC__
				inline void draw_line_tex_zset_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_zset_perspect( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW, Sint32 sZ )
			#else
				#ifdef __GNUC__
				inline void draw_line_tex_perspect( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_perspect( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sW  )
			#endif
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__ 
		#ifdef __SPARROW_INTERNAL_ALPHA___
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_zset_alpha_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_zset_alpha_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_alpha_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_alpha_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZSET__
				#ifdef __GNUC__
				inline void draw_line_tex_zset_alpha_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_zset_alpha_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#else
				#ifdef __GNUC__
				inline void draw_line_tex_alpha_pattern( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_alpha_pattern( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV )
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_zset_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZSET__
				#ifdef __GNUC__
				inline void draw_line_tex_zset_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_zset_pattern( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#else
				#ifdef __GNUC__
				inline void draw_line_tex_pattern( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_pattern( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV )
			#endif
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ALPHA___
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_zset_alpha( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_zset_alpha( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_alpha( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_alpha( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZSET__
				#ifdef __GNUC__
				inline void draw_line_tex_zset_alpha( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_zset_alpha( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#else
				#ifdef __GNUC__
				inline void draw_line_tex_alpha( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_alpha( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV )
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest_zset( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest_zset( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				#ifdef __GNUC__
				inline void draw_line_tex_ztest( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_ztest( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#elif defined __SPARROW_INTERNAL_ZSET__
				#ifdef __GNUC__
				inline void draw_line_tex_zset( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex_zset( Sint32 x1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV, Sint32 sZ )
			#else
				#ifdef __GNUC__
				inline void draw_line_tex( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV ) __attribute__((always_inline));
				#endif
				inline void draw_line_tex( Sint32 x1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 u2, Sint32 v2, Sint32 y, Uint32 color, Sint32 sU, Sint32 sV )
			#endif
		#endif
	#endif
#endif
{
	if ( x1 >= spTargetX )
		return;
	if ( x2 < 0 )
		return;
	Sint32 u = u1;
	Sint32 v = v1;
	#ifdef __SPARROW_INTERNAL_PERSPECT__
		Sint32 w = w1;
	#endif
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 z = z1;
	#endif
	if ( x1 < 0 )
	{
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			z -= x1 * sZ;
		#endif
		u -= x1 * sU;
		v -= x1 * sV;
		#ifdef __SPARROW_INTERNAL_PERSPECT__
			w -= x1 * sW;
		#endif
		x1 = 0;
	}
	if ( x2 >= spTargetX )
		x2 = spTargetX - 1;
	int x;
	for ( x = x1; x <= x2; x++ )
	{
		#ifdef __SPARROW_INTERNAL_PERSPECT__
			#ifdef __SPARROW_INTERNAL_PATTERN__
				#ifdef __SPARROW_INTERNAL_ALPHA___
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_pixel_tex_ztest_zset_alpha_pattern_perspect( x, y, z, u, v, w, color );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_pixel_tex_ztest_alpha_pattern_perspect( x, y, z, u, v, w, color );
					#elif defined __SPARROW_INTERNAL_ZSET__
						draw_pixel_tex_zset_alpha_pattern_perspect( x, y, z, u, v, w, color );
					#else
						draw_pixel_tex_alpha_pattern_perspect( x, y, u, v, w, color );
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_pixel_tex_ztest_zset_pattern_perspect( x, y, z, u, v, w, color );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_pixel_tex_ztest_pattern_perspect( x, y, z, u, v, w, color );
					#elif defined __SPARROW_INTERNAL_ZSET__
						draw_pixel_tex_zset_pattern_perspect( x, y, z, u, v, w, color );
					#else
						draw_pixel_tex_pattern_perspect( x, y, u, v, w, color );
					#endif
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_ALPHA___
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_pixel_tex_ztest_zset_alpha_perspect( x, y, z, u, v, w, color );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_pixel_tex_ztest_alpha_perspect( x, y, z, u, v, w, color );
					#elif defined __SPARROW_INTERNAL_ZSET__
						draw_pixel_tex_zset_alpha_perspect( x, y, z, u, v, w, color );
					#else
						draw_pixel_tex_alpha_perspect( x, y, u, v, w, color );
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_pixel_tex_ztest_zset_perspect( x, y, z, u, v, w, color );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_pixel_tex_ztest_perspect( x, y, z, u, v, w, color );
					#elif defined __SPARROW_INTERNAL_ZSET__
						draw_pixel_tex_zset_perspect( x, y, z, u, v, w, color );
					#else
						draw_pixel_tex_perspect( x, y, u, v, w, color );
					#endif
				#endif
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_PATTERN__
				#ifdef __SPARROW_INTERNAL_ALPHA___
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_pixel_tex_ztest_zset_alpha_pattern( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_pixel_tex_ztest_alpha_pattern( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#elif defined __SPARROW_INTERNAL_ZSET__
						draw_pixel_tex_zset_alpha_pattern( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#else
						draw_pixel_tex_alpha_pattern( x, y, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_pixel_tex_ztest_zset_pattern( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_pixel_tex_ztest_pattern( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#elif defined __SPARROW_INTERNAL_ZSET__
						draw_pixel_tex_zset_pattern( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#else
						draw_pixel_tex_pattern( x, y, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#endif
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_ALPHA___
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_pixel_tex_ztest_zset_alpha( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_pixel_tex_ztest_alpha( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#elif defined __SPARROW_INTERNAL_ZSET__
						draw_pixel_tex_zset_alpha( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#else
						draw_pixel_tex_alpha( x, y, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ZBOTH__
						draw_pixel_tex_ztest_zset( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#elif defined __SPARROW_INTERNAL_ZTEST__
						draw_pixel_tex_ztest( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#elif defined __SPARROW_INTERNAL_ZSET__
						draw_pixel_tex_zset( x, y, z, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#else
						draw_pixel_tex( x, y, u >> SP_ACCURACY, v >> SP_ACCURACY, color );
					#endif
				#endif
			#endif
		#endif
		u += sU;
		v += sV;
		#ifdef __SPARROW_INTERNAL_PERSPECT__
			w += sW;
		#endif
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			z += sZ;
		#endif
	}
}

#ifdef __SPARROW_INTERNAL_PERSPECT__
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ALPHA___
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				inline void sp_intern_Triangle_tex_ztest_zset_alpha_pattern_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				inline void sp_intern_Triangle_tex_ztest_alpha_pattern_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZSET__
				inline void sp_intern_Triangle_tex_zset_alpha_pattern_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#else
				inline void sp_intern_Triangle_tex_alpha_pattern_perspect( Sint32 x1, Sint32 y1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				inline void sp_intern_Triangle_tex_ztest_zset_pattern_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				inline void sp_intern_Triangle_tex_ztest_pattern_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZSET__
				inline void sp_intern_Triangle_tex_zset_pattern_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#else
				inline void sp_intern_Triangle_tex_pattern_perspect( Sint32 x1, Sint32 y1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#endif
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ALPHA___
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				inline void sp_intern_Triangle_tex_ztest_zset_alpha_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				inline void sp_intern_Triangle_tex_ztest_alpha_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZSET__
				inline void sp_intern_Triangle_tex_zset_alpha_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#else
				inline void sp_intern_Triangle_tex_alpha_perspect( Sint32 x1, Sint32 y1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				inline void sp_intern_Triangle_tex_ztest_zset_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				inline void sp_intern_Triangle_tex_ztest_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZSET__
				inline void sp_intern_Triangle_tex_zset_perspect( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#else
				inline void sp_intern_Triangle_tex_perspect( Sint32 x1, Sint32 y1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color )
			#endif
		#endif
	#endif
#else
	#ifdef __SPARROW_INTERNAL_PATTERN__
		#ifdef __SPARROW_INTERNAL_ALPHA___
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				inline void sp_intern_Triangle_tex_ztest_zset_alpha_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				inline void sp_intern_Triangle_tex_ztest_alpha_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZSET__
				inline void sp_intern_Triangle_tex_zset_alpha_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#else
				inline void sp_intern_Triangle_tex_alpha_pattern( Sint32 x1, Sint32 y1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 u3, Sint32 v3, Uint32 color )
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				inline void sp_intern_Triangle_tex_ztest_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				inline void sp_intern_Triangle_tex_ztest_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZSET__
				inline void sp_intern_Triangle_tex_zset_pattern( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#else
				inline void sp_intern_Triangle_tex_pattern( Sint32 x1, Sint32 y1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 u3, Sint32 v3, Uint32 color )
			#endif
		#endif
	#else
		#ifdef __SPARROW_INTERNAL_ALPHA___
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				inline void sp_intern_Triangle_tex_ztest_zset_alpha( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				inline void sp_intern_Triangle_tex_ztest_alpha( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZSET__
				inline void sp_intern_Triangle_tex_zset_alpha( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#else
				inline void sp_intern_Triangle_tex_alpha( Sint32 x1, Sint32 y1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 u3, Sint32 v3, Uint32 color )
			#endif
		#else
			#ifdef __SPARROW_INTERNAL_ZBOTH__
				inline void sp_intern_Triangle_tex_ztest_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZTEST__
				inline void sp_intern_Triangle_tex_ztest( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#elif defined __SPARROW_INTERNAL_ZSET__
				inline void sp_intern_Triangle_tex_zset( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color )
			#else
				inline void sp_intern_Triangle_tex( Sint32 x1, Sint32 y1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 u3, Sint32 v3, Uint32 color )
			#endif
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

	int div = y2 - y1;
	Sint32 mul = y3 - y1;
	Sint32 mul32 = mul * one_over_x( div );
	Sint32 x4 = x1 + ( ( x2 - x1 ) * mul32 >> SP_ACCURACY );
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 z4 = z1 + mul * z_div( z2 - z1, div );
	#endif
	
	#ifdef __SPARROW_INTERNAL_PERSPECT__
		Sint32 w4 = w1 + mul * z_div( w2 - w1, div );
		Sint32 u4 = u1 + mul * z_div( u2 - u1, div );
		Sint32 v4 = v1 + mul * z_div( v2 - v1, div );
	#else
		Sint32 u4 = u1 + ( ( u2 - u1 ) * mul32 >> SP_ACCURACY );
		Sint32 v4 = v1 + ( ( v2 - v1 ) * mul32 >> SP_ACCURACY );	
	#endif

	Sint32 xl = x1 << SP_ACCURACY;
	#ifdef __SPARROW_INTERNAL_PERSPECT__
		Sint32 ul = u1;
		Sint32 vl = v1;	
		Sint32 wl = w1;
	#else
		Sint32 ul = u1 << SP_ACCURACY;
		Sint32 vl = v1 << SP_ACCURACY;	
	#endif
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 zl = z1;
	#endif

	mul = one_over_x( y1 - y2 );
	Sint32 sX_l = ( x1 - x2 ) * mul;
	#ifdef __SPARROW_INTERNAL_PERSPECT__
		Sint32 sW_l = z_div( w1 - w2, y1 - y2 );
		Sint32 sU_l = z_div( u1 - u2, y1 - y2 );
		Sint32 sV_l = z_div( v1 - v2, y1 - y2 );
	#else
		Sint32 sU_l = ( u1 - u2 ) * mul;
		Sint32 sV_l = ( v1 - v2 ) * mul;
	#endif
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 sZ_l = z_div( z1 - z2, y1 - y2 );
	#endif

	Sint32 xr = xl;
	Sint32 ur = ul;
	Sint32 vr = vl;
	#ifdef __SPARROW_INTERNAL_PERSPECT__
		Sint32 wr = wl;
	#endif
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 zr = zl;
	#endif
	mul = one_over_x( y1 - y3 );
	Sint32 sX_r = ( x1 - x3 ) * mul;
	#ifdef __SPARROW_INTERNAL_PERSPECT__
		Sint32 sW_r = z_div( w1 - w3, y1 - y3 );
		Sint32 sU_r = z_div( u1 - u3, y1 - y3 );
		Sint32 sV_r = z_div( v1 - v3, y1 - y3 );
	#else
		Sint32 sU_r = ( u1 - u3 ) * mul;
		Sint32 sV_r = ( v1 - v3 ) * mul;	
	#endif
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 sZ_r = z_div( z1 - z3, y1 - y3 );
	#endif

	if ( y3 < 0 )
	{
		int diff = y3 - y1;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		#ifdef __SPARROW_INTERNAL_PERSPECT__
			wl += sW_l * diff;
		#endif
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			zl += sZ_l * diff;
		#endif
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		#ifdef __SPARROW_INTERNAL_PERSPECT__
			wr += sW_r * diff;
		#endif
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
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
			ul += sU_l * diff;
			vl += sV_l * diff;
			#ifdef __SPARROW_INTERNAL_PERSPECT__
				wl += sW_l * diff;
			#endif
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				zl += sZ_l * diff;
			#endif
			xr += sX_r * diff;
			ur += sU_r * diff;
			vr += sV_r * diff;
			#ifdef __SPARROW_INTERNAL_PERSPECT__
				wr += sW_r * diff;
			#endif
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				zr += sZ_r * diff;
			#endif
		}
		if ( y3 >= spTargetY )
			y3 = spTargetY - 1;

		Sint32 mul = one_over_x( x4 - x3 );
		#ifdef __SPARROW_INTERNAL_PERSPECT__
			Sint32 sW = z_div( w4 - w3, x4 - x3 );
			Sint32 sU = z_div( u4 - u3, x4 - x3 );
			Sint32 sV = z_div( v4 - v3, x4 - x3 );
		#else
			Sint32 sU = ( u4 - u3 ) * mul;
			Sint32 sV = ( v4 - v3 ) * mul;
		#endif
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			Sint32 sZ = z_div( z4 - z3, x4 - x3 );
		#endif
		if ( x4 < x3 )
		{
			for ( y = y1; y < y3; y++ )
			{
				#ifdef __SPARROW_INTERNAL_PERSPECT__
					#ifdef __SPARROW_INTERNAL_PATTERN__
						#ifdef __SPARROW_INTERNAL_ALPHA___
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_alpha_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
																xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_alpha_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
															 xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_alpha_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
															xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#else
								draw_line_tex_alpha_pattern_perspect( xl >> SP_ACCURACY, ul, vl, wl,
													 xr >> SP_ACCURACY, ur, vr, wr, y, color, sU, sV ,sW);
							#endif
						#else
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
															xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
													 xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
													xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#else
								draw_line_tex_pattern_perspect( xl >> SP_ACCURACY, ul, vl, wl,
												 xr >> SP_ACCURACY, ur, vr, wr, y, color, sU, sV ,sW);
							#endif
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ALPHA___
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_alpha_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
																xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_alpha_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
															 xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_alpha_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
															xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#else
								draw_line_tex_alpha_perspect( xl >> SP_ACCURACY, ul, vl, wl,
													 xr >> SP_ACCURACY, ur, vr, wr, y, color, sU, sV ,sW);
							#endif
						#else
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
															xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
													 xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
													xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV ,sW ,sZ );
							#else
								draw_line_tex_perspect( xl >> SP_ACCURACY, ul, vl, wl,
												 xr >> SP_ACCURACY, ur, vr, wr, y, color, sU, sV ,sW);
							#endif
						#endif
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_PATTERN__
						#ifdef __SPARROW_INTERNAL_ALPHA___
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_alpha_pattern( xl >> SP_ACCURACY, zl, ul, vl,
																xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_alpha_pattern( xl >> SP_ACCURACY, zl, ul, vl,
															 xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_alpha_pattern( xl >> SP_ACCURACY, zl, ul, vl,
															xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#else
								draw_line_tex_alpha_pattern( xl >> SP_ACCURACY, ul, vl,
													 xr >> SP_ACCURACY, ur, vr, y, color, sU, sV );
							#endif
						#else
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_pattern( xl >> SP_ACCURACY, zl, ul, vl,
															xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_pattern( xl >> SP_ACCURACY, zl, ul, vl,
													 xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_pattern( xl >> SP_ACCURACY, zl, ul, vl,
													xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#else
								draw_line_tex_pattern( xl >> SP_ACCURACY, ul, vl,
												 xr >> SP_ACCURACY, ur, vr, y, color, sU, sV );
							#endif
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ALPHA___
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_alpha( xl >> SP_ACCURACY, zl, ul, vl,
																xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_alpha( xl >> SP_ACCURACY, zl, ul, vl,
															 xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_alpha( xl >> SP_ACCURACY, zl, ul, vl,
															xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#else
								draw_line_tex_alpha( xl >> SP_ACCURACY, ul, vl,
													 xr >> SP_ACCURACY, ur, vr, y, color, sU, sV );
							#endif
						#else
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset( xl >> SP_ACCURACY, zl, ul, vl,
															xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest( xl >> SP_ACCURACY, zl, ul, vl,
													 xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset( xl >> SP_ACCURACY, zl, ul, vl,
													xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
							#else
								draw_line_tex( xl >> SP_ACCURACY, ul, vl,
												 xr >> SP_ACCURACY, ur, vr, y, color, sU, sV );
							#endif
						#endif
					#endif
				#endif
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				#ifdef __SPARROW_INTERNAL_PERSPECT__
					wl += sW_l;
				#endif
				#ifndef __SPARROW_INTERNAL_ZNOTHING__
					zl += sZ_l;
				#endif
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				#ifdef __SPARROW_INTERNAL_PERSPECT__
					wr += sW_r;
				#endif
				#ifndef __SPARROW_INTERNAL_ZNOTHING__
					zr += sZ_r;
				#endif
			}
		}
		else
		{
			for ( y = y1; y < y3; y++ )
			{
				#ifdef __SPARROW_INTERNAL_PERSPECT__
					#ifdef __SPARROW_INTERNAL_PATTERN__
						#ifdef __SPARROW_INTERNAL_ALPHA___
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_alpha_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
																xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_alpha_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
															 xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_alpha_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
															xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#else
								draw_line_tex_alpha_pattern_perspect( xr >> SP_ACCURACY, ur, vr, wr,
													 xl >> SP_ACCURACY, ul, vl, wl, y, color, sU, sV, sW );
							#endif
						#else
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
															xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
													 xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
													xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#else
								draw_line_tex_pattern_perspect( xr >> SP_ACCURACY, ur, vr, wr,
												 xl >> SP_ACCURACY, ul, vl, wl, y, color, sU, sV, sW );
							#endif
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ALPHA___
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_alpha_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
																xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_alpha_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
															 xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_alpha_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
															xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#else
								draw_line_tex_alpha_perspect( xr >> SP_ACCURACY, ur, vr, wr,
													 xl >> SP_ACCURACY, ul, vl, wl, y, color, sU, sV, sW );
							#endif
						#else
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
															xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
													 xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
													xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
							#else
								draw_line_tex_perspect( xr >> SP_ACCURACY, ur, vr, wr,
												 xl >> SP_ACCURACY, ul, vl, wl, y, color, sU, sV, sW );
							#endif
						#endif
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_PATTERN__
						#ifdef __SPARROW_INTERNAL_ALPHA___
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_alpha_pattern( xr >> SP_ACCURACY, zr, ur, vr,
																xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_alpha_pattern( xr >> SP_ACCURACY, zr, ur, vr,
															 xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_alpha_pattern( xr >> SP_ACCURACY, zr, ur, vr,
															xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#else
								draw_line_tex_alpha_pattern( xr >> SP_ACCURACY, ur, vr,
													 xl >> SP_ACCURACY, ul, vl, y, color, sU, sV );
							#endif
						#else
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_pattern( xr >> SP_ACCURACY, zr, ur, vr,
															xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_pattern( xr >> SP_ACCURACY, zr, ur, vr,
													 xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_pattern( xr >> SP_ACCURACY, zr, ur, vr,
													xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#else
								draw_line_tex_pattern( xr >> SP_ACCURACY, ur, vr,
												 xl >> SP_ACCURACY, ul, vl, y, color, sU, sV );
							#endif
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ALPHA___
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset_alpha( xr >> SP_ACCURACY, zr, ur, vr,
																xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest_alpha( xr >> SP_ACCURACY, zr, ur, vr,
															 xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset_alpha( xr >> SP_ACCURACY, zr, ur, vr,
															xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#else
								draw_line_tex_alpha( xr >> SP_ACCURACY, ur, vr,
													 xl >> SP_ACCURACY, ul, vl, y, color, sU, sV );
							#endif
						#else
							#ifdef __SPARROW_INTERNAL_ZBOTH__
								draw_line_tex_ztest_zset( xr >> SP_ACCURACY, zr, ur, vr,
															xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZTEST__
								draw_line_tex_ztest( xr >> SP_ACCURACY, zr, ur, vr,
													 xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#elif defined __SPARROW_INTERNAL_ZSET__
								draw_line_tex_zset( xr >> SP_ACCURACY, zr, ur, vr,
													xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
							#else
								draw_line_tex( xr >> SP_ACCURACY, ur, vr,
												 xl >> SP_ACCURACY, ul, vl, y, color, sU, sV );
							#endif
						#endif
					#endif
				#endif
				xl += sX_l;
				ul += sU_l;
				vl += sV_l;
				#ifdef __SPARROW_INTERNAL_PERSPECT__
					wl += sW_l;
				#endif
				#ifndef __SPARROW_INTERNAL_ZNOTHING__
					zl += sZ_l;
				#endif
				xr += sX_r;
				ur += sU_r;
				vr += sV_r;
				#ifdef __SPARROW_INTERNAL_PERSPECT__
					wr += sW_r;
				#endif
				#ifndef __SPARROW_INTERNAL_ZNOTHING__
					zr += sZ_r;
				#endif
			}
		}
	}

	xr = x3 << SP_ACCURACY;
	#ifdef __SPARROW_INTERNAL_PERSPECT__
		wr = w3;
		ur = u3;
		vr = v3;
	#else
		ur = u3 << SP_ACCURACY;
		vr = v3 << SP_ACCURACY;
	#endif
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		zr = z3;
	#endif
	mul = one_over_x( y2 - y3 );
	sX_r = ( x2 - x3 ) * mul;
	#ifdef __SPARROW_INTERNAL_PERSPECT__
		sW_r = z_div( w2 - w3, y2 - y3 );
		sU_r = z_div( u2 - u3, y2 - y3 );
		sV_r = z_div( v2 - v3, y2 - y3 );
	#else
		sU_r = ( u2 - u3 ) * mul;
		sV_r = ( v2 - v3 ) * mul;
	#endif
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		sZ_r = z_div( z2 - z3, y2 - y3 );
	#endif

	if ( y3 < 0 )
	{
		int diff = -y3;
		y3 = 0;
		xl += sX_l * diff;
		ul += sU_l * diff;
		vl += sV_l * diff;
		#ifdef __SPARROW_INTERNAL_PERSPECT__
			wl += sW_l * diff;
		#endif
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			zl += sZ_l * diff;
		#endif
		xr += sX_r * diff;
		ur += sU_r * diff;
		vr += sV_r * diff;
		#ifdef __SPARROW_INTERNAL_PERSPECT__
			wr += sW_r * diff;
		#endif
		#ifndef __SPARROW_INTERNAL_ZNOTHING__
			zr += sZ_r * diff;
		#endif
	}
	if ( y2 >= spTargetY )
		y2 = spTargetY - 1;

	mul = one_over_x( x4 - x3 );
	#ifdef __SPARROW_INTERNAL_PERSPECT__
		Sint32 sW = z_div( w4 - w3, x4 - x3 );
		Sint32 sU = z_div( u4 - u3, x4 - x3 );
		Sint32 sV = z_div( v4 - v3, x4 - x3 );
	#else
		Sint32 sU = ( u4 - u3 ) * mul;
		Sint32 sV = ( v4 - v3 ) * mul;
	#endif
	#ifndef __SPARROW_INTERNAL_ZNOTHING__
		Sint32 sZ = z_div( z4 - z3, x4 - x3 );
	#endif
	if ( x4 < x3 )
	{
		for ( y = y3; y <= y2; y++ )
		{
			#ifdef __SPARROW_INTERNAL_PERSPECT__
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ALPHA___
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_alpha_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
															xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_alpha_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
														 xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_alpha_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
														xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#else
							draw_line_tex_alpha_pattern_perspect( xl >> SP_ACCURACY, ul, vl, wl,
												 xr >> SP_ACCURACY, ur, vr, wr, y, color, sU, sV, sW );
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
														xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
												 xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_pattern_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
												xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#else
							draw_line_tex_pattern_perspect( xl >> SP_ACCURACY, ul, vl, wl,
											 xr >> SP_ACCURACY, ur, vr, wr, y, color, sU, sV, sW );
						#endif
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ALPHA___
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_alpha_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
															xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_alpha_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
														 xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_alpha_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
														xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#else
							draw_line_tex_alpha_perspect( xl >> SP_ACCURACY, ul, vl, wl,
												 xr >> SP_ACCURACY, ur, vr, wr, y, color, sU, sV, sW );
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
														xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
												 xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_perspect( xl >> SP_ACCURACY, zl, ul, vl, wl,
												xr >> SP_ACCURACY, zr, ur, vr, wr, y, color, sU, sV, sW, sZ );
						#else
							draw_line_tex_perspect( xl >> SP_ACCURACY, ul, vl, wl,
											 xr >> SP_ACCURACY, ur, vr, wr, y, color, sU, sV, sW );
						#endif
					#endif
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ALPHA___
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_alpha_pattern( xl >> SP_ACCURACY, zl, ul, vl,
															xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_alpha_pattern( xl >> SP_ACCURACY, zl, ul, vl,
														 xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_alpha_pattern( xl >> SP_ACCURACY, zl, ul, vl,
														xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#else
							draw_line_tex_alpha_pattern( xl >> SP_ACCURACY, ul, vl,
												 xr >> SP_ACCURACY, ur, vr, y, color, sU, sV );
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_pattern( xl >> SP_ACCURACY, zl, ul, vl,
														xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_pattern( xl >> SP_ACCURACY, zl, ul, vl,
												 xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_pattern( xl >> SP_ACCURACY, zl, ul, vl,
												xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#else
							draw_line_tex_pattern( xl >> SP_ACCURACY, ul, vl,
											 xr >> SP_ACCURACY, ur, vr, y, color, sU, sV );
						#endif
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ALPHA___
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_alpha( xl >> SP_ACCURACY, zl, ul, vl,
															xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_alpha( xl >> SP_ACCURACY, zl, ul, vl,
														 xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_alpha( xl >> SP_ACCURACY, zl, ul, vl,
														xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#else
							draw_line_tex_alpha( xl >> SP_ACCURACY, ul, vl,
												 xr >> SP_ACCURACY, ur, vr, y, color, sU, sV );
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset( xl >> SP_ACCURACY, zl, ul, vl,
														xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest( xl >> SP_ACCURACY, zl, ul, vl,
												 xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset( xl >> SP_ACCURACY, zl, ul, vl,
												xr >> SP_ACCURACY, zr, ur, vr, y, color, sU, sV, sZ );
						#else
							draw_line_tex( xl >> SP_ACCURACY, ul, vl,
											 xr >> SP_ACCURACY, ur, vr, y, color, sU, sV );
						#endif
					#endif
				#endif
			#endif
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			#ifdef __SPARROW_INTERNAL_PERSPECT__
				wl += sW_l;
			#endif
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				zl += sZ_l;
			#endif
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			#ifdef __SPARROW_INTERNAL_PERSPECT__
				wr += sW_r;
			#endif
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				zr += sZ_r;
			#endif
		}
	}
	else
	{
		for ( y = y3; y <= y2; y++ )
		{
			#ifdef __SPARROW_INTERNAL_PERSPECT__
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ALPHA___
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_alpha_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
															xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_alpha_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
														 xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_alpha_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
														xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#else
							draw_line_tex_alpha_pattern_perspect( xr >> SP_ACCURACY, ur, vr, wr,
												 xl >> SP_ACCURACY, ul, vl, wl, y, color, sU, sV, sW );
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
														xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
												 xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_pattern_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
												xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#else
							draw_line_tex_pattern_perspect( xr >> SP_ACCURACY, ur, vr, wr,
											 xl >> SP_ACCURACY, ul, vl, wl, y, color, sU, sV, sW );
						#endif
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ALPHA___
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_alpha_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
															xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_alpha_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
														 xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_alpha_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
														xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#else
							draw_line_tex_alpha_perspect( xr >> SP_ACCURACY, ur, vr, wr,
												 xl >> SP_ACCURACY, ul, vl, wl, y, color, sU, sV, sW );
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
														xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
												 xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_perspect( xr >> SP_ACCURACY, zr, ur, vr, wr,
												xl >> SP_ACCURACY, zl, ul, vl, wl, y, color, sU, sV, sW, sZ );
						#else
							draw_line_tex_perspect( xr >> SP_ACCURACY, ur, vr, wr,
											 xl >> SP_ACCURACY, ul, vl, wl, y, color, sU, sV, sW );
						#endif
					#endif
				#endif
			#else
				#ifdef __SPARROW_INTERNAL_PATTERN__
					#ifdef __SPARROW_INTERNAL_ALPHA___
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_alpha_pattern( xr >> SP_ACCURACY, zr, ur, vr,
															xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_alpha_pattern( xr >> SP_ACCURACY, zr, ur, vr,
														 xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_alpha_pattern( xr >> SP_ACCURACY, zr, ur, vr,
														xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#else
							draw_line_tex_alpha_pattern( xr >> SP_ACCURACY, ur, vr,
												 xl >> SP_ACCURACY, ul, vl, y, color, sU, sV );
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_pattern( xr >> SP_ACCURACY, zr, ur, vr,
														xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_pattern( xr >> SP_ACCURACY, zr, ur, vr,
												 xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_pattern( xr >> SP_ACCURACY, zr, ur, vr,
												xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#else
							draw_line_tex_pattern( xr >> SP_ACCURACY, ur, vr,
											 xl >> SP_ACCURACY, ul, vl, y, color, sU, sV );
						#endif
					#endif
				#else
					#ifdef __SPARROW_INTERNAL_ALPHA___
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset_alpha( xr >> SP_ACCURACY, zr, ur, vr,
															xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest_alpha( xr >> SP_ACCURACY, zr, ur, vr,
														 xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset_alpha( xr >> SP_ACCURACY, zr, ur, vr,
														xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#else
							draw_line_tex_alpha( xr >> SP_ACCURACY, ur, vr,
												 xl >> SP_ACCURACY, ul, vl, y, color, sU, sV );
						#endif
					#else
						#ifdef __SPARROW_INTERNAL_ZBOTH__
							draw_line_tex_ztest_zset( xr >> SP_ACCURACY, zr, ur, vr,
														xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZTEST__
							draw_line_tex_ztest( xr >> SP_ACCURACY, zr, ur, vr,
												 xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#elif defined __SPARROW_INTERNAL_ZSET__
							draw_line_tex_zset( xr >> SP_ACCURACY, zr, ur, vr,
												xl >> SP_ACCURACY, zl, ul, vl, y, color, sU, sV, sZ );
						#else
							draw_line_tex( xr >> SP_ACCURACY, ur, vr,
											 xl >> SP_ACCURACY, ul, vl, y, color, sU, sV );
						#endif
					#endif
				#endif
			#endif
			xl += sX_l;
			ul += sU_l;
			vl += sV_l;
			#ifdef __SPARROW_INTERNAL_PERSPECT__
				wl += sW_l;
			#endif
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				zl += sZ_l;
			#endif
			xr += sX_r;
			ur += sU_r;
			vr += sV_r;
			#ifdef __SPARROW_INTERNAL_PERSPECT__
				wr += sW_r;
			#endif
			#ifndef __SPARROW_INTERNAL_ZNOTHING__
				zr += sZ_r;
			#endif
		}
	}
	SDL_UnlockSurface( spTarget );
}
