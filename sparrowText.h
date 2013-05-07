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

/* File: sparrowText
 * 
 * SparrowText is for easy maintaining different translations of texts
 * used by your application. */
#ifndef _SPARROW_TEXT_H
#define _SPARROW_TEXT_H

#include "sparrowDefines.h"
#include <SDL.h>
#include <SDL_ttf.h>

/* Defines:
 * 
 * Every language has a unsigned 16 bit numbers calculated from the ascii
 * characters of the two-byte-strings from the ISO 639-1
 * http://www.sil.org/iso639-3/codes.asp?order=639
 * 
 * SP_LANGUAGE_EN - the English language
 * SP_LANGUAGE_DE - the German language
 * SP_LANGUAGE_FR - the French language*/
#define SP_LANGUAGE_EN ('e'*256+'n')
#define SP_LANGUAGE_DE ('d'*256+'e')
#define SP_LANGUAGE_FR ('f'*256+'r')

/* Define: SP_TEXT_MAX_READABLE_LINE
 * 
 * You shouldn't have more signs in a line than this value ;-)
 * Nobody will EVER need more than 64k letters in one translation...*/
#define SP_TEXT_MAX_READABLE_LINE 65536

/* Type: spTranslation
 * 
 * Contains a translation text and the language.
 * 
 * Variables:
 * text - a text in some language
 * language - the language of the text above
 * next - needed for the internal linked list. Do not change*/
typedef struct spTranslation *spTranslationPointer;
typedef struct spTranslation
{
	char* text;
	Uint16 language;
	spTranslationPointer next;
} spTranslation;

typedef struct spBundle *spBundlePointer;

/* Type: spText
 * 
 * Contains different Translation of the same text, which is identified by a
 * caption. The captions should be unique in bundles (See <spBundle>).
 * 
 * Variables:
 * char* - name of the text
 * firstTranslation - start of a linked list of possible translations
 * bundle - bundle of the text
 * prev,next - internal double linked list variables for the bundle. Do not
 * change*/
typedef struct spText *spTextPointer;
typedef struct spText
{
	char* caption; //Name of the text - same for every translation
	spTranslationPointer firstTranslation; //the translations
	spBundlePointer bundle; //the bundle of the text
	spTextPointer prev,next;
} spText;

/* Type: spBundle
 * 
 * A bundle is a collection of texts. Every text is in exactly one bunch.
 * 
 * Variables:
 * firstText - start of a double linked list of the texts
 * next - internal pointer to the next bundle in a linked list. Do not use or
 * change!*/
typedef struct spBundle
{
	spTextPointer firstText;
	spBundlePointer next;
} spBundle;

/* Function: spCreateText
 * 
 * Creates a new text, which contains different translations of the same text.
 * 
 * Parameters:
 * caption - the name of this text
 * bundle - optional bundle to add this text to. If no bundle is given, the
 * text will be added to the internal global bundle
 * 
 * Returns:
 * spText* - pointer to the created <spText> struct
 * 
 * See Also:
 * <spCreateTextWithTranslation>*/
PREFIX spTextPointer spCreateText(const char* caption,spBundlePointer bundle);

/* Function: spAddTranslation
 * 
 * Adds a translation to a text.
 * 
 * Parameters:
 * text - pointer to a <spText> struct, to which the translation shall be added
 * language - Uint16 language number of the added translation
 * translation - the text of the translation
 * 
 * See Also:
 * <spCreateTextWithTranslation>*/
PREFIX void spAddTranslation(spTextPointer text,Uint16 language,const char* translation);

/* Function: spCreateTextWithTranslation
 * 
 * Works like a mix of <spCreateText> and <spAddTranslation>. It creates a text
 * and adds a first translation to it.
 * 
 * Parameters:
 * caption - the name of this text
 * bundle - optional bundle to add this text to. If no bundle is given, the
 * text will be added to the internal global bundle
 * language - Uint16 language number of the added translation
 * translation - the text of the translation
 * 
 * Returns:
 * spText* - pointer to the created <spText> struct
 * 
 * See Also:
 * <spCreateText>, <spAddTranslation> */
PREFIX spTextPointer spCreateTextWithTranslation(const char* caption,spBundlePointer bundle,Uint16 language,const char* translation);

/* Function: spSearchCaption
 * 
 * Finds a specific <spText> in a bundle. Attention! This function is relative
 * slow. If you have *much* text, try to mind the returned pointer.
 * 
 * Parameters:
 * bundle - the bundle, in which the function shall search. If the text is in
 * the internal global bundel, pass NULL
 * caption - the caption of the text to find
 * 
 * Returns:
 * spText* - a pointer of the found <spText> or NULL if nothing was found
 * 
 * See Also:
 * <spGetTranslationFromCaption>*/
PREFIX spTextPointer spSearchCaption(spBundlePointer bundle, char* caption);

/* Function: spGetTranslation
 * 
 * Gives you the translation a text in the default language or (if not avaible)
 * in the *first* added language. That is the reason, why it makes sense, to add
 * Englisch first everytime!
 * 
 * Parameters:
 * text - a pointer to a <spText> struct, from which you want the right
 * translation
 * 
 * Returns:
 * char* - the translation as null terminated string
 * 
 * See Also:
 * <spGetTranslationFromCaption>*/
PREFIX char* spGetTranslation(spTextPointer text);

