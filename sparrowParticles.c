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

#include "sparrowParticles.h"

PREFIX spParticleBunchPointer spParticleCreate(int count,int ( *feedback )( spParticleBunchPointer bunch, Sint32 action, Sint32 extra_data),spParticleBunchPointer *addBunch)
{
	spParticleBunchPointer bunch = (spParticleBunchPointer)malloc(sizeof(spParticleBunch));
	bunch->count = count;
	bunch->particle = (spParticlePointer)malloc(sizeof(spParticle)*count);
	memset(bunch->particle,0,sizeof(spParticle)*count);
	bunch->feedback = feedback;
	bunch->age = 0;
	if (addBunch)
	{
		bunch->next = *addBunch;
		*addBunch = bunch;
	}
	else
		bunch->next = NULL;
	return bunch;
}

PREFIX void spParticleUpdate(spParticleBunchPointer *firstBunch,int steps)
{
	if (firstBunch == NULL)
		return;
	spParticleBunchPointer bunch = *firstBunch;
	spParticleBunchPointer prev = NULL;
	while (bunch)
	{
		spParticleBunchPointer next = bunch->next;
		int res = 0;
		bunch->age += steps;
		if (bunch->feedback)
			res = bunch->feedback(bunch,SP_PARTICLE_UPDATE,steps);
		if (res) //deleting the bunch
		{
			bunch->feedback(bunch,SP_PARTICLE_CLEAN_UP,0);
			if (prev == NULL)
				*firstBunch = next;
			else
				prev->next = next;
			free(bunch->particle);
			free(bunch);
		}
		else
			prev = bunch;
		bunch = next;
	}
}

PREFIX void spParticleDraw(spParticleBunchPointer firstBunch)
{
	if (firstBunch == NULL)
		return;
	spParticleBunchPointer bunch = firstBunch;
	while (bunch)
	{
		if (bunch->feedback)
			bunch->feedback(bunch,SP_PARTICLE_DRAW,0);
		bunch = bunch->next;
	}
}

PREFIX void spParticleDelete(spParticleBunchPointer *firstBunch)
{
	if (firstBunch == NULL)
		return;
	while (*firstBunch)
	{
		spParticleBunchPointer next = (*firstBunch)->next;
		if ((*firstBunch)->feedback)
			(*firstBunch)->feedback(*firstBunch,SP_PARTICLE_CLEAN_UP,0);
		free((*firstBunch)->particle);
		free(*firstBunch);
		*firstBunch = next;
	}
}

PREFIX spParticleBunchPointer spParticleFromSprite(spSpritePointer sprite,int ( *feedback )( spParticleBunchPointer bunch, Sint32 action, Sint32 extra_data),spParticleBunchPointer* addBunch)
{
	spParticleBunchPointer bunch = spParticleCreate(sprite->momSub->pixelCount,feedback,addBunch);
	SDL_LockSurface(sprite->momSub->surface);
	Uint16* pixel = (Uint16*)sprite->momSub->surface->pixels;
	int line = sprite->momSub->surface->pitch/sprite->momSub->surface->format->BytesPerPixel;
	int x,y,c = 0;
	for (x = sprite->momSub->sx; x < sprite->momSub->sx+sprite->momSub->sw; x++)
		for (y = sprite->momSub->sy; y < sprite->momSub->sy+sprite->momSub->sh; y++)
			if (pixel[x+y*line] != SP_ALPHA_COLOR)
			{
				bunch->particle[c].x = x-sprite->momSub->sx;
				bunch->particle[c].y = y-sprite->momSub->sy;
				bunch->particle[c].data.color = pixel[x+y*line];
				bunch->particle[c].dx = (rand() & 131071) - SP_ONE; // mod SP_ONE
				bunch->particle[c].dy = (rand() & 131071) - SP_ONE; // mod SP_ONE
				c++;
			}
	SDL_UnlockSurface(sprite->momSub->surface);	
	return bunch;
}
