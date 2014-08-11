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

/* File: sparrowParticles
 * 
 * SparrowParticles TODO*/
#ifndef _SPARROW_PARTICLES_H
#define _SPARROW_PARTICLES_H

#include "sparrowSprite.h"
#include <SDL.h>

typedef struct spParticleStruct *spParticlePointer;
typedef struct spParticleStruct
{
	Sint32 x,y,z;
	Sint32 dx,dy,dz;
	Sint32 status; //<0 death, >=0 alive
	union {
		Uint16 color;
		Uint32 reserved[9];
	} data;
} spParticle;

typedef struct spParticleBunchStruct *spParticleBunchPointer;
typedef struct spParticleBunchStruct
{
	int count;
	spParticlePointer particle;
	int ( *feedback )( spParticleBunchPointer bunch, Sint32 action, Sint32 extra_data);
	int age;
	spParticleBunchPointer next;
} spParticleBunch;

#define SP_PARTICLE_DRAW 0
#define SP_PARTICLE_UPDATE 1
#define SP_PARTICLE_CLEAN_UP 2

PREFIX spParticleBunchPointer spParticleCreate(int count,int ( *feedback )( spParticleBunchPointer bunch, Sint32 action, Sint32 extra_data),spParticleBunchPointer *addBunch);

PREFIX void spParticleUpdate(spParticleBunchPointer *firstBunch,int steps);

PREFIX void spParticleDraw(spParticleBunchPointer firstBunch);

PREFIX void spParticleDelete(spParticleBunchPointer *firstBunch);

PREFIX spParticleBunchPointer spParticleFromSprite(spSpritePointer sprite,int ( *feedback )( spParticleBunchPointer bunch, Sint32 action, Sint32 extra_data),spParticleBunchPointer* addBunch);

#endif
