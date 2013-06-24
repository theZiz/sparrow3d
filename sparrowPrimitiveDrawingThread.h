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

#include "sparrow3d.h"
#include <SDL.h>

typedef struct {
	Sint32 mode; //0 triangle, 1 texTriangle, 2 perspectiveTexTriangle, 3 rectangle, 4 rectangleBorder, 5 ellipse, 6 ellipseBorder
	//informations for setPixel:
	Uint16* texturePixel;
	Sint32 textureX;
	Sint32 textureScanLine;
	Sint32 textureY;
	Uint8  pattern[8];
	//informations for the "tree of choices" in the different functions (2^5 = 32 states)
	Sint32 zTest;
	Sint32 zSet;
	Sint32 alphaTest;
	Sint32 usePattern;
	Sint32 blending;
	union {
		struct {
			Sint32 x1;
			Sint32 y1;
			Sint32 z1;
			Sint32 x2;
			Sint32 y2;
			Sint32 z2;
			Sint32 x3;
			Sint32 y3;
			Sint32 z3;
			Uint32 color; } triangle;
		struct {
			Sint32 x1;
			Sint32 y1;
			Sint32 z1;
			Sint32 u1;
			Sint32 v1;
			Sint32 x2;
			Sint32 y2;
			Sint32 z2;
			Sint32 u2;
			Sint32 v2;
			Sint32 x3;
			Sint32 y3;
			Sint32 z3;
			Sint32 u3;
			Sint32 v3;
			Uint32 color; } texTriangle;
		struct {
			Sint32 x1;
			Sint32 y1;
			Sint32 z1;
			Sint32 u1;
			Sint32 v1;
			Sint32 w1;
			Sint32 x2;
			Sint32 y2;
			Sint32 z2;
			Sint32 u2;
			Sint32 v2;
			Sint32 w2;
			Sint32 x3;
			Sint32 y3;
			Sint32 z3;
			Sint32 u3;
			Sint32 v3;
			Sint32 w3;
			Uint32 color; } perspectiveTexTriangle;
		struct {
			Sint32 x1;
			Sint32 y1;
			Sint32 x2;
			Sint32 y2;
			Sint32 z;
			Uint32 color; } rectangle;
		struct {
			Sint32 x1;
			Sint32 y1;
			Sint32 x2;
			Sint32 y2;
			Sint32 z;
			Sint32 bx;
			Sint32 by;
			Uint32 color; } rectangleBorder;
		struct {
			Sint32 x1;
			Sint32 y1;
			Sint32 rxl;
			Sint32 rxr;
			Sint32 rx;
			Sint32 ryl;
			Sint32 ryr;
			Sint32 ry;
			Sint32 z;
			Uint32 color; } ellipse;
		struct {
			Sint32 x1;
			Sint32 y1;
			Sint32 rxl;
			Sint32 rxr;
			Sint32 rx;
			Sint32 ryl;
			Sint32 ryr;
			Sint32 ry;
			Sint32 z;
			Sint32 bx;
			Sint32 by;
			Uint32 color; } ellipseBorder;
		struct {
			Sint32 x1;
			Sint32 x3;
			Sint32 y1;
			Sint32 y3;
			Sint32 sx;
			Sint32 sy;
			Sint32 w;
			Sint32 h; } rotozoom;
	} primitive;
} type_spScanLineCache;

int sp_intern_drawing_thread(void* reserved);
