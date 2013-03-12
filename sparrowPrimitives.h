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

/* sparrowPrimitives is for drawing primivites like triangles or quads
 * (with and without texture), blitting or rotozooming of surfaces, lines,
 * ellipses, rectangles and for everything providing a zBuffer. You have
 * to understand the difference between your SCREEN, which could be
 * provided by sparrowCore (spCreateWindow), and your TARGET. Every call
 * from this file, draws to the target. This MAY be your screen - but
 * have no to be! Every SDL Surface could be the target.
 * 
 * Some words to the z Buffer: First of all. If you DON'T want any z
 * buffering or stuff like this, call spSetZSet(0); and spSetZTest(0);
 * at beginning. zBuffers will be created and cached, but not used.
 * Furthermore most calls will be faster, but you loose a lot of fun and
 * possibilities. :-P
 * But if you use it, keep in mind, that the NEAREST z value is -1 and
 * the farest away z value SP_MAX_NEGATIVE! You see: z has always to
 * be negative (it's the same at opengl). If some function in
 * sparrowPrimitves wants to draw a pixel (and you enabled zTest), it
 * will check, whether the new pixel has a greater z value. If not, it
 * will not be drawn. Short: Just keep in mind:
 * - -1 nearest
 * - SP_MAX_NEGATIVE farest
 *
 * Some words to the order of definition of edges of triangles and
 * quads: It matter! You have to define the edges COUNTERCLOCKWISE. 
 *
 * C________B    E.g. the triangle A(150,150) B(200,100) C(100,100) is
 *  \      /     is definied counterclockwise (z doesn't matter here) 
 *   \    /      and will be drawn. The triangle C B A would NOT be
 *    \  /       drawn. ;-)
 *     \/        The reason is to not draw not seeable triangles/quads
 *      A        of closed fields.
 * 
 * Some words to colors, too: Even if I use 32 Bit values, this part of
 * the engine works ONLY with 16 Bit color, with the 5 bit red, 6 bit
 * green and 5 bit blue. I just use 32 bit values, because it is faster.
 * Otherwise the compiler always converts 16 bit to 32 bit and the other
 * direction. If the compiler "thinks", that it is just 32 Bit, it is
 * faster. */

#ifndef _SPARROW_PRIMITVES_H
#define _SPARROW_PRIMITVES_H

#include "sparrow3d.h"
#include <SDL.h>

/* If alphatest is enabled, this color will not be drawn */
#define SP_ALPHA_COLOR 63519

/* Initializes some Look up tables and the zBufferCache. Is called from
 * sparrowCore. */
PREFIX void spInitPrimitives();

/* Releases stuff and frees memory. Is called from sparrowCore. */
PREFIX void spQuitPrimitives();

/* Returns a pointer th 1<<SP_PRIM_ACCURACY values of 1/x */
PREFIX Sint32* spGetOne_over_x_pointer();

/* Selects the Render Surface. Attention: With every Target change the
 * Z Buffer will be cleaned! */
PREFIX void spSelectRenderTarget( SDL_Surface* target );

/* Returns the Rendertarget */
PREFIX SDL_Surface* spGetRenderTarget();

/* Locks the RenderTarget and returns a pointer to the raw 16 bit pixel data.
 * Keep in mind to change the zBuffer too, if you need it and to unlock at
 * the end!*/
PREFIX Uint16* spLockRenderTarget();

/* Unlocks the RenderTarget*/
PREFIX void spUnlockRenderTarget();

/* Returns a pointer to the zBuffer of the render target */
PREFIX Sint32* spGetRenderTargetZBuffer();

/* This texture will be used for all following draw operations with textures */
PREFIX void spBindTexture( SDL_Surface* texture );

/* (De)Activates the Z test (default on) */
PREFIX void spSetZTest( Uint32 test );

/* (De)Activates the Z set (default on)*/
PREFIX void spSetZSet( Uint32 test );

/* (De)Activates the Alpha test (default on)*/
PREFIX void spSetAlphaTest( Uint32 test );

/* The Texture Rendering is fast - and a bit wrong. It is affine. To
 * reduce the effect, activate this dirty hack. It could be a bit
 * slower. Only affects Quads. (default on) */
PREFIX void spSetAffineTextureHack( Uint32 test );

/* Clears the Rendertarget with the color. But attention: The z Buffer will
 * not be cleaned! */
PREFIX void spClearTarget( Uint32 color );

/* Draws a Triangle without texture and without alpha value. Returns 0
 * if not drawn (culling) or different bits, where the edges are:
 * 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right,
 * 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spTriangle( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color );

/* Draws a Triangle with texture and without alpha value. Returns 0
 * if not drawn (culling) or different bits, where the edges are:
 * 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right,
 * 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spTriangle_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color );

PREFIX int spPerspectiveTriangle_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color );

/* Draws a Quad without texture and without alpha value. Returns 0
 * if not drawn (culling) or different bits, where the edges are:
 * 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right,
 * 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spQuad( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 x4, Sint32 y4, Sint32 z4, Uint32 color );

/* Draws a Quad with texture and without alpha value. Returns 0
 * if not drawn (culling) or different bits, where the edges are:
 * 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right,
 * 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spQuad_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Uint32 color );

PREFIX int spPerspectiveQuad_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Sint32 w4, Uint32 color );

/* Cache ZBuffers. If you use many Render Targets every time you switch
 * it the old ZBuffer will be destroyed and a new one created, which is
 * slow. So I implemented this caching. The engine will hold as much
 * zBuffer as you say. Default: 16*/
