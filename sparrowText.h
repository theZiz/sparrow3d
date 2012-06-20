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
#ifndef _SPARROW_TEXT_H
#define _SPARROW_TEXT_H

#include "sparrowDefines.h"
#ifdef SDL_INCLUDE_SUBDIR
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_ttf.h>
#endif

//Some languages as two-byte-strings from the ISO 639-1
//http://www.sil.org/iso639-3/codes.asp?order=639
#define SP_LANGUAGE_EN ('e'*256+'n')
#define SP_LANGUAGE_DE ('d'*256+'e')
#define SP_LANGUAGE_FR ('f'*256+'r')

typedef struct spTextTranslation *spTextTranslationPointer;
typedef struct spTextTranslation
{
	char* text;
	Uint16 language; //ISO 639-1
	int used; //in how many bundles is it used?
	spTextTranslationPointer next;
} spTextTranslation;

typedef struct spTextBundle *spTextBundlePointer;

typedef struct spText *spTextPointer;
typedef struct spText
{
	char* caption; //Name of the text - same for every translation
	spTextTranslationPointer firstTranslation; //the translations
	spTextBundlePointer bundle;
	spTextPointer prev,next;
} spText;

typedef struct spTextBundle
{
	spTextPointer firstText;
	spTextBundlePointer next; //only used itnernal. DO NOT CHANGE OR USE!
} spTextBundle;



/* spTextCreateText creates a new text (which contains different translations
 * of the same content) with the name "caption" and adds it (optinal) to the to
 * the bundle "bundle". With bundles you can easily access texts of one kind,
 * e.g. of one level. */
PREFIX spTextPointer spTextCreate(char* caption,spTextBundlePointer bundle);

PREFIX void spTextAddTranslation(spTextPointer text,Uint16 language,char* translation); //TODO

PREFIX spTextPointer spTextCreateWithTranslation(char* caption,spTextBundlePointer bundle,Uint16 language,char* translation); //TODO

PREFIX spTextBundlePointer spTextCreateBundle();

PREFIX void spTextChangeBundle(spTextPointer text,spTextBundlePointer bundle);

PREFIX void spTextDeleteBundle(spTextBundlePointer bundle,int keepText);

PREFIX void spTextSetDefaultLanguage(Uint16 language);

PREFIX void spTextDelete(spTextPointer text);

#endif
