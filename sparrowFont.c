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
#include "sparrowFont.h"
#include "sparrowPrimitives.h"
#include <stdlib.h>
#include <string.h>


Uint32 spFontButtonLeft = '[';
Uint32 spFontButtonRight = ']';
int spFontStrategy = SP_FONT_INTELLIGENT;
int spFontLastUTF8Length = 0;
int spFontBackgroundColor = SP_FONT_NO_BORDER;
int spFontButtonShade = 0;

PREFIX spFontPointer spFontLoad(const char* fontname, Uint32 size )
{
	TTF_Font* ttf = TTF_OpenFont( fontname, size );
	if ( !ttf )
	{
		printf( "Failed to load Font \"%s\", dude...\n", fontname );
		printf( "	Error was: \"%s\"\n", TTF_GetError() );
		return NULL;
	}
	spFontPointer font = ( spFontPointer )malloc( sizeof( spFontStruct ) );
	font->font = ttf;
	font->root = NULL;
	font->buttonRoot = NULL;
	font->size = size;
	font->maxheight = 0;
	font->cacheOffset = 0;
	font->cache.cache = NULL;
	spFontChangeCacheSize( font, SP_FONT_DEFAULT_CACHE );
	return font;
}

static Sint32 spLetterGetHeight( spLetterPointer letter )
{
	return letter ? letter->binary_height : -1;
}

static spLetterPointer spLetterRotateLeft( spLetterPointer K2 )
{
	spLetterPointer K1;

	K1 = K2->left;
	K2->left = K1->right;
	K1->right = K2;

	K2->binary_height = spMax( spLetterGetHeight( K2->left ), spLetterGetHeight( K2->right ) ) + 1;
	K1->binary_height = spMax( spLetterGetHeight( K1->left ), spLetterGetHeight( K2				) ) + 1;

	return K1;
}

static spLetterPointer spLetterRotateRight( spLetterPointer K1 )
{
	spLetterPointer K2;

	K2 = K1->right;
	K1->right = K2->left;
	K2->left = K1;

	K1->binary_height = spMax( spLetterGetHeight( K1->left	), spLetterGetHeight( K1->right ) ) + 1;
	K2->binary_height = spMax( spLetterGetHeight( K2->right ), spLetterGetHeight( K1				) ) + 1;

	return K2;
}

static spLetterPointer spLetterDoubleLeft( spLetterPointer K3 )
{
	K3->left = spLetterRotateRight( K3->left );
	return spLetterRotateLeft( K3 );
}

static spLetterPointer spLetterDoubleRight( spLetterPointer K3 )
{
	K3->right = spLetterRotateLeft( K3->right );
	return spLetterRotateRight( K3 );
}

static spLetterPointer spFontInsert( spLetterPointer letter, spLetterPointer root )
{
	if( root == NULL )
	{
		root = letter;
		letter->binary_height = 0;
		letter->left = NULL;
		letter->right = NULL;
	}
	else if( letter->character < root->character )
	{
		root->left = spFontInsert( letter, root->left );
		if( spLetterGetHeight( root->left ) - spLetterGetHeight( root->right ) == 2 )
			if( letter->character < root->left->character )
				root = spLetterRotateLeft( root );
			else
				root = spLetterDoubleLeft( root );
	}
	else if( letter->character > root->character )
	{
		root->right = spFontInsert( letter, root->right );
		if( spLetterGetHeight( root->right ) - spLetterGetHeight( root->left ) == 2 )
			if( letter->character > root->right->character )
				root = spLetterRotateRight( root );
			else
				root = spLetterDoubleRight( root );
	}
	root->binary_height = spMax( spLetterGetHeight( root->left ), spLetterGetHeight( root->right ) ) + 1;
	return root;
}

