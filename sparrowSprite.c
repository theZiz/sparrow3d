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
#include "sparrowSprite.h"
#include <stdlib.h>
#include <string.h>

PREFIX spSpritePointer spNewSprite(char* name)
{
	spSpritePointer sprite = ( spSpritePointer )malloc( sizeof( spSprite ) );
	sprite->wholeDuration = 0;
	sprite->wholeAge = 0;
	sprite->maxWidth = 0;
	sprite->maxHeight = 0;
	sprite->rotation = 0;
	sprite->zoomX = SP_ONE;
	sprite->zoomY = SP_ONE;
	sprite->firstSub = NULL;
	sprite->momSub = NULL;
	if (name)
	{
		sprite->name = (char*)malloc(strlen(name)+1);
		sprintf(sprite->name,"%s",name);
	}
	else
		sprite->name = NULL;
	sprite->collection = NULL;
	sprite->next = NULL;
	return sprite;
}

PREFIX void spDeleteSprite( spSpritePointer sprite)
{
	if ( sprite == NULL )
		return;
	if (sprite->name)
		free(sprite->name);
	spSubSpritePointer momSub = sprite->firstSub;
	do
	{
		spSubSpritePointer next = momSub->next;
		spDeleteSurface(momSub->surface);
		free( momSub );
		momSub = next;
	}
	while ( momSub != sprite->firstSub );
	if (sprite->collection)
		spRemoveSpriteFromCollection(sprite);
	free( sprite );
}

PREFIX spSubSpritePointer spNewSubSpriteNoTiling( spSpritePointer sprite, SDL_Surface* surface, Sint32 duration )
{
	return spNewSubSpriteWithTiling( sprite, surface, -1, -1, surface->w, surface->h, duration );
}

PREFIX spSubSpritePointer spNewSubSpriteWithTiling( spSpritePointer sprite, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 duration )
{
	spSubSpritePointer sub = ( spSubSpritePointer )malloc( sizeof( spSubSprite ) );
	sub->surface = spCopySurface(surface); //for increasing ref counter!
	sub->sx = sx;
	sub->sy = sy;
	sub->sw = sw;
	sub->sh = sh;
	if (duration <= 0)
		duration = 1;
	sub->duration = duration;
	sub->age = 0;

	sprite->wholeDuration += duration;
	if ( sw > sprite->maxWidth )
		sprite->maxWidth = sw;
	if ( sh > sprite->maxHeight )
		sprite->maxHeight = sh;

	if ( sprite->firstSub )
	{
		sub->next = sprite->firstSub;
		sub->before = sprite->firstSub->before;
		sprite->firstSub->before->next = sub;
		sprite->firstSub->before = sub;
	}
	else
	{
		sub->next = sub;
		sub->before = sub;
		sprite->firstSub = sub;
		sprite->momSub = sub;
	}
  return sub;
}

PREFIX void spNewSubSpriteTilingRow( spSpritePointer sprite, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 sw, Sint32 sh, Sint32 hopw,Sint32 hoph, Sint32 count,Sint32 duration )
{
  int i;
  for (i = 0; i < count; i++)
  {
    spNewSubSpriteWithTiling(sprite,surface,sx,sy,sw,sh,duration);
    sx+=hopw;
    if (sx >= surface->w)
    {
      sx = sx % hopw;
      sy += hoph;
      if (sy+sh > surface->h)
        return;
    }
  }
}


PREFIX void spUpdateSprite( spSpritePointer sprite, Sint32 time )
{
	if ( sprite->momSub == NULL )
		return;
	while ( time > sprite->wholeDuration )
		time -= sprite->wholeDuration;
	while ( time > sprite->momSub->duration - sprite->momSub->age )
	{
		time -= sprite->momSub->duration - sprite->momSub->age;
		sprite->momSub->age = 0;
		sprite->momSub = sprite->momSub->next;
	}
	sprite->momSub->age += time;
}

PREFIX void spSetSpriteRotation( spSpritePointer sprite, Sint32 rotation )
{
	sprite->rotation = rotation;
}

PREFIX void spSetSpriteZoom( spSpritePointer sprite, Sint32 zoomX, Sint32 zoomY )
{
	sprite->zoomX = zoomX;
	sprite->zoomY = zoomY;
}

