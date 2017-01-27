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

#define SP_MAPPING_POOL_MAX 323

char __spMapError[] = "None";
int __spMapDesktopHack = 0;
int __spMapDesktopButton[SP_MAPPING_POOL_MAX];
int __spMapSet = 0;

struct
{
	char* caption;
	char* name;
	int active;
	int poolButton;
} __spMapButton[SP_MAPPING_SET_MAX][SP_MAPPING_MAX];

struct
{
	int active;
	char* realCaption;
} __spMapPool[SP_MAPPING_SET_MAX][SP_MAPPING_POOL_MAX];

int __spMapIsInitialized = 0;
int __spMapStrategy[SP_MAPPING_SET_MAX] = {SP_MAPPING_NONE};

const unsigned char __spMapOver255[87] = {
	  1,   2,   3,   4,   5,   6,   7,  10,  11,  14,
	 15,  16,  17,  18,  20,  21,  22,  23,  24,  25,
	 26,  28,  29,  31,  37,  65,  66,  67,  68,  69,
	 70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
	 90, 123, 124, 125, 126, 128, 129, 130, 131, 132,
	133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
	143, 144, 145, 146, 147, 148, 149, 150, 151, 152,
	153, 154, 155, 156, 157, 158, 159
};

PREFIX unsigned char spMapSDLKeyToChar(SDLKey key)
{
	if (key == '<')
		return __spMapOver255[83];
	if (key == '>')
		return __spMapOver255[84];
	if (key == '^')
		return __spMapOver255[85];
	if (key == 'v')
		return __spMapOver255[86];
	if (key < 256)
		return (unsigned char)key;
	return __spMapOver255[key-256];
}

PREFIX void spInitMapping( void )
{
	if (__spMapIsInitialized)
		return;
	__spMapIsInitialized = 1;
	int i,j;
	for (j = 0; j < SP_MAPPING_SET_MAX; j++)
	{
		for (i = 0; i < SP_MAPPING_MAX; i++)
			__spMapButton[j][i].active = 0;
		for (i = 0; i < SP_MAPPING_POOL_MAX; i++)
			__spMapPool[j][i].active = 0;
	}
	__spMapSet = 0;
};

PREFIX void spMapClean( void )
{
	int i,j;
	for (j = 0; j < SP_MAPPING_SET_MAX; j++)
	{
		for (i = 0; i < SP_MAPPING_MAX; i++)
			if (__spMapButton[j][i].active)
			{
				__spMapButton[j][i].active = 0;
				free(__spMapButton[j][i].caption);
				free(__spMapButton[j][i].name);
			}
		for (i = 0; i < SP_MAPPING_POOL_MAX; i++)
			__spMapPool[j][i].active = 0;
	}
}

PREFIX void spMapPoolAdd(int button_id,char* caption)
{
	if (button_id < 0 || button_id >= SP_MAPPING_POOL_MAX)
		return;
	__spMapPool[__spMapSet][button_id].active = 1;
	__spMapPool[__spMapSet][button_id].realCaption = (char*)malloc(strlen(caption)+1);
	sprintf(__spMapPool[__spMapSet][button_id].realCaption,"%s",caption);
}

PREFIX void spMapPoolAddForDesktopHack()
{
	int i;
	unsigned char buffer[4] = "[ ]";
	for (i = 0; i < SP_MAPPING_POOL_MAX; i++)
	{
		buffer[1] = spMapSDLKeyToChar(i);
		spMapPoolAdd(i,buffer);
	}
}

PREFIX void spMapSetStrategy(int strategy)
{
	__spMapStrategy[__spMapSet] = strategy;
}

char* __spMapCollisionCaption[SP_MAPPING_SET_MAX] = {NULL};
char* __spMapCollisionPool[SP_MAPPING_SET_MAX] = {NULL};