PREFIX void spFontChangeLetter( spFontPointer font, spLetterPointer letter, Uint32 character, Uint16 color )
{
	letter->color = color;
	Uint16 buffer[2];
	buffer[0] = character;
	buffer[1] = 0;
	SDL_Color sdlcolor = {( color >> 11 ) << 3, ( ( color << 5 ) >> 10 ) << 2, ( ( color & 31 ) << 3 )};
	SDL_Surface* surface;
	if (spFontBackgroundColor == SP_FONT_NO_BORDER)
		surface = TTF_RenderUNICODE_Solid( font->font, buffer, sdlcolor );
	else
	{
		SDL_Color background =	{( spFontBackgroundColor >> 11 ) << 3, ( ( spFontBackgroundColor << 5 ) >> 10 ) << 2, ( ( spFontBackgroundColor & 31 ) << 3 )};
		surface = TTF_RenderUNICODE_Shaded( font->font, buffer, sdlcolor ,background );
	}
	int width = surface->w + SP_FONT_EXTRASPACE * 2;
	if ( width & 1 )
		width++;
	letter->surface = spCreateSurface(width, surface->h + SP_FONT_EXTRASPACE * 2);
	SDL_FillRect( letter->surface, NULL, SP_ALPHA_COLOR );

	SDL_Rect DestR;
	DestR.x = SP_FONT_EXTRASPACE;
	DestR.y = SP_FONT_EXTRASPACE;
	DestR.w = surface->w;
	DestR.h = surface->h;
	SDL_BlitSurface( surface, NULL, letter->surface, &DestR );
	SDL_FreeSurface( surface );

	//Setting every spFontBackgroundColor to the alpha color
	SDL_LockSurface( letter->surface );
	Uint16* pixel = ( Uint16* )( letter->surface->pixels );
	int scanline = letter->surface->pitch/letter->surface->format->BytesPerPixel;
	int x, y;
	for ( x = 0; x < letter->surface->w; x++ )
		for ( y = 0; y < letter->surface->h; y++ )
			if ( pixel[x + y * scanline] == spFontBackgroundColor )
				pixel[x + y * scanline] = SP_ALPHA_COLOR;
	SDL_UnlockSurface( letter->surface );		

	TTF_SizeUNICODE( font->font, buffer, &( letter->width ), &( letter->height ) );
	if ( font->maxheight < letter->height )
		font->maxheight = letter->height;
}

static void spFontInternalAddOneCharacter( spFontPointer font, Uint32 character, Uint16 color )
{
	if (spFontGetLetter(font,character))
		return;
	spLetterPointer letter = ( spLetterPointer )malloc( sizeof( spLetterStruct ) );

	spFontChangeLetter( font, letter, character, color );
	letter->character = character;

	//tree insert
	font->root = spFontInsert( letter, font->root );
}

PREFIX void spFontAdd( spFontPointer font, char* characters, Uint16 color )
{
	//parsing the characters string.
	int pos = 0;
	while (1)
	{
		Uint32 character = spFontGetUnicodeFromUTF8(&(characters[pos]));
		if (character == 0)
			break;
		pos+=spFontLastUTF8Length;
		spFontInternalAddOneCharacter(font,character,color);
	}
}

PREFIX void spFontAddRange( spFontPointer font, char* from, char* to, Uint16 color )
{
	//getting the characters strings
	Uint32 c_from = spFontGetUnicodeFromUTF8(from);
	if (c_from == 0)
		return;
	Uint32 c_to = spFontGetUnicodeFromUTF8(to);
	if (c_to == 0)
		return;
	Uint32 character;
	if (c_from > c_to)
	{	
		character = c_from;
		c_from = c_to;
		c_to = character;
	}
	for (character = c_from; character <= c_to; character++)
		spFontInternalAddOneCharacter(font,character,color);
}

