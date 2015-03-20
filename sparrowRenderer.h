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

/* File: sparrowRenderer
 * 
 * SparrowRenderer is for rendering 3D stuff to the target of sparrowPrimitives.
 * Like sparrowPrimitves it provides triangles, quads (with and without
 * texture), lines, ellipses, rectangles and blitting or rotozoom of
 * surfaces, furthermore light calculation. Like other 3d libraries
 * (e.g. OpenGL) sparrow3d uses matrizes for the orientation and
 * position in space.*/
#ifndef _SPARROW_RENDERER_H
#define _SPARROW_RENDERER_H

#include "sparrowDefines.h"
#include <SDL.h>

/* Defines: Light
 * 
 * SP_MAX_LIGHTS - maximal count of lights. However more would look shitty
 * nevertheless.*/
 
#define SP_MAX_LIGHTS 8

/* define: SP_MODELVIEW_STACK_SIZE
 * 
 * The maximum depth of <spPushModelView> and <spPopModelView> Operations*/
#define SP_MODELVIEW_STACK_SIZE 16

/* Type: spPoint
 * 
 * a point struct with space, modelview and projected coordinates
 * 
 * Variables:
 * x,y,z (Sint32) - coordinates in space
 * tx,ty,tz (Sint32) - with modelview multiplied coordinates
 * px,py,pz (Sint32) - projected coordinates*/
typedef struct spPointStruct *spPointPointer;
typedef struct spPointStruct
{
	Sint32 x, y, z;
	Sint32 tx, ty, tz;
	Sint32 px, py, pz;
} spPoint;

/* Type: spTexPoint
 * 
 * a point struct with space, modelview, projected and texture coordinates and
 * the w clip
 * 
 * Variables:
 * x,y,z (Sint32) - coordinates in space
 * tx,ty,tz (Sint32) - with modelview multiplied coordinates
 * px,py,pz (Sint32) - projected coordinates
 * u,v (Sint32) - texture coordinates
 * w (Sint32) - w clip*/
typedef struct spTexPointStruct *spTexPointPointer;
typedef struct spTexPointStruct
{
	Sint32 x, y, z;
	Sint32 tx, ty, tz;
	Sint32 px, py, pz;
	Sint32 u, v, w;
} spTexPoint;

/* Type: spEdge
 * 
 * edge struct for meshes.
 * 
 * Variables:
 * point (int*) - array of the two points, which create an edge
 * status (int) - flag after drawing (optional), whether the edge is in the background
 * (-1), in the foreground (1) or at the border (0)*/
typedef struct spEdgeStruct *spEdgePointer;
typedef struct spEdgeStruct
{
	int point[2];
	int status;
	int face[2];
	int face_count;
} spEdge;

/* Type: spTriangleS
 * 
 * A triangle struct for meshes.
 * 
 * Variables:
 * point (int*) - Array of 3 references to points
 * was_drawn (int) - determines, where the triangle was drawn the last time:
 * 0 = not drawn, else 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop,
 * 32 right, 64 rightbottom, 128 bottom, 256 leftbottom
 * edge (int*) - Array of 3 references to edges
 * normal (Sint32) - Array with 3 fixed point numbers as normal
 * pNormal (Sint32) - Array with 3 fixed point numbers as projected normal */
typedef struct spTriangleStruct *spTrianglePointer;
typedef struct spTriangleStruct
{
	int point[3];
	int was_drawn;
	int edge[3];
	Sint32 normal[3];
	Sint32 pNormal[3];
} spTriangleS;

/* Type: spQuadS
 * 
 * A triangle struct for meshes.
 * 
 * Variables:
 * point (int*) - Array of 4 references to points
 * was_drawn (int) - determines, where the quad was drawn the last time:
 * 0 = not drawn, else 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop,
 * 32 right, 64 rightbottom, 128 bottom, 256 leftbottom
 * edge (int*) - Array of 4 references to edges
 * normal (Sint32) - Array with 3 fixed point numbers as normal
 * pNormal (Sint32) - Array with 3 fixed point numbers as projected normal */
typedef struct spQuadStruct *spQuadPointer;
typedef struct spQuadStruct
{
	int point[4];
	int was_drawn;
	int edge[4];
	Sint32 normal[3];
	Sint32 pNormal[3];
} spQuadS;