PREFIX int spMapChange(int id,int poolButton)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return -1;
	if (poolButton < 0 || poolButton >= SP_MAPPING_POOL_MAX)
		return -1;
	if (!__spMapButton[__spMapSet][id].active)
		return -1;
	if (!__spMapPool[__spMapSet][poolButton].active) //not in pool!
		return -1;
	int i;
	switch (__spMapStrategy[__spMapSet])
	{
		case SP_MAPPING_SWITCH:
			//Searching, whether the button is already used.
			for (i = 0; i < SP_MAPPING_MAX; i++)
				if (__spMapButton[__spMapSet][i].active && __spMapButton[__spMapSet][i].poolButton == poolButton)
					break;
			if (i != SP_MAPPING_MAX) //found
				__spMapButton[__spMapSet][i].poolButton = __spMapButton[__spMapSet][id].poolButton;
			__spMapButton[__spMapSet][id].poolButton = poolButton;
			break;
		case SP_MAPPING_OTHER_INVALID:
			//Searching, whether the button is already used.
			for (i = 0; i < SP_MAPPING_MAX; i++)
				if (__spMapButton[__spMapSet][i].active && __spMapButton[__spMapSet][i].poolButton == poolButton)
					__spMapButton[__spMapSet][i].poolButton = -1;
			__spMapButton[__spMapSet][id].poolButton = poolButton;
			break;
		case SP_MAPPING_CANCEL:
			//Searching, whether the button is already used.
			for (i = 0; i < SP_MAPPING_MAX; i++)
				if (__spMapButton[__spMapSet][i].active && __spMapButton[__spMapSet][i].poolButton == poolButton)
					break;
			if (i == SP_MAPPING_MAX) //no break!
				__spMapButton[__spMapSet][id].poolButton = poolButton;
			else
			{
				__spMapCollisionCaption[__spMapSet] = __spMapButton[__spMapSet][i].caption;
				__spMapCollisionPool[__spMapSet] = __spMapPool[__spMapSet][poolButton].realCaption;
				return 1;
			}
			break;
		default:
			__spMapButton[__spMapSet][id].poolButton = poolButton;
	}
	return 0;
}

PREFIX void spMapChangeNextInPool(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return;
	if (!__spMapButton[__spMapSet][id].active)
		return;
	int poolButton = __spMapButton[__spMapSet][id].poolButton;
	do
	{
		poolButton = (poolButton+1)%SP_MAPPING_POOL_MAX;
		while (!__spMapPool[__spMapSet][poolButton].active) //not in pool
			poolButton = (poolButton+1)%SP_MAPPING_POOL_MAX;
	}
	while (spMapChange(id,poolButton));
}

PREFIX void spMapChangePreviousInPool(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return;
	if (!__spMapButton[__spMapSet][id].active)
		return;
	int poolButton = __spMapButton[__spMapSet][id].poolButton;
	do
	{
		poolButton = (poolButton+SP_MAPPING_POOL_MAX-1)%SP_MAPPING_POOL_MAX;
		while (!__spMapPool[__spMapSet][poolButton].active) //not in pool
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
	if (__spMapButton[__spMapSet][id].active) //Already added
		return;
	if (!__spMapPool[__spMapSet][poolButton].active) //not in pool!
		return;
	__spMapButton[__spMapSet][id].active = 1;
	__spMapButton[__spMapSet][id].poolButton = poolButton;
	__spMapButton[__spMapSet][id].caption = (char*)malloc(strlen(caption)+1);
	sprintf(__spMapButton[__spMapSet][id].caption,"%s",caption);
	__spMapButton[__spMapSet][id].name = (char*)malloc(strlen(name)+1);
	sprintf(__spMapButton[__spMapSet][id].name,"%s",name);
}

PREFIX int spMapGetByID(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return 0;
	if (!__spMapButton[__spMapSet][id].active)
		return 0;
	if (__spMapButton[__spMapSet][id].poolButton < 0)
		return 0;
	if (__spMapDesktopHack)
		return __spMapDesktopButton[__spMapButton[__spMapSet][id].poolButton];
	return spGetInput()->button[__spMapButton[__spMapSet][id].poolButton];
}

PREFIX void spMapSetByID(int id, int value)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return;
	if (!__spMapButton[__spMapSet][id].active)
		return;
	if (__spMapButton[__spMapSet][id].poolButton < 0)
		return;
	if (__spMapDesktopHack)
		__spMapDesktopButton[__spMapButton[__spMapSet][id].poolButton] = value;
	else
		spGetInput()->button[__spMapButton[__spMapSet][id].poolButton] = value;
}

PREFIX int spMapGetByName(char* name)
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[__spMapSet][i].active && __spMapButton[__spMapSet][i].poolButton >= 0 && strcmp(__spMapButton[__spMapSet][i].name,name) == 0)
		{
			if (__spMapDesktopHack)
				return __spMapDesktopButton[__spMapButton[__spMapSet][i].poolButton];
			return spGetInput()->button[__spMapButton[__spMapSet][i].poolButton];
		}
	return 0;
}

PREFIX void spMapSetByName(char* name, int value)
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[__spMapSet][i].active && __spMapButton[__spMapSet][i].poolButton >= 0 && strcmp(__spMapButton[__spMapSet][i].name,name) == 0)
		{
			if (__spMapDesktopHack)
				__spMapDesktopButton[__spMapButton[__spMapSet][i].poolButton] = value;
			else
				spGetInput()->button[__spMapButton[__spMapSet][i].poolButton] = value;
			return;
		}
}

PREFIX char* spMapCaptionByID(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return __spMapError;
	if (!__spMapButton[__spMapSet][id].active)
		return __spMapError;
	return __spMapButton[__spMapSet][id].caption;
}

