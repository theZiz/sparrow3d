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

/* sparrowText is for easy maintaining different translations of texts
 * used by your application. */
#ifndef _SPARROW_TEXT_H
#define _SPARROW_TEXT_H

#include "sparrowDefines.h"
#include <SDL.h>
#include <SDL_ttf.h>

/* Some languages as two-byte-strings from the ISO 639-1
 * http://www.sil.org/iso639-3/codes.asp?order=639 */
#define SP_LANGUAGE_EN ('e'*256+'n')
#define SP_LANGUAGE_DE ('d'*256+'e')
#define SP_LANGUAGE_FR ('f'*256+'r')

/* you shouldn't have more signs in a line than this value ;-)
 * Nobody will EVER need more than 64k letters in one translation... */
#define SP_TEXT_MAX_READABLE_LINE 65536

/* spTranslation contains a translation text and the language */
typedef struct spTranslation *spTranslationPointer;
typedef struct spTranslation
{
	char* text; //utf8 text
	Uint16 language; //ISO 639-1
	spTranslationPointer next;
} spTranslation;

/* a bundle is a collection of texts. Every text is in exactly one bunch */
typedef struct spBundle *spBundlePointer;

/* spText contains different Translation of the same text, which is
 * identified by a caption. The captions should be unique in bundles */
typedef struct spText *spTextPointer;
typedef struct spText
{
	char* caption; //Name of the text - same for every translation
	spTranslationPointer firstTranslation; //the translations
	spBundlePointer bundle; //the bundle of the text
	spTextPointer prev,next;
} spText;

typedef struct spBundle
{
	spTextPointer firstText;
	spBundlePointer next; //only used internal. DO NOT CHANGE OR USE!
} spBundle;

/* spCreateTextText creates a new text (which contains different translations
 * of the same content) with the name "caption" and adds it (optinal) to the to
 * the bundle "bundle". With bundles you can easily access texts of one kind,
 * e.g. of one level. */
PREFIX spTextPointer spCreateText(const char* caption,spBundlePointer bundle);

PREFIX void spAddTranslation(spTextPointer text,Uint16 language,const char* translation);

PREFIX spTextPointer spCreateTextWithTranslation(const char* caption,spBundlePointer bundle,Uint16 language,const char* translation);

PREFIX spTextPointer spSearchCaption(spBundlePointer bundle, char* caption);

PREFIX char* spGetTranslation(spTextPointer text);

PREFIX char* spGetTranslationFromCaption(spBundlePointer bundle, char* caption);

PREFIX spBundlePointer spCreateTextBundle();

PREFIX spBundlePointer spLoadBundle(const char* filename,int own_bundle);

PREFIX void spReadPossibleLanguages(const char* filename);

PREFIX int spGetPossibleLanguagesCount();

PREFIX Uint16 spGetPossibleLanguage(int nr);

PREFIX char* spGetPossibleLanguageName(int nr);

PREFIX void spChangeBundle(spTextPointer text,spBundlePointer bundle);

PREFIX void spDeleteBundle(spBundlePointer bundle,int keepText);

PREFIX void spSetDefaultLanguage(Uint16 language);

PREFIX void spDeleteText(spTextPointer text);

#endif
