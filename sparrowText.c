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
 of the GNU Lesser General Public license (the	"LGPL License"), in which case the
 which case the provisions of LGPL License are applicable instead of those above
 
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include "sparrowText.h"
#include <stdlib.h>
#include <string.h>

Uint16 spDefaultLanguage = SP_LANGUAGE_EN;
spBundlePointer spUberBundle = NULL;
spBundle spMainBundle = {NULL,NULL};
char spErrorTranslation[] = "No translations found";
int spLanguageCount = 0;
Uint16* spLanguageCaption = NULL;
char** spLanguageName = NULL;

PREFIX spTextPointer spCreateText(const char* caption,spBundlePointer bundle)
{
	spTextPointer text = (spTextPointer) malloc(sizeof(spText));
	text->caption = (char*)malloc(strlen(caption)+1);
	sprintf(text->caption,"%s",caption);
	text->firstTranslation = NULL;
	text->bundle = NULL;
	spChangeBundle(text,bundle); 
	return text;
}

PREFIX void spAddTranslation(spTextPointer text,Uint16 language,const char* translation)
{
	spTranslationPointer result = (spTranslationPointer)malloc(sizeof(spTranslation));
	result->text = (char*)malloc(strlen(translation)+1);
	sprintf(result->text,"%s",translation);
	result->language = language;
	result->next = text->firstTranslation;
	text->firstTranslation = result;
}

PREFIX spTextPointer spCreateTextWithTranslation(const char* caption,spBundlePointer bundle,Uint16 language,const char* translation)
{
	spTextPointer text = spCreateText(caption,bundle);
	spAddTranslation(text,language,translation);
	return text;
}

PREFIX spTextPointer spSearchCaption(spBundlePointer bundle, char* caption)
{
	if (bundle == NULL)
		bundle = &spMainBundle;
	spTextPointer text = bundle->firstText;
	while (text)
	{
		if (strcmp(text->caption,caption) == 0)
			return text;
		text = text->next;
	}
	return NULL;
}

PREFIX char* spGetTranslationFromCaption(spBundlePointer bundle, char* caption)
{
	return spGetTranslation(spSearchCaption(bundle,caption));
}

PREFIX char* spGetTranslation(spTextPointer text)
{
	if (text == NULL)
		return spErrorTranslation;
	spTranslationPointer translation = text->firstTranslation;
	spTranslationPointer lastTranslation = NULL;
	while (translation)
	{
		if (translation->language == spDefaultLanguage)
			return translation->text;
		lastTranslation = translation;
		translation = translation->next;
	}
	if (lastTranslation)
		return lastTranslation->text;
	return spErrorTranslation;
}

PREFIX void spChangeBundle(spTextPointer text,spBundlePointer bundle)
{
	if (text == NULL)
		return;
	if (text->bundle)
	{
		//Deleting from the old bundle
		spTextPointer prev = text->prev;
		spTextPointer next = text->next;
		if (prev)
			prev->next = next;
		if (next)
			next->prev = prev;
	}
	if (bundle == NULL) //Adding in the main bundle
		bundle = &spMainBundle;
	//putting in the new bundle at first position
	text->bundle = bundle;
	text->prev = NULL;
	text->next = bundle->firstText;
	if (bundle->firstText)
		bundle->firstText->prev = text;
	bundle->firstText = text;
	
}

PREFIX spBundlePointer spCreateTextBundle()
{
	spBundlePointer bundle = (spBundlePointer)malloc(sizeof(spBundle));
	bundle->next = spUberBundle;
	spUberBundle = bundle;
	bundle->firstText = NULL;
	return bundle;
}

PREFIX void spSetDefaultLanguage(Uint16 language)
{
	spDefaultLanguage = language;
}

PREFIX void spDeleteBundle(spBundlePointer bundle,int keepText)
{
	if (bundle == NULL)
		bundle = &spMainBundle;
	if (bundle == &spMainBundle && keepText) //The mainBundle is undestroyable!
		return;
	spTextPointer text = bundle->firstText;
	if (keepText)
	{
		while (text)
		{
			spTextPointer next = text->next;
			spChangeBundle(text,NULL);
			text = next;
		}
	}
	else
	{
		while (text)
		{
			spTextPointer next = text->next;
			spDeleteText(text);
			text = next;
		}
	}
	if (bundle != &spMainBundle)
		free(bundle);
}

PREFIX void spDeleteText(spTextPointer text)
{
	free(text->caption);
	while (text->firstTranslation)
	{
		spTranslationPointer next = text->firstTranslation->next;
		free(text->firstTranslation->text);
		free(text->firstTranslation);
		text->firstTranslation = next;
	}
	//Deleting from the bundle
	spTextPointer prev = text->prev;
	spTextPointer next = text->next;
	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
	free(text);
}

PREFIX spBundlePointer spLoadBundle(const char* filename,int own_bundle)
{
	spBundlePointer bundle = &spMainBundle;
	if (own_bundle)
		bundle = spCreateTextBundle();
	
	SDL_RWops *file = SDL_RWFromFile(filename, "rt");
	if (!file)
		return bundle;
	
	//Loading the file line by line
	int end = 0;
	spTextPointer text = NULL;
	while (!end)
	{
		char line[SP_TEXT_MAX_READABLE_LINE];
		end = spReadOneLine(file,line,SP_TEXT_MAX_READABLE_LINE);
		//parsing the line
		
		if (line[0] == '-' && line[1] == '-' && line[2] != 0) //new text
		{
			char* caption = &(line[3]);
			text = spCreateText(caption,bundle);
		}
		else
		if (line[0] == '#' || line[0] == ' ' || line[0] == '\n' || line[0] == 0) {} //comment, empty line
		else
		if (line[0] != 0 && line[1] != 0 && line[2] != 0) //new translation
		{
			if (text == NULL)
			{
				printf("Error: no new text (caption) defined!\n");
				break;
			}
			Uint16 language = line[0]*256+line[1];
			char* translation = &(line[3]);
			spAddTranslation(text,language,translation);
		}
	}

	SDL_RWclose(file);
	return bundle;
}

PREFIX void spReadPossibleLanguages(const char* filename)
{
	spBundlePointer tempBundle = spLoadBundle(filename,1);
	if (tempBundle == NULL || tempBundle->firstText == NULL || tempBundle->firstText->firstTranslation == NULL)
		return;
	//Reading the languages and the names
	spLanguageCount = 0;
	spTranslationPointer translation = tempBundle->firstText->firstTranslation;
	while (translation)
	{
		spLanguageCount++;
		translation = translation->next;
	}
	spLanguageCaption = (Uint16*)malloc(sizeof(Uint16)*spLanguageCount);
	spLanguageName = (char**)malloc(sizeof(char*)*spLanguageCount);
	translation = tempBundle->firstText->firstTranslation;
	int i;
	for (i = 0; i < spLanguageCount; i++)
	{
		spLanguageCaption[i] = translation->language;
		spLanguageName[i] = (char*)malloc(strlen(translation->text));
		sprintf(spLanguageName[i],"%s",translation->text);
		translation = translation->next;
	}
	spDeleteBundle(tempBundle,0);
}

PREFIX int spGetPossibleLanguagesCount()
{
	return spLanguageCount;
}

PREFIX Uint16 spGetPossibleLanguage(int nr)
{
	if (nr >=0 && nr < spLanguageCount)
		return spLanguageCaption[nr];
	return 0;
}

PREFIX char* spGetPossibleLanguageName(int nr)
{
	if (nr >=0 && nr < spLanguageCount)
		return spLanguageName[nr];
	return 0;
}

