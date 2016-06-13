 /* This file is part of sparrow3d.
  * Sparrow3d is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  *
  * Sparrow3d is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Foobar.  If not, see <http://www.gnu.org/licenses/>
  *
  * For feedback and questions about my Files and Projects please mail me,
  * Alexander Matthes (Ziz) , zizsdl_at_googlemail.com */

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
 * will be drawn in another way (like buttons or keys), and you can
 * create text blocks for an easy implementation of scrolling text. */
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
 * Per default 16384 signs (needs 64 kilobytes) are cached.
 * Thats enough most of the time. In fact - because SDL_ttf is just able to use
 * plane 0 (the first 65536 letters) of unicode, nearly everything you
 * will need is cached, and the tree is useles... But it was fun to
 * implement it!*/
#define SP_FONT_DEFAULT_CACHE 16384

// Define: SP_FONT_EXTRASPACE
// Defines some extra space for the border ;-)
#define SP_FONT_EXTRASPACE 2

// Define: SP_FONT_NO_BORDER
// Use this in spFontSetShadeColor, if you don't want / need aliasing
#define SP_FONT_NO_BORDER -1

/* type: spLetter
 *
 * a letter in a binary tree of a font
 *
 * Variables:
 * character (Uint32) - unicode character
 * surface (SDL_Surface*) - the character's surface
 * width (Sint32) - the character's width in pixels
 * height (Sint32) - the character's height in pixels
 * binary_height (Sint32) - internal value for the binary tree. *Not* the height in pixels
 * color (Uint16) - 16 Bit color of the character
 * left (spLetter*) - internal pointer for the tree
 * right (spLetter*) - internal pointer for the tree*/
typedef struct spLetterStruct *spLetterPointer;
typedef struct spLetterStruct
{
	Uint32 character;
	SDL_Surface* surface;
	Sint32 width;
	Sint32 height;
	Sint32 binary_height;
	Uint16 color;
	spLetterPointer left, right;
} spLetter;

/* type: spFontCache
 *
 * the font cache
 *
 * Variables:
 * size (Uint32) - size of the cache
 * cache (spLetter*) - an array of pointers of <spLetter>*/
typedef struct spFontCacheStruct *spFontCachePointer;
typedef struct spFontCacheStruct
{
	Uint32 size;
	spLetterPointer *cache;
} spFontCache;

/* type: spFont
 *
 * Root of a binary tree of all letters in this font
 *
 * Variables:
 * font (TTF_Font*) - the SDL_ttf font struct
 * maxheight (Sint32) - the height of the heighest letter in the tree
 * root (spLetter*) - the root of the binary letter tree
 * cacheOffset (Uint32) - the offset of the cache, where it "starts"
 * cache (spFontCache) - the cache
 * size (Uint32) - the size of the font
 * buttonRoot (spLetter*) - the root of the buttons of the binary letter tree */
typedef struct spFontStruct *spFontPointer;
typedef struct spFontStruct
{
	TTF_Font* font;
	Sint32 maxheight;
	spLetterPointer root;
	Uint32 cacheOffset;
	spFontCache cache;
	Uint32 size;
	spLetterPointer buttonRoot;
} spFont;

/* type: spTextLine
 *
 * A line of a <spTextBlock> struct.
 *
 * Variables:
 * count (int) - the number of letters in this line
 * width (int) - the width in pixel of this line
 * text (char*) - the content of the line
 * font (spFontPointer> - the used font*/
typedef struct spTextLineStruct *spTextLinePointer;
typedef struct spTextLineStruct
{
	int count;
	int width;
	char* text;
	spFontPointer font;
} spTextLine;

/* type: spTextBlock
 *
 * A text block is used to store a block of text with a maximum width and a
 * resultung count of lines. Made to use for scrollable text.
 *
 * Variables:
 * line_count (int) - the total number of lines in this text block
 * max_width (int) - the maximum width of the text block
 * line (spTextLine*) - array of the lines in the block. See <spTextLine> */