PREFIX Uint32 spFontGetUnicodeFromUTF8(const char* sign)
{
	if (!(sign[0] & 128))
	{
		spFontLastUTF8Length = 1;
		return sign[0]; //	0xxxxxxx
	}
	//No ASCII sign => counting the following bytes:
	int pos = 128; //first bit
	spFontLastUTF8Length = 0;
	while (sign[0] & pos) //until we find the first zero
	{
		spFontLastUTF8Length++;
		pos=pos>>1; //pos = pos / 2; => next bit
	}
	switch (spFontLastUTF8Length)
	{
		case 2: //110xxxxx 10xxxxxx
			//the first bit has to be and the second one DOESN'T have to be set
			if (!(sign[1] & 128) || (sign[1] & 64))
				return 0;
			return (((Uint32)sign[0] & 31) << 6) | ((Uint32)sign[1] & 63);
		case 3: //1110xxxx 10xxxxxx 10xxxxxx
			if (!(sign[1] & 128) || (sign[1] & 64))
				return 0;
			if (!(sign[2] & 128) || (sign[2] & 64))
				return 0;
			return (((Uint32)sign[0] & 15) << 12) | (((Uint32)sign[1] & 63) << 6) | ((Uint32)sign[2] & 63);
		case 4: //11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			if (!(sign[1] & 128) || (sign[1] & 64))
				return 0;
			if (!(sign[2] & 128) || (sign[2] & 64))
				return 0;
			if (!(sign[3] & 128) || (sign[3] & 64))
				return 0;
			return (((Uint32)sign[0] & 7) << 18) | (((Uint32)sign[1] & 63) << 12) | (((Uint32)sign[2] & 63) << 6) | ((Uint32)sign[3] & 63);
	}
	return 0;
}

PREFIX char* spFontGetUTF8FromUnicode(Uint32 sign,char* buffer,int len)
{
	if (buffer == NULL)
		return NULL;
	if (len < 1)
		return NULL;
	buffer[0] = 0;

	if (len < 2)
		return buffer;
	if (sign < (1 << 7)) //ASCII
	{
		buffer[0] = sign;
		buffer[1] = 0;
		return buffer;
	}

	if (len < 3)
		return buffer;
	if (sign < (1 << 11)) //2 Byte UTF8
	{
		buffer[0] = 128+64+(sign >> 6);
		buffer[1] = 128+	 (sign & 63);
		buffer[2] = 0;
		return buffer;
	}

	if (len < 4)
		return buffer;
	if (sign < (1 << 16)) //3 Byte UTF8
	{
		buffer[0] = 128+64+32+ (sign >> 12);
		buffer[1] = 128+			((sign >>	6) & 63);
		buffer[2] = 128+			( sign				& 63);
		buffer[3] = 0;
		return buffer;
	}

	if (len < 5)
		return buffer;
	if (sign < (1 << 21)) //4 Byte UTF8
	{
		buffer[0] = 128+64+32+16+ (sign >> 18);
		buffer[1] = 128+				 ((sign >> 12) & 63);
		buffer[2] = 128+				 ((sign >>	6) & 63);
		buffer[3] = 128+				 ( sign				& 63);
		buffer[4] = 0;
	}
	return buffer;
}

static int spFontCorrectStrategy(char* button)
{
	if (spFontStrategy != SP_FONT_INTELLIGENT)
		return spFontStrategy;
	#if defined GP2X || defined CAANOO || defined DINGUX || defined GCW || defined PANDORA
		if (strcmp(button,SP_BUTTON_A_NAME) == 0 ||
				strcmp(button,SP_BUTTON_B_NAME) == 0 ||
				strcmp(button,SP_BUTTON_X_NAME) == 0 ||
				strcmp(button,SP_BUTTON_Y_NAME) == 0 ||
				strcmp(button,SP_BUTTON_L_NAME) == 0 ||
				strcmp(button,SP_BUTTON_R_NAME) == 0)
				return SP_FONT_BUTTON;
		else
			return SP_FONT_KEY;
	#else
		return SP_FONT_KEY;
	#endif
}

