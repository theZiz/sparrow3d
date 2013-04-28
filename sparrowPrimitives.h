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

/* File: sparrowPrimitives
 * 
 * SparrowPrimitives is for drawing primitives like triangles or quads (with and
 * without texture), blitting or rotozooming of surfaces, lines, ellipses,
 * rectangles and for everything it provides a zBuffer test and set. You have to
 * understand the difference between your *screen* / *window*, which could be
 * (but doesn't have to!) provided by <sparrowCore> (<spCreateWindow>), and your
 * *target*, which can be every surface. Every call from this file, draws to the
 * target. This *may* be your screen - but have not to be! Every SDL Surface*
 * could be the target.
 * 
 * Some words to the z Buffer: First of all. If you *don't* want any z buffering
 * or stuff like this, call <spSetZSet> (0); and <spSetZTest> (0); at beginning.
 * Z-Buffers will be created and cached, but not used. Furthermore most calls
 * will be faster, but you loose a lot of fun and possibilities and have to
 * think harder about sorting and stuff. :-P But if you use it, keep in mind,
 * that the *nearest* z value is 0 and the farest away z value is set by
 * <spSetZFar>-<spSetZNear> and positive. In difference to <sparrowRenderer>,
 * which operates in the 3D space, where the z value go from -<spSetZNear> to
 * -<spSetZFar> and are automaticly set by the projection setup function! So
 * always think about the system you use:
 * * 2D space with sparrowPrimitives: z from 0 to <spSetZFar>-<spSetZNear>
 * * 3D space with sparrowRenderer: z from -<spSetZNear> to -<spSetZFar>, but
 * however setup by <spSetPerspective> or similar.
 * 
 * If Z-Test is enabled, a pixel with a greater z-value than an already drawn
 * pixel will not be drawn, because the new pixel would be "deeper in the
 * screen".
 *
 * Some words to the order of definition of edges of triangles and
 * quads: It matter! You have to define the edges *counterclockwise*, if you
 * activated culling with <spSetCulling>, which is nevertheless on at default!
 *
 * > C________B    E.g. the triangle A(150,150) B(200,100) C(100,100) is
 * >  \      /     is definied counterclockwise (z doesn't matter here) 
 * >   \    /      and will be drawn. The triangle C B A would *not* be
 * >    \  /       drawn. ;-)
 * >     \/        The reason is to not draw not seeable triangles/quads
 * >      A        of closed fields in the 3d space.
 * 
 * Some words to colors, too: Even if I use 32 Bit values, the engine works
 * *only* with 16 Bit colors, with the 5 bit red, 6 bit green and 5 bit blue. I
 * just use 32 bit values, because it is faster as parameters. Otherwise the
 * compiler always would convert 16 bit to 32 bit and the other direction. If
 * the compiler "thinks", that it is just 32 Bit, it is faster. */

#ifndef _SPARROW_PRIMITVES_H
#define _SPARROW_PRIMITVES_H

#include "sparrow3d.h"
#include <SDL.h>

// Functions: Setup and getter functions
// These functions are for setting up all graphical stuff like the different per
// pixel tests (z, alpha, pattern), sets (z), other optional optimizations and
// also the Z-Buffer and it's cache plus getting some pointers for faster direct
// access.

/* Function: spInitPrimitives
 * 
 * Initializes some Look up tables and the zBufferCache. Is called by
 * <spInitCore>. */
PREFIX void spInitPrimitives();

/* Function: spQuitPrimitives
 * 
 * Releases stuff and frees memory. Is called by <spQuitCore>. */
PREFIX void spQuitPrimitives();

/* Function: spGetOne_over_x_pointer
 * 
 * Returns a pointer/array of SP_ONE (65536) values of 1/x
 * 
 * Returns:
 * Sint32* - the pointer/array*/
PREFIX Sint32* spGetOne_over_x_pointer();

/* Function: spSelectRenderTarget
 * 
 * Selects the render surface. Attention: With every change of the target
 * a new z Buffer is created or a cached is used. However, you can't be sure
 * of the content of the zbuffer after this call!
 * 
 * Parameters:
 * target - the new target every draw call will render to*/
