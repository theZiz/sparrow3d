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

//A point with the projected coordinates, but without uv-coordinates
typedef struct spPointStruct *spPointPointer;
typedef struct spPointStruct {
  Sint32 x,y,z;
  Sint32 px,py; //projected
} spPoint;

//A point with the projected coordinates and uv-coordinates
typedef struct spTexPointStruct *spTexPointPointer;
typedef struct spTexPointStruct {
  Sint32 x,y,z;
  Sint32 px,py; //projected
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
  int was_drawn; //shows (always), whether the triangle was drawn the last time
  int edge[3]; //the update of the Edges status flag is optional!
} spTriangleS;

typedef struct spModelStruct *spModelPointer;
typedef struct spModelStruct {
  SDL_Surface* texture;
  int pointCount;
  spPointPointer    point; //"normal" points
  int texPointCount;
  spTexPointPointer texPoint; //points with u,v coordinates
  int triangleCount,texTriangleCount;
  spTrianglePointer triangle,texTriangle; //the triangles of the modell
  int edgeCount,texEdgeCount;
  spEdgePointer edge,texEdges; //the edges of the modell
} spModel;


PREFIX void spSetPerspective(float fovyInDegrees, float aspectRatio,
                             float znear, float zfar);

PREFIX Sint32* spGetProjectionMatrix();

PREFIX void spSetCulling(char value);

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

PREFIX int spTriangle3D(Sint32 x1,Sint32 y1,Sint32 z1,
  Sint32 x2,Sint32 y2,Sint32 z2,
  Sint32 x3,Sint32 y3,Sint32 z3,Uint16 color);

PREFIX int spQuad3D(Sint32 x1,Sint32 y1,Sint32 z1,
  Sint32 x2,Sint32 y2,Sint32 z2,
  Sint32 x3,Sint32 y3,Sint32 z3,
  Sint32 x4,Sint32 y4,Sint32 z4,Uint16 color);

PREFIX int spTriangleTex3D(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 u1,Sint32 v1,
  Sint32 x2,Sint32 y2,Sint32 z2,Sint32 u2,Sint32 v2,
  Sint32 x3,Sint32 y3,Sint32 z3,Sint32 u3,Sint32 v3,Uint16 color);

PREFIX int spQuadTex3D(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 u1,Sint32 v1,
  Sint32 x2,Sint32 y2,Sint32 z2,Sint32 u2,Sint32 v2,
  Sint32 x3,Sint32 y3,Sint32 z3,Sint32 u3,Sint32 v3,
  Sint32 x4,Sint32 y4,Sint32 z4,Sint32 u4,Sint32 v4,Uint16 color);
  
PREFIX void spBlit3D(Sint32 x1,Sint32 y1,Sint32 z1,SDL_Surface* surface);

PREFIX void spMesh3D(spModelPointer mesh);

#endif
