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
 
/* ******************** */
/* draw_pixel functions */
/* ******************** */

#define draw_pixel_ztest_zset(x,y,z,color) \
	{if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{\
		spZBuffer[(x) + (y) * spTargetScanLine] = (z);\
		spTargetPixel[(x) + (y) * spTargetScanLine] = (color);\
	}}

#define draw_pixel_ztest(x,y,z,color) \
	{if ( (z) > spZBuffer[(x) + (y) * spTargetScanLine] ) \
		spTargetPixel[(x) + (y) * spTargetScanLine] = (color);}

#define draw_pixel_zset(x,y,z,color) \
	{spTargetPixel[(x) + (y) * spTargetScanLine] = (color); \
	spZBuffer[(x) + (y) * spTargetScanLine] = (z);}

#define draw_pixel(x,y,color) \
	{spTargetPixel[(x) + (y) * spTargetScanLine] = (color);}
	
#define draw_pixel_blending_ztest_zset(x,y,z,color,blend) \
	{if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{\
		spZBuffer[(x) + (y) * spTargetScanLine] = (z);\
		Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine];  \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  color & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  color & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  color & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    );}}

#define draw_pixel_blending_ztest(x,y,z,color,blend) \
	{if ( (z) > spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine];  \
			Sint32 one_minus_blend = SP_ONE - (blend); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = \
				( ( (  color & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
			+ ( ( (  color & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
			+ ( ( (  color & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
			+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
			+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
			+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    );}}

#define draw_pixel_blending_zset(x,y,z,color,blend) \
	{	Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine];  \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  color & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  color & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  color & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z);}

#define draw_pixel_blending(x,y,color,blend) \
	{	Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine];  \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  color & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  color & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  color & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    );}
	
// + Pattern

#define draw_pixel_ztest_zset_pattern(x,y,z,color,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_ztest_zset(x,y,z,color)}

#define draw_pixel_ztest_pattern(x,y,z,color,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_ztest(x,y,z,color)}

#define draw_pixel_zset_pattern(x,y,z,color,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_zset(x,y,z,color)}

#define draw_pixel_pattern(x,y,color,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel(x,y,color)}

#define draw_pixel_blending_ztest_zset_pattern(x,y,z,color,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_ztest_zset(x,y,z,color,blend)}

#define draw_pixel_blending_ztest_pattern(x,y,z,color,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_ztest(x,y,z,color,blend)}

#define draw_pixel_blending_zset_pattern(x,y,z,color,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_zset(x,y,z,color,blend)}

#define draw_pixel_blending_pattern(x,y,color,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending(x,y,color,blend)}


/* ********************************** */
/* draw_pixel functions with textures */
/* ********************************** */

/*#ifdef PANDORA
	#define TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY) \
		int32x2_t T = {u,v}; \
		int32x2_t MAX = {0,0}; \
		int32x2_t MIN = {textureX-1,textureY-1}; \
		T = vmin_s32(vmax_s32(T,MAX),MIN); \
		Uint32 pixel = texturePixel[T[0] + T[1] * textureScanLine];*/
		
#ifdef UNSAFE_MAGIC
	#define TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY) \
		Uint32 pixel = (u) + (v) * textureScanLine; \
		pixel = texturePixel[(pixel < 0)?0:(pixel>textureY*textureScanLine)?textureY*textureScanLine:pixel];
#else
	#define TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY) \
		Uint32 pixel = texturePixel[(((u)<0)?0:((u)>=textureX)?textureX-1:(u)) + (((v)<0)?0:((v)>=textureY)?textureY-1:(v)) * textureScanLine];
#endif

#define draw_pixel_tex_ztest_zset(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	} \
}

#define draw_pixel_tex_ztest(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	} \
}

#define draw_pixel_tex_zset(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
	TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
	spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																				+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
}

#define draw_pixel_tex(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
	spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																				+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
}

#define draw_pixel_blending_tex_ztest_zset(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
		Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
		pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
	} \
}

#define draw_pixel_blending_tex_ztest(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
		Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
		pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
	} \
}

#define draw_pixel_blending_tex_zset(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
	TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
	Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
	pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
				+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
				+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	Sint32 one_minus_blend = SP_ONE - (blend); \
	spTargetPixel[(x) + (y) * spTargetScanLine] = \
		( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
	+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
	+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
	+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
	+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
	+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
}