/* Type: spModel
 * 
 * Contains a mesh. You can fill it on your own (have fun, it is a lot of work)
 * or use <sparrowMesh> to load (e.g.) wavefront object files. SparrowRenderer
 * provides functions for drawing these meshes very fast.
 * 
 * Variables:
 * texture (SDL_Surface*) - the texture. Sparrow3D Meshes only can have 1 texture
 * pointCount (int) - number of points without texture coordinates in the mesh
 * point (spPoint*) - pointer/array to pointCount <spPoint> structs
 * texPointCount (int) - number of points with texture coordinates in the mesh
 * texPoint (spTexPoint*) - pointer/array to texPointCount <spTexPoint> structs
 * triangleCount (int) - numbers of untextured triangles in the Mesh
 * triangle (spTriangleS*) - pointer/array to triangleCount <spTriangleS> structs
 * texTriangleCount (int) - numbers of textured triangles in the Mesh
 * texTriangle (spTriangleS*) - pointer/array to texTriangleCount <spTriangleS> structs
 * quadCount (int) - numbers of untextured quads in the Mesh
 * quad (spQuadS)- pointer/array to quadCount <spQuadS> structs
 * texQuadCount (int) - numbers of textured quads in the Mesh
 * texQuad (spQuadS) - pointer/array to texQuadCount <spQuadS> structs
 * edgeCount (int) - numbers of untextured edges in the Mesh
 * edge (spEdge*) - pointer/array to edgeCount <spEdge> structs
 * texEdgeCount (int) - numbers of textured edges in the Mesh
 * texEdge (spEdge*) - pointer/array to texEdgeCount <spEdge> structs
 * color (Uint16) - 16 bit colour of the whole mesh
 * */
typedef struct spModelStruct *spModelPointer;
typedef struct spModelStruct
{
	SDL_Surface* texture;
	int pointCount;
	spPointPointer point;
	int texPointCount;
	spTexPointPointer texPoint;
	int triangleCount, texTriangleCount;
	spTrianglePointer triangle, texTriangle;
	int quadCount, texQuadCount;
	spQuadPointer quad, texQuad;
	int edgeCount, texEdgeCount;
	spEdgePointer edge, texEdge;
	Uint16 color;
} spModel;

/* Type: spLight
 * 
 * Struct for light sources.
 * 
 * Variables:
 * r,g,b (Uint32) - fixed point color of the light. (SP_ONE,SP_ONE,SP_ONE) is a bright
 * white
 * x,y,z (Sint32) - position of the light
 * tx,ty,tz (Sint32) - translated position of the light
 * active (Sint32) - determines, whether the light is enabled in calculation or not. 1 is
 * on, 0 is off*/
typedef struct spLightStruct *spLightPointer;
typedef struct spLightStruct
{
	Uint32 r, g, b;
	Sint32 x, y, z;
	Sint32 tx,ty,tz;
	Sint32 active;
} spLight;


/* Functions: Operations for the Matrizes
 * 
 * Like other 3d libraries like OpenGL, sparrow3d uses homolog 4x4 matrizes for
 * the orientation in space, too. These functions are for manipulating the
 * projection and modelview matrix. The last is for the orientation in space,
 * the first for projecting from the 3d space to the 2d space of the screen.
 * Most of the times you will not have to change the matrizes on your own and
 * you can just functions for manipulating them */
 
 
/* Function: spSetFrustumf2
 * 
 * Function for setting up the frustom of the projection matrix. Most of the
 * time you will not use this directly, but the perspective setting up functions
 * do.
 * 
 * Parameters:
 * matrix - a pointer to at least 16 Sint32 values for the matrix
 * left - the left border of the frustum
 * right - the right border of the frustum
 * bottom - the bottom border of the frustum
 * top - the top border of the frustum
 * znear - z position auf the near plane
 * zfar - z position of the far plane*/
PREFIX void spSetFrustumf2( Sint32 *matrix, Sint32 left, Sint32 right, Sint32 bottom, Sint32 top,
							Sint32 znear, Sint32 zfar );
							
							
/* Function: spSetPerspective
 * 
 * Sets the projection matrix. Because you call this function mostly just one
 * time at beginning and the need of REALLY high accuracy, this is one of the
 * rare functions from sparrow3d, which has floats as parameter.
 * 
 * Parameters:
 * fovyInDegrees - the field of View in degrees. 45° is a good value
 * aspectRatio - the aspect ratio of the projection. width / height is a good
 * value most of the times
 * znear,zfar - the nearest and farest value for the z ordinate
 * 
 * See Also:
 * <spSetPerspectiveStereoscopic>*/
