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
#ifndef _SPARROW_RENDERER_H
#define _SPARROW_RENDERER_H

#include "sparrowDefines.h"
#include <SDL.h>

#define SP_MAX_LIGHTS 8

//A point with the projected coordinates, but without uv-coordinates
typedef struct spPointStruct *spPointPointer;
typedef struct spPointStruct {
  Sint32 x,y,z;
  Sint32 tx,ty,tz; //multiplied with modelview
  Sint32 px,py,pz; //projected
} spPoint;

//A point with the projected coordinates and uv-coordinates
typedef struct spTexPointStruct *spTexPointPointer;
typedef struct spTexPointStruct {
  Sint32 x,y,z;
  Sint32 tx,ty,tz; //multiplied with modelview
  Sint32 px,py,pz; //projected
  Sint32 u,v;
} spTexPoint;

//An edge with a status-flag, e.g. for borderlines
typedef struct spEdgeStruct *spEdgePointer;
typedef struct spEdgeStruct {
  int point[2];
  int status; //-1 background, 0 border, 1 foreground
} spEdge;

typedef struct spTriangleStruct *spTrianglePointer;
typedef struct spTriangleStruct {
  int point[3];
  int was_drawn; //shows (always), whether the quad was drawn the last time
                 //0 = not drawn, else:
                 //1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right, 64 rightbottom, 128 bottom, 256 leftbottom
  int edge[3]; //the update of the Edges status flag is optional!
  Sint32 position; //1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right, 64 rightbottom, 128 bottom, 256 leftbottom
} spTriangleS;

typedef struct spQuadStruct *spQuadPointer;
typedef struct spQuadStruct {
  int point[4];
  int was_drawn; //shows (always), whether the quad was drawn the last time
                 //0 = not drawn, else:
                 //1 screen, 2 left, 4 lefttop, 8 top, 16 righttop, 32 right, 64 rightbottom, 128 bottom, 256 leftbottom
  int edge[4]; //the update of the Edges status flag is optional!
} spQuadS;

typedef struct spModelStruct *spModelPointer;
typedef struct spModelStruct {
  SDL_Surface* texture;
  int pointCount;
  spPointPointer    point; //"normal" points
  int texPointCount;
  spTexPointPointer texPoint; //points with u,v coordinates
  int triangleCount,texTriangleCount;
  spTrianglePointer triangle,texTriangle; //the triangles of the modell
  int quadCount,texQuadCount;
  spQuadPointer quad,texQuad; //the quads of the modell
  int edgeCount,texEdgeCount;
  spEdgePointer edge,texEdge;
  Uint16 color;
} spModel;

typedef struct spLightStruct *spLightPointer;
typedef struct spLightStruct {
  Uint32 r,g,b; //fix point number! (1,1,1) is "normal"
  Sint32 x,y,z;
  Sint32 active;
} spLight;


PREFIX void spSetPerspective(float fovyInDegrees, float aspectRatio,
                             float znear, float zfar);

PREFIX Sint32* spGetProjectionMatrix();

PREFIX void spSetProjectionMatrix(Sint32* matrix);

PREFIX void spIdentity();

PREFIX void spScale(Sint32 x,Sint32 y,Sint32 z);

PREFIX void spRotate(Sint32 x,Sint32 y,Sint32 z,Sint32 rad);

PREFIX void spRotateX(Sint32 rad);

PREFIX void spRotateY(Sint32 rad);

PREFIX void spRotateZ(Sint32 rad);

PREFIX void spTranslate(Sint32 x,Sint32 y,Sint32 z);

/* Be carefull! You get the REAL matrix pointer! That means, every change is
 * directly updated */
PREFIX Sint32* spGetMatrix();

PREFIX void spSetMatrix(Sint32* matrix);

/* Returns 1 if drawn (Culling) */
PREFIX int spTriangle3D(Sint32 x1,Sint32 y1,Sint32 z1,
  Sint32 x2,Sint32 y2,Sint32 z2,
  Sint32 x3,Sint32 y3,Sint32 z3,Uint16 color);

/* Returns 1 if drawn (Culling) */
PREFIX int spQuad3D(Sint32 x1,Sint32 y1,Sint32 z1,
  Sint32 x2,Sint32 y2,Sint32 z2,
  Sint32 x3,Sint32 y3,Sint32 z3,
  Sint32 x4,Sint32 y4,Sint32 z4,Uint16 color);

/* Returns 1 if drawn (Culling) */
PREFIX int spTriangleTex3D(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 u1,Sint32 v1,
  Sint32 x2,Sint32 y2,Sint32 z2,Sint32 u2,Sint32 v2,
  Sint32 x3,Sint32 y3,Sint32 z3,Sint32 u3,Sint32 v3,Uint16 color);

/* Returns 1 if drawn (Culling) */
PREFIX int spQuadTex3D(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 u1,Sint32 v1,
  Sint32 x2,Sint32 y2,Sint32 z2,Sint32 u2,Sint32 v2,
  Sint32 x3,Sint32 y3,Sint32 z3,Sint32 u3,Sint32 v3,
  Sint32 x4,Sint32 y4,Sint32 z4,Sint32 u4,Sint32 v4,Uint16 color);
  
PREFIX void spBlit3D(Sint32 x1,Sint32 y1,Sint32 z1,SDL_Surface* surface);

/* Returns the number of drawn Faces */
PREFIX int spMesh3D(spModelPointer mesh,int updateEdgeList);

PREFIX void spLine3D(Sint32 x1,Sint32 y1,Sint32 z1,
                     Sint32 x2,Sint32 y2,Sint32 z2,Uint16 color);

/* Sets Light Calculation on or off. Default off (0) */
PREFIX void spSetLight(int value);

/* Specifies, whether the light number is used or not (default: just
 * number 0 is enabled)*/
PREFIX void spEnableLight(int number,Sint32 active);

/* Sets the Light Values */
PREFIX void spSetLightColor(int number,Uint32 r,Uint32 g,Uint32 b);

/* Sets the Light Values */
PREFIX void spSetLightPosition(int number,Sint32 x,Sint32 y,Sint32 z);

/* Sets the global ambient light value used be every 3D quad or triangle
 * Default: 0.25, 0.25, 0.25*/
PREFIX void spGlobalAmbientLight(Uint32 r,Uint32 g,Uint32 b);

PREFIX void spRectangle3D(Sint32 x,Sint32 y,Sint32 z,Sint32 w,Sint32 h,Uint16 color);


#endif