typedef struct spTextBlockStruct *spTextBlockPointer;
typedef struct spTextBlockStruct
{
	int line_count;
	int max_width;
	spTextLinePointer line;
} spTextBlock;

/* enum: spTextBlockAlignment
 *
 * Defines the orientation of a text block, when drawn. Used by
 * <spFontDrawTextBlock>.
 *
 * Values:
 * left - left orientation
 * middle - centered orientation
 * right - right orientation*/
typedef enum
{
	left = 0,
	middle = 1,
	right = 2
} spTextBlockAlignment;

/* Function: spFontLoad
 *
 * Loads a ttf font
 *
 * Parameters:
 * fontname - the name of the ttf font file
 * size - the size to load the font
 *
 * Returns:
 * spFont* - a pointer to a spFont for later use */
PREFIX spFontPointer spFontLoad( const char* fontname, Uint32 size );

/* Function: spFontLoad
 *
 * Deletes a <spFont> and loads a new font in the same struct.
 *
 * Parameters:
 * font - pointer to <spFont> to reload
 * fontname - the name of the ttf font file
 * size - the size to load the font
 *
 * Returns:
 * int - 0 no error, 1 load failed */
PREFIX int spFontReload(spFontPointer font,const char* fontname,Uint32 size);

/* Function: spFontAdd
 *
 * Adds characters to the font.
 * You need to call this once after spFontLoad and before drawing, with every
 * character you eventually want to draw
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

/* Function: spFontGetUnicodeFromUTF8
 *
 * Converts a utf8 sign to unicode
 *
 * Parameters:
 * sign - utf8 character
 *
 * Returns:
 * Uint32 - unicode value*/
PREFIX Uint32 spFontGetUnicodeFromUTF8(const char* sign);

/* Function: spFontGetUTF8FromUnicode
 *
 * Converts a unicode character to utf8
 *
 * Parameters:
 * sign - unicode sign
 * buffer - buffer for the resulting utf8 character, you have to alloc it!
 * However, 5 bytes are always enough
 * len - maximal length of the buffer
 *
 * Returns:
 * char* - the same pointer as buffer points, too. Just for better usement.*/
PREFIX char* spFontGetUTF8FromUnicode(Uint32 sign,char* buffer,int len);

/* Function: spFontAddButton
 *
 * Adds a button the the font.
 *
 * Parameters:
 * font - the font pointer, which shall be filled with new stuff
 * character - this unicode character will identify the button, e.g. 'A' for the
 * Button A or 'S' for Select. Use whatever you want.
 * caption - the name of the button. If you use the button design, use only
 * names of one letter, with the key design more letters are fine.
 * fgColor - a 16 bit color value identifying the foreground color of the
 * button, that means the color of the caption and the border
 * bgColor - a 16 bit color value identifying the background color of the button*/
PREFIX void spFontAddButton( spFontPointer font, Uint32 character, char* caption, Uint16 fgColor, Uint16 bgColor );

#define SP_BUTTON_ARROW_LEFT 1
#define SP_BUTTON_ARROW_UP 2
#define SP_BUTTON_ARROW_RIGHT 3
#define SP_BUTTON_ARROW_DOWN 4

/* Function: spFontAddArrowButton
 *
 * Adds an arrow button the the font.
 *
 * Parameters:
 * font - the font pointer, which shall be filled with new stuff
 * character - this unicode character will identify the arrow button, e.g. '<'
 * for the left arrow or 'v' for down. Use whatever you want.
 * direction - Possible values are 1 (left), 2 (up), 3 (right) and 4 (down),
 * also called SP_BUTTON_ARROW_LEFT, SP_BUTTON_ARROW_UP, SP_BUTTON_ARROW_RIGHT
 * and SP_BUTTON_ARROW_DOWN
 * fgColor - a 16 bit color value identifying the foreground color of the
 * button, that means the color of the caption and the border
 * bgColor - a 16 bit color value identifying the background color of the button*/
