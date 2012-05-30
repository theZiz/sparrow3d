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
 of the GNU Lesser General Public license (the	"LGPL License"), in which case the	
 provisions of LGPL License are applicable instead of those									
 above.																																		 
																																						
 For feedback and questions about my Files and Projects please mail me,		 
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com												 
*/
#ifndef _SPARROW_PRIMITVES_H
#define _SPARROW_PRIMITVES_H

#include "sparrowDefines.h"
#include <SDL.h>

/* Some words to colors: Even if I use 32 Bit values, this part of the engine
 * works ONLY with 16 Bit color, with the 5 bit red, 6 bit green and 5 bit blue.
 * I just use 32 bit values, because it is faster. Otherwise the compiler always
 * convertets 16 bit to 32 bit and the other direction. If the compiler,
 * "thinks", that it is just 32 Bit, it is faster. */

/* IMPORTANT: That means, that you still have 14 Bit for your Pixel Range
 * minus 1 Bit for the sign. So you have 13 Bit == 8192 Pixel Width or Height
 * Maximum!, if you use the software renderer! Furthermore the lookup table
 * for the one_over_x function needs a 18 Bit Array. That is 1 MByte!*/
#define SP_PRIM_ACCURACY 18
#define SP_HALF_PRIM_ACCURACY 9
#define SP_ALPHA_COLOR 63519
#define SP_MAX_NEGATIVE -0x80000000

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
PREFIX void spClearTarget(Uint32 color);

/* Draws a Triangle without texture and without alpha value. Returns 1
 * if drawn (culling)*/
PREFIX int spTriangle(Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color);

/* Draws a Triangle with texture and without alpha value Returns 1
 * if drawn (culling)*/
PREFIX int spTriangle_tex(Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color);

/* Draws a Quad without texture and without alpha value Returns 1
 * if drawn (culling)*/
PREFIX int spQuad(Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 x4,Sint32 y4, Sint32 z4, Uint32 color);

/* Draws a Quad with texture and without alpha value Returns 1
 * if drawn (culling)*/
PREFIX int spQuad_tex(Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Uint32 color);

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

/* Draws a very fast horizental line with one color. Used it e.g with
 * your_render_target->pixels. Don't forget to lock your surface for this
 * function! Because of some optimizations I can't do it for you. If you are
 * not sure, use spLine instead. */
PREFIX void spHorizentalLine(Uint16* pixel,Sint32 x,Sint32 y,Sint32 l_,Uint32 color_,Uint32 check,Sint32 engineWindowX,Sint32 engineWindowY);

/* Draws a Surface on the target */
PREFIX void spBlitSurface(Sint32 x,Sint32 y,Sint32 z,SDL_Surface* surface);

/* Draws a part of a Surface on the target */
PREFIX void spBlitSurfacePart(Sint32 x,Sint32 y,Sint32 z,SDL_Surface* surface,Sint32 sx,Sint32 sy,Sint32 w,Sint32 h);

/* Draws a surface with rotozoom. Blitting is much faster ;-) zoomX, zoomY and
 * angle are fixed point values! 1<<SP_ACCURACY is default zoom and the
 * angle goes from 0 to 2*SP_PI. */
PREFIX void spRotozoomSurface(Sint32 x,Sint32 y,Sint32 z,SDL_Surface* surface,Sint32 zoomX,Sint32 zoomY,Sint32 angle);

PREFIX void spRotozoomSurfacePart(Sint32 x,Sint32 y,Sint32 z,SDL_Surface* surface,Sint32 sx,Sint32 sy,Sint32 w,Sint32 h,Sint32 zoomX,Sint32 zoomY,Sint32 angle);

/* Sets Culling on or off. Culling means, that depending on the order of the
 * edges, the primitive is drawn - or not. Default is on. That means: Every
 * primitive defined counterclockwise will be drawn. That's very important for
 * Rendering meshes, because you can't see the backside of a triangle or quad
 * of an object. So why drawing it? */
PREFIX void spSetCulling(char value);

/* Draws a line from (x1,y1,z1) to (x2,y2,z2) with the specified color*/
PREFIX void spLine(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 x2,Sint32 y2,Sint32 z2, Uint32 color);

PREFIX int spGetPixelPosition(Sint32 x,Sint32 y);

/* Draws a filled Rectangle */
PREFIX void spRectangle(Sint32 x,Sint32 y,Sint32 z,Sint32 w,Sint32 h, Uint32 color);

/* Draws a filled RectangleBorder with the specified BorderSizes*/
PREFIX void spRectangleBorder(Sint32 x,Sint32 y,Sint32 z,Sint32 w,Sint32 h,Sint32 bx,Sint32 by, Uint32 color);

/* Draws a filled Ellipse. Doesn't work for huge ellipses at the moment...*/
PREFIX void spEllipse(Sint32 x,Sint32 y,Sint32 z,Sint32 rx,Sint32 ry, Uint32 color);

/* Draws a filled EllipseBorder. Doesn't work for huge ellipses at the moment...*/
PREFIX void spEllipseBorder(Sint32 x,Sint32 y,Sint32 z,Sint32 rx,Sint32 ry,Sint32 bx,Sint32 by,Uint32 color);

/* Sets the horizontal origin of the Surface and Rectangle functions.
 * Possible is: SP_LEFT, SP_RIGHT and SP_CENTER. SP_CENTER is default*/
PREFIX void spSetHorizontalOrigin(Sint32 origin);

/* Sets the vertical origin of the Surface and Rectangle functions.
 * Possible is: SP_TOP, SP_BOTTOM and SP_CENTER. SP_CENTER is default*/
PREFIX void spSetVerticalOrigin(Sint32 origin);

#endif
