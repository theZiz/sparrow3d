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

/* This file is for rendering 3D stuff to the target of sparrowPrimitives.
 * Like sparrowPrimitves it provides triangles, quads (with and without
 * texture), lines, ellipses, rectangles and blitting or rotozoom of
 * surfaces. Furthermore light calculation. Like other 3d libraries
 * (e.g. OpenGL) sparrow3d uses matrizes for the orientation and
 * position in space.*/
#ifndef _SPARROW_RENDERER_H
#define _SPARROW_RENDERER_H

#include "sparrowDefines.h"
#include <SDL.h>

/* This #define defines the max count of lights possible in
 * sparrowRenderer. Even if you could use more lights: Because of the
 * very simple light calculation it wouldn't be look good. */
#define SP_MAX_LIGHTS 8
/* These are the types for the light calculation. Sint64 because of the
 * high accuracy. Really: With Sint32 it is flickering like hell... */
#define SP_LIGHT_TYPE Sint64
#define SP_LIGHT_TYPE_SIZE 64
#define SP_LIGHT_ACCURACY 24
#define SP_LIGHT_HALF_ACCURACY 12
#define SP_LIGHT_ACCURACY_FACTOR (65536.0f*256.0f)

/* A point with the projected coordinates, but without uv-coordinates.
 * Used for Meshes.*/
typedef struct spPointStruct *spPointPointer;
typedef struct spPointStruct
{
	Sint32 x, y, z;    //space coordinates
	Sint32 tx, ty, tz; //multiplied with modelview
	Sint32 px, py, pz; //projected
} spPoint;

/* A point with the projected coordinates and uv-coordinates. Used for
 * Meshes. */
typedef struct spTexPointStruct *spTexPointPointer;
typedef struct spTexPointStruct
{
	Sint32 x, y, z;    //space coordinates
	Sint32 tx, ty, tz; //multiplied with modelview
	Sint32 px, py, pz; //projected
	Sint32 u, v, w; //texture coordinates + w clip
} spTexPoint;

/* An edge with a status-flag, e.g. for borderlines */
typedef struct spEdgeStruct *spEdgePointer;
typedef struct spEdgeStruct
{
	int point[2];
	int status; //-1 background, 0 border, 1 foreground
} spEdge;

/* A triangle struct for meshes */
typedef struct spTriangleStruct *spTrianglePointer;
typedef struct spTriangleStruct
{
	int point[3]; //the edges
	/* was_drawn shows (always), where the triangle was drawn the
	 * last time: 0 = not drawn, else 1 screen, 2 left, 4 lefttop,
	 * 8 top, 16 righttop, 32 right, 64 rightbottom, 128 bottom,
	 * 256 leftbottom */
	int was_drawn;
	int edge[3]; //the update of the Edges status flag is optional!
	Sint32 normal[3];
	Sint32 pNormal[3];
} spTriangleS;

/* A quad struct for meshes */
typedef struct spQuadStruct *spQuadPointer;
typedef struct spQuadStruct
{
	int point[4]; //the edges
	/* was_drawn shows (always), where the triangle was drawn the
	 * last time: 0 = not drawn, else 1 screen, 2 left, 4 lefttop,
	 * 8 top, 16 righttop, 32 right, 64 rightbottom, 128 bottom,
	 * 256 leftbottom */
	int was_drawn;
	int edge[4]; //the update of the Edges status flag is optional!
	Sint32 normal[3];
	Sint32 pNormal[3];
} spQuadS;

/* spModelSturct contains a mesh. You can fill it on your own (have fun,
 * it is a lot of work) or use sparrowMesh to load (e.g.) wave object
 * files. sparrowRenderer provides functions for drawing these meshes
 * very fast */
typedef struct spModelStruct *spModelPointer;
typedef struct spModelStruct
{
	SDL_Surface* texture;
	int pointCount;
	spPointPointer    point; //"normal" points
	int texPointCount;
	spTexPointPointer texPoint; //points with u,v coordinates
	int triangleCount, texTriangleCount;
	spTrianglePointer triangle, texTriangle; //the triangles of the modell
	int quadCount, texQuadCount;
	spQuadPointer quad, texQuad; //the quads of the modell
	int edgeCount, texEdgeCount;
	spEdgePointer edge, texEdge;
	Uint16 color;
} spModel;

/* This is a struct for one light source */
typedef struct spLightStruct *spLightPointer;
typedef struct spLightStruct
{
	Uint32 r, g, b; //fix point color! (1,1,1) is "normal"
	Sint32 x, y, z; //the position
	Sint32 tx,ty,tz; //the translated position;
	Sint32 active;  //just guess, what is says ;-)
} spLight;


//--- Operations for the Matrizes ---
/* spSetFrustumf2 setups the frustum of the projection matrix. In most
 * cases you don't need it directly, because spSetPerspective calls it */