PREFIX void spSetPerspective( float fovyInDegrees, float aspectRatio,
							  float znear, float zfar );

/* Function: spSetPerspectiveStereoscopic
 * 
 * Sets and returns a projection matrix for steroscopic viewing, so most of the
 * times you will call this function for each eye once. Because you call this
 * function mostly just one time at beginning and the need of REALLY high
 * accuracy, this is one of the rare functions from sparrow3d, which has floats
 * as parameter.
 * 
 * Parameters:
 * projectionMatrix - a pointer to at least 16 Sint32 values for the calculated
 * projection matrix
 * fovyInDegrees - the field of View in degrees. 45° is a good value
 * aspectRatio - the aspect ratio of the projection. width / height is a good
 * value most of the times
 * znear,zfar - the nearest and farest value for the z ordinate
 * z0 - position of the null paralaxis. That is the plane, where the left and
 * right eye got the same image and it seems, that the object is at screen
 * position while stereoscopic rendering
 * distance - the distance from the centre, for one eye positive, for one
 * negative
 * 
 * See Also:
 * <spSetPerspective>*/
PREFIX void spSetPerspectiveStereoscopic( Sint32* projectionMatrix, float fovyInDegrees, float aspectRatio,
							  float znear, float zfar , float z0,float distance);

/* Function: spStereoCreateProjectionMatrixes
 * 
 * Use this function, if you want two projection matrixes with the given eye
 * distance for stereoscopic projection. The projection matrix of sparrow3d is
 * not set! You have to set it with
 * memcpy(spGetProjectionMatrix(),matrix,sizeof(Sint32)*16) by yourself.
 * For stereoscopic rendering, you have to render the scene two times with the
 * colours of your 3d glasses and merge it afterwars, e.g. with
 * <spStereoMergeSurfaces> (screen,right_screen,crossedEyes).
 * 
 * Parameters:
 * left_matrix, right_matrix - the matrizes (pointer to at least 16 Sint32
 * values) of the left and right eye
 * fovyInDegrees - the field of View in degrees. 45° is a good value
 * aspectRatio - the aspect ratio of the projection. width / height is a good
 * value most of the times
 * znear,zfar - the nearest and farest value for the z ordinate
 * z0 - position of the null paralaxis. That is the plane, where the left and
 * right eye got the same image and it seems, that the object is at screen
 * position while stereoscopic rendering
 * distance - the distance from the centre, for one eye positive, for one
 * negative
 * crossed - determines, whether the you will use crossed eye or color merging
 * for merging the two resulting projections. For colored glassed use crossed=0,
 * for two seperate images crossed=0
 * */
PREFIX void spStereoCreateProjectionMatrixes(Sint32* left_matrix,Sint32* right_matrix,float fovyInDegrees, float aspectRatio,
							  float znear, float zfar , float z0,float distance,int crossed);

/* Function: spGetProjectionMatrix
 * 
 * Returns the projetion matrix pointer. Be carefull: You get the real pointer,
 * no copy. Usefull for setting the projection matrix on yourself, e.g. while
 * rendering stereoscopic:
 * >spSelectRenderTarget(right_screen);
 * >memcpy(spGetProjectionMatrix(),right_projection,sizeof(Sint32)*16);
 * 
 * Returns:
 * Sint32* - pointer to the projection matrix
 * 
 * See Also:
 * <spGetMatrix>*/
PREFIX Sint32* spGetProjectionMatrix( void );

/* Function: spGetMatrix
 * 
 * Returns you the REAL modelview matrix pointer! That
 * means, every change is directly updated.
 * 
 * Returns:
 * Sint32* - pointer to the modelview matrix
 * 
 * See Also:
 * <spGetProjectionMatrix>*/
PREFIX Sint32* spGetMatrix( void );

/* Function: spPushModelView
 * 
 * Saves the recent ModelView matrix on a stack. Get the last element
 * of the stack with <spPopModelView>.
 * 
 * See Also:
 * <spPopModelView>*/
PREFIX void spPushModelView( void );

/* Function: spPopModelView
 * 
 * Loads the ModelView matrix from the stack.
 * 
 * See Also:
 * <spPushModelView>*/
PREFIX void spPopModelView( void );

/* Function: spIdentity
 * 
 * Sets the modelview matrix to the identity matrix. This resets the
 * view. If you now would draw something, it would be drawn to (0,0,0),
 * without rotation and without scaling. */