PREFIX void spFontChangeButton( spFontPointer font, spLetterPointer letter, Uint32 character, char* caption, Uint16 fgColor, Uint16 bgColor )
{
	letter->color = fgColor;
	SDL_Color sdlcolor = {( fgColor >> 11 ) << 3, ( ( fgColor << 5 ) >> 10 ) << 2, ( ( fgColor & 31 ) << 3 )};
	SDL_Color background =	{( bgColor >> 11 ) << 3, ( ( bgColor << 5 ) >> 10 ) << 2, ( ( bgColor & 31 ) << 3 )};
	SDL_Surface* surface;
	if (spFontButtonShade)
		surface = TTF_RenderUTF8_Shaded( font->font, caption, sdlcolor ,background );
	else
		surface = TTF_RenderUTF8_Solid( font->font, caption, sdlcolor );
	int width;
	if (spFontCorrectStrategy(caption) == SP_FONT_BUTTON)
	{
		width = font->maxheight + SP_FONT_EXTRASPACE * 2;
		if ( width & 1 )
			width++;
		letter->surface = spCreateSurface(width, width);
		SDL_LockSurface(letter->surface);
		int x,y;
		Uint16* pixel = (Uint16*)letter->surface->pixels;
		int w = letter->surface->pitch/letter->surface->format->BytesPerPixel;
		for (x = 0; x < letter->surface->w; x++)
			for (y = 0; y < letter->surface->h; y++)
			{
				if ((x-width/2)*(x-width/2)+(y-width/2)*(y-width/2) <= width*width/4)
				{
					if ((x-width/2)*(x-width/2)+(y-width/2)*(y-width/2) <= width*width/5)
						pixel[x+y*w] = bgColor;
					else
						pixel[x+y*w] = fgColor;
				}
				else
					pixel[x+y*w] = SP_ALPHA_COLOR;
			}
		SDL_UnlockSurface(letter->surface);

		SDL_Rect DestR;
		DestR.x = width/2-surface->w/2;
		DestR.y = font->maxheight/2+SP_FONT_EXTRASPACE-surface->h/2;
		DestR.w = surface->w;
		DestR.h = surface->h;
		SDL_BlitSurface( surface, NULL, letter->surface, &DestR );
	}
	else
	{
		int buttonWidth = font->maxheight + SP_FONT_EXTRASPACE * 2;
		int border = buttonWidth/14;
		width = surface->w+2*SP_FONT_EXTRASPACE+2*border;
		if ( width & 1 )
			width++;
		letter->surface = spCreateSurface(width, font->maxheight+SP_FONT_EXTRASPACE*2);
		SDL_LockSurface(letter->surface);
		int x,y;
		Uint16* pixel = (Uint16*)letter->surface->pixels;
		int w = letter->surface->pitch/letter->surface->format->BytesPerPixel;
		for (x = 0; x < letter->surface->w; x++)
			for (y = 0; y < letter->surface->h; y++)
			{
				if (x < border				|| y < border ||
						x >= width-border || y >= letter->surface->h-border)
					pixel[x+y*w] = fgColor;
				else
					pixel[x+y*w] = bgColor;
			}
		SDL_UnlockSurface(letter->surface);

		SDL_Rect DestR;
		DestR.x = SP_FONT_EXTRASPACE+border;
		DestR.y = font->maxheight/2+SP_FONT_EXTRASPACE-surface->h/2;
		DestR.w = surface->w;
		DestR.h = surface->h;
		SDL_BlitSurface( surface, NULL, letter->surface, &DestR );
	}
	SDL_FreeSurface( surface );
	letter->height = font->maxheight;
	letter->width = width;
}


PREFIX void spFontAddButton( spFontPointer font, Uint32 character, char* caption, Uint16 fgColor, Uint16 bgColor )
{
	spLetterPointer letter = ( spLetterPointer )malloc( sizeof( spLetterStruct ) );

	spFontChangeButton( font, letter, character, caption, fgColor, bgColor );
	letter->character = character;

	//tree insert
	font->buttonRoot = spFontInsert( letter, font->buttonRoot );
}

PREFIX void spFontSetButtonBorderSigns(Uint32 left,Uint32 right)
{
	spFontButtonLeft = left;
	spFontButtonRight = right;
}

PREFIX void spFontSetButtonStrategy(int strategy)
{
	spFontStrategy = strategy;
}