void spSetFrustumf2( Sint32 *matrix, Sint32 left, Sint32 right, Sint32 bottom, Sint32 top,
							Sint32 znear, Sint32 zfar );
							
							
/* spSetPerspective sets the Projection matrix. Because you call this
 * function mostly just one time at beginning and the need of REALLY
 * high accuracy, this is one of the only functions from sparrow3d,
 * which has floats as parameter.
 * - fovyInDegrees is the field of View in degrees
 * - aspectRatio is self-explanatory. width / height is a good value
 *   most of the times.
 * - znear and zfar are the nearest and farest value for the z ordinate.
 *   in fact: znear is ignored most of the time*/
PREFIX void spSetPerspective( float fovyInDegrees, float aspectRatio,
							  float znear, float zfar );

/* Works like spSetPerspective, but for stereoscopic projection */
PREFIX void spSetPerspectiveStereoscopic( Sint32* projectionMatrix, float fovyInDegrees, float aspectRatio,
							  float znear, float zfar , float z0,float distance);

/* Use this function, if you want two projection matrixes with the given
 * distance for stereoscopic projection. The projection matrix of sparrow3d is
 * not set! You have to set it with
 * memcpy(spGetProjectionMatrix(),matrix,sizeof(Sint32)*16) by yourself.
 * For stereoscopic rendering, you have to render the scene two times with the
 * colours of your 3d glasses and merge it afterwars, e.g. with
 * spStereoMergeSurfaces(screen,right_screen,crossedEyes). */
PREFIX void spStereoCreateProjectionMatrixes(Sint32* left_matrix,Sint32* right_matrix,float fovyInDegrees, float aspectRatio,
							  float znear, float zfar , float z0,float distance,int crossed);

/* spGetProjectionMatrix returns the projetion matrix. Be carefull: You
 * get the real pointer, no copy. */
PREFIX Sint32* spGetProjectionMatrix();

/* you can set the projectio matrix on your own, if you want to. matrix
 * will be copied to the internal projection matrix variable (which is
 * returned by spGetProjectionMatrix) */
PREFIX void spSetProjectionMatrix( Sint32* matrix );

/* Sets the Modellview matrix to the identity matrix. This resets the
 * view. If you now would draw something, it would be drawn to (0,0,0),
 * without rotation and without scaling. */
PREFIX void spIdentity();

/* spScale scales the modell view matrix. Use it with care, seems to be
 * a bit buggy... */
PREFIX void spScale( Sint32 x, Sint32 y, Sint32 z );

/* spRotate rotates rad radians around the vector (x,y,z). Also a bit
 * buggy. spRotate(X|Y|Z) are better most of the times. */
PREFIX void spRotate( Sint32 x, Sint32 y, Sint32 z, Sint32 rad );

/* Rotates around the X-Axis. Every transformation done AFTERwards is
 * rotated around rad radians */
PREFIX void spRotateX( Sint32 rad );

/* Rotates around the Y-Axis. Every transformation done AFTERwards is
 * rotated around rad radians */
PREFIX void spRotateY( Sint32 rad );

/* Rotates around the Z-Axis. Every transformation done AFTERwards is
 * rotated around rad radians */
PREFIX void spRotateZ( Sint32 rad );

/* spTranslate does a relative translation to (x,y,z) */
PREFIX void spTranslate( Sint32 x, Sint32 y, Sint32 z );

/* spGetMatrix returns you the REAL modellview matrix pointer! That
 * means, every change is directly updated. It is usefull for saving
 * the modell view matrix like glPush.*/
PREFIX Sint32* spGetMatrix();

/* Sets the modell view matrix (copies matrix). Usefull for restoring 
 * a modell view matrix like glPop */
PREFIX void spSetMatrix( Sint32* matrix );

//--- Light functions. Only "real" 3D functions (except spLine3D) are enlighted ---

/* Sets Light Calculation on or off. Default off (0) */
PREFIX void spSetLight( int value );

/* Specifies, whether the light number is used or not (default: just
 * number 0 is enabled)*/
PREFIX void spEnableLight( int number, Sint32 active );

/* Sets the Light Color */
PREFIX void spSetLightColor( int number, Uint32 r, Uint32 g, Uint32 b );

/* Sets the Light Position. The Position will be transformed with the
 * Modelview matrix as it is at call! */
PREFIX void spSetLightPosition( int number, Sint32 x, Sint32 y, Sint32 z );

/* If you want to update the lights position without a recall of
 * spSetLightPosition use this. This just multiplies the lights position
 * with the Modelviewmatrix. */
PREFIX void spUpdateLight(int number);

/* Sets the global ambient light value used be every 3D quad or triangle
 * Default: 0.25, 0.25, 0.25 */
PREFIX void spSetAmbientLightColor( Uint32 r, Uint32 g, Uint32 b );

//--- "Real" 3D functions, where the primitives is correct rotated ---