PREFIX void spIdentity( void );

/* Function: spMulMatrix
 * 
 * Multiplies the modelview matrix with the given matrix.
 * 
 * Parameters:
 * matrix - a pointer to 16 Sint32 values*/
PREFIX void spMulMatrix(Sint32* matrix);

/* Function: spMulMatrixLeft
 * 
 * Multiplies the modelview matrix with the given matrix, but from the
 * left side.
 * 
 * Parameters:
 * matrix - a pointer to 16 Sint32 values*/
PREFIX void spMulMatrixLeft(Sint32* matrix);

/* Function: spScale
 * 
 * spScale scales the modelview matrix. Use it with care, seems to be
 * a bit buggy...
 * 
 * Parameters:
 * x,y,z - zoom parameters. Set everything to SP_ONE to get no result*/
PREFIX void spScale( Sint32 x, Sint32 y, Sint32 z );

/* Function: spRotate
 * 
 * Rotates rad radians around the vector (x,y,z). Also a bit
 * buggy. spRotate(X|Y|Z) are better most of the times.
 * 
 * Parameters:
 * x,y,z - axis to rotate
 * rad - angle to rotate
 * 
 * See Also:
 * <spRotateX>, <spRotateY>, <spRotateZ>*/
PREFIX void spRotate( Sint32 x, Sint32 y, Sint32 z, Sint32 rad );

/* Function: spRotateX
 * 
 * Rotates rad radians around the x-axis.
 * 
 * Parameters:
 * rad - angle to rotate around the x-axis
 * 
 * See Also:
 * <spRotate>, <spRotateY>, <spRotateZ>*/
PREFIX void spRotateX( Sint32 rad );

/* Function: spRotateY
 * 
 * Rotates rad radians around the y-axis.
 * 
 * Parameters:
 * rad - angle to rotate around the y-axis
 * 
 * See Also:
 * <spRotate>, <spRotateX>, <spRotateZ>*/
PREFIX void spRotateY( Sint32 rad );

/* Function: spRotateZ
 * 
 * Rotates rad radians around the z-axis.
 * 
 * Parameters:
 * rad - angle to rotate around the z-axis
 * 
 * See Also:
 * <spRotate>, <spRotateX>, <spRotateY>*/
PREFIX void spRotateZ( Sint32 rad );

/* Function: spTranslate
 * 
 * This function does a relative translation to (x,y,z).
 * 
 * Parameters:
 * x,y,z - way to translate*/
PREFIX void spTranslate( Sint32 x, Sint32 y, Sint32 z );

/* Functions: Light functions
 * 
 * Only <"Real" 3D functions> (except spLine3D) are enlighted. You have up to
 * 8 light sources (in fact, even more than 4 look shitty).*/

/* Function: spSetLight
 * 
 * Sets Light Calculation on or off. Default off (0)
 * 
 * Parameters:
 * value - 0 means off, 1 means on*/
PREFIX void spSetLight( int value );

/* Function: spEnableLight
 * 
 * Specifies, whether a light is used or not for the light calculation.
 * (default: just number 0 is enabled)
 * 
 * Paramaters:
 * number - the light to (de)activate
 * active - 1 for turning on, 0 for switching off*/
PREFIX void spEnableLight( int number, Sint32 active );

/* Function: spSetLightColor
 * 
 * Sets the color of a light
 * 
 * Parameters:
 * number - the light to change the color
 * r,g,b - Uint32 fixed point RGB color of the light*/
PREFIX void spSetLightColor( int number, Uint32 r, Uint32 g, Uint32 b );

/* Function: spSetLightPosition
 * 
 * Sets the Light Position. The Position will be transformed with the
 * modelview matrix *as it is* at call!
 * 
 * Parameters:
 * number - the light to change position
 * x,y,z - new position of the light
 * 
 * See Also:
 * <spUpdateLight>*/
PREFIX void spSetLightPosition( int number, Sint32 x, Sint32 y, Sint32 z );

/* Function: spUpdateLight
 * 
 * If you want to update the lights position without a recall of
 * <spSetLightPosition> use this function. This just multiplies the lights
 * position with the modelview matrix to get the new position in camera space.
 * 
 * Parameters:
 * number - the light to update position
 * 
 * See Also:
 * <spSetLightPosition>*/
PREFIX void spUpdateLight(int number);