static void spLetterAddBorder( spLetterPointer letter, Uint16 bordercolor )
{
	if ( letter == NULL )
		return;
	spLetterAddBorder( letter->left, bordercolor );
	spLetterAddBorder( letter->right, bordercolor );
	SDL_LockSurface( letter->surface );
	//create copy of the pixel data
	Uint16* inputpixel = ( Uint16* )malloc( sizeof( Uint16 ) * letter->surface->w * letter->surface->h );
	Uint16* pixel = ( Uint16* )( letter->surface->pixels );
	memcpy( inputpixel, pixel, sizeof( Uint16 )*letter->surface->w * letter->surface->h );
	int x, y;
	for ( x = 0; x < letter->surface->w; x++ )
		for ( y = 0; y < letter->surface->h; y++ )
			if ( inputpixel[x + y * letter->surface->w] != SP_ALPHA_COLOR )
			{
				//Left
				if ( x - 1 >= 0 && inputpixel[x - 1 + y * letter->surface->w] == SP_ALPHA_COLOR )
					pixel[x - 1 + y * letter->surface->w] = bordercolor;
				//Right
				if ( x + 1 < letter->surface->w && inputpixel[x + 1 + y * letter->surface->w] == SP_ALPHA_COLOR )
					pixel[x + 1 + y * letter->surface->w] = bordercolor;
				//Up
				if ( y - 1 >= 0 && inputpixel[x + ( y - 1 )*letter->surface->w] == SP_ALPHA_COLOR )
					pixel[x + ( y - 1 )*letter->surface->w] = bordercolor;
				//Down
				if ( y + 1 < letter->surface->h && inputpixel[x + ( y + 1 )*letter->surface->w] == SP_ALPHA_COLOR )
					pixel[x + ( y + 1 )*letter->surface->w] = bordercolor;

			}
	free( inputpixel );
	SDL_UnlockSurface( letter->surface );
	letter->width++;
}

PREFIX void spFontAddBorder( spFontPointer font, Uint16 bordercolor )
{
	spLetterAddBorder( font->root, bordercolor );
	spLetterAddBorder( font->buttonRoot, bordercolor );
}

static void spLetterReplaceColor( spLetterPointer letter, Uint16 oldcolor, Uint16 newcolor )
{
	if ( letter == NULL )
		return;
	spLetterReplaceColor( letter->left, oldcolor, newcolor );
	spLetterReplaceColor( letter->right, oldcolor, newcolor );
	SDL_LockSurface( letter->surface );
	//create copy of the pixel data
	Uint16* inputpixel = ( Uint16* )malloc( sizeof( Uint16 ) * letter->surface->w * letter->surface->h );
	Uint16* pixel = ( Uint16* )( letter->surface->pixels );
	memcpy( inputpixel, pixel, sizeof( Uint16 )*letter->surface->w * letter->surface->h );
	int x, y;
	for ( x = 0; x < letter->surface->w; x++ )
		for ( y = 0; y < letter->surface->h; y++ )
			if ( inputpixel[x + y * letter->surface->w] == oldcolor )
				pixel[x + y * letter->surface->w] = newcolor;
	free( inputpixel );
	SDL_UnlockSurface( letter->surface );
}

PREFIX void spFontReplaceColor( spFontPointer font, Uint16 oldcolor, Uint16 newcolor )
{
	spLetterReplaceColor( font->root, oldcolor, newcolor );
}

void spLetterMulWidth( spLetterPointer letter, Sint32 factor )
{
	if ( letter == NULL )
		return;
	spLetterMulWidth( letter->left, factor );
	spLetterMulWidth( letter->right, factor );
	letter->width = letter->width * factor >> SP_ACCURACY;
}

PREFIX void spFontMulWidth( spFontPointer font, Sint32 factor )
{
	spLetterMulWidth( font->root, factor );
}

static spLetterPointer spLetterFind( spLetterPointer root, Uint32 character )
{
	if ( root == NULL )
		return NULL;
	if ( character < root->character )
		return spLetterFind( root->left, character );
	if ( character > root->character )
		return spLetterFind( root->right, character );
	return root;
}