PREFIX void spDrawSprite( Sint32 x, Sint32 y, Sint32 z, spSpritePointer sprite )
{
	if ( sprite->rotation == 0 &&
			sprite->zoomX == ( SP_ONE ) &&
			sprite->zoomY == ( SP_ONE ) )
	{
		if ( sprite->momSub->sx < 0 )
			spBlitSurface( x, y, z, sprite->momSub->surface );
		else
			spBlitSurfacePart( x, y, z, sprite->momSub->surface, sprite->momSub->sx, sprite->momSub->sy, sprite->momSub->sw, sprite->momSub->sh );
	}
	else
	{
		if ( sprite->momSub->sx < 0 )
			spRotozoomSurface( x, y, z, sprite->momSub->surface, sprite->zoomX, sprite->zoomY, sprite->rotation );
		else
			spRotozoomSurfacePart( x, y, z, sprite->momSub->surface, sprite->momSub->sx, sprite->momSub->sy, sprite->momSub->sw, sprite->momSub->sh, sprite->zoomX, sprite->zoomY, sprite->rotation );
	}
}

PREFIX void spDrawSprite3D( Sint32 x, Sint32 y, Sint32 z, spSpritePointer sprite )
{
	if ( sprite->momSub->sx < 0 )
		spRotozoomSurface3D( x, y, z, sprite->momSub->surface, sprite->zoomX, sprite->zoomY, sprite->rotation );
	else
		spRotozoomSurfacePart3D( x, y, z, sprite->momSub->surface, sprite->momSub->sx, sprite->momSub->sy, sprite->momSub->sw, sprite->momSub->sh, sprite->zoomX, sprite->zoomY, sprite->rotation );
}


PREFIX spSpriteCollectionPointer spNewSpriteCollection()
{
	spSpriteCollectionPointer result = (spSpriteCollectionPointer)malloc(sizeof(spSpriteCollection));
	result->firstSprite = NULL;
	result->active = NULL;
	return result;
}

PREFIX void spDeleteSpriteCollection(spSpriteCollectionPointer collection, int keepSprites)
{
	if (collection == NULL)
		return;
	spSpritePointer sprite = collection->firstSprite;
	while (sprite)
	{
		spSpritePointer next = sprite->next;
		if (keepSprites)
			sprite->collection = NULL;
		else
			spDeleteSprite(sprite);
		sprite = next;
	}
	free(collection);
}

PREFIX void spAddSpriteToCollection(spSpriteCollectionPointer collection, spSpritePointer sprite)
{
	if (collection == NULL || sprite == NULL)
		return;
	if (sprite->collection)
		spRemoveSpriteFromCollection(sprite);
	sprite->collection = collection;
	sprite->next = collection->firstSprite;
	collection->firstSprite = sprite;
	if (collection->active == NULL)
		collection->active = sprite;
}

PREFIX void spRemoveSpriteFromCollection(spSpritePointer sprite)
{
	spSpritePointer before = sprite->collection->firstSprite;
	if (before == sprite)
		sprite->collection->firstSprite = NULL;
	else
	{
		while (before && before->next != sprite)
			before = before->next;
		if (before)
			before->next = sprite->next;
	}
	sprite->collection = NULL;
}

PREFIX void spSelectSprite(spSpriteCollectionPointer collection,char* name)
{
	if (collection == NULL)
		return;
	spSpritePointer sprite = collection->firstSprite;
	while (sprite)
	{
		if (strcmp(sprite->name,name) == 0)
			break;
		sprite = sprite->next;
	}
	if (sprite)
		collection->active = sprite;
}

PREFIX spSpritePointer spActiveSprite(spSpriteCollectionPointer collection)
{
	if (collection == NULL)
		return NULL;
	return collection->active;
}

int spSpriteCollectionGetKeyword(char* name)
{
	if (strcmp(name,"default") == 0)
		return 1;
	if (strcmp(name,"image") == 0)
		return 2;
	if (strcmp(name,"fps") == 0)
		return 3;
	if (strcmp(name,"framesize") == 0)
		return 4;
	if (strcmp(name,"bordersize") == 0)
		return 5;
	if (strcmp(name,"frame") == 0)
		return 6;
	return 0;
}