/* Function: spSetAmbientLightColor
 * 
 * Sets the global ambient light value. This value is added to all other lights
 * independing from position of the face in space.
 * 
 * Parameters:
 * r,g,b - Fixed point Uint32 color value. Default: 0.25, 0.25, 0.25*/
PREFIX void spSetAmbientLightColor( Uint32 r, Uint32 g, Uint32 b );

/* Functions: "Real" 3D functions
 * 
 * These functions are for drawing 3d primitives in space with light calculation,
 * rotation and so on.*/

/* Function: spTriangle3D
 * 
 * Draws a triangle in 3D space. Returns 0 if not drawn (culling) or different
 * bits, where the edges are: 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 
 * 32 right, 64 rightbottom, 128 bottom, 256 leftbottom.
 * 
 * Parameters:
 * x1,y1,z1 - position of the first triangle point in space
 * x2,y2,z2 - position of the second triangle point in space
 * x3,y3,z3 - position of the third triangle point in space
 * color - 16 Bit color of the triangle
 * 
 * Returns:
 * int - see above
 * 
 * See Also:
 * <spTriangleTex3D>, <spQuad3D>*/
PREFIX int spTriangle3D( Sint32 x1, Sint32 y1, Sint32 z1,
						 Sint32 x2, Sint32 y2, Sint32 z2,
						 Sint32 x3, Sint32 y3, Sint32 z3, Uint16 color );

/* Function: spQuad3D
 * 
 * Draws a quad in 3D space. Returns 0 if not drawn (culling) or different
 * bits, where the edges are: 1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 
 * 32 right, 64 rightbottom, 128 bottom, 256 leftbottom.
 * 
 * Parameters:
 * x1,y1,z1 - position of the first quad point in space
 * x2,y2,z2 - position of the second quad point in space
 * x3,y3,z3 - position of the third quad point in space
 * x4,y4,z4 - position of the fourth quad point in space
 * color - 16 Bit color of the quad
 * 
 * Returns:
 * int - see above
 * 
 * See Also:
 * <spTriangle3D>, <spQuadTex3D>*/
PREFIX int spQuad3D( Sint32 x1, Sint32 y1, Sint32 z1,
					 Sint32 x2, Sint32 y2, Sint32 z2,
					 Sint32 x3, Sint32 y3, Sint32 z3,
					 Sint32 x4, Sint32 y4, Sint32 z4, Uint16 color );

/* Function: spTriangleTex3D
 * 
 * Draws a textured triangle in 3D space. Returns 0 if not drawn (culling) or
 * different bits, where the edges are: 1 screen, 2 left, 4 lefttop, 8 top,
 * 16 righttop, 32 right, 64 rightbottom, 128 bottom, 256 leftbottom.
 * The used texture is determined with <spBindTexture>.
 * 
 * Parameters:
 * x1,y1,z1 - position of the first triangle point in space
 * u1,v1 - texture position of the first triangle point
 * x2,y2,z2 - position of the second triangle point in space
 * u2,v2 - texture position of the second triangle point
 * x3,y3,z3 - position of the third triangle point in space
 * u3,v3 - texture position of the third triangle point
 * color - 16 Bit color of the triangle
 * 
 * Returns:
 * int - see above
 * 
 * See Also:
 * <spTriangle3D>, <spQuadTex3D>*/
PREFIX int spTriangleTex3D( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1,
							Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2,
							Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint16 color );

/* Function: spQuadTex3D
 * 
 * Draws a textured quad in 3D space. Returns 0 if not drawn (culling) or
 * different bits, where the edges are: 1 screen, 2 left, 4 lefttop, 8 top,
 * 16 righttop, 32 right, 64 rightbottom, 128 bottom, 256 leftbottom.
 * The used texture is determined with <spBindTexture>.
 * 
 * Parameters:
 * x1,y1,z1 - position of the first quad point in space
 * u1,v1 - texture position of the first quad point
 * x2,y2,z2 - position of the second quad point in space
 * u2,v2 - texture position of the second quad point
 * x3,y3,z3 - position of the third quad point in space
 * u3,v3 - texture position of the third quad point
 * x4,y4,z4 - position of the fourth quad point in space
 * u4,v4 - texture position of the fourth quad point
 * color - 16 Bit color of the quad
 * 
 * Returns:
 * int - see above
 * 
 * See Also:
 * <spQuad3D>, <spTriangleTex3D>*/
PREFIX int spQuadTex3D( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1,
						Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2,
						Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3,
						Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Uint16 color );

