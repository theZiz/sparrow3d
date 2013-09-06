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
 * spModelPointer - a pointer to a new created <spModel> struct
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
 * spModelPointer - a pointer to a new created <spModel> struct
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
