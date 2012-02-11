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
#ifndef _SPARROW_PRIMITVES_H
#define _SPARROW_PRIMITVES_H

#include "sparrowDefines.h"
#include <SDL.h>

/* IMPORTANT: That means, that you still have 14 Bit for your Pixel Range
 * minus 1 Bit for the sign. So you have 13 Bit == 8192 Pixel Width or Height
 * Maximum!, if you use the software renderer! Furthermore the lookup table
 * for the one_over_x function needs a 18 Bit Array. That is 1 MByte!*/
#define SP_PRIM_ACCURACY 18
#define SP_HALF_PRIM_ACCURACY 9
#define SP_ALPHA_COLOR 63519
#define SP_MAX_NEGATIVE -0x80000000
#define SP_SingedInt16 Sint32
#define SP_UnsingedInt16 Uint32

/* Initializes some Look up tables and the zBufferCache. Is called from
 * sparrowCore. */
PREFIX void spInitPrimitives();

/* Releases stuff and frees memory. Is called from sparrowCore. */
PREFIX void spQuitPrimitives();

/* Returns a pointer th 1<<SP_PRIM_ACCURACY values of 1/x */
PREFIX Sint32* spGetOne_over_x_pointer();

/* Selects the Render Surface. Attention: With every Target change the
 * Z Buffer will be cleaned! */
PREFIX void spSelectRenderTarget(SDL_Surface* target);

/* This texture will be used for all following draw operations with textures */
PREFIX void spBindTexture(SDL_Surface* texture);

/* (De)Activates the Z test (default on) */
PREFIX void spSetZTest(Uint32 test);

/* (De)Activates the Z set (default on)*/
PREFIX void spSetZSet(Uint32 test);

/* (De)Activates the Alpha test (default on)*/
PREFIX void spSetAlphaTest(Uint32 test);

/* The Texture Rendering is fast - and a bit wrong. It is affine. To
 * reduce the effect, activate this dirty hack. It could be a bit
 * slower. Only affects Quads. (default on) */
PREFIX void spSetAffineTextureHack(Uint32 test);

/* Clears the Rendertarget with the color. But attention: The z Buffer will
 * not be cleaned! */
PREFIX void spClearTarget(SP_UnsingedInt16 color);

/* Draws a Triangle without texture and without alpha value */
PREFIX void spTriangle(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_UnsingedInt16 color);

/* Draws a Triangle with texture and without alpha value */
PREFIX void spTriangle_tex(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_UnsingedInt16 color);

/* Draws a Quad without texture and without alpha value */
PREFIX void spQuad(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 x4,SP_SingedInt16 y4, Sint32 z4, SP_UnsingedInt16 color);

/* Draws a Quad with texture and without alpha value */
PREFIX void spQuad_tex(SP_SingedInt16 x1, SP_SingedInt16 y1, Sint32 z1, SP_SingedInt16 u1, SP_SingedInt16 v1, SP_SingedInt16 x2, SP_SingedInt16 y2, Sint32 z2, SP_SingedInt16 u2, SP_SingedInt16 v2, SP_SingedInt16 x3, SP_SingedInt16 y3, Sint32 z3, SP_SingedInt16 u3, SP_SingedInt16 v3, SP_SingedInt16 x4, SP_SingedInt16 y4, Sint32 z4, SP_SingedInt16 u4, SP_SingedInt16 v4, SP_UnsingedInt16 color);

/* Cache ZBuffers. If you use many Render Targets every time you switch
 * it the old ZBuffer will be destroyed and a new one created, which is
 * slow. So I implemented this caching. The engine will hold as much
 * zBuffer as you say. Default: 16*/
PREFIX void spSetZBufferCache(Uint32 value);

/* Reallocates the zBuffer. If you switch to a new Render Target, this function
 * is called */
PREFIX void spReAllocateZBuffer();

/* Reset the zBuffer. If you switch to a new Render Target, this function
 * is NOT called. You have to call it before you draw by yourself! */
PREFIX void spResetZBuffer();

/* Returns the Z Buffer Array*/
PREFIX Sint32* spGetZBuffer();

/* Draws a very fast horizental line with one color */
PREFIX void spHorizentalLine(Uint16* pixel,Sint32 x,Sint32 y,Sint32 l_,SP_UnsingedInt16 color_,Uint32 check,Sint32 engineWindowX,Sint32 engineWindowY);

/* Draws a Surface on the targer */
PREFIX void spBlitSurface(Sint32 x,Sint32 y,Sint32 z,SDL_Surface* surface);

PREFIX void spBlitSurfacePart(Sint32 x,Sint32 y,Sint32 z,SDL_Surface* surface,Sint32 sx,Sint32 sy,Sint32 w,Sint32 h);
#endif
