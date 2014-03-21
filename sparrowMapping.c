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

#include "sparrowMapping.h"
#include "sparrowDefines.h"
#include "sparrowCore.h"
#include "sparrowFile.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char __spMapError[] = "None";

struct
{
	char* caption;
	char* name;
	int active;
	int poolButton;
} __spMapButton[SP_MAPPING_MAX];

#define SP_MAPPING_POOL_MAX 19
struct
{
	int active;
	char* realCaption;
} __spMapPool[SP_MAPPING_POOL_MAX]; //biggest button number, however only on gp2x

int __spMapIsInitialized = 0;
int __spMapStrategy = SP_MAPPING_NONE;

PREFIX void spInitMapping()
{
	if (__spMapIsInitialized)
		return;
	__spMapIsInitialized = 1;
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		__spMapButton[i].active = 0;
	for (i = 0; i < SP_MAPPING_POOL_MAX; i++)
		__spMapPool[i].active = 0;
};

PREFIX void spMapClean()
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[i].active)
		{
			__spMapButton[i].active = 0;
			free(__spMapButton[i].caption);
			free(__spMapButton[i].name);
		}
	for (i = 0; i < SP_MAPPING_POOL_MAX; i++)
		__spMapPool[i].active = 0;
}

PREFIX void spMapPoolAdd(int button_id,char* caption)
{
	if (button_id < 0 || button_id >= SP_MAPPING_POOL_MAX)
		return;
	__spMapPool[button_id].active = 1;
	__spMapPool[button_id].realCaption = (char*)malloc(strlen(caption)+1);
	sprintf(__spMapPool[button_id].realCaption,"%s",caption);
}

PREFIX void spMapSetStrategy(int strategy)
{
	__spMapStrategy = strategy;
}

char* __spMapCollisionCaption = NULL;
char* __spMapCollisionPool = NULL;

PREFIX int spMapChange(int id,int poolButton)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return;
	if (poolButton < 0 || poolButton >= SP_MAPPING_POOL_MAX)
		return;
	if (!__spMapButton[id].active)
		return;
	if (!__spMapPool[poolButton].active) //not in pool!
		return;
	int i;
	switch (__spMapStrategy)
	{
		case SP_MAPPING_SWITCH:
			//Searching, whether the button is already used.
			for (i = 0; i < SP_MAPPING_MAX; i++)
				if (__spMapButton[i].active && __spMapButton[i].poolButton == poolButton)
					break;
			if (i != SP_MAPPING_MAX) //found
				__spMapButton[i].poolButton = __spMapButton[id].poolButton;
			__spMapButton[id].poolButton = poolButton;
			break;
		case SP_MAPPING_OTHER_INVALID:
			//Searching, whether the button is already used.
			for (i = 0; i < SP_MAPPING_MAX; i++)
				if (__spMapButton[i].active && __spMapButton[i].poolButton == poolButton)
					__spMapButton[i].poolButton = -1;
			__spMapButton[id].poolButton = poolButton;
			break;
		case SP_MAPPING_CANCEL:
			//Searching, whether the button is already used.
			for (i = 0; i < SP_MAPPING_MAX; i++)
				if (__spMapButton[i].active && __spMapButton[i].poolButton == poolButton)
					break;
			if (i == SP_MAPPING_MAX) //no break!
				__spMapButton[id].poolButton = poolButton;
			else
			{
				__spMapCollisionCaption = __spMapButton[i].caption;
				__spMapCollisionPool = __spMapPool[poolButton].realCaption;
				return 1;
			}
			break;
		default:
			__spMapButton[id].poolButton = poolButton;
	}
	return 0;
}

PREFIX void spMapChangeNextInPool(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return;
	if (!__spMapButton[id].active)
		return;
	int poolButton = __spMapButton[id].poolButton;
	do
	{
		poolButton = (poolButton+1)%SP_MAPPING_POOL_MAX;
		while (!__spMapPool[poolButton].active) //not in pool
			poolButton = (poolButton+1)%SP_MAPPING_POOL_MAX;
	}
	while (spMapChange(id,poolButton));
}

PREFIX void spMapChangePreviousInPool(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return;
	if (!__spMapButton[id].active)
		return;
	int poolButton = __spMapButton[id].poolButton;
	do
	{
		poolButton = (poolButton+SP_MAPPING_POOL_MAX-1)%SP_MAPPING_POOL_MAX;
		while (!__spMapPool[poolButton].active) //not in pool
			poolButton = (poolButton+SP_MAPPING_POOL_MAX-1)%SP_MAPPING_POOL_MAX;
	}
	while (spMapChange(id,poolButton));
}