/* every 3D function with textures can be rendered with affine texture mapping,
 * what may look a bit "wobly" or perspectivly correct, which looks the best,
 * but may be slower! */
PREFIX void spSetPerspectiveTextureMapping(int value);

/* Draws a triangle in 3D space. Returns 0
 * if not drawn (culling) or different bits, where the edges are:
 * 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right,
 * 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spTriangle3D( Sint32 x1, Sint32 y1, Sint32 z1,
						 Sint32 x2, Sint32 y2, Sint32 z2,
						 Sint32 x3, Sint32 y3, Sint32 z3, Uint16 color );

/* Draws a quad in 3D space. Returns 0
 * if not drawn (culling) or different bits, where the edges are:
 * 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right,
 * 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spQuad3D( Sint32 x1, Sint32 y1, Sint32 z1,
					 Sint32 x2, Sint32 y2, Sint32 z2,
					 Sint32 x3, Sint32 y3, Sint32 z3,
					 Sint32 x4, Sint32 y4, Sint32 z4, Uint16 color );

/* Draws a triangles with textue in 3D space. Returns 0
 * if not drawn (culling) or different bits, where the edges are:
 * 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right,
 * 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spTriangleTex3D( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1,
							Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2,
							Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint16 color );

/* Draws a quad with textue in 3D space. Returns 0
 * if not drawn (culling) or different bits, where the edges are:
 * 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right,
 * 64 rightbottom, 128 bottom, 256 leftbottom */
PREFIX int spQuadTex3D( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1,
						Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2,
						Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3,
						Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Uint16 color );

/* Draws a mesh in 3D space. Returns the count of drawns faces. If
 * updateEdgeList is uneven 0, the edge struct of the mesh is updated.
 * Furthermore every time the was_drawn variable of every face is sets,
 * which determines, whether the face was drawn (uneven 0) and where
 * (like e.g. the return value of spQuadTex3D). */
PREFIX int spMesh3D( spModelPointer mesh, int updateEdgeList );

/* like spMesh3D, but with a additional position */
PREFIX int spMesh3DwithPos(Sint32 x,Sint32 y,Sint32 z, spModelPointer mesh, int updateEdgeList );

/* Draws a line in the 3d space with the given color. No light
 * calculation is done. */
PREFIX void spLine3D( Sint32 x1, Sint32 y1, Sint32 z1,
					  Sint32 x2, Sint32 y2, Sint32 z2, Uint16 color );

//--- "Billboard" 3D Functions. The position and size are projected, but the rotation is always, that you see the front. No light! ---

/* spRotozoomSurface3D draws a surface only rotated angle radians around
 * the z axis. Furthermore it can be zoomed in x and y direction. The
 * surface is getting smaller, if it is more fare away. */
PREFIX void spRotozoomSurface3D( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 zoomX, Sint32 zoomY, Sint32 angle );

/* draws a part of a surface only rotated angle radians around
 * the z axis. Furthermore it can be zoomed in x and y direction. The
 * surface is getting smaller, if it is more fare away. */
PREFIX void spRotozoomSurfacePart3D( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, Sint32 zoomX, Sint32 zoomY, Sint32 angle );

/* draws a billboard rectangle in 3d space */
PREFIX void spRectangle3D( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Uint16 color );

/* draws a billboard ellipse in 3d space */
PREFIX void spEllipse3D( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Uint16 color );

/* draws a billboard rectangle border in 3d space */
PREFIX void spRectangleBorder3D( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Sint32 bx, Sint32 by, Uint16 color );

/* draws a billboard ellipse border in 3d space */
PREFIX void spEllipseBorder3D( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Sint32 bx, Sint32 by, Uint16 color );

//--- "Blitting" 3D Functions. Very much like the "Billboard" 3D Functions, but the size and rotation is fix (0 and 1)!

/* Blits a Surface. very fast! */
PREFIX void spBlit3D( Sint32 x1, Sint32 y1, Sint32 z1, SDL_Surface* surface );

/* Projects a point in the 3D scene. Usefull if you will draw your own
 * stuff in the 3D world. "withModelview" says, whether the rotation in
 * the ModelViewMatrix should be involved. E.g. if you want to get a
 * perspective correct width, you don't want the rotation. In this case
 * set it to 0. Most of the time you should be fine with 1. */
PREFIX void spProjectPoint3D( Sint32 x, Sint32 y, Sint32 z, Sint32 *px, Sint32 *py, Sint32 *pz, Sint32 *w,Sint32 withModelview );
/* spUsePrecalculatedNormals desides, whether the precalculated normal
 * values of meshes are used or not. So this works only with spMesh3D*
 * functions. Be carefull! You can use this optimization only, if you
 * don't scale! Translation and rotations are okay. At default this
 * feature is disabled */
PREFIX void spUsePrecalculatedNormals(int value);

#endif