#define draw_pixel_blending_tex(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
	Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
	pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
				+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
				+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	Sint32 one_minus_blend = SP_ONE - (blend); \
	spTargetPixel[(x) + (y) * spTargetScanLine] = \
		( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
	+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
	+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
	+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
	+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
	+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
}

//Perspective

//#define reciprocal_w_clip(parameter,w) (spMulLow(parameter,spDiv(SP_ONE,(w>>spMaxWLogDiff))>>spMaxWLogDiff) >> SP_ACCURACY)
#define reciprocal_w_clip(parameter,w) ((parameter >> SP_HALF_ACCURACY)*(spOne_over_x_look_up_fixed[(w>>spMaxWLogDiff+SP_MAGIC_W_CLIP_ACCURACY_CONSTANT) & (SP_ONE-1)]>>spMaxWLogDiff) >> SP_ACCURACY)
//#define reciprocal_w_clip(parameter,w) ((parameter)/(w>>SP_MAGIC_W_CLIP_ACCURACY_CONSTANT))

#define draw_pixel_tex_ztest_zset_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	} \
}

#define draw_pixel_tex_ztest_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	} \
}

#define draw_pixel_tex_zset_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
	Sint32 uw = reciprocal_w_clip(u,w); \
	Sint32 vw = reciprocal_w_clip(v,w); \
	TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
	spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																				+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
}

#define draw_pixel_tex_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	Sint32 uw = reciprocal_w_clip(u,w); \
	Sint32 vw = reciprocal_w_clip(v,w); \
	TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
	spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																				+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
}

#define draw_pixel_blending_tex_ztest_zset_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
		Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
		pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
	} \
}

#define draw_pixel_blending_tex_ztest_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
		Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
		pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
	} \
}

#define draw_pixel_blending_tex_zset_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
	Sint32 uw = reciprocal_w_clip(u,w); \
	Sint32 vw = reciprocal_w_clip(v,w); \
	TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
	Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
	pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
				+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
				+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	Sint32 one_minus_blend = SP_ONE - (blend); \
	spTargetPixel[(x) + (y) * spTargetScanLine] = \
		( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
	+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
	+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
	+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
	+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
	+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
}

#define draw_pixel_blending_tex_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	Sint32 uw = reciprocal_w_clip(u,w); \
	Sint32 vw = reciprocal_w_clip(v,w); \
	TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
	Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
	pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
				+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
				+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	Sint32 one_minus_blend = SP_ONE - (blend); \
	spTargetPixel[(x) + (y) * spTargetScanLine] = \
		( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
	+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
	+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
	+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
	+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
	+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
}

// + Pattern
#define draw_pixel_tex_ztest_zset_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_zset(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_ztest_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_zset_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_zset(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_pattern(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_ztest_zset_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_zset_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_ztest_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_zset_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_zset_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_pattern_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_blending_tex_ztest_zset_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_ztest_zset(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_ztest_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_ztest(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_zset_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_zset(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_pattern(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_ztest_zset_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_ztest_zset_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_ztest_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_ztest_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_zset_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_zset_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_pattern_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend)}

/* ******************************************** */
/* draw_pixel functions with textures and alpha */
/* ******************************************** */

#define draw_pixel_tex_ztest_zset_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
		if (pixel != SP_ALPHA_COLOR) \
		{ \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	} \
}

#define draw_pixel_tex_ztest_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
		if (pixel != SP_ALPHA_COLOR) \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	} \
}

#define draw_pixel_tex_zset_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
	if (pixel != SP_ALPHA_COLOR) \
	{ \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	} \
}

#define draw_pixel_tex_alpha(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
	if (pixel != SP_ALPHA_COLOR) \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
}

#define draw_pixel_tex_ztest_zset_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
		if (pixel != SP_ALPHA_COLOR) \
		{ \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	} \
}

#define draw_pixel_tex_ztest_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
		if (pixel != SP_ALPHA_COLOR) \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	} \
}

#define draw_pixel_tex_zset_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	Sint32 uw = reciprocal_w_clip(u,w); \
	Sint32 vw = reciprocal_w_clip(v,w); \
	TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
	if (pixel != SP_ALPHA_COLOR) \
	{ \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	} \
}

#define draw_pixel_tex_alpha_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY) \
{ \
	Sint32 uw = reciprocal_w_clip(u,w); \
	Sint32 vw = reciprocal_w_clip(v,w); \
	TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
	if (pixel != SP_ALPHA_COLOR) \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
}