/* Function: spMesh3D
 * 
 * Draws a mesh in 3D space. Every time the was_drawn variable of every face is
 * sets, which determines, whether the face was drawn (uneven 0) and where
 * (like e.g. the return value of spTriangle3D).
 * 
 * Parameters:
 * mesh - pointer to a spModel struct to be drawn
 * updateEdgeList - determines, whether the edgeList shall be updated.
 * If 0, no update is made. When 1 every edge is marked. When 2 a solid
 * body is assumed and "holes" (e.g. because of textured and untextured
 * subbodys) are ignored.
 * 
 * Returns:
 * int - the count of drawn faces
 * 
 * See Also:
 * <spMesh3DwithPos>*/
PREFIX int spMesh3D( spModelPointer mesh, int updateEdgeList );

/* Function: spMesh3DwithPos
 * 
 * like <spMesh3D>, but with an additional position.
 * 
 * Parameters:
 * x,y,z - position to draw to mesh
 * mesh - pointer to a spModel struct to be drawn
 * updateEdgeList - determines, whether the edgeList shall be updated.
 * If 0, no update is made. When 1 every edge is marked. When 2 a solid
 * body is assumed and "holes" (e.g. because of textured and untextured
 * subbodys) are ignored.
 *  
 * Returns:
 * int - the count of drawn faces
 * 
 * See Also:
 * <spMesh3D>*/
PREFIX int spMesh3DwithPos(Sint32 x,Sint32 y,Sint32 z, spModelPointer mesh, int updateEdgeList );

/* Function: spLine3D
 * 
 * Draws a line in the 3d space with the given color. No light
 * calculation is done!
 * 
 * Parameters:
 * x1,y1,z1 - the point from which the line should be drawn
 * x2,y2,z2 - the point to which the line should be drawn
 * color - 16 bit color of the line*/
PREFIX void spLine3D( Sint32 x1, Sint32 y1, Sint32 z1,
					  Sint32 x2, Sint32 y2, Sint32 z2, Uint16 color );

/* Functions: "Billboard" 3D Functions
 * 
 * For these functions the position and size are projected, but the
 * rotation is always, that you see the front (also called "billboard").
 * No lighting, as it woudln't make much sense with the face always
 * showing in the same direction!*/

/* Function: spRotozoomSurface3D
 * 
 * Draws a rotozoomed surface in 3d space. If father away, it gets
 * smaller. If you don't want these effect evaluate zoomX and zoomY with
 * the z-coordinate or use <spBlit3D>.
 * 
 * Parameters:
 * x,y,z - position in 3d space
 * surface - SDL_Surface* to be drawn
 * zoomX,zoomY - x and y zoom of the surface
 * angle - the rotation of the surface in fixed point radian
 * 
 * See Also:
 * <spRotozoomSurfacePart3D>, <spBlit3D>*/
PREFIX void spRotozoomSurface3D( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 zoomX, Sint32 zoomY, Sint32 angle );

/* Function: spRotozoomSurfacePart3D
 * 
 * Draws a rotozoomed part of a surface in 3d space. If father away, it
 * gets smaller. If you don't want these effect evaluate zoomX and zoomY
 * with the z-coordinate or use <spBlitPart3D>.
 * 
 * Parameters:
 * x,y,z - position in 3d space
 * surface - SDL_Surface* to be drawn
 * sx,sy - position of the left top corner of the part of the surface to be
 * drawn
 * w,h - size of the part to be drawn
 * zoomX,zoomY - x and y zoom of the surface
 * angle - the rotation of the surface in fixed point radian*/
PREFIX void spRotozoomSurfacePart3D( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, Sint32 zoomX, Sint32 zoomY, Sint32 angle );

/* Function: spRectangle3D
 * 
 * Draws a billboard rectangle in 3d space.
 * 
 * Parameters:
 * x,y,z - position in 3d space
 * w,h - width and height of the rectangle
 * color - 16 bit color of the rectangle
 * 
 * See Also:
 * <spRectangleBorder3D>*/
PREFIX void spRectangle3D( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Uint16 color );

/*Function: spCollapsePrimitives
 * 
 * Determines, whether ellipses, rectangles, ellipses borders and
 * rectangle borders can collapse to an (unseeable) object smaller than
 * one pixel.
 * 
 * Paramaters:
 * value - 0 means, that the primitive do not collapse, if 1 for a
 * specific distance it will not be drawn anymore (too small). Default
 * is 1.*/
