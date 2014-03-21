/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_mapping.h"
#include <sparrow3d.h>
#include <strings.h>

int map_selection = 0;
int already_used = 0;

void init_mapping()
{
	spMapPoolAdd(SP_BUTTON_A,"[A]");
	spMapPoolAdd(SP_BUTTON_B,"[B]");
	spMapPoolAdd(SP_BUTTON_X,"[X]");
	spMapPoolAdd(SP_BUTTON_Y,"[Y]");
	spMapPoolAdd(SP_BUTTON_SELECT,"[E]");
	spMapButtonAdd(0,"fire","Fire",SP_BUTTON_A);
	spMapButtonAdd(1,"jump","Jump",SP_BUTTON_B);
	spMapButtonAdd(2,"run","Run",SP_BUTTON_X);
	spMapButtonAdd(3,"bomb","Bomb",SP_BUTTON_Y);
	spMapLoad("testsparrow","controls.cfg");
	spMapSetStrategy(SP_MAPPING_CANCEL); //or SP_MAPPING_NONE or SP_MAPPING_SWITCH or SP_MAPPING_OTHER_INVALID
}

char* caption_mapping(char* caption)
{
	sprintf(caption,"Button Mapping Test");
	return caption;
}

char* settings_mapping(char* caption,int button)
{
	switch (button)
	{
		case SP_BUTTON_B:
			sprintf(caption,"Use Up and Down to select an entry and Left");
			break;
		case SP_BUTTON_A:
			sprintf(caption,"or Right or [E] to change the mapping!");
			break;
	}
	return caption;
}

void draw_mapping(int rotation, spFontPointer font)
{
	spSetZTest( 0 );
	spSetZSet( 0 );
	int r = spMapContinueChange();
	if (r == 0)
		spFontDrawMiddle(spGetWindowSurface()->w/2,font->maxheight*3,0,"Press [A],[B],[X],[Y] or [E]!",font);
	else
	if (already_used)
	{
		char buffer[256];
		sprintf(buffer,"Button %s is already used by %s!",spMapLastCollisionPool(),spMapLastCollisionCaption());
		spFontDrawMiddle(spGetWindowSurface()->w/2,font->maxheight*3,0,buffer,font);
	}
	if (r == 1)
		spMapSave("testsparrow","controls.cfg");
	if (r == 2)
		already_used = 1;
	int i;
	for (i = 0; i < 4; i++)
	{
		if (i == map_selection)
			spFontDrawMiddle(spGetWindowSurface()->w/2,font->maxheight*(5+i),0,"->                                   <-",font);
		spFontDrawRight(spGetWindowSurface()->w*7/15,font->maxheight*(5+i),0,spMapCaptionByID(i),font);
		spFontDraw     (spGetWindowSurface()->w*8/15,font->maxheight*(5+i),0,spMapButtonByID(i),font);
	}
	i++;
	spFontDrawMiddle(spGetWindowSurface()->w/2,font->maxheight*(5+i),0,"Action:",font);
	i++;
	int j;
	for (j = 0; j < 4; j++)
	if (spMapGetByID(j))
	{
		spFontDrawMiddle(spGetWindowSurface()->w/2,font->maxheight*(5+i),0,spMapCaptionByID(j),font);
		i++;
	}
	
	spSetZTest( 1 );
	spSetZSet( 1 );
}

void calc_mapping(int steps)
{
	int r = spMapContinueChange();
	if (r == 0)
		return;
	if (r == 1)
		spMapSave("testsparrow","controls.cfg");
	if (r == 2)
		already_used = 1;
	if (spGetInput()->button[SP_BUTTON_SELECT])
	{
		spGetInput()->button[SP_BUTTON_SELECT] = 0;
		spMapStartChangeByID(map_selection);
		already_used = 0;
		return;
	}
	if (spGetInput()->axis[0] > 0)
	{
		spGetInput()->axis[0] = 0;
		spMapChangeNextInPool(map_selection);
		spMapSave("testsparrow","controls.cfg");
		already_used = 0;
	}
	if (spGetInput()->axis[0] < 0)
	{
		spGetInput()->axis[0] = 0;
		spMapChangePreviousInPool(map_selection);
		spMapSave("testsparrow","controls.cfg");
		already_used = 0;
	}
	if (spGetInput()->axis[1] > 0 && map_selection < 3)
	{
		spGetInput()->axis[1] = 0;
		map_selection++;
	}
	if (spGetInput()->axis[1] < 0 && map_selection > 0)
	{
		spGetInput()->axis[1] = 0;
		map_selection--;
	}
}

void quit_mapping()
{
}