PREFIX void spSelectRenderTarget( SDL_Surface* target );

/* Function: spGetRenderTarget
 * 
 * Returns the render target.
 * 
 * Returns:
 * SDL_Surface* - the render target*/
PREFIX SDL_Surface* spGetRenderTarget();

/* Function: spLockRenderTarget
 * 
 * Locks the RenderTarget and returns a pointer to the raw 16 bit pixel data.
 * Keep in mind to change the zBuffer too, if you need it and to unlock at
 * the end!
 * 
 * Returns:
 * Uint16* - 16 bit int pointer / array for free access. Keep the dimension of
 * the array in mind, which is
 * renderTarget->pitch/renderTarget->format->BytesPerPixel * renderTarget->h.
 * 
 * See Also:
 * <spUnlockRenderTarget>, <spGetRenderTargetZBuffer>*/
PREFIX Uint16* spLockRenderTarget();

/* Function: spUnlockRenderTarget
 * 
 * Unlocks the RenderTarget locked by <spLockRenderTarget>
 * 
 * See Also:
 * <spLockRenderTarget>
 * */
PREFIX void spUnlockRenderTarget();

/* Function: spGetRenderTargetZBuffer
 * 
 * Returns a pointer to the zBuffer of the render target
 * 
 * Returns:
 * Sint32* - the z-buffer pointer. Keep the dimension of the array in mind,
 * which is renderTarget->pitch/renderTarget->format->BytesPerPixel *
 * renderTarget->h*/
PREFIX Sint32* spGetRenderTargetZBuffer();

/* Function: spBindTexture
 * 
 * This texture will be used for all following draw operations with textures
 * 
 * Parameters:
 * texture - the texture to be used*/
PREFIX void spBindTexture( SDL_Surface* texture );

/* Function: spSetZTest
 * 
 * (De)Activates the Z test (default on).
 * 
 * Parameters:
 * test - 1 means on, 0 means off
 * 
 * See Also:
 * <spSetZSet>, <spSetAlphaTest>*/
PREFIX void spSetZTest( Uint32 test );

/* Function: spSetZSet
 * 
 * (De)Activates the Z set (default on).
 * 
 * Parameters:
 * test - 1 means on, 0 means off
 * 
 * See Also:
 * <spSetZTest>, <spSetAlphaTest>*/

PREFIX void spSetZSet( Uint32 test );

/* Function: spSetAlphaTest
 * 
 * (De)Activates the alpha test (default on). It tests against <SP_ALPHA_COLOR>.
 * 
 * Parameters:
 * test - 1 means on, 0 means off
 * 
 * See Also:
 * <spSetZSet>, <spSetZTest>*/
PREFIX void spSetAlphaTest( Uint32 test );

/* Function: spSetAffineTextureHack
 * 
 * The texture rendering is fast, but a bit wrong. It is affine. To
 * reduce this effect, which is best seeable with big, quadratic faces, activate
 * this dirty hack. It could be a bit slower. Only affects Quads
 * 
 * Parameters:
 * test - 0 deactivates this hack (default), 1 makes look it better and 2 the
 * best. However most of the times you will not see any difference between 1
 * and 2*/
PREFIX void spSetAffineTextureHack( Uint32 test );

/* Function: spSetCulling
 * 
 * (De)Activates culling. Culling means, that depending on the order of the
 * edges, the primitive is drawn - or not. Default is on. That means: Every
 * primitive defined counterclockwise will be drawn. That's very important for
 * Rendering meshes, because you can't see the backside of a triangle or quad
 * of an object. So why drawing it?
 * 
 * Parameters:
 * value - 1 means on, 0 means off*/
PREFIX void spSetCulling( char value );

/* Function: spClearTarget
 * 
 * Clears the Rendertarget. The Z Buffer is not cleand!
 * 
 * Parameters:
 * color - 16 bit color. The target will be filled with this color.
 * 
 * See Also:
 * <spResetZBuffer>*/
PREFIX void spClearTarget( Uint32 color );

