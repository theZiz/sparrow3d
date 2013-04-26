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

/* File: sparrowFont
 * 
 * SparrowFont is for loading truetype fonts with SDL_ttf and creating
 * bitmap-lookup tables for them. SDL_ttf and especially blending are
 * slow like hell. Not good for e.g. handhelds like the gp2x. So the
 * basic idea is to render every needed sign just once in a SDL surface
 * and then just blit it - which is pretty fast. Internal sparrowFont
 * uses a binary tree and a cache for easy finding the right surface.
 * Because it depends, which texts you want to draw, you have to
 * determine first, which sign you want.
 * Furthermore sparrowFont is able to parse button-description, which
 * will be drawn in another way (like buttons or keys)*/
#ifndef _SPARROW_FONT_H
#define _SPARROW_FONT_H

#include "sparrowDefines.h"
#include "sparrowText.h"
#include <SDL.h>
#include <SDL_ttf.h>

/* Defines: Defines for the look of the buttons and keys
 * 
 * SP_FONT_INTELLIGENT - Uses keys or buttons as it should make sense on the
 * target, e.g. keys for the PC, buttons for handhelds buttons, but "keys" for
 * the Start or Select button
 * SP_FONT_BUTTON - shows always buttons
 * SP_FONT_KEY - shows always keys */
#define SP_FONT_INTELLIGENT 0
#define SP_FONT_BUTTON 1
#define SP_FONT_KEY 2

/* Defines: Default ranges for spFontAdd
 * 
 * SP_FONT_GROUP_ASCII - whole ASCII set (sign 32 (space) until 127 (~))
 * SP_FONT_GROUP_GERMAN - German letters like umlauts
 * SP_FONT_GROUP_ALPHABET - All 26 latin letters big and small
 * SP_FONT_GROUP_NUMBERS - the arab numbers (0 to 9), dot, comma and minus*/
#define SP_FONT_GROUP_ASCII " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
#define SP_FONT_GROUP_GERMAN "äüöÄÜÖßẞ"
#define SP_FONT_GROUP_ALPHABET " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define SP_FONT_GROUP_NUMBERS "0123456789.,-"

/* Define: SP_FONT_DEFAULT_CACHE
 * 
 * Per default 16384 signs (needs 64 kilobytes) are cache.
 * Thats enough most of the times. In fact - because SDL_ttf is just able to use
 * plane 0 (the first 65536 letters) of unicode, nearly everything you
 * will need is cached, and the tree is useles... But it was fun to
 * implement it!*/
#define SP_FONT_DEFAULT_CACHE 16384

// Define: SP_FONT_EXTRASPACE
// Defines some extra space for the border ;-)
#define SP_FONT_EXTRASPACE 1

// Define: SP_FONT_NO_BORDER
// Use this in spFontSetShadeColor, if you don't want / need aliasing
#define SP_FONT_NO_BORDER -1

/* type: spLetterStruct
 * 
 * a letter in a binary tree of a font
 * 
 * Variables:
 * character - unicode character
 * surface - the character's surface
 * width - the character's width in pixels
 * height - the character's height in pixels
 * binary_height - internal value for the binary tree. NOT the height in pixels
 * color - 16 Bit color of the character
 * left - internal pointer for the tree
 * right - internal pointer for the tree*/
typedef struct spLetterStruct_ *spLetterPointer;
typedef struct spLetterStruct_
{
	Uint32 character;
	SDL_Surface* surface;
	Sint32 width;
	Sint32 height;
	Sint32 binary_height;
	Uint16 color;
	spLetterPointer left, right;
} spLetterStruct;

/* type: spFontCacheStruct
 * 
 * the font cache
 * 
 * Variables:
 * size - size of the cache
 * cache - an array of pointers of <spLetterStruct>*/
typedef struct spFontCacheStruct *spFontCachePointer;
typedef struct spFontCacheStruct
{
	Uint32 size;
	spLetterPointer *cache;
} spFontCacheStruct;

/* type: spFontStruct
 * 
 * Root of a binary tree of all letters in this font
 * 
 * Variables:
 * font - the SDL_ttf font struct
 * maxheight - the height of the heighest letter in the tree
 * root - the root of the binary letter tree
 * cacheOffset - the offset of the cache, where it "starts"
 * cache - the cache
 * size - the size of the font
 * buttonRoot - the root of the buttons of the binary letter tree */
typedef struct spFontStruct_ *spFontPointer;
typedef struct spFontStruct_
{
	TTF_Font* font;
	Sint32 maxheight;
	spLetterPointer root;
	Uint32 cacheOffset;
	spFontCacheStruct cache;
	Uint32 size;
	spLetterPointer buttonRoot;
} spFontStruct;

/* Function: spFontLoad
 * 
 * Loads a ttf font
 * 
 * Parameters:
 * fontname - the name of the ttf font file
 * size - the size to load the font
 * 
 * Returns:
 * spFontStruct* - a pointer to a spFontStruct for later use */
PREFIX spFontPointer spFontLoad(const char* fontname, Uint32 size );

/* Function: spFontAdd
 * 
 * Adds characters to the font.
 * 
 * Parameters:
 * font - the font pointer, which shall be filled with new stuff
 * characters - characters, that shall be added. It adds every utf8 character
 * passed in this char* string. Some defines (<SP_FONT_GROUP_ASCII>,
 * <SP_FONT_GROUP_GERMAN>, etc. ) are provided for ease of use. However: If a
 * letter already is in the range, the later mentions will be ignored
 * color - color of the added letters */
PREFIX void spFontAdd( spFontPointer font, char* characters, Uint16 color );

/* Function: spFontAddRange
 * 
 * Adds a range of utf8 characters to the font defined by a start and end
 * character
 * 
 * Parameters:
 * font - the font pointer, which shall be filled with new stuff
 * from - start character
 * to - end character. All characters between from and to will be added, e.g.
 * usw from = 0 and to = 9 to add all numbers. See ASCII/utf8 tables for more
 * usefull ranges. If to smaller than from, the values are switched
 * color - color of the added characters*/
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