#define draw_pixel_blending_tex_ztest_zset_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
		if (pixel != SP_ALPHA_COLOR) \
		{ \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
			pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
			Sint32 one_minus_blend = SP_ONE - (blend); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = \
				( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
			+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
			+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
			+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
			+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
			+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
		} \
	} \
}

#define draw_pixel_blending_tex_ztest_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
		if (pixel != SP_ALPHA_COLOR) \
		{ \
			Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
			pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
			Sint32 one_minus_blend = SP_ONE - (blend); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = \
				( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
			+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
			+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
			+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
			+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
			+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
		} \
	} \
}

#define draw_pixel_blending_tex_zset_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
	if (pixel != SP_ALPHA_COLOR) \
	{ \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
		pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
	} \
}

#define draw_pixel_blending_tex_alpha(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	TEXTURE_CLAMP_MACRO(u,v,texturePixel,textureScanLine,textureX,textureY)  \
	if (pixel != SP_ALPHA_COLOR) \
	{ \
		Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
		pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
	} \
}

#define draw_pixel_blending_tex_ztest_zset_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
		if (pixel != SP_ALPHA_COLOR) \
		{ \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
			pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
			Sint32 one_minus_blend = SP_ONE - (blend); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = \
				( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
			+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
			+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
			+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
			+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
			+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
		} \
	} \
}

#define draw_pixel_blending_tex_ztest_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
		if (pixel != SP_ALPHA_COLOR) \
		{ \
			Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
			pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
			Sint32 one_minus_blend = SP_ONE - (blend); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = \
				( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
			+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
			+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
			+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
			+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
			+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
		} \
	} \
}

#define draw_pixel_blending_tex_zset_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	Sint32 uw = reciprocal_w_clip(u,w); \
	Sint32 vw = reciprocal_w_clip(v,w); \
	TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
	if (pixel != SP_ALPHA_COLOR) \
	{ \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
		pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
	} \
}

#define draw_pixel_blending_tex_alpha_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend) \
{ \
	Sint32 uw = reciprocal_w_clip(u,w); \
	Sint32 vw = reciprocal_w_clip(v,w); \
	TEXTURE_CLAMP_MACRO(uw,vw,texturePixel,textureScanLine,textureX,textureY)  \
	if (pixel != SP_ALPHA_COLOR) \
	{ \
		Uint32 buffer = spTargetPixel[(x) + (y) * spTargetScanLine]; \
		pixel = ( ( pixel * (color) >> 16 ) & 63488 )  \
					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
					+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		Sint32 one_minus_blend = SP_ONE - (blend); \
		spTargetPixel[(x) + (y) * spTargetScanLine] = \
			( ( (  pixel & 63488 ) *           blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( (  pixel & 2016  ) *           blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( (  pixel & 31    ) *           blend >> SP_ACCURACY) & 31    ) \
		+ ( ( ( buffer & 63488 ) * one_minus_blend >> SP_ACCURACY) & 63488 ) \
		+ ( ( ( buffer & 2016  ) * one_minus_blend >> SP_ACCURACY) & 2016  ) \
		+ ( ( ( buffer & 31    ) * one_minus_blend >> SP_ACCURACY) & 31    ); \
	} \
}


// + Pattern
#define draw_pixel_tex_ztest_zset_alpha_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_zset_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_ztest_alpha_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_zset_alpha_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_zset_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_alpha_pattern(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_alpha(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_ztest_zset_alpha_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_zset_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_ztest_alpha_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_zset_alpha_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_zset_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY)}

#define draw_pixel_tex_alpha_pattern_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_alpha_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY)}



#define draw_pixel_blending_tex_ztest_zset_alpha_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_ztest_zset_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_ztest_alpha_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_ztest_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_zset_alpha_pattern(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_zset_alpha(x,y,z,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_alpha_pattern(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_alpha(x,y,u,v,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_ztest_zset_alpha_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_ztest_zset_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_ztest_alpha_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_ztest_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_zset_alpha_pattern_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_zset_alpha_perspect(x,y,z,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend)}

#define draw_pixel_blending_tex_alpha_pattern_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,pattern,blend) \
	{if ((pattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_blending_tex_alpha_perspect(x,y,u,v,w,color,texturePixel,textureScanLine,textureX,textureY,blend)}