/* Function: spSetZBufferCache
 * 
 * If you use many Render Targets every time you switch it, the old ZBuffer will
 * be destroyed and a new one created, which is slow. So I implemented this
 * caching. The engine will hold as much Z-Buffers as you say. Default: 16
 * 
 * Parameters:
 * value - count of Z-Buffers to hold cached*/
PREFIX void spSetZBufferCache( Uint32 value );

/* Function: spReAllocateZBuffer
 * 
 * Reallocates the zBuffer. If you switch to a new Render Target, this function
 * is called.*/
PREFIX void spReAllocateZBuffer();

/* Function: spResetZBuffer
 * 
 * Resets the zBuffer with the farest value. If you switch to a new Render
 * Target, this function is NOT called. You have to call it before you draw by
 * yourself!
 * 
 * See Also:
 * <spClearTarget>*/
PREFIX void spResetZBuffer();

/* Function: spSetHorizontalOrigin
 * 
 * Sets the horizontal origin of the Surface rectangle and ellipses functions.
 * 
 * Parameters:
 * origin - <SP_LEFT> for left origin, <SP_RIGHT> for right origin and
 * <SP_CENTER> (default) for centered origin
 * 
 * See Also:
 * <spSetVerticalOrigin>*/
PREFIX void spSetHorizontalOrigin( Sint32 origin );

/* Function: spSetVerticalOrigin
 * 
 * Sets the vertical origin of the Surface rectangle and ellipses functions.
 * 
 * Parameters:
 * origin - <SP_TOP> for top origin, <SP_BOTTOM> for bottom origin and
 * <SP_CENTER> (default) for centered origin
 * 
 * See Also:
 * <spSetHorizontalOrigin>*/
PREFIX void spSetVerticalOrigin( Sint32 origin );

/* Function: spGetHorizontalOrigin
 * 
 * Returns the horizontal origin.
 * 
 * Returns:
 * Sint32 - <SP_LEFT> for left origin, <SP_RIGHT> for right origin and
 * <SP_CENTER> (default) for centered origin
*/
PREFIX Sint32 spGetHorizontalOrigin();

/* Function: spGetVerticalOrigin
 * 
 * Returns the vertical origin.
 * 
 * Returns:
 * Sint32<SP_TOP> for top origin, <SP_BOTTOM> for bottom origin and
 * <SP_CENTER> (default) for centered origin
*/
PREFIX Sint32 spGetVerticalOrigin();

/* Function: spSetZFar
 * 
 * Sets the value, to which the zbuffer will be reseted. Mostly called by
 * <spSetPerspective>
 * 
 * Parameters:
 * zfar - new farest value. Has to be positive
 * 
 * See Also:
 * <spGetZFar>, <spSetZNear>*/
PREFIX void spSetZFar(Sint32 zfar);

/* Function: spGetZFar
 * 
 * Gets the value, to which the zbuffer will be reseted.
 * 
 * Returns:
 * Sint32 - the farest z value
 * 
 * See Also:
 * <spSetZFar>, <spGetZNear>*/
PREFIX Sint32 spGetZFar();

/* Function: spSetNear
 * 
 * Sets the value, to which the zbuffer will be reseted. Mostly called by
 * <spSetPerspective>
 * 
 * Parameters:
 * znear - new nearest value. Has to be positive
 * 
 * See Also:
 * <spSetZFar>, <spGetZNear>*/
PREFIX void spSetZNear(Sint32 znear);

/* Function: spGetZFar
 * 
 * Gets the value, to which the zbuffer will be reseted.
 * 
 * Returns:
 * Sint32 - the nearest z value
 * 
 * See Also:
 * <spGetZFar>, <spSetZNear>*/
PREFIX Sint32 spGetZNear();