PREFIX char* spMapCaptionByName(char* name)
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[__spMapSet][i].active && strcmp(__spMapButton[__spMapSet][i].name,name) == 0)
			return __spMapButton[__spMapSet][i].caption;
	return __spMapError;
}

PREFIX char* spMapButtonByID(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return __spMapError;
	if (!__spMapButton[__spMapSet][id].active)
		return __spMapError;
	if (__spMapButton[__spMapSet][id].poolButton < 0)
		return __spMapError;
	return __spMapPool[__spMapSet][__spMapButton[__spMapSet][id].poolButton].realCaption;
}

PREFIX char* spMapButtonByName(char* name)
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[__spMapSet][i].active && __spMapButton[__spMapSet][i].poolButton >= 0 && strcmp(__spMapButton[__spMapSet][i].name,name) == 0)
			return __spMapPool[__spMapSet][__spMapButton[__spMapSet][i].poolButton].realCaption;
	return __spMapError;
}

PREFIX int spMapPoolByID(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return -1;
	if (!__spMapButton[__spMapSet][id].active)
		return -1;
	if (__spMapButton[__spMapSet][id].poolButton < 0)
		return -1;
	return __spMapButton[__spMapSet][id].poolButton;
}

PREFIX int spMapPoolByName(char* name)
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[__spMapSet][i].active && __spMapButton[__spMapSet][i].poolButton >= 0 && strcmp(__spMapButton[__spMapSet][i].name,name) == 0)
			return __spMapButton[__spMapSet][i].poolButton;
	return -1;
}

PREFIX char* spMapNameByID(int id)
{
	if (id < 0 || id >= SP_MAPPING_MAX)
		return __spMapError;
	if (!__spMapButton[__spMapSet][id].active)
		return __spMapError;
	return __spMapButton[__spMapSet][id].name;
}

PREFIX int spMapIDByName(char* name)
{
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[__spMapSet][i].active && strcmp(__spMapButton[__spMapSet][i].name,name) == 0)
			return i;
	return -1;
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
		if (__spMapPool[__spMapSet][i].active)
		{
			if (__spMapDesktopHack)
				__spMapDesktopButton[i] = 0;
			else
				spGetInput()->button[i] = 0;
		}
	__spMapChangingID = id;
}

PREFIX void spMapStartChangeByName(char* name)
{
	int i,id = -1;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[__spMapSet][i].active && __spMapButton[__spMapSet][i].poolButton >= 0 && strcmp(__spMapButton[__spMapSet][i].name,name) == 0)
		{
			spMapStartChangeByID(id);
			return;
		}
}

PREFIX int spMapContinueChange( void )
{
	if (__spMapChangingID < 0)
		return -1;
	int i;
	for (i = 0; i < SP_MAPPING_POOL_MAX; i++)
		if (__spMapPool[__spMapSet][i].active && ((__spMapDesktopHack == 0 && spGetInput()->button[i]) || (__spMapDesktopHack && __spMapDesktopButton[i])))
		{
			int r = spMapChange(__spMapChangingID,i);
			if (__spMapDesktopHack)
				__spMapDesktopButton[i] = 0;
			else
				spGetInput()->button[i] = 0;
			__spMapChangingID = -1;
			if (r == 1)
				return 2;
			return 1;
		}
	return 0;
}

PREFIX char* spMapLastCollisionCaption( void )
{
	return __spMapCollisionCaption[__spMapSet];
}

PREFIX char* spMapLastCollisionPool( void )
{
	return __spMapCollisionPool[__spMapSet];
}

PREFIX void spMapCancelChange( void )
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
		if (__spMapButton[__spMapSet][i].active)
			__spMapButton[__spMapSet][i].poolButton = spConfigGetInt(config,__spMapButton[__spMapSet][i].name,__spMapButton[__spMapSet][i].poolButton);
	spConfigFree(config);
}

PREFIX void spMapSave(char* subfolder,char* filename)
{
	printf("Save mapping...\n");
	spConfigPointer config = spConfigRead(filename,subfolder);
	//For every mapping changing the config:
	int i;
	for (i = 0; i < SP_MAPPING_MAX; i++)
		if (__spMapButton[__spMapSet][i].active)
			spConfigSetInt(config,__spMapButton[__spMapSet][i].name,__spMapButton[__spMapSet][i].poolButton);
	spConfigWrite(config);
	spConfigFree(config);
}

PREFIX void spMapDesktopHack(int value)
{
	__spMapDesktopHack = value;
}

PREFIX void spMapSetMapSet(int set)
{
	if (set < 0)
		return;
	if (set >= SP_MAPPING_SET_MAX)
		return;
	__spMapSet = set;
}