PREFIX void spMapButtonAdd(int id,char* name,char* caption,int poolButton)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return;
	if (poolButton < 0 || poolButton >= SP_MAPPING_POOL_MAX)
		return;
	if (__spMapButton[id].active) //Already added
		return;
	if (!__spMapPool[poolButton].active) //not in pool!
		return;
	__spMapButton[id].active = 1;
	__spMapButton[id].poolButton = poolButton;
	__spMapButton[id].caption = (char*)malloc(strlen(caption)+1);
	sprintf(__spMapButton[id].caption,"%s",caption);
	__spMapButton[id].name = (char*)malloc(strlen(name)+1);
	sprintf(__spMapButton[id].name,"%s",name);
}

PREFIX int spMapGetByID(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return 0;
	if (!__spMapButton[id].active)
		return 0;
	if (__spMapButton[id].poolButton < 0)
		return 0;
	return spGetInput()->button[__spMapButton[id].poolButton];
}

PREFIX void spMapSetByID(int id, int value)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return;
	if (!__spMapButton[id].active)
		return;
	if (__spMapButton[id].poolButton < 0)
		return;
	spGetInput()->button[__spMapButton[id].poolButton] = value;
}

PREFIX int spMapGetByName(char* name)
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[i].active && __spMapButton[i].poolButton >= 0 && strcmp(__spMapButton[i].name,name) == 0)
			return spGetInput()->button[__spMapButton[i].poolButton];
	return 0;
}

PREFIX void spMapSetByName(char* name, int value)
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[i].active && __spMapButton[i].poolButton >= 0 && strcmp(__spMapButton[i].name,name) == 0)
		{
			spGetInput()->button[__spMapButton[i].poolButton] = value;
			return;
		}
}

PREFIX char* spMapCaptionByID(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return __spMapError;
	if (!__spMapButton[id].active)
		return __spMapError;
	return __spMapButton[id].caption;
}

PREFIX char* spMapCaptionByName(char* name)
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[i].active && strcmp(__spMapButton[i].name,name) == 0)
			return __spMapButton[i].caption;
	return __spMapError;
}

PREFIX char* spMapButtonByID(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return __spMapError;
	if (!__spMapButton[id].active)
		return __spMapError;
	if (__spMapButton[id].poolButton < 0)
		return __spMapError;
	return __spMapPool[__spMapButton[id].poolButton].realCaption;
}

PREFIX char* spMapButtonByName(char* name)
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[i].active && __spMapButton[i].poolButton >= 0 && strcmp(__spMapButton[i].name,name) == 0)
			return __spMapPool[__spMapButton[i].poolButton].realCaption;
	return __spMapError;
}

int __spMapChangingID = -1;

PREFIX void spMapStartChangeByID(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return;
	if (__spMapChangingID >= 0)
		return;
	int i;
	for (i = 0; i < SP_MAPPING_POOL_MAX; i++)
		if (__spMapPool[i].active)
			spGetInput()->button[i] = 0;
	__spMapChangingID = id;
}

PREFIX void spMapStartChangeByName(char* name)
{
	int i,id = -1;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[i].active && __spMapButton[i].poolButton >= 0 && strcmp(__spMapButton[i].name,name) == 0)
		{
			spMapStartChangeByID(id);
			return;
		}
}

PREFIX int spMapContinueChange()
{
	if (__spMapChangingID < 0)
		return -1;
	int i;
	for (i = 0; i < SP_MAPPING_POOL_MAX; i++)
		if (__spMapPool[i].active && spGetInput()->button[i])
		{
			int r = spMapChange(__spMapChangingID,i);
			spGetInput()->button[i] = 0;
			__spMapChangingID = -1;
			if (r == 1)
				return 2;
			return 1;
		}
	return 0;
}

PREFIX char* spMapLastCollisionCaption()
{
	return __spMapCollisionCaption;
}
	
PREFIX char* spMapLastCollisionPool()
{
	return __spMapCollisionPool;
}

PREFIX void spMapCancelChange()
{
	__spMapChangingID = -1;
}

PREFIX void spMapLoad(char* subfolder,char* filename)
{
	printf("Load mapping...\n");
	spConfigPointer config = spConfigRead(filename,subfolder);
	//For every mapping changing the config:
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[i].active)
			__spMapButton[i].poolButton = spConfigGetInt(config,__spMapButton[i].name,__spMapButton[i].poolButton);
	spConfigFree(config);
}

PREFIX void spMapSave(char* subfolder,char* filename)
{
	printf("Save mapping...\n");
	spConfigPointer config = spConfigRead(filename,subfolder);
	//For every mapping changing the config:
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[i].active)
			spConfigSetInt(config,__spMapButton[i].name,__spMapButton[i].poolButton);
	spConfigWrite(config);
	spConfigFree(config);
}
