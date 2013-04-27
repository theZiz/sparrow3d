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

/* File: sparrowMesh
 * 
 * SparrowMesh is for loading meshes (definied at <sparrowRenderer>.h).
 * In fact it is just an extension of <sparrowRenderer>, but maybe later
 * with more loaders for more formats or a skeleton animation format.*/
#ifndef _SPARROW_MESH_H
#define _SPARROW_MESH_H

#include <SDL.h>
#include "sparrowDefines.h"
#include "sparrow3d.h"
#include "sparrowRenderer.h"

/* Function: spMeshLoadObj
 * 
 * Loads a wavefront object file (.obj).
 * 
 * Parameters:
 * name - the filename of the obj file to load
 * texture - SDL_Surface*, which will be saved as texture and used if the obj
 * will be drawn. At the moment only one texture per mesh is possible! All
 * texture coordinates will be refered to this texture. If you want more
 * textures split your meshfile in submeshes with your 3d modelling program.
 * color - every face in a sparrow3d mesh file has a color (no material or such
 * a thing). If you don't wont a color, set it to white (65535), it will be
 * multiplied with the color of the texture and the result will be the texture.
 * 
 * Returns:
 * spModelPointer - a pointer to a new created <spModelStruct> struct
 * 
 * See Also:
 * <spMeshLoadObjSize>*/
PREFIX spModelPointer spMeshLoadObj( char* name, SDL_Surface* texture, Uint16 color );

/* Function: spMeshLoadObjSize
 * 
 * Loads a wavefront object file (.obj) like <spMeshLoadObj>, but with a fourth
 * parameter for resizing. This functions uses the more accurate float
 * multiplication, which is slower but fine at loading time.
 * 
 * Parameters:
 * name - the filename of the obj file to load
 * texture - SDL_Surface*, which will be saved as texture and used if the obj
 * will be drawn. At the moment only one texture per mesh is possible! All
 * texture coordinates will be refered to this texture. If you want more
 * textures split your meshfile in submeshes with your 3d modelling program.
 * color - every face in a sparrow3d mesh file has a color (no material or such
 * a thing). If you don't wont a color, set it to white (65535), it will be
 * multiplied with the color of the texture and the result will be the texture.
 * size - a fixed point factor for the mesh. size = SP_ONE does the same like
 * <spMeshLoadObj>
 * 
 * Returns:
 * spModelPointer - a pointer to a new created <spModelStruct> struct
 * 
 * See Also:
 * <spMeshLoadObj>*/
PREFIX spModelPointer spMeshLoadObjSize( char* name, SDL_Surface* texture, Uint16 color, Sint32 size );

/* Function: spMeshDelete
 * 
 * Deletes a mesh
 * 
 * Parameters:
 * mesh - the mesh to be deleted and freed. The texture is not freed (!)*/
PREFIX void spMeshDelete( spModelPointer mesh );

#endif
