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
	
// + Pattern

#define draw_pixel_ztest_zset_pattern(x,y,z,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_ztest_zset(x,y,z,color)}

#define draw_pixel_ztest_pattern(x,y,z,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_ztest(x,y,z,color)}

#define draw_pixel_zset_pattern(x,y,z,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_zset(x,y,z,color)}

#define draw_pixel_pattern(x,y,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel(x,y,color)}

/* ********************************** */
/* draw_pixel functions with textures */
/* ********************************** */

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_ztest_zset(x,y,z,u,v,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			Uint32 pixel = spTexturePixel[(u) + (v) * spTextureScanLine];  \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#else
	#define draw_pixel_tex_ztest_zset(x,y,z,u,v,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			Uint32 pixel = spTexturePixel[(((u)<0)?0:((u)>=spTextureX)?spTextureX-1:(u)) + (((v)<0)?0:((v)>=spTextureY)?spTextureY-1:(v)) * spTextureScanLine];  \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_ztest(x,y,z,u,v,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Uint32 pixel = spTexturePixel[(u) + (v) * spTextureScanLine];  \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#else
	#define draw_pixel_tex_ztest(x,y,z,u,v,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Uint32 pixel = spTexturePixel[(((u)<0)?0:((u)>=spTextureX)?spTextureX-1:(u)) + (((v)<0)?0:((v)>=spTextureY)?spTextureY-1:(v)) * spTextureScanLine];  \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_zset(x,y,z,u,v,color) \
	{ \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		Uint32 pixel = spTexturePixel[(u) + (v) * spTextureScanLine];  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#else
	#define draw_pixel_tex_zset(x,y,z,u,v,color) \
	{ \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		Uint32 pixel = spTexturePixel[(((u)<0)?0:((u)>=spTextureX)?spTextureX-1:(u)) + (((v)<0)?0:((v)>=spTextureY)?spTextureY-1:(v)) * spTextureScanLine];  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex(x,y,u,v,color) \
	{ \
		Uint32 pixel = spTexturePixel[(u) + (v) * spTextureScanLine];  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#else
	#define draw_pixel_tex(x,y,u,v,color) \
	{ \
		Uint32 pixel = spTexturePixel[(((u)<0)?0:((u)>=spTextureX)?spTextureX-1:(u)) + (((v)<0)?0:((v)>=spTextureY)?spTextureY-1:(v)) * spTextureScanLine];  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#endif

//Perspective

//#define reciprocal_w_clip(parameter,w) (spMulLow(parameter,spDiv(SP_ONE,(w>>spMaxWLogDiff))>>spMaxWLogDiff) >> SP_ACCURACY)
#define reciprocal_w_clip(parameter,w) ((parameter >> SP_HALF_ACCURACY)*(spOne_over_x_look_up_fixed[(w>>spMaxWLogDiff) & (SP_ONE-1)]>>spMaxWLogDiff) >> SP_ACCURACY)

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_ztest_zset_perspect(x,y,z,u,v,w,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Sint32 uw = reciprocal_w_clip(u,w); \
			Sint32 vw = reciprocal_w_clip(v,w); \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			Uint32 pixel = spTexturePixel[(uw) + (vw) * spTextureScanLine];  \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#else
	#define draw_pixel_tex_ztest_zset_perspect(x,y,z,u,v,w,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Sint32 uw = reciprocal_w_clip(u,w); \
			Sint32 vw = reciprocal_w_clip(v,w); \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			Uint32 pixel = spTexturePixel[(((uw)<0)?0:((uw)>=spTextureX)?spTextureX-1:(uw)) + (((vw)<0)?0:((vw)>=spTextureY)?spTextureY-1:(vw)) * spTextureScanLine];  \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_ztest_perspect(x,y,z,u,v,w,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Sint32 uw = reciprocal_w_clip(u,w); \
			Sint32 vw = reciprocal_w_clip(v,w); \
			Uint32 pixel = spTexturePixel[(uw) + (vw) * spTextureScanLine];  \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#else
	#define draw_pixel_tex_ztest_perspect(x,y,z,u,v,w,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Sint32 uw = reciprocal_w_clip(u,w); \
			Sint32 vw = reciprocal_w_clip(v,w); \
			Uint32 pixel = spTexturePixel[(((uw)<0)?0:((uw)>=spTextureX)?spTextureX-1:(uw)) + (((vw)<0)?0:((vw)>=spTextureY)?spTextureY-1:(vw)) * spTextureScanLine];  \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_zset_perspect(x,y,z,u,v,w,color) \
	{ \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		Uint32 pixel = spTexturePixel[(uw) + (vw) * spTextureScanLine];  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#else
	#define draw_pixel_tex_zset_perspect(x,y,z,u,v,w,color) \
	{ \
		spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		Uint32 pixel = spTexturePixel[(((uw)<0)?0:((uw)>=spTextureX)?spTextureX-1:(uw)) + (((vw)<0)?0:((vw)>=spTextureY)?spTextureY-1:(vw)) * spTextureScanLine];  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_perspect(x,y,u,v,w,color) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		Uint32 pixel = spTexturePixel[(uw) + (vw) * spTextureScanLine];  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#else
	#define draw_pixel_tex_perspect(x,y,u,v,w,color) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		Uint32 pixel = spTexturePixel[(((uw)<0)?0:((uw)>=spTextureX)?spTextureX-1:(uw)) + (((vw)<0)?0:((vw)>=spTextureY)?spTextureY-1:(vw)) * spTextureScanLine];  \
		spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																					+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																						+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#endif

// + Pattern
#define draw_pixel_tex_ztest_zset_pattern(x,y,z,u,v,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_zset(x,y,z,u,v,color)}

#define draw_pixel_tex_ztest_pattern(x,y,z,u,v,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest(x,y,z,u,v,color)}

#define draw_pixel_tex_zset_pattern(x,y,z,u,v,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_zset(x,y,z,u,v,color)}

#define draw_pixel_tex_pattern(x,y,u,v,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex(x,y,u,v,color)}

#define draw_pixel_tex_ztest_zset_pattern_perspect(x,y,z,u,v,w,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_zset_perspect(x,y,z,u,v,w,color)}

#define draw_pixel_tex_ztest_pattern_perspect(x,y,z,u,v,w,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_perspect(x,y,z,u,v,w,color)}

#define draw_pixel_tex_zset_pattern_perspect(x,y,z,u,v,w,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_zset_perspect(x,y,z,u,v,w,color)}

#define draw_pixel_tex_pattern_perspect(x,y,u,v,w,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_perspect(x,y,u,v,w,color)}

/* ******************************************** */
/* draw_pixel functions with textures and alpha */
/* ******************************************** */

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_ztest_zset_alpha(x,y,z,u,v,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Uint32 pixel = spTexturePixel[(u) + (v) * spTextureScanLine];  \
			if (pixel != SP_ALPHA_COLOR) \
			{ \
				spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
				spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																							+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																								+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
			} \
		} \
	}
#else
	#define draw_pixel_tex_ztest_zset_alpha(x,y,z,u,v,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Uint32 pixel = spTexturePixel[(((u)<0)?0:((u)>=spTextureX)?spTextureX-1:(u)) + (((v)<0)?0:((v)>=spTextureY)?spTextureY-1:(v)) * spTextureScanLine];  \
			if (pixel != SP_ALPHA_COLOR) \
			{ \
				spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
				spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																							+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																								+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
			} \
		} \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_ztest_alpha(x,y,z,u,v,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Uint32 pixel = spTexturePixel[(u) + (v) * spTextureScanLine];  \
			if (pixel != SP_ALPHA_COLOR) \
				spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																							+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																								+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#else
	#define draw_pixel_tex_ztest_alpha(x,y,z,u,v,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Uint32 pixel = spTexturePixel[(((u)<0)?0:((u)>=spTextureX)?spTextureX-1:(u)) + (((v)<0)?0:((v)>=spTextureY)?spTextureY-1:(v)) * spTextureScanLine];  \
			if (pixel != SP_ALPHA_COLOR) \
				spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																							+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																								+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_zset_alpha(x,y,z,u,v,color) \
	{ \
		Uint32 pixel = spTexturePixel[(u) + (v) * spTextureScanLine];  \
		if (pixel != SP_ALPHA_COLOR) \
		{ \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#else
	#define draw_pixel_tex_zset_alpha(x,y,z,u,v,color) \
	{ \
		Uint32 pixel = spTexturePixel[(((u)<0)?0:((u)>=spTextureX)?spTextureX-1:(u)) + (((v)<0)?0:((v)>=spTextureY)?spTextureY-1:(v)) * spTextureScanLine];  \
		if (pixel != SP_ALPHA_COLOR) \
		{ \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_alpha(x,y,u,v,color) \
	{ \
		Uint32 pixel = spTexturePixel[(u) + (v) * spTextureScanLine];  \
		if (pixel != SP_ALPHA_COLOR) \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#else
	#define draw_pixel_tex_alpha(x,y,u,v,color) \
	{ \
		Uint32 pixel = spTexturePixel[(((u)<0)?0:((u)>=spTextureX)?spTextureX-1:(u)) + (((v)<0)?0:((v)>=spTextureY)?spTextureY-1:(v)) * spTextureScanLine];  \
		if (pixel != SP_ALPHA_COLOR) \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_ztest_zset_alpha_perspect(x,y,z,u,v,w,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Sint32 uw = reciprocal_w_clip(u,w); \
			Sint32 vw = reciprocal_w_clip(v,w); \
			Uint32 pixel = spTexturePixel[(uw) + (vw) * spTextureScanLine];  \
			if (pixel != SP_ALPHA_COLOR) \
			{ \
				spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
				spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																							+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																								+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
			} \
		} \
	}
#else
	#define draw_pixel_tex_ztest_zset_alpha_perspect(x,y,z,u,v,w,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Sint32 uw = reciprocal_w_clip(u,w); \
			Sint32 vw = reciprocal_w_clip(v,w); \
			Uint32 pixel = spTexturePixel[(((uw)<0)?0:((uw)>=spTextureX)?spTextureX-1:(uw)) + (((vw)<0)?0:((vw)>=spTextureY)?spTextureY-1:(vw)) * spTextureScanLine];  \
			if (pixel != SP_ALPHA_COLOR) \
			{ \
				spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
				spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																							+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																								+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
			} \
		} \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_ztest_alpha_perspect(x,y,z,u,v,w,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Sint32 uw = reciprocal_w_clip(u,w); \
			Sint32 vw = reciprocal_w_clip(v,w); \
			Uint32 pixel = spTexturePixel[(uw) + (vw) * spTextureScanLine];  \
			if (pixel != SP_ALPHA_COLOR) \
				spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																							+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																								+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#else
	#define draw_pixel_tex_ztest_alpha_perspect(x,y,z,u,v,w,color) \
	{ \
		if ( (Uint32)(z) < spZBuffer[(x) + (y) * spTargetScanLine] ) \
		{ \
			Sint32 uw = reciprocal_w_clip(u,w); \
			Sint32 vw = reciprocal_w_clip(v,w); \
			Uint32 pixel = spTexturePixel[(((uw)<0)?0:((uw)>=spTextureX)?spTextureX-1:(uw)) + (((vw)<0)?0:((vw)>=spTextureY)?spTextureY-1:(vw)) * spTextureScanLine];  \
			if (pixel != SP_ALPHA_COLOR) \
				spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																							+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																								+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_zset_alpha_perspect(x,y,z,u,v,w,color) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		Uint32 pixel = spTexturePixel[(uw) + (vw) * spTextureScanLine];  \
		if (pixel != SP_ALPHA_COLOR) \
		{ \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#else
	#define draw_pixel_tex_zset_alpha_perspect(x,y,z,u,v,w,color) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		Uint32 pixel = spTexturePixel[(((uw)<0)?0:((uw)>=spTextureX)?spTextureX-1:(uw)) + (((vw)<0)?0:((vw)>=spTextureY)?spTextureY-1:(vw)) * spTextureScanLine];  \
		if (pixel != SP_ALPHA_COLOR) \
		{ \
			spZBuffer[(x) + (y) * spTargetScanLine] = (z); \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
		} \
	}
#endif

#ifdef FAST_BUT_UGLY
	#define draw_pixel_tex_alpha_perspect(x,y,u,v,w,color) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		Uint32 pixel = spTexturePixel[(uw) + (vw) * spTextureScanLine];  \
		if (pixel != SP_ALPHA_COLOR) \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#else
	#define draw_pixel_tex_alpha_perspect(x,y,u,v,w,color) \
	{ \
		Sint32 uw = reciprocal_w_clip(u,w); \
		Sint32 vw = reciprocal_w_clip(v,w); \
		Uint32 pixel = spTexturePixel[(((uw)<0)?0:((uw)>=spTextureX)?spTextureX-1:(uw)) + (((vw)<0)?0:((vw)>=spTextureY)?spTextureY-1:(vw)) * spTextureScanLine];  \
		if (pixel != SP_ALPHA_COLOR) \
			spTargetPixel[(x) + (y) * spTargetScanLine] = ( ( pixel * (color) >> 16 ) & 63488 )  \
																						+ ( ( ( pixel & 2047 ) * ( (color) & 2047 ) >> 11 ) & 2016 )  \
																							+ ( ( pixel & 31 ) * ( (color) & 31 ) >> 5 ); \
	}
#endif

// + Pattern
#define draw_pixel_tex_ztest_zset_alpha_pattern(x,y,z,u,v,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_zset_alpha(x,y,z,u,v,color)}

#define draw_pixel_tex_ztest_alpha_pattern(x,y,z,u,v,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_alpha(x,y,z,u,v,color)}

#define draw_pixel_tex_zset_alpha_pattern(x,y,z,u,v,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_zset_alpha(x,y,z,u,v,color)}

#define draw_pixel_tex_alpha_pattern(x,y,u,v,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_alpha(x,y,u,v,color)}

#define draw_pixel_tex_ztest_zset_alpha_pattern_perspect(x,y,z,u,v,w,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_zset_alpha_perspect(x,y,z,u,v,w,color)}

#define draw_pixel_tex_ztest_alpha_pattern_perspect(x,y,z,u,v,w,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_ztest_alpha_perspect(x,y,z,u,v,w,color)}

#define draw_pixel_tex_zset_alpha_pattern_perspect(x,y,z,u,v,w,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_zset_alpha_perspect(x,y,z,u,v,w,color)}

#define draw_pixel_tex_alpha_pattern_perspect(x,y,u,v,w,color) \
	{if ((spPattern[y & 7] >> (x & 7)) & 1) \
		draw_pixel_tex_alpha_perspect(x,y,u,v,w,color)}

