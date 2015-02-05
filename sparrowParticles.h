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
 * SparrowParticles is a very basic, generic attempt for particles.*/
#ifndef _SPARROW_PARTICLES_H
#define _SPARROW_PARTICLES_H

#include "sparrowSprite.h"
#include <SDL.h>
/* type: spParticle
 * 
 * Struct for one partcle.
 * 
 * Variables:
 * x,y,z (Sint32) - position of the particle
 * dx,dy,dz (Sint32) - velocity of the particle
 * status (Sint32) - status of the particle. < 0 means death, >= 0 means alive
 * data (union) - first element is color, but can be used in any way with the
 * 9 reserved fields.*/
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

/* type spParticleBunch
 * 
 * A bunch of particles.
 * 
 * Variables:
 * count (int) - count of the particles in this bunch
 * feedback (function) - callback function for this bunch
 * age (int) - the age in ms of this bunch
 * next (pointer of spParticleBunch) - next element in a bunch of bunches*/
typedef struct spParticleBunchStruct *spParticleBunchPointer;
typedef struct spParticleBunchStruct
{
	int count;
	spParticlePointer particle;
	int ( *feedback )( spParticleBunchPointer bunch, Sint32 action, Sint32 extra_data);
	int age;
	spParticleBunchPointer next;
} spParticleBunch;

/* Defines: Events for the particle bunch update function
 * 
 * SP_PARTICLE_DRAW - the particles shall be drawn
 * SP_PARTICLE_UPDATE - the particles shall be updated
 * SP_PARTICLE_CLEAN_UP - if the particles e.g. allocated memory, free it NOW*/
#define SP_PARTICLE_DRAW 0
#define SP_PARTICLE_UPDATE 1
#define SP_PARTICLE_CLEAN_UP 2

/* Function: spParticleCreate
 * 
 * Creates a bunch of a fixed number of particles. In every update or draw step
 * the feedback function will be called ONCE for all particles of the bunch
 * (which is faster than doing this for every particle).
 * 
 * Parameters:
 * count - number of particles. It is not possible to add particles later, but
 * to create a new particle bunch and add it to this bunch (see last parameter)
 * feedback - a feedback function, which shall have the parameters
 * spParticleBunchPointer bunch, Sint32 action and Sint32 extra_data. The bunch
 * is the bunch returned by this function, action tells the feedback function,
 * what to do (see also <Events for the particle bunch update function>) and
 * extra_data gives some extra data for some actions, e.g. the time since the
 * last frame in ms if the action is SP_PARTICLE_UPDATE.
 * addBunch - if not NULL, this new created particle bunch will be added to the
 * given bunch. Usefull to keep your particle bunches in meta particle bunches
 * and to call <spParticleUpdate>, <spParticleDraw> and <spParticleDelete> just
 * once.
 * 
 * Returns:
 * Pointer of spParticleBunch - pointer to the created particle bunch, but
 * be carful! The data are unset! It is up to you to fill the bunch with
 * positions, velocities, colours and other metadata (see also <spParticle>)!*/
PREFIX spParticleBunchPointer spParticleCreate(int count,int ( *feedback )( spParticleBunchPointer bunch, Sint32 action, Sint32 extra_data),spParticleBunchPointer *addBunch);

/* Function: spParticleUpdate
 * 
 * Updates all particles of the particles bunch and connected bunches. It does
 * this by calling the feedback function of the bunches with the action
 * SP_PARTICLE_UPDATE!
 * 
 * Parameters:
 * firstBunch - the first bunch to update it's particles
 * steps - the time since the last frame to pass to the feedback function*/
PREFIX void spParticleUpdate(spParticleBunchPointer *firstBunch,int steps);

/* Function: spParticleDraw
 * 
 * Draws all particles of the particles bunch and connected bunches. It does
 * this by calling the feedback function of the bunches with the action
 * SP_PARTICLE_DRAW!
 * 
 * Parameters:
 * firstBunch - the first bunch to update it's particles*/
PREFIX void spParticleDraw(spParticleBunchPointer firstBunch);

/* Function: spParticleDelete
 * 
 * Deletes all particles of the particles bunch and connected bunches. Right
 * before deleting, it calls the feedback function one last time with the action
 * SP_PARTICLE_CLEAN_UP, which tells the feedback function clean up stuff if
 * necessary.
 * 
 * Parameters:
 * firstBunch - the first bunch to update it's particles*/
PREFIX void spParticleDelete(spParticleBunchPointer *firstBunch);

/* Function: spParticleFromSprite
 * 
 * This special function creates a particle bunch out of a sprite. Useful for
 * sprites, which shall explode. Every particle stand for a pixel of the sprite.
 * 
 * Parameters:
 * sprite - spSpritePointer to use
 * feedback - feedback function for this bunch. See alo <spParticleCreate>.
 * addBunch - optional meta bunch to add this bunch to. See also
 * <spParticleCreate>.
 * 
 * Returns:
 * Pointer of spParticleBunch - pointer to the created particle bunch. But be
 * careful! The positions and colours of the sprite are set, furthermore an
 * "exploding" velocity, but keep in mind to scale this data to your system. ;)*/
PREFIX spParticleBunchPointer spParticleFromSprite(spSpritePointer sprite,int ( *feedback )( spParticleBunchPointer bunch, Sint32 action, Sint32 extra_data),spParticleBunchPointer* addBunch);

#endif
