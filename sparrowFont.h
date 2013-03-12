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

/* sparrowFont is for loading truetype fonts with SDL_ttf and creating
 * bitmap-lookup tables for them. SDL_ttf and especially blending are
 * slow like hell. Not good for e.g. handhelds like the gp2x. So the
 * basic idea is to render every needed sign just once in a SDL surface
 * and then just blit it - which is pretty fast. Internal sparrowFont
 * uses a binary tree and a cache for easy finding the right surface.
 * Because it depends, which texts you want to draw, you have to
 * determine first, which sign you want.
 * Furthermore sparrowFont is able to parse button-description, which
 * will be drawn in another way (like buttons...)*/
#ifndef _SPARROW_FONT_H
#define _SPARROW_FONT_H

#include "sparrowDefines.h"
#include "sparrowText.h"
#include <SDL.h>
#include <SDL_ttf.h>

/* That means: per default 16384 signs (needs 64 kilobytes) are cache.
 * Thats most enough. In fact - because SDL_ttf is just able to use
 * plane 0 (the first 65536 letters) of unicode, nearly everything you
 * will need is cached, and the tree is useles... But is was fun to
 * implement it!*/
#define SP_FONT_DEFAULT_CACHE 16384

/* This #define is some extra space for the border ;-) */
#define SP_FONT_EXTRASPACE 1

/* These constants determine, how buttons have to look */
#define SP_FONT_INTELLIGENT 0
#define SP_FONT_BUTTON 1
#define SP_FONT_KEY 2

/* Some default ranges for spFontAdd. */
#define SP_FONT_GROUP_ASCII " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
#define SP_FONT_GROUP_GERMAN "äüöÄÜÖßẞ"
#define SP_FONT_GROUP_ALPHABET " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define SP_FONT_GROUP_NUMBERS "0123456789.,-"

/* Use this in spFontSetShadeColor, if you don't want / need aliasing */
#define SP_FONT_NO_BORDER -1
/*a letter in a binary tree of a font*/
typedef struct spLetterStruct_ *spLetterPointer;
typedef struct spLetterStruct_
{
	Uint32 character; //unicode character
	SDL_Surface* surface; //the character's surface
	Sint32 width; //the character's length
	Sint32 height; //the character's height
	Sint32 binary_height; //NOT the height of the letter! just for the binary tree
	Uint16 color; //the color of the character
	spLetterPointer left, right; //smaller and bigger character (used by tree)
	spLetterPointer next; //For the Drawing functions
} spLetterStruct;

/* Needed by spFontDrawRight and spFontDrawMiddle */
typedef struct spLetterIterStruct_ *spLetterIterPointer;
typedef struct spLetterIterStruct_
{
	spLetterPointer letter;
	spLetterIterPointer next; //For the Drawing functions
} spLetterIterStruct;

/* the cache */
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
	TTF_Font* font; //the SDL_ttf font struct
	Sint32 maxheight; //the height of the heighest letter in the tree
	spLetterPointer root; //the root of the binary letter tree
	Uint32 cacheOffset; //the offset of the cache
	spFontCacheStruct cache; //the cache
	Uint32 size; //the size of the font
    spLetterPointer buttonRoot; //the root of the buttons of the binary letter tree
} spFontStruct;

/* spFontLoad loads the ttf font "fontname" with the size "size" and
 * returns a spFontPointer for later use */
PREFIX spFontPointer spFontLoad(const char* fontname, Uint32 size );

/* spFontAdd adds characters definied in "characters" to the font.
 * It adds every utf8 character passed in the char* string. Some defines
 * (SP_FONT_GROUP_ASCII, SP_FONT_GROUP_GERMAN, etc. ) are provided for ease
 * of use. However: If a letter already is in the range, the later mentions
 * will be ignored */
PREFIX void spFontAdd( spFontPointer font, char* characters, Uint16 color );

/* spFontAddRange adds a range of utf8 characters to the font.
 * You pass two single utf8-characters (or more, but only the first ones
 * will be used), it converts them to unicode and adds every in between
 * For example, you call
 * spFontAddRange(font,"ä","ü",color); It will add "ä" (Unicode 228),
 * "ü" (Unicode 252) and EVERYTHING between, which is:
 * "åæçèéêëìíîïðñòóôõö÷øùúû". So have a look at unicode tables, when
 * playing with this function, or you may get trouble. ;-) If you
 * are not familar with unicode or this function: Don't use it.
 * spFontAdd is a bit more writing effort, but much easier to
 * understand. One last word: If from is greater than to, they will
 * be switched.*/
PREFIX void spFontAddRange( spFontPointer font, char* from, char* to, Uint16 color );

/* spFontGetUnicodeFromUTF8 converts a utf8 sign passed as char*-string
 * to a 32 bit unicode value */
PREFIX Uint32 spFontGetUnicodeFromUTF8(const char* sign);