/* Function: spGetPixelPosition
 * 
 * Returns where the pixel is in relation to the screen.
 * 
 * Parameters:
 * x,y - position of the pixel
 * 
 * Returns:
 * int - 1 means on the screen, 2 left of the screen, 4 lefttop, 8 top,
 * 16 righttop, 32 right, 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spGetPixelPosition( Sint32 x, Sint32 y );

// Functions: Drawing functions
// All these functions use (optional) z test, z set, alpha test and pattern test
// except <spHorizentalLine>, <spAddWhiteLayer> and <spAddBlackLayer>

/* Function: spTriangle
 * 
 * Draws a colored Triangle. The z value is interpolated over the triangle.
 * Don't forget to defines the edges counterclockwise if culling is enabled.
 * 
 * Parameters:
 * x1,y1,z1 - position and z value of the first edge
 * x2,y2,z2 - position and z value of the second edge
 * x3,y3,z3 - position and z value of the third edge
 * color - 16 bit color of the whole triangle
 * 
 * Returns:
 * int - 0 if not drawn (culling) or different bits, which define, where the
 * edges are: 1 on screen, 2 left of screen, 4 lefttop, 8 top, 16 righttop,
 * 32 right, 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spTriangle( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Uint32 color );

/* Function: spTriangle_tex
 * 
 * Draws a colored Triangle with texture mapping. The z value is interpolated
 * over the triangle. Don't forget to defines the edges counterclockwise if
 * culling is enabled. Furthermore a texture had to be defined with
 * <spBindTexture>.
 * 
 * Parameters:
 * x1,y1,z1 - position and z value of the first edge
 * u1,v1 - texture coordinates of the first edge
 * x2,y2,z2 - position and z value of the second edge
 * u2,v2 - texture coordinates of the second edge
 * x3,y3,z3 - position and z value of the third edge
 * u3,v3 - texture coordinates of the third edge
 * color - 16 bit color of the whole triangle
 * 
 * Returns:
 * int - 0 if not drawn (culling) or different bits, which define, where the
 * edges are: 1 on screen, 2 left of screen, 4 lefttop, 8 top, 16 righttop,
 * 32 right, 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spTriangle_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint32 color );

/* Function: spPerspectiveTriangle_tex
 * 
 * Draws a colored Triangle with perspectively correct texture mapping. The z
 * value is interpolated over the triangle. Don't forget to defines the edges
 * counterclockwise if culling is enabled. Furthermore a texture had to be
 * defined with <spBindTexture>.
 * 
 * Parameters:
 * x1,y1,z1 - position and z value of the first edge
 * u1,v1 - texture coordinates of the first edge
 * w1 - w clipping ordinate of the first edge. It is a result of the projection.
 * x2,y2,z2 - position and z value of the second edge
 * u2,v2 - texture coordinates of the second edge
 * w2 - w clipping ordinate of the second edge. It is a result of the projection.
 * x3,y3,z3 - position and z value of the third edge
 * u3,v3 - texture coordinates of the third edge
 * w3 - w clipping ordinate of the third edge. It is a result of the projection.
 * color - 16 bit color of the whole triangle
 * 
 * Returns:
 * int - 0 if not drawn (culling) or different bits, which define, where the
 * edges are: 1 on screen, 2 left of screen, 4 lefttop, 8 top, 16 righttop,
 * 32 right, 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spPerspectiveTriangle_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Uint32 color );

/* Function: spQuad
 * 
 * Draws a colored quad. The z value is interpolated over the quad.
 * Don't forget to defines the edges counterclockwise if culling is enabled.
 * 
 * Parameters:
 * x1,y1,z1 - position and z value of the first edge
 * x2,y2,z2 - position and z value of the second edge
 * x3,y3,z3 - position and z value of the third edge
 * x4,y4,z4 - position and z value of the fourth edge
 * color - 16 bit color of the whole quad
 * 
 * Returns:
 * int - 0 if not drawn (culling) or different bits, which define, where the
 * edges are: 1 on screen, 2 left of screen, 4 lefttop, 8 top, 16 righttop,
 * 32 right, 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spQuad( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 x4, Sint32 y4, Sint32 z4, Uint32 color );

/* Function: spQuad_tex
 * 
 * Draws a colored quad with texture mapping. The z value is interpolated
 * over the quad. Don't forget to defines the edges counterclockwise if
 * culling is enabled. Furthermore a texture had to be defined with
 * <spBindTexture>.
 * 
 * Parameters:
 * x1,y1,z1 - position and z value of the first edge
 * u1,v1 - texture coordinates of the first edge
 * x2,y2,z2 - position and z value of the second edge
 * u2,v2 - texture coordinates of the second edge
 * x3,y3,z3 - position and z value of the third edge
 * u3,v3 - texture coordinates of the third edge
 * x4,y4,z4 - position and z value of the fourth edge
 * u4,v4 - texture coordinates of the fourth edge
 * color - 16 bit color of the whole quad
 * 
 * Returns:
 * int - 0 if not drawn (culling) or different bits, which define, where the
 * edges are: 1 on screen, 2 left of screen, 4 lefttop, 8 top, 16 righttop,
 * 32 right, 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spQuad_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Uint32 color );

/* Function: spPerspectiveQuad_tex
 * 
 * Draws a colored quad with perspectively correct texture mapping. The z
 * value is interpolated over the quad. Don't forget to defines the edges
 * counterclockwise if culling is enabled. Furthermore a texture had to be
 * defined with <spBindTexture>.
 * 
 * Parameters:
 * x1,y1,z1 - position and z value of the first edge
 * u1,v1 - texture coordinates of the first edge
 * w1 - w clipping ordinate of the first edge. It is a result of the projection.
 * x2,y2,z2 - position and z value of the second edge
 * u2,v2 - texture coordinates of the second edge
 * w2 - w clipping ordinate of the second edge. It is a result of the projection.
 * x3,y3,z3 - position and z value of the third edge
 * u3,v3 - texture coordinates of the third edge
 * w3 - w clipping ordinate of the third edge. It is a result of the projection.
 * x4,y4,z4 - position and z value of the fourth edge
 * u4,v4 - texture coordinates of the fourth edge
 * w4 - w clipping ordinate of the fourth edge. It is a result of the projection.
 * color - 16 bit color of the whole quad
 * 
 * Returns:
 * int - 0 if not drawn (culling) or different bits, which define, where the
 * edges are: 1 on screen, 2 left of screen, 4 lefttop, 8 top, 16 righttop,
 * 32 right, 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spPerspectiveQuad_tex( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1, Sint32 w1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2, Sint32 w2, Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Sint32 w3, Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Sint32 w4, Uint32 color );

/* Function: spBlitSurface
 * 
 * Draws a Surface on the target. The origin is at the center as default. Use
 * <spSetHorizontalOrigin> and <spSetVerticalOrigin> to change this behaviour.
 * 
 * Parameters:
 * x,y,z - position and z value, where to draw. 
 * surface - the surface to draw
 * 
 * See Also:
 * <spBlitSurfacePart>*/
