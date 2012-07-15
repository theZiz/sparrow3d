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
#ifndef _SPARROW_FONT_H
#define _SPARROW_FONT_H

#include "sparrowDefines.h"
#include <SDL.h>
#include <SDL_ttf.h>

#define SP_FONT_DEFAULT_CACHE 16384
#define SP_FONT_EXTRASPACE 1

#define SP_FONT_INTELLIGENT 0
#define SP_FONT_BUTTON 1
#define SP_FONT_KEY 2

/*a letter in a binary tree of a font*/
typedef struct spLetterStruct_ *spLetterPointer;
typedef struct spLetterStruct_
{
	Uint32 character; //later it could be utf8 characters
	SDL_Surface* surface; //the character's surface
	Sint32 width; //the character's length
	Sint32 height; //the character's height
	Sint32 binary_height; //NOT the height of the letter! just for the binary tree
	Uint16 color;
	spLetterPointer left, right; //smaller and bigger character
	spLetterPointer next; //For the Drawing functions
} spLetterStruct;

typedef struct spLetterIterStruct_ *spLetterIterPointer;
typedef struct spLetterIterStruct_
{
	spLetterPointer letter;
	spLetterIterPointer next; //For the Drawing functions
} spLetterIterStruct;

typedef struct spFontCacheStruct *spFontCachePointer;
typedef struct spFontCacheStruct
{
	Uint32 size;
	spLetterPointer *cache;
} spFontCacheStruct;

/*root of a binary tree of all letters in this font*/
typedef struct spFontStruct_ *spFontPointer;
typedef struct spFontStruct_
{
	TTF_Font* font;
	Sint32 maxheight;
	spLetterPointer root; //the root of the binary letter tree
	Uint32 cacheOffset;
	spFontCacheStruct cache;
	Uint32 size;
  spLetterPointer buttonRoot; //the root of the buttons of the binary letter tree
} spFontStruct;

PREFIX spFontPointer spFontLoad(const char* fontname, Uint32 size );

PREFIX void spFontAdd( spFontPointer font, Uint32 character, Uint16 color );

PREFIX void spFontAddRange( spFontPointer font, Uint32 from, Uint32 to, Uint16 color );

PREFIX Uint32 spFontGetUnicodeFromUTF8(const char* sign);

PREFIX char* spFontGetUTF8FromUnicode(Uint32 sign,char* buffer,int len);

/* spFontAddButton creates a new button formed sign with "character" in
 * center. The background color is bgColor and the color of letter and
 * border will be fgColor. */
PREFIX void spFontAddButton( spFontPointer font, Uint32 character, char* caption, Uint16 fgColor, Uint16 bgColor );

/* spFontSetButtonBoderSigns sets the starting and end sign for buttons.
 * Default values are '[' and ']', so e.g. out of [a] will be made a
 * button with a in the middle, if the button exist (it has to be
 * created with spFontAddButton).*/
PREFIX void spFontSetButtonBorderSigns(Uint32 left,Uint32 right);

/* spFontSetButtonStrategy sets the strategy used to determine, whether
 * buttons are round (like normal buttons on handhelds) or quadric
 * (like keyboard buttons). SP_FONT_INTELLIGENT uses e.g. rounds buttons
 * for normal button, but quadric buttons for Start or Select and always
 * quadric buttons for keyboard keys. But nevertheless, if it doesn't
 * work for you, use this function to force round (SP_FONT_BUTTON) or
 * quadric (SP_FONT_KEY) buttons. */
PREFIX void spFontSetButtonStrategy(int strategy);

PREFIX void spFontAddBorder( spFontPointer font, Uint16 bordercolor );

PREFIX void spFontReplaceColor( spFontPointer font, Uint16 oldcolor, Uint16 newcolor );

PREFIX void spFontMulWidth( spFontPointer font, Sint32 factor );

PREFIX void spFontChangeLetter( spFontPointer font, spLetterPointer letter, Uint32 character, Uint16 color );

PREFIX void spFontChangeButton( spFontPointer font, spLetterPointer letter, Uint32 character, char* caption,  Uint16 fgColor, Uint16 bgColor);

PREFIX spLetterPointer spFontGetLetter( spFontPointer font, Uint32 character );

PREFIX void spFontDraw( Sint32 x, Sint32 y, Sint32 z, const char* text, spFontPointer font );

PREFIX void spFontDrawRight( Sint32 x, Sint32 y, Sint32 z,const char* text, spFontPointer font );

PREFIX void spFontDrawMiddle( Sint32 x, Sint32 y, Sint32 z,const char* text, spFontPointer font );

PREFIX int spFontWidth(const char* text, spFontPointer font );

PREFIX void spFontDelete( spFontPointer font );

PREFIX void spFontChangeCacheSize( spFontPointer font, Sint32 size );

PREFIX void spFontSetCacheStart( spFontPointer font, Sint32 letter );

PREFIX Sint32 spFontGetCacheStart( spFontPointer font );

#endif