PREFIX spLetterPointer spFontGetLetter( spFontPointer font, Uint32 character )
{
	//Caching
	if ( character >= font->cacheOffset && character < font->cacheOffset + font->cache.size )
	{
		spLetterPointer result = font->cache.cache[character - font->cacheOffset];
		if ( result != ( void* )( -1 ) ) //Found
			return result;
		//Else: Save in Cache
		result = spLetterFind( font->root, character );
		if (result)
			font->cache.cache[character - font->cacheOffset] = result;
		return result;
	}
	return spLetterFind( font->root, character );
}

PREFIX spLetterPointer spFontGetButton( spFontPointer font, Uint32 character )
{
	return spLetterFind( font->buttonRoot, character );
}

PREFIX int spFontDraw( Sint32 x, Sint32 y, Sint32 z, const char* text, spFontPointer font )
{
	int l = 0;
	int pos = x;
	while ( text[l] != 0 )
	{
		if (text[l] == '\n')
		{
			pos = x;
			y+=font->maxheight;
			l++;
			continue;
		}
		spLetterPointer letter;
		//first we look for buttons
		if (text[l]==spFontButtonLeft && (letter = spLetterFind( font->buttonRoot, text[l+1])) && text[l+2]==spFontButtonRight)
			l+=2;
		else
		{
			Uint32 sign = spFontGetUnicodeFromUTF8(&(text[l]));
			letter = spFontGetLetter( font, sign );
		}
		if ( letter )
		{
			pos += letter->width >> 1;
			spBlitSurface( pos, y + letter->height / 2, z, letter->surface );
			pos += letter->width - ( letter->width >> 1 );
		}
		l+=spFontLastUTF8Length;
	}
	return pos-x;
}

PREFIX int spFontDrawRight( Sint32 x, Sint32 y, Sint32 z, const char *text, spFontPointer font )
{
	int l = 0;
	int width;
	spLetterIterPointer first = NULL;
	spLetterIterPointer last = NULL;
	int again = 1;
	while ( again )
	{
		width = 0;
		again = 0;
		while ( text[l] != 0 )
		{
			if ( text[l] == '\n' )
			{
				again = 1;
				l++;
				break;
			}
			spLetterPointer letter;
			//first we look for buttons
			if ( text[l] == spFontButtonLeft && ( letter = spLetterFind( font->buttonRoot, text[l + 1] ) ) && text[l + 2] == spFontButtonRight )
				l += 2;
			else
			{
				Uint32 sign = spFontGetUnicodeFromUTF8( &( text[l] ) );
				letter = spFontGetLetter( font, sign );
			}
			if ( letter )
			{
				spLetterIterPointer iter = ( spLetterIterPointer )malloc( sizeof( spLetterIterStruct ) );
				iter->letter = letter;
				if ( first == NULL )
				{
					first = iter;
					last = iter;
				}
				else
				{
					last->next = iter;
					last = iter;
				}
				width += letter->width;
				iter->next = NULL;
			}
			l += spFontLastUTF8Length;
		}
		int pos = x - width;
		while ( first != NULL )
		{
			pos += first->letter->width >> 1;
			spBlitSurface( pos, y + first->letter->height / 2, z, first->letter->surface );
			pos += first->letter->width - ( first->letter->width >> 1 );
			spLetterIterPointer mom = first;
			first = first->next;
			free( mom );
		}
		if ( again )
			y += font->maxheight;
	}
	return width;
}