PREFIX void spSetZBufferCache( Uint32 value );

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
PREFIX void spHorizentalLine( Uint16* pixel, Sint32 x, Sint32 y, Sint32 l_, Uint32 color_, Uint32 check, Sint32 engineWindowX, Sint32 engineWindowY );

/* Draws a Surface on the target */
PREFIX void spBlitSurface( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface );

/* Draws a part of a Surface on the target */
PREFIX void spBlitSurfacePart( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h );

/* Draws a surface with rotozoom. Blitting is much faster ;-) zoomX,
 * zoomY and angle are fixed point values! 1<<SP_ACCURACY is default
 * zoom and the angle goes from 0 to 2*SP_PI. */
PREFIX void spRotozoomSurface( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 zoomX, Sint32 zoomY, Sint32 angle );

/* Works like spRotozoomSurface, but only draws a part of the surface */
PREFIX void spRotozoomSurfacePart( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, Sint32 zoomX, Sint32 zoomY, Sint32 angle );

/* Sets Culling on or off. Culling means, that depending on the order of the
 * edges, the primitive is drawn - or not. Default is on. That means: Every
 * primitive defined counterclockwise will be drawn. That's very important for
 * Rendering meshes, because you can't see the backside of a triangle or quad
 * of an object. So why drawing it? */
PREFIX void spSetCulling( char value );

/* Draws a line from (x1,y1,z1) to (x2,y2,z2) with the specified color*/
PREFIX void spLine( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Uint32 color );

/* Returns where the pixel (x,y) is in relation to the screen:
 * 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right,
 * 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spGetPixelPosition( Sint32 x, Sint32 y );

/* Draws a filled Rectangle */
PREFIX void spRectangle( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Uint32 color );

/* Draws a filled RectangleBorder with the specified BorderSizes*/
PREFIX void spRectangleBorder( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Sint32 bx, Sint32 by, Uint32 color );

/* Draws a filled Ellipse. Doesn't work for huge ellipses at the moment...*/
PREFIX void spEllipse( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Uint32 color );

/* Draws a filled EllipseBorder. Doesn't work for huge ellipses at the moment...*/
PREFIX void spEllipseBorder( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Sint32 bx, Sint32 by, Uint32 color );

/* Sets the horizontal origin of the Surface and Rectangle functions.
 * Possible is: SP_LEFT, SP_RIGHT and SP_CENTER. SP_CENTER is default*/
PREFIX void spSetHorizontalOrigin( Sint32 origin );

/* Sets the vertical origin of the Surface and Rectangle functions.
 * Possible is: SP_TOP, SP_BOTTOM and SP_CENTER. SP_CENTER is default*/
PREFIX void spSetVerticalOrigin( Sint32 origin );

/* Returns the horizontal origin */
PREFIX Sint32 spGetHorizontalOrigin();

/* Returns the vertical origin */
PREFIX Sint32 spGetVerticalOrigin();

/* Sets the value, to which the zbuffer will be reseted */
PREFIX void spSetZFar(Sint32 zfar);

/* Gets the value, to which the zbuffer will be reseted */
PREFIX Sint32 spGetZFar();

/* Sets the znear value for z test*/
PREFIX void spSetZNear(Sint32 znear);

/* Gets the znear value for z test*/
PREFIX Sint32 spGetZNear();

/* Adds a transparent white layer to the target surface. Slow, but
 * usefull for menu fade in and out */
PREFIX void spAddWhiteLayer(int alpha);

/* Adds a transparent black layer to the target surface. Slow, but 
 * usefull for menu fade in and out */
PREFIX void spAddBlackLayer(int alpha);

/* The following 3 functions set the 64 Bit 8x8 pattern used for
 * a dirty transparancy effect or scanline simulations. Default is
 * all bits 1 */
PREFIX void spSetPattern32(Uint32 first_32_bit,Uint32 last_32_bit);
PREFIX void spSetPattern64(Uint64 pattern);
PREFIX void spSetPattern8(Uint8 line1,Uint8 line2,Uint8 line3,Uint8 line4,Uint8 line5,Uint8 line6,Uint8 line7,Uint8 line8);

/* spDeactivatePattern deactivates the pattern. They are reactivated
 * again with a call of one of the 3 functions above. */
PREFIX void spDeactivatePattern();

/* Sets the pattern in dependence of an alpha value to "emulate" alpha blending.
 * The shift shifts the whole pattern. But keep one problem in mind: Two
 * primitives drawn with the same pattern (== same alpha and shift value) will
 * cover each other, because exactly the same pixel will not be drawn. Solution:
 * Using alpha simulation just a bit, using very different alpha values for
 * covering primitives or using different shift values for every primitive.
 * alpha goes from 0 to 255, shift from 0 to 63.*/
PREFIX void spSetAlphaPattern(int alpha,int shift);

/* Works pretty like spSetAlphaPattern, but it uses 4x4 pattern internal. So
 * there are only 16 different "alpha values", but it looks better most of
 * the time. (And I think it is faster, too.) shift goes only from 0 to 15!*/
PREFIX void spSetAlphaPattern4x4(int alpha,int shift);

#endif