PREFIX spSpriteCollectionPointer spLoadSpriteCollection(char* filename,SDL_Surface* fallback_surface)
{
	SDL_RWops *file = SDL_RWFromFile(filename, "rt");
	if (!file)
		return NULL;
	spSpriteCollectionPointer collection = spNewSpriteCollection();
	
	//Loading the file line by line
	int end = 0;
	spSpritePointer sprite = NULL;
	char surface_d[1024] = "";
	//border default
	int bw_d = 0;
	int bh_d = 0;
	//frame default
	int fw_d = 0;
	int fh_d = 0;
	int fps_d = 0;
	char sprite_d[1024] = "";
	char surface[1024] = "";
	int bw = bw_d;
	int bh = bh_d;
	int fw = fw_d;
	int fh = fh_d;
	int fps = fps_d;
	while (!end) 
	{
		char line[512];
		end = spReadOneLine(file,line,512);
		//parsing the line
		if (line[0] == '#' || line[0] == ';') {} //comment
		else
		if (line[0] == '[') //new sprite
		{
			int i;
			for (i = 1; line[i]!=']' && line[i]!=0; i++);
			line[i] = 0;
			sprite = spNewSprite(&(line[1]));
			spAddSpriteToCollection(collection,sprite);
			sprintf(surface,"%s",surface_d);
			bw = bw_d;
			bh = bh_d;
			fw = fw_d;
			fh = fh_d;
			fps = fps_d;
		}
		else //some keywords
		{
			//searching the '=', ' ' or \0.
			int i;
			for (i = 1; line[i]!='=' && line[i]!=' ' && line[i]!=0; i++);
			if (line[i]==0) //hm, not good...
				continue;
			int keyword = 0;
			if (line[i]=='=')
			{
				line[i] = 0;
				keyword = spSpriteCollectionGetKeyword(line);
				line[i] = '=';
			}
			else // ' '
			{
				line[i] = 0;
				keyword = spSpriteCollectionGetKeyword(line);
				line[i] =' ';
				for (i++;line[i]!='=' && line[i]!=0; i++);
				if (line[i]==0) //hm, not good...
					continue;
			}
			for (i++;line[i]==' '; i++);
			char* value = &(line[i]);
			int j;
			for (j = strlen(value)-1;value[j]==' ' && j>=0;j--);
			value[j+1] = 0;
			int x,y,n;
			switch (keyword)
			{
				case 1: //"default"
					sprintf(sprite_d,"%s",value);
					break;
				case 2: //"image"
					if (sprite)
						sprintf(surface,"%s",value);
					else
						sprintf(surface_d,"%s",value);
					break;
				case 3: //fps
					if (sprite)
						fps = atoi(value);
					else
						fps_d = atoi(value);
					break;
				case 4: //framesize
					if (sprite)
						fw = atoi(value);
					else
						fw_d = atoi(value);
					for (i++;line[i]!=',' && line[i]!=0; i++);
					if (line[i] == 0)
						continue;
					value = &(line[i+1]);
					if (sprite)
						fh = atoi(value);
					else
						fh_d = atoi(value);
					break;
				case 5: //bordersize
					if (sprite)
						bw = atoi(value);
					else
						bw_d = atoi(value);
					for (i++;line[i]!=',' && line[i]!=0; i++);
					if (line[i] == 0)
						continue;
					value = &(line[i+1]);
					if (sprite)
						bh = atoi(value);
					else
						bh_d = atoi(value);
					break;
				case 6: //frame
					if (!sprite)
						break;
					x = atoi(value);
					for (i++;line[i]!=',' && line[i]!=0; i++);
					if (line[i] == 0)
						continue;
					value = &(line[i+1]);
					y = atoi(value);
					for (i++;line[i]!=',' && line[i]!=0; i++);
					
					SDL_Surface* s = spLoadSurface( surface );
					if (s == NULL)
						s = fallback_surface;
					if (line[i] == 0)
						spNewSubSpriteWithTiling(sprite,s,x+(bw-fw)/2,y+(bh-fh)/2,fw,fh,1000/(fps>0?fps:1));
					else
					{
						value = &(line[i+1]);
						n = atoi(value);						
						if (n > 0)
							spNewSubSpriteTilingRow(sprite,s,x+(bw-fw)/2,y+(bh-fh)/2,fw,fh,bw,bh,n,1000/(fps>0?fps:1));
					}
					//We loaded the surface one time more, than we will it delete later, so lets decrease the ref counter:
					spDeleteSurface( s );
					break;
			}
		}
	}
	spSelectSprite(collection,sprite_d);
	SDL_RWclose(file);
	return collection;
}
