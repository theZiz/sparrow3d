/*
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms
 of the GNU Lesser General Public license (the  "LGPL License"), in which case the
 provisions of LGPL License are applicable instead of those
 above.

 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include "sparrowText.h"

Uint16 spTextDefaultLanguage = SP_LANGUAGE_EN;
spTextBundlePointer spUberBundle = NULL;
spTextBundlePointer spMainBundle = NULL;

PREFIX spTextPointer spTextCreateText(char* caption,spTextBundlePointer bundle)
{
	spTextPointer text = (spTextPointer) malloc(sizeof(spText));
	text->caption = (char*)malloc(strlen(caption)+1);
	sprintf(text->caption,"%s",caption);
	text->firstTranslation = NULL;
	text->bundle = NULL;
	spTextChangeBundle(text,bundle); 
}

PREFIX void spTextChangeBundle(spTextPointer text,spTextBundlePointer bundle)
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
	{
		if (spMainBundle == NULL)
			spMainBundle = spTextCreateBundle();
		bundle = spMainBundle;
	}
	//putting in the new bundle at first position
	text->bundle = bundle;
	text->prev = NULL;
	text->next = bundle->firstText;
	bundle->firstText->prev = text;
	bundle->firstText = text;
	
}

PREFIX spTextBundlePointer spTextCreateBundle()
{
	spTextBundlePointer bundle = (spTextBundlePointer)malloc(sizeof(spTextBundle));
	bundle->next = spUberBundle;
	spUberBundle = bundle;
	bundle->firstText = NULL;
}

PREFIX void spTextSetDefaultLanguage(Uint16 language)
{
	spTextDefaultLanguage = language;
}

PREFIX void spTextDeleteBundle(spTextBundlePointer bundle,int keepText)
{
	if (bundle == spMainBundle) //The mainBundle is undestroyable!
		return;
	spTextPointer text = bundle->firstText;
	if (keepText)
	{
		while (text)
		{
			spTextPointer next = text->next;
			spTextChangeBundle(text,NULL);
			text = next;
		}
	}
	else
	{
		while (text)
		{
			spTextPointer next = text->next;
			spTextDelete(text);
			text = next;
		}
	}
	free(bundle);
}

PREFIX void spTextDelete(spTextPointer text)
{
	free(text->caption);
	while (text->firstTranslation)
	{
		spTextTranslationPointer next = text->firstTranslation->next;
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