PREFIX void spBlitSurface( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface );

/* Function: spBlitSurface
 * 
 * Draws a part of a Surface on the target. The origin is at the center as
 * default. Use <spSetHorizontalOrigin> and <spSetVerticalOrigin> to change this
 * behaviour.
 * 
 * Parameters:
 * x,y,z - position and z value, where to draw. 
 * surface - the surface to draw
 * sx,sy - position of the left top corner of the part of the surface to be
 * drawn
 * w,h - size of the part to be drawn
 * 
 * See Also:
 * <spBlitSurface>*/
PREFIX void spBlitSurfacePart( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h );

/* Function: spRotozoomSurface
 * 
 * Draws a surface with rotozoom (Free rotating and zooming of the surface).
 * Blitting with <spBlitSurface> is much faster ;-).
 * 
 * Parameters:
 * x,y,z - position and z value of the surface on the target
 * surface - surface, which shall be rotozoomed
 * zoomX,zoomY - x and y zoom of the surface. These are fixed point factors,
 * which means, that SP_ONE does nothing, a smaller value decreases and a bigger
 * one increases the size
 * angle - fixed point radian value for rotating. Goes from 0 to 2*SP_PI
 * 
 * See Also:
 * <spRotozoomSurfacePart>*/
PREFIX void spRotozoomSurface( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 zoomX, Sint32 zoomY, Sint32 angle );