PREFIX void spCollapsePrimitives( int value);

/* Function: spEllipse3D
 * 
 * Draws a billboard ellipse in 3d space.
 * 
 * Parameters:
 * x,y,z - position in 3d space
 * rx,ry - x and y radius of the ellipse
 * color - 16 bit color of the ellipse
 * 
 * See Also:
 * <spEllipseBorder3D>*/
PREFIX void spEllipse3D( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Uint16 color );

/* Function: spRectangleBorder3D
 * 
 * Draws a billboard rectangle border in 3d space.
 * 
 * Parameters:
 * x,y,z - position in 3d space
 * w,h - width and height of the rectangle
 * bx,by - width and height of the border
 * color - 16 bit color of the rectangle
 * 
 * See Also:
 * <spRectangle3D>*/
PREFIX void spRectangleBorder3D( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Sint32 bx, Sint32 by, Uint16 color );

/* Function: spEllipseBorder3D
 * 
 * Draws a billboard ellipse border in 3d space.
 * 
 * Parameters:
 * x,y,z - position in 3d space
 * rx,ry - x and y radius of the ellipse
 * bx,by - width and height of the border
 * color - 16 bit color of the ellipse
 * 
 * See Also:
 * <spEllipse3D>*/
PREFIX void spEllipseBorder3D( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Sint32 bx, Sint32 by, Uint16 color );

/* Functions: "Blitting" 3D Functions
 * 
 * Very much like the <"Billboard" 3D Functions>, but the size and rotation is fix (0 and SP_ONE)!*/

/* Function: spBlit3D
 * 
 * Blits a surface in 3d space very fast.
 * 
 * Parameters:
 * 
 * x1,y1,z1 - position in space
 * surface - SDL_Surface* to be drawn
 * 
 * See Also:
 * <spRotozoomSurface3D>, <spBlitPart3D>*/
PREFIX void spBlit3D( Sint32 x1, Sint32 y1, Sint32 z1, SDL_Surface* surface );

/* Function: spBlitPart3D
 * 
 * Blits a part of a surface in 3d space very fast.
 * 
 * Parameters:
 * 
 * x1,y1,z1 - position in space
 * surface - SDL_Surface* to be drawn
 * sx,sy - position of the left top corner of the part of the surface to be
 * drawn
 * w,h - size of the part to be drawn
 * 
 * See Also:
 * <spRotozoomSurfacePart3D>, <spBlit3D>*/
PREFIX void spBlitPart3D( Sint32 x1, Sint32 y1, Sint32 z1, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h);

/* Functions: Optimization and other functions
 * 
 * Some optional optimizations and helpers.*/

/* Function: spSetPerspectiveTextureMapping
 * 
 * Every of the <"Real" 3D functions> with textures can be rendered with
 * affine texture mapping what may look a bit "wobly" or perspectivly
 * correct, which looks the best, but may be slower!
 * 
 * Parameters:
 * value - 1 enables perspectively correct mapping, 0 disables. 0 is
 * default*/
PREFIX void spSetPerspectiveTextureMapping(int value);

/* Function: spProjectPoint3D
 * 
 * Projects a point in the 3D scene. Usefull if you will draw your own
 * stuff in the 3D world.
 * 
 * Parameters:
 * x,y,z - 3d space point to project
 * px,py,pz - pointer (!) to Sint32 coordinates, where to write the
 * projection result
 * w - pointer (!) to a Sint32 variable to save the w coordinate for
 * w-clipping if needed
 * withModelview - says, whether the rotation in the ModelViewMatrix
 * should be involved. E.g. if you want to get a perspective correct
 * width, you don't want the rotation. In this case set it to 0. Most of
 * the time you should be fine with 1*/
PREFIX void spProjectPoint3D( Sint32 x, Sint32 y, Sint32 z, Sint32 *px, Sint32 *py, Sint32 *pz, Sint32 *w,Sint32 withModelview );

/* Function: spUsePrecalculatedNormals
 * 
 * Desides, whether the precalculated normal values of meshes are used
 * or not. So this works only with spMesh3D* functions. Be carefull! You
 * can use this optimization only, if you don't scale! Translation and
 * rotations are fine. At default this feature is disabled.
 * 
 * Parameters:
 * value - 1 enabled, 0 disabled (default)*/
PREFIX void spUsePrecalculatedNormals(int value);

#endif