PREFIX void spFontAddArrowButton( spFontPointer font, Uint32 character, int direction, Uint16 fgColor, Uint16 bgColor );

/* Function: spFontSetButtonBoderSigns
 *
 * Sets the starting and end sign for buttons, the defaults are '[' and ']',
 * which means, that "[A]" will be drawn as a button with the caption A, if you
 * called spFontAddButton(font,'A',"A",fg,bg) before.
 *
 * Parameters:
 * left - the start of a button
 * right - the end of a button*/
PREFIX void spFontSetButtonBorderSigns(Uint32 left,Uint32 right);

/* Function: spFontSetSemanticButtonBoderSigns
 *
 * Sets the starting and end sign for semantic buttons from <sparrowMapping>,
 * the defaults are '{' and '}', which means, that "{fire}" will be drawn as
 * the button mapping to "fire".
 *
 * Parameters:
 * left - the start of a semantic button
 * right - the end of a semantic button*/
PREFIX void spFontSetSemanticButtonBorderSigns(Uint32 left,Uint32 right);

/* Function: spFontSetButtonStrategy
 *
 * Sets the strategy used to determine, whether buttons are round (like normal
 * buttons on handhelds) or quadric (like keyboard buttons).
 *
 * Parameters:
 * strategy - <SP_FONT_INTELLIGENT> uses rounds buttons for normal button, but
 * quadric buttons for Start or Select and always quadric buttons for keyboard
 * keys. With <SP_FONT_BUTTON> or <SP_FONT_KEY> you can force another design. */
PREFIX void spFontSetButtonStrategy(int strategy);

/* Function: spFontAddBorder
 *
 * For better viewing on different backgrounds, most of the time it is useful to
 * have a border around the letters. With this function you can do this.
 * It adds a border to every *currently* loaded letter. (Every letter loaded
 * afterwards will have no border by default )
 * Note: Works on a pixel level, therefore needs a non-transparent font to work.
 *
 * Parameters:
 * font - the font, that shall get a border
 * bordercolor - the 16 bit color of the border*/
PREFIX void spFontAddBorder( spFontPointer font, Uint16 bordercolor );

/* Function: spFontReplaceColor
 *
 * If you want to change the color of every letter in the font, use
 * this function. It might be useful to draw just the border and no
 * letter inside: First draw the letter in a color of your choice
 * (e.g. black), than add a border (e.g. white) and than call
 * spFontReplaceColor and replace the black with SP_ALPHA_COLOR (pink)
 *
 * Parameters:
 * font - the font, which color shall be changed
 * oldcolor - 16 Bit color to be changed
 * newcolor - 16 Bit color to be set*/
PREFIX void spFontReplaceColor( spFontPointer font, Uint16 oldcolor, Uint16 newcolor );

/* Function: spFontMulWidth
 *
 * This function changes the width, which is similar to the distance of
 * the letters, of the whole font.
 *
 * Parameters:
 * font - the font, which width will be changed
 * factor - fixed point factor for the width of the output. factor = SP_ONE does
 * nothing, greater does increase the distance and smaller ones will decrease
 * the distance.*/
PREFIX void spFontMulWidth( spFontPointer font, Sint32 factor );

/* Function: spFontChangeLetter
 *
 * Once I had a bug in sparrowFont. The "0" just looked ugly and wrong
 * and it seemed to be the failure of SDL_ttf. So I wrote this function
 * to replace the surface, but not the NAME of a letter. I called
 * spFontChangeLetter(font,spFontGetLetter(font,'0'),'O',color); and
 * my 0 was O. Looked a bit funny, but worked.
 *
 * Parameters:
 * font - the font to be changed
 * letter - the letter in the font to be changed, use <spFontGetLetter>
 * character - the character to be replaced. The character value of the letter
 * will *not* be changed, this is only for the redrawing of the surface
 * color - color of the letter
 *
 * See Also:
 * <spFontChangeButton>*/
PREFIX void spFontChangeLetter( spFontPointer font, spLetterPointer letter, Uint32 character, Uint16 color );

