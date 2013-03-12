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

/* spAddTranslation adds the translation "translation" of the language
 * "language" to the spText "text". There is no check, whether a
 * translation of this language already exist, but it wouldn't make
 * sense, would it? ;-) */
PREFIX void spAddTranslation(spTextPointer text,Uint16 language,const char* translation);

/* spCreateTextWithTranslation creates a text like spCreateText, but
 * also adds a first translatio (like spAddTranslation) to this new
 * text. */
PREFIX spTextPointer spCreateTextWithTranslation(const char* caption,spBundlePointer bundle,Uint16 language,const char* translation);

/* With spSearchCaption you are able to find a specific spTextPointer
 * in a bundle, definied by its caption. This function is a bit
 * expensive, so try to mind the found value! */
PREFIX spTextPointer spSearchCaption(spBundlePointer bundle, char* caption);

/* spGetTranslation gives you the translation of the text "text" of the
 * default language or (if not avaible) to FIRST added language. That is
 * the reason, why it makes sense, to add Englisch first everytime! */
PREFIX char* spGetTranslation(spTextPointer text);

/* spGetTranslationFromCaption gives you the translation of the default
 * language or the first defined language of the found text in the
 * bundle "bundle". The translation/text is identified by it's caption */
PREFIX char* spGetTranslationFromCaption(spBundlePointer bundle, char* caption);

/* Creates a text bundle and returns it. Bundles are very practical for
 * encapsuling texts of different kinds. However: If you don't need them
 * and always refer to bundle "NULL", the undeletable internal main
 * bundle of sparrowText is used. */
PREFIX spBundlePointer spCreateTextBundle();

/* spLoadBundle loads and returns a bundle out of the file "filename".
 * The files are like ini-files and the interpretation of one line
 * depends on the beginning. There are 3 kinds of beginning a line:
 * - "#" starts a comment
 * - "--" followed by a random sign (like " " or ":") and followed by
 *   any string defines a new spText with the caption of the given
 *   string
 * - "XX" followed by a random sign (like " " or ":") and followed by
 *   any string defines a new translation of the language "XX", which
 *   can be any language of the ISO 639-1, e.g. "de" or "en", which is
 *   added to the LAST defined text. Example:
 * # this is a comment.
 * --:example text
 * en This is an example text.
 * de Das ist ein Beispieltext
 * la lorem ipsum ;-) */
PREFIX spBundlePointer spLoadBundle(const char* filename,int own_bundle);

/* spReadPossibleLanguages reads a "possible languages file". In fact
 * it is the same like the bundle files explained above, but there is
 * just one "text" defined and this text HAVE to be named "languages".
 * The following defined translations are the possible languages and
 * their names. Example:
 * --:languages
 * en:English
 * de:Deutsch
 * la:Latinus */
PREFIX void spReadPossibleLanguages(const char* filename);

/* This gets you the total number of all possible languages, if you
 * read them with spReadPossibleLanguages */
PREFIX int spGetPossibleLanguagesCount();

/* This gives you the possible language number "nr". */
PREFIX Uint16 spGetPossibleLanguage(int nr);

/* This gives you the name of the language above */
PREFIX char* spGetPossibleLanguageName(int nr);

/* spTexts can change their bundles. Do it with this function ;-) */
PREFIX void spChangeBundle(spTextPointer text,spBundlePointer bundle);

/* With spDeleteBundle you can delete a whole bundle. If "keepText" is
 * 1, no text well be deleted and the bundle of the texts will be the
 * internal main bundle (which will be searched, if you pass "NULL" as
 * bunch parameter), else the texts are deleted, too. Obviously
 * spDeleteBundle(NULL,1); doesn't make sense. XD But of course you can
 * use it to clean the main bundle. */
PREFIX void spDeleteBundle(spBundlePointer bundle,int keepText);

/* spSetDefaultLanguage sets the default language of sparrowText. The
 * default default language is Englisch (en). */
PREFIX void spSetDefaultLanguage(Uint16 language);

/* spDeleteText deletes the spText "text" */
PREFIX void spDeleteText(spTextPointer text);

#endif