/* Function: spRotozoomSurfacePart
 * 
 * Draws a part of a surface with rotozoom (Free rotating and zooming of the
 * surface). Blitting with <spBlitSurfacePart> is much faster ;-).
 * 
 * Parameters:
 * x,y,z - position and z value of the surface on the target
 * surface - surface, which shall be rotozoomed
 * sx,sy - position of the left top corner of the part of the surface to be
 * drawn
 * w,h - size of the part to be drawn
 * zoomX,zoomY - x and y zoom of the surface. These are fixed point factors,
 * which means, that SP_ONE does nothing, a smaller value decreases and a bigger
 * one increases the size
 * angle - fixed point radian value for rotating. Goes from 0 to 2*SP_PI
 * 
 * See Also:
 * <spRotozoomSurface>*/
PREFIX void spRotozoomSurfacePart( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, Sint32 zoomX, Sint32 zoomY, Sint32 angle );

/* Function: spLine
 * 
 * Draws a colored line
 * 
 * Parameters:
 * x1,y1,z1 - position and z value of the first edge
 * x2,y2,z2 - position and z value of the second edge
 * color - 16 bit color of the whole line*/
PREFIX void spLine( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Uint32 color );

/* Function: spRectangle
 * 
 * Draws a filled Rectangle. The origin is at the center as default. Use
 * <spSetHorizontalOrigin> and <spSetVerticalOrigin> to change this behaviour.
 * 
 * Parameters:
 * x,y,z - position and z value, where to draw
 * w,h - size of the rectangle
 * color - 16 bit color of the whole rectangle*/
PREFIX void spRectangle( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Uint32 color );

/* Function: spRectangleBorder
 * 
 * Draws a filled Rectangle border. The origin is at the center as default. Use
 * <spSetHorizontalOrigin> and <spSetVerticalOrigin> to change this behaviour.
 * 
 * Parameters:
 * x,y,z - position and z value, where to draw
 * w,h - size of the rectangle
 * bx,by - size of the border
 * color - 16 bit color of the whole rectangle border*/
PREFIX void spRectangleBorder( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Sint32 bx, Sint32 by, Uint32 color );

/* Function: spEllipse
 * 
 * Draws a filled ellipse. The origin is at the center all the time.
 * 
 * Parameters:
 * x,y,z - position and z value, where to draw
 * rx,ry - radii of the ellipse
 * color - 16 bit color of the whole ellipse*/
PREFIX void spEllipse( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Uint32 color );

/* Function: spEllipseBorder
 * 
 * Draws a filled ellipse border. The origin is at the center all the time.
 * 
 * Parameters:
 * x,y,z - position and z value, where to draw
 * rx,ry - radii of the ellipse
 * bx,by - size of the border
 * color - 16 bit color of the whole ellipse*/
PREFIX void spEllipseBorder( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Sint32 bx, Sint32 by, Uint32 color );

/* Function: spHorizentalLine
 * 
 * Draws a fast horizental line with one color. Don't forget to lock your surface
 * for this function! Use it e.g with <spLockRenderTarget> to get the pixel
 * array and lock it automaticly. Because of some optimizations I can't do this
 * for you. If you are not sure, use spLine instead. No z test, z set, alpha
 * test and pattern test!
 * 
 * Parameters:
 * pixel - pointer to an Uint16 array of pixels
 * x,y - position of the line
 * l_ - length of the line (to the right)
 * color_ - 16 bit color value of the line
 * check - if 1 it will checked, whether the line reaches the end of the surface
 * borders, with 0 no check happens
 * engineWindowX,engineWindowY - size of the pixel array*/
PREFIX void spHorizentalLine( Uint16* pixel, Sint32 x, Sint32 y, Sint32 l_, Uint32 color_, Uint32 check, Sint32 engineWindowX, Sint32 engineWindowY );

/* Function: spAddWhiteLayer
 * 
 * Adds a transparent white layer to the target surface. Slow, but
 * usefull for menu fade in and out. No z test, z set, alpha
 * test and pattern test!
 * 
 * Parameters:
 * alpha - how much shall be added? Goes from 0 (nothing happens) to 255
 * (complete white)
 * 
 * See Also:
 * <spAddBlackLayer>*/