// Function: spFontChangeButton
// Same like <spFontChangeLetter>, but for the Buttons.
PREFIX void spFontChangeButton( spFontPointer font, spLetterPointer letter, Uint32 character, char* caption,  Uint16 fgColor, Uint16 bgColor);

// Function: spFontChangeArrowButton
// Same like <spFontChangeLetter>, but for the arrow buttons.
PREFIX void spFontChangeArrowButton( spFontPointer font, spLetterPointer letter, Uint32 character, int direction,  Uint16 fgColor, Uint16 bgColor);

/* Function: spFontGetLetter
 *
 * Returns the letter struct of a letter
 *
 * Parameters:
 * font - the font to be searched
 * character - the unicode character, which shall be found
 *
 * Returns:
 * spLetter* - NULL if not found, else the found letter struct
 *
 * See Also:
 * <spFontGetButton>*/
PREFIX spLetterPointer spFontGetLetter( spFontPointer font, Uint32 character );

// Function: spFontGetButton
// Works like <spFontGetLetter> but for buttons. ;-)
PREFIX spLetterPointer spFontGetButton( spFontPointer font, Uint32 character );

/* Function: spFontDraw
 *
 * Draws a left aligned text to the render target
 *
 * Parameters:
 * x,y - position on the render target
 * z - z value for z-test, z-set, etc.
 * text - text to be drawn
 * font - font, that shall be used for drawing the text
 *
 * Returns:
 * int - the printed width in pixels*/
PREFIX int spFontDraw( Sint32 x, Sint32 y, Sint32 z, const unsigned char* text, spFontPointer font );

/* Function: spFontDrawRight
 *
 * Draws a right aligned text to the render target
 *
 * Parameters:
 * x,y - position on the render target
 * z - z value for z-test, z-set, etc.
 * text - text to be drawn
 * font - font, that shall be used for drawing the text
 *
 * Returns:
 * int - the printed width in pixels*/
PREFIX int spFontDrawRight( Sint32 x, Sint32 y, Sint32 z,const unsigned  char* text, spFontPointer font );

/* Function: spFontDrawMiddle
 *
 * Draws a center aligned text to the render target
 *
 * Parameters:
 * x,y - position on the render target
 * z - z value for z-test, z-set, etc.
 * text - text to be drawn
 * font - font, that shall be used for drawing the text
 *
 * Returns:
 * int - the printed width in pixels*/
PREFIX int spFontDrawMiddle( Sint32 x, Sint32 y, Sint32 z,const unsigned  char* text, spFontPointer font );

/* Function: spFontWidth
 *
 * Width of a text
 *
 * Parameters:
 * text - the text, which size you want to know
 * font - the font, which defines the size of the text
 *
 * Returns:
 * int - the width in pixel, if you would draw the text with the font*/
PREFIX int spFontWidth(const unsigned  char* text, spFontPointer font );

/* Function: spFontDelete
 *
 * Deletes a font
 *
 * Parameters:
 * font - the font to be deleted*/
PREFIX void spFontDelete( spFontPointer font );

/* Function: spFontChangeCacheSize
 *
 * Changes to size of the cache.
 *
 * Parameters:
 * font - the font, which cache you want to modify
 * size - new size of the cache*/
PREFIX void spFontChangeCacheSize( spFontPointer font, Sint32 size );

/* Function: spFontChangeCacheSize
 *
 * Changes to start of the cache.
 *
 * Parameters:
 * font - the font, which cache you want to modify
 * letter - the new start of the cache. Default is 0. The whole cache goes from
 * the start to start + cache size - 1. Everything IN the cache has a finding
 * time of 1.*/
PREFIX void spFontSetCacheStart( spFontPointer font, Sint32 letter );

/* Function: spFontGetCacheStart
 *
 * Returns the start of the cache.
 *
 * Parameters:
 * font - the font, which cache property you want to get
 *
 * Returns:
 * Sint32 - the start of the cache*/