/* spFontGetUTF8FromUnicode converts a 32 bit unicode value passed in
 * "sign" to a utf8 char*-string. The char*-string has to be allocated
 * by yourself and the length of the string WITH nullterminating passed
 * to "len". At least 5 Bytes should always be enough. For better use,
 * spFontGetUTF8FromUnicode returns buffer at the end.*/
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

/* For better viewing on different backgrounds, most of the time it is
 * useful to have a border around the letters. With this function you
 * can do this. It adds a border to every YET loading letter. */
PREFIX void spFontAddBorder( spFontPointer font, Uint16 bordercolor );

/* If you want to change the color of every letter in the font, use
 * this function. It is e.g. usefull, to have JUSE the border and not
 * the letter inside. First draw the letter in a color of your choice
 * (maybe black), than add a border (maybe white) and than call
 * spFontReplaceColor and replace the black with SP_ALPHA_COLOR (pink).*/
PREFIX void spFontReplaceColor( spFontPointer font, Uint16 oldcolor, Uint16 newcolor );

/* This function changes the width, which is similar to the distance of
 * the letters, of the whole font. factor is a fixed point value.
 * factor = SP_ONE does nothing, greater does increase the distance and
 * smaller ones will decrease the distance. */
PREFIX void spFontMulWidth( spFontPointer font, Sint32 factor );

/* Once I had a bug in sparrowFont. The "0" just looked ugly and wrong
 * and it seemed to be the failure of SDL_ttf. So I wrote this function
 * to replace the surface, but not the NAME of a letter. I called
 * spFontChangeLetter(font,spFontGetLetter(font,'0'),'O',color); and
 * my 0 was O. Looked a bit funny, but worked. */
PREFIX void spFontChangeLetter( spFontPointer font, spLetterPointer letter, Uint32 character, Uint16 color );

/* Same like spFontChangeLetter, but for the Buttons */
PREFIX void spFontChangeButton( spFontPointer font, spLetterPointer letter, Uint32 character, char* caption,  Uint16 fgColor, Uint16 bgColor);

/* spFontGetLetter returns the letter struct of the letter with the
 * unicode character "character". Returns NULL if not found. */
PREFIX spLetterPointer spFontGetLetter( spFontPointer font, Uint32 character );

/* spFontGetButton works like spFontGetLetter but for buttons. ;-) */
PREFIX spLetterPointer spFontGetButton( spFontPointer font, Uint32 character );

/* spFontDraw draws the text "text" at x,y on the screen. z stuff
 * is handled! It uses the font "font". Returns the printed width in pixel */
PREFIX int spFontDraw( Sint32 x, Sint32 y, Sint32 z, const char* text, spFontPointer font );

/* spFontDrawRight works like spFontDraw, but the text has a right
 * alignment. Returns the printed width in pixel */
PREFIX int spFontDrawRight( Sint32 x, Sint32 y, Sint32 z,const char* text, spFontPointer font );

/* spFontDrawRight works like spFontDraw, but the text has a center
 * alignment. Returns the printed width in pixel */
PREFIX int spFontDrawMiddle( Sint32 x, Sint32 y, Sint32 z,const char* text, spFontPointer font );

/* spFontWidth returns, how width the text would be, if you would draw
 * it. */
PREFIX int spFontWidth(const char* text, spFontPointer font );

/* Deletes the font "font". */
PREFIX void spFontDelete( spFontPointer font );

/* spFontChangeCacheSize changes to size of the cache. */
PREFIX void spFontChangeCacheSize( spFontPointer font, Sint32 size );

/* spFontChangeCacheSize changes to start of the cache. Default it 0.
 * The whole cache goes from the start to start + cache size - 1.
 * Everything IN the cache has a finding time of 1.*/
PREFIX void spFontSetCacheStart( spFontPointer font, Sint32 letter );

/* spFontGetCacheStart returns the start of the cache */
PREFIX Sint32 spFontGetCacheStart( spFontPointer font );

/* spFontSetShadeColor sets the "background" color, if you want to shade
 * smoothy. It is still no alpha blending, but the blended font will be look
 * better. However, it could be, that the font looks ugly at least with a
 * shitty background color. So the default is SP_FONT_NO_BORDER, which means,
 * that no shading is done and the font looks normal pixelize. */
PREFIX void spFontSetShadeColor(int value);

/* spFontShadeButtons desides, whether the button font should be shaded
 * or not. For compatibility (especially with transparent background
 * color of the button) the default is false (0). But in most cases
 * true (1) should look much better. :) */
PREFIX void spFontShadeButtons(int value);

/* spFontAddEveryLetterOfTextBundle adds every letter used in the given
 * sparrowText Bundle of Translations */
PREFIX void spFontAddEveryLetterOfTextBundle( spFontPointer font, spBundlePointer bundle,Uint16 color);
#endif
