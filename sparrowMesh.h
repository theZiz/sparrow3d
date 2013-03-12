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

/* sparrowMesh is for loading Meshes (definied at sparrowRenderer.h).
 * In fact it is just an extension of sparrowRenderer, but maybe later
 * with more loader for more formats or a skeleton format */
#ifndef _SPARROW_MESH_H
#define _SPARROW_MESH_H

#include <SDL.h>
#include "sparrowDefines.h"
#include "sparrow3d.h"
#include "sparrowRenderer.h"

/* Loads the object file "name". Every face in a sparrow3d mesh file has
 * a color (no material or such a thing). If you don't wont a color, set
 * it to white (65535), it will be multiplied with the color of the
 * texture and the result will be the texture. However, set texture to
 * any surface, that shall be the texture for the mesh. As you can see:
 * One mesh can only have on texture. If you want different textures,
 * split your object files before. */
PREFIX spModelPointer spMeshLoadObj( char* name, SDL_Surface* texture, Uint16 color );

/* same like spMeshLoadObj, but because of the buggyness of spScale, you
 * are able to scale the mesh here at loading more accurate. */
PREFIX spModelPointer spMeshLoadObjSize( char* name, SDL_Surface* texture, Uint16 color, Sint32 size );

/* Deletes the mesh */
PREFIX void spMeshDelete( spModelPointer mesh );

#endif