PREFIX Sint32 spFontGetCacheStart( spFontPointer font );

/* Function: spFontSetShadeColor
 *
 * Sets the "background" color, if you want to shade smoothly. It is still no
 * alpha blending, but the blended font will be look better. However the font
 * might look ugly at least with a shitty background color. So the
 * default is <SP_FONT_NO_BORDER>, which means, that no shading is done and the
 * font looks normal pixelized.
 *
 * Parameters:
 * value - the 16 bit alpha color to which it shall be shaded*/
PREFIX void spFontSetShadeColor(int value);

/* Function: spFontShadeButtons
 *
 * Desides, whether the button font should be shaded or not. For compatibility
 * (especially with transparent background color of the buttons) the default is
 * false (0). But in most cases true (1) should look much better. :)
 *
 * Parameters:
 * value - 1 enabled, 0 disabled*/
PREFIX void spFontShadeButtons(int value);

/* Function: spFontAddEveryLetterOfTextBundle
 *
 * Adds every letter used in the given sparrowText Bundle of Translations. See
 * <sparrowText> for more.
 *
 * Parameters:
 * font - the font to be changed
 * bundle - the bundle, to be scaned for every used character
 * color - the color of the added characters*/
PREFIX void spFontAddEveryLetterOfTextBundle( spFontPointer font, spBundlePointer bundle,Uint16 color);

/* Function: spCreateTextBlock
 *
 * Creates an <spTextBlock> out of an text. Use it in <spFontDrawTextBlock> to
 * draw scrollable text.
 *
 * Parameters:
 * text - the text to convert to a text block
 * max_width - the maximal width one line should have at all. If a line consists
 * of one word, which is longer than max_width the word is splitted and - is
 * added. Otherwise a line break always happens between words
 * font - the font (pointer to <spFont>) to use to determine the width of the
 * rendered text lines
 *
 * Returns:
 * spTextBlockPointer - pointer to the created <spTextBlock> struct.*/
PREFIX spTextBlockPointer spCreateTextBlock( const unsigned  char* text, int max_width, spFontPointer font);

/* Function: spDeleteTextBlock
 *
 * Deletes a text block created by <spCreateTextBlock>.
 *
 * Parameters:
 * block - pointer to <spTextBlock> to free*/
PREFIX void spDeleteTextBlock(spTextBlockPointer block);

/* Function: spFontDrawTextBlock
 *
 * Draws a previously created text block with a maximum height. Also supports
 * scrolling.
 *
 * Parameters:
 * aligment - determines, whether the text should be aligned left, centered or
 * right. Possible values (<spTextBlockAlignment>) are left, middle and right
 * x,y - possion of the text on the screen
 * z - z value to set and test (if enabled) while drawing
 * block - the text block created by <spCreateTextBlock> to draw
 * height - the maximum height of the text to be drawn.
 * position - fixed point value between 0 and SP_ONE, which determines which
 * part of the block shall be drawn. Use it for scrolling. If the whole block
 * fits in the block->max_width X height block, of course this parameters doesn't
 * make any sense and is ignored
 * font - the font to draw the text. Should be the same as used in
 * <spCreateTextBlock> or NULL, but however don't have to. If NULL the font of
 * <spCreateTextBlock> is used.
 *
 * Returns:
 * int - the number of lines drawn, -1 at error*/
PREFIX int spFontDrawTextBlock(spTextBlockAlignment alignment,Sint32 x, Sint32 y, Sint32 z, spTextBlockPointer block, Sint32 height, Sint32 position, spFontPointer font );

/* Function: spFontGetLetterString
 *
 * Generates a string to use maybe for another call of <spFontAdd>.
 *
 * Parameters:
 * font - spFontPointer to use
 * buffer - char* buffer to fill
 * length - size of the buffer
 *
 * Returns:
 * char* - the pointer to buffer*/
PREFIX char* spFontGetLetterString(spFontPointer font,unsigned char* buffer,int length);
#endif