/* Function: spGetTranslationFromCaption
 * 
 * Gives you the translation in the default language of a text.
 * 
 * Parameters:
 * bundle - the bundle, in which shall be searched
 * caption - the caption of the text
 * 
 * Returns:
 * char* - the translation as null terminated string. However, if the language
 * is not found, but the caption, the first definied language is returned. If
 * the caption is not found an error string is returned.
 * 
 * See Also:
 * <spGetTranslation>, <spSearchCaption>*/
PREFIX char* spGetTranslationFromCaption(spBundlePointer bundle, char* caption);

/* Function: spCreateTextBundle
 * 
 * Creates a text bundle. Bundles are very practical for
 * encapsuling texts of different kinds. However: If you don't need them
 * and always refer to bundle "NULL", the undeletable internal main
 * bundle of sparrowText is used.
 * 
 * Returns:
 * spBundle* - pointer to the created bundle*/
PREFIX spBundlePointer spCreateTextBundle();

/* Function: spLoadBundle
 * 
 * Loads a bundle out of a text file. The files are like ini-files and the
 * interpretation of one line depends on the beginning. There are 3 kinds of
 * beginning a line:
 * * "#" starts a comment
 * * "--" followed by a random sign (like " " or ":") and followed by
 *   any string defines a new spText with the caption of the given
 *   string
 * * "XX" followed by a random sign (like " " or ":") and followed by
 *   any string defines a new translation of the language "XX", which
 *   can be any language of the ISO 639-1, e.g. "de" or "en", which is
 *   added to the LAST defined text. Example:
 * ># this is a comment.
 * >--:example text
 * >en This is an example text.
 * >de Das ist ein Beispieltext
 * >la lorem ipsum ;-)
 * 
 * Parameters:
 * filename - file to read
 * own_bundle - Pass 1 to create an own bundle or 0 to add the loaded
 * translations to the internal main bundle
 * 
 * Returns:
 * spBundle* - pointer to the <spBundle> struct the translations are added in*/
PREFIX spBundlePointer spLoadBundle(const char* filename,int own_bundle);

/* Function: spReadPossibleLanguages
 * 
 * Reads a "possible languages file". In fact it is the same like the bundle
 * files explained in <spLoadBundle>, but there is just one "text" defined and
 * this text HAVE to be named "languages". The following defined translations
 * are the possible languages and their names. Example:
 * >--:languages
 * >en:English
 * >de:Deutsch
 * >la:Latinus
 * 
 * Parameters:
 * filename - file with possible languages to read
 * 
 * See Also:
 * <spGetPossibleLanguage>, <spGetPossibleLanguageName>, <spGetPossibleLanguagesCount>*/
PREFIX void spReadPossibleLanguages(const char* filename);

/* Function: spGetPossibleLanguagesCount
 * 
 * Gets you the total number of all possible languages, if you
 * read them with <spReadPossibleLanguages>.
 * 
 * Returns:
 * int - the number of possible languages
 * 
 * See Also:
 * <spReadPossibleLanguages>, <spGetPossibleLanguage>, <spGetPossibleLanguageName> */
PREFIX int spGetPossibleLanguagesCount();

/* Function: spGetPossibleLanguage
 * 
 * Gives you the language code for a possible language identified by its number.
 * 
 * Parameters:
 * nr - the number of the language
 * 
 * Returns:
 * Uint16 - the language code
 * 
 * See Also:
 * <spReadPossibleLanguages>, <spGetPossibleLanguagesCount>, <spGetPossibleLanguageName> */
PREFIX Uint16 spGetPossibleLanguage(int nr);

/* Function: spGetPossibleLanguageName
 * 
 * Gives you the language name for a possible language identified by its number.
 * 
 * Parameters:
 * nr - the number of the language
 * 
 * Returns:
 * char* - name of the language
 * 
 * See Also:
 * <spReadPossibleLanguages>, <spGetPossibleLanguagesCount>, <spGetPossibleLanguage> */
PREFIX char* spGetPossibleLanguageName(int nr);

/* Function: spChangeBundle
 * 
 * Texts can change their bundles. Do it with this function ;-)
 * 
 * Parameters:
 * text - the text to change it's bundle
 * bundle - the new bundle of the text. Can be NULL, then the text is added to
 * the global main internal bundle*/
PREFIX void spChangeBundle(spTextPointer text,spBundlePointer bundle);

/* Function: spDeleteBundle
 * 
 * Deletes a whole bundle.
 * 
 * Parameters:
 * bundle - bundle to deleted
 * keepText - if 1, no text well be deleted and the bundle of the texts will be
 * the internal main bundle (which will be searched, if you pass "NULL" as
 * bunch parameter), else the texts are deleted, too. Obviously
 * >spDeleteBundle(NULL,1);
 * doesn't make sense. But of course you can use it to clean the main bundle
 * with
 * >spDeleteBundle(NULL,0);*/
PREFIX void spDeleteBundle(spBundlePointer bundle,int keepText);

/* Function: spSetDefaultLanguage
 * 
 * Sets the default language of sparrowText. The default language is Englisch
 * (en).
 * 
 * Parameters:
 * language - Uint16 code of the new default language*/
PREFIX void spSetDefaultLanguage(Uint16 language);

/* Function: spDeleteText
 * 
 * Deletes a text.
 * 
 * Parameters:
 * text - text to be deleted. It is also removed from it's bundle of course.*/
PREFIX void spDeleteText(spTextPointer text);

#endif