PREFIX void spAddWhiteLayer(int alpha);

/* Function: spAddBlackLayer
 * 
 * Adds a transparent black layer to the target surface. Slow, but
 * usefull for menu fade in and out. No z test, z set, alpha
 * test and pattern test!
 * 
 * Parameters:
 * alpha - how much shall be added? Goes from 0 (nothing happens) to 255
 * (complete black)
 * 
 * See Also:
 * <spAddWhiteLayer>*/
PREFIX void spAddBlackLayer(int alpha);

/* Functions: Pattern functions
 * 
 * sparrow3d doesn't provide alpha blending. However, with these three functions
 * you can get some kind of dithering alpha effect. In movement and on screens
 * with a high dpi value it looks pretty good and is quite fast.
 * 
 * You setup a 8x8 1-bit-pattern (=64 bit), which is tested for as for z or
 * alpha values. If the pattern-bit says 1, the pixel is drawn, if 0 it is not
 * drawn. E.g. the mask
 * > 10101010
 * > 01010101
 * > 10101010
 * > 01010101
 * > 10101010
 * > 01010101
 * > 10101010
 * > 01010101
 * would make some kind of 50% alpha effect, because only 50% of the pixels are
 * drawn. */
 
 /* Function: spSetPattern32
  * 
  * Sets the pattern with two 32 bit ints, each defines 4 lines
  * 
  * Parameters:
  * first_32_bit,last_32_bit - the two 32 bit ints*/
PREFIX void spSetPattern32(Uint32 first_32_bit,Uint32 last_32_bit);

/* Function: spSetPattern64
 * 
 * Sets the pattern with two 64 bit int, it defines all 8 lines
 * 
 * Parameters:
 * pattern - the big 64 bit int*/
PREFIX void spSetPattern64(Uint64 pattern);

/* Function: spSetPattern8
 * 
 * Sets the pattern with eight 8 bit ints line by line
 * 
 * Parameters:
 * line(1..8) - the 8 bit int for the 8 lines*/
PREFIX void spSetPattern8(Uint8 line1,Uint8 line2,Uint8 line3,Uint8 line4,Uint8 line5,Uint8 line6,Uint8 line7,Uint8 line8);

/* Function: spDeactivatePattern
 * 
 * Deactivates the pattern. They are reactivated again with a call of one of the
 * 3 functions above.*/
PREFIX void spDeactivatePattern();

/* Function: spSetAlphaPattern
 * 
 * Sets the pattern in dependence of an alpha value to "emulate" alpha blending.
 * 
 * Parameters:
 * alpha - the faked alpha value from 0 to 255
 * shift - Shifting of the pattern, goes from 0 to 63. The problem is: Two
 * primitives drawn with the same pattern (== same alpha and shift value) will
 * cover each other, because exactly the same pixel will not be drawn. The alpha
 * blending fake effect is gone. Solution: Using alpha simulation just a bit,
 * using very different alpha values for covering primitives and using different
 * shift values for every primitive, but for better look use for every single
 * primitive in every frame the same shifting value, else it will look shitty.
 * 
 * See Also:
 * <spSetAlphaPattern4x4>*/
PREFIX void spSetAlphaPattern(int alpha,int shift);

/* Function: spSetAlphaPattern4x4
 * 
 * Works pretty like <spSetAlphaPattern>, but it uses 4x4 pattern internal. So
 * there are only 16 different "alpha values", but it looks better most of
 * the time. (And I think it is faster, too.)
 * 
 * Parameters:
 * alpha - still goes from 0 to 255, nevertheless only 16 value are used
 * internal, so e.g the alpha value 3 and 7 are handeld the same way.
 * shift - goes only from 0 to 15, but works in the same way as in
 * <spSetAlphaPattern>
 * 
 * See Also:
 * <spSetAlphaPattern>*/
PREFIX void spSetAlphaPattern4x4(int alpha,int shift);

#endif