PREFIX int spFontDrawMiddle( Sint32 x, Sint32 y, Sint32 z, const char *text, spFontPointer font )
{
	int l = 0;
	int width;
	spLetterIterPointer first = NULL;
	spLetterIterPointer last = NULL;
	int again = 1;
	while ( again )
	{
		width = 0;
		again = 0;
		while ( text[l] != 0 )
		{
			if ( text[l] == '\n' )
			{
				again = 1;
				l++;
				break;
			}
			spLetterPointer letter;
			//first we look for buttons
			if ( text[l] == spFontButtonLeft && ( letter = spLetterFind( font->buttonRoot, text[l + 1] ) ) && text[l + 2] == spFontButtonRight )
				l += 2;
			else
			{
				Uint32 sign = spFontGetUnicodeFromUTF8( &( text[l] ) );
				letter = spFontGetLetter( font, sign );
			}
			if ( letter )
			{
				spLetterIterPointer iter = ( spLetterIterPointer )malloc( sizeof( spLetterIterStruct ) );
				iter->letter = letter;
				if ( first == NULL )
				{
					first = iter;
					last = iter;
				}
				else
				{
					last->next = iter;
					last = iter;
				}
				width += letter->width;
				iter->next = NULL;
			}
			l += spFontLastUTF8Length;
		}
		int pos = x - width / 2;
		while ( first != NULL )
		{
			pos += first->letter->width >> 1;
			spBlitSurface( pos, y + first->letter->height / 2, z, first->letter->surface );
			pos += first->letter->width - ( first->letter->width >> 1 );
			spLetterIterPointer mom = first;
			first = first->next;
			free( mom );
		}
		if ( again )
			y += font->maxheight;
	}
	return width;
}

PREFIX int spFontWidth(const char* text, spFontPointer font )
{
	int l = 0;
	int width;
	int max_width = 0;
	int again = 1;
	while ( again )
	{
		width = 0;
		again = 0;
		while ( text[l] != 0 )
		{
			if ( text[l] == '\n' )
			{
				again = 1;
				l++;
				if (width > max_width)
					max_width = width;
				break;
			}
			spLetterPointer letter;
			//first we look for buttons
			if ( text[l] == spFontButtonLeft && ( letter = spLetterFind( font->buttonRoot, text[l + 1] ) ) && text[l + 2] == spFontButtonRight )
				l += 2;
			else
			{
				Uint32 sign = spFontGetUnicodeFromUTF8( &( text[l] ) );
				letter = spFontGetLetter( font, sign );
			}
			if (letter)
				width += letter->width;
			l += spFontLastUTF8Length;
		}
	}
	if (width > max_width)
		max_width = width;
	return max_width;
}

static void spLetterDelete( spLetterPointer letter )
{
	if ( letter == NULL )
		return;
	spLetterDelete( letter->left );
	spLetterDelete( letter->right );
	spDeleteSurface( letter->surface );
	free( letter );
}

PREFIX void spFontDelete( spFontPointer font )
{
	spLetterDelete( font->root );
	TTF_CloseFont( font->font );
	if ( font->cache.cache )
		free( font->cache.cache );
	free( font );
}

PREFIX void spFontChangeCacheSize( spFontPointer font, Sint32 size )
{
	if ( font->cache.cache )
		free( font->cache.cache );
	font->cache.size = size;
	font->cache.cache = ( spLetterPointer* )malloc( font->cache.size * sizeof( spLetterPointer ) );
	memset( font->cache.cache, 0xFF, font->cache.size * sizeof( spLetterPointer ) );
}

PREFIX void spFontSetCacheStart( spFontPointer font, Sint32 letter )
{
	font->cacheOffset = letter;
	memset( font->cache.cache, 0xFF, font->cache.size * sizeof( spLetterPointer ) );
}

PREFIX Sint32 spFontGetCacheStart( spFontPointer font )
{
	return font->cacheOffset;
}

PREFIX void spFontSetShadeColor(int value)
{
	spFontBackgroundColor = value;
}

PREFIX void spFontShadeButtons(int value)
{
	spFontButtonShade = value;
}

PREFIX void spFontAddEveryLetterOfTextBundle( spFontPointer font, spBundlePointer bundle,Uint16 color)
{
	//Parsing bundle
	spTextPointer momText = bundle->firstText;
	while (momText)
	{
		spTranslationPointer momTranslastion = momText->firstTranslation;
		while (momTranslastion)
		{
			spFontAdd( font, momTranslastion->text, color );
			momTranslastion = momTranslastion->next;
		}
		momText = momText->next;
	}
}
