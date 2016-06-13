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

#include "sparrowFont.h"
#include "sparrowPrimitives.h"
#include <stdlib.h>
#include <string.h>


Uint32 spFontButtonLeft = '[';
Uint32 spFontButtonRight = ']';
Uint32 spFontMapLeft = '{';
Uint32 spFontMapRight = '}';
int spFontStrategy = SP_FONT_INTELLIGENT;
int spFontLastUTF8Length = 0;
int spFontBackgroundColor = SP_FONT_NO_BORDER;
int spFontButtonShade = 0;

static void spLetterDelete( spLetterPointer letter )
{
	if ( letter == NULL )
		return;
	spLetterDelete( letter->left );
	spLetterDelete( letter->right );
	spDeleteSurface( letter->surface );
	free( letter );
}

PREFIX int spFontReload(spFontPointer font,const char* fontname,Uint32 size)
{
	spLetterDelete( font->root );
	font->root = NULL;
	spLetterDelete( font->buttonRoot );
	font->buttonRoot = NULL;
	if ( font->cache.cache )
		free( font->cache.cache );
	TTF_CloseFont( font->font );
	font->font = TTF_OpenFont( fontname, size );
	if ( !(font->font) )
	{
		printf( "Failed to load Font \"%s\", dude...\n", fontname );
		printf( "	Error was: \"%s\"\n", TTF_GetError() );
		free(font);
		return 1;
	}
	font->size = size;
	font->maxheight = 0;
	font->cacheOffset = 0;
	font->cache.cache = NULL;
	spFontChangeCacheSize( font, SP_FONT_DEFAULT_CACHE );
	return 0;
}

PREFIX spFontPointer spFontLoad(const char* fontname, Uint32 size )
{
	TTF_Font* ttf = TTF_OpenFont( fontname, size );
	if ( !ttf )
	{
		printf( "Failed to load Font \"%s\", dude...\n", fontname );
		printf( "	Error was: \"%s\"\n", TTF_GetError() );
		return NULL;
	}
	spFontPointer font = ( spFontPointer )malloc( sizeof( spFont ) );
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
	if (!surface)
		surface = spCreateSurface(0,font->maxheight);
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
	spLetterPointer letter = ( spLetterPointer )malloc( sizeof( spLetter ) );

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
		spFontLastUTF8Length = 1;
		return buffer;
	}

	if (len < 3)
		return buffer;
	if (sign < (1 << 11)) //2 Byte UTF8
	{
		buffer[0] = 128+64+(sign >> 6);
		buffer[1] = 128+	 (sign & 63);
		buffer[2] = 0;
		spFontLastUTF8Length = 2;
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
		spFontLastUTF8Length = 3;
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
		spFontLastUTF8Length = 4;
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
		surface = TTF_RenderUTF8_Shaded( font->font, caption, sdlcolor , background);
	else
		surface = TTF_RenderUTF8_Solid( font->font, caption, sdlcolor );
	int height = font->maxheight;
	//if ( height & 1 )
	//	height++;
	int width = spMax(height,surface->w+font->maxheight/2);
	//if ( width & 1 )
	//	width++;
	letter->surface = spCreateSurface(width, height);
	SDL_Surface* result = SDL_ConvertSurface( surface , letter->surface->format, letter->surface->flags);
	SDL_FreeSurface(surface);
	surface = result;
	SDL_LockSurface(letter->surface);
	int x,y;
	Uint16* pixel = (Uint16*)letter->surface->pixels;
	int w = letter->surface->pitch/letter->surface->format->BytesPerPixel;
	int R = spGetRFromColor(bgColor);
	int G = spGetGFromColor(bgColor);
	int B = spGetBFromColor(bgColor);
	int circle_width = height*9/10;
	for (x = 0; x < letter->surface->w; x++)
		for (y = 0; y < letter->surface->h; y++)
		{
			int f_width = abs(x-width/2);
			f_width = f_width - width/2 + height/2;
			f_width = spMax(0,f_width);
			if (f_width*f_width+(y-height/2)*(y-height/2) <= (circle_width+1)*(circle_width+1)/4)
			{
				int factor = (f_width*f_width+(y-height/2)*(y-height/2));
				factor *= 256;
				factor /= circle_width*circle_width/4;
				if (factor > 256)
					factor = 256;
				factor = 256 - factor/2;
				pixel[x+y*w] = spGetRGB(R*factor/256,G*factor/256,B*factor/256);
			}
			else
				pixel[x+y*w] = SP_ALPHA_COLOR;
		}
	SDL_UnlockSurface(letter->surface);
	SDL_LockSurface(surface);
	pixel = (Uint16*)surface->pixels;
	w = surface->pitch/surface->format->BytesPerPixel;
	Uint16 BGCOLOR = pixel[0];//SDL_MapRGB(surface->format,background.r,background.g,background.b);
	for (x = 0; x < surface->w; x++)
		for (y = 0; y < surface->h; y++)
			if (pixel[x+y*w] == BGCOLOR)
				pixel[x+y*w] = SP_ALPHA_COLOR;
	SDL_UnlockSurface(surface);

	SDL_Rect DestR;
	DestR.x = (width-surface->w+1)/2;
	DestR.y = (height-surface->h+1)/2;
	DestR.w = surface->w;
	DestR.h = surface->h;
	SDL_SetColorKey( surface, SDL_SRCCOLORKEY, SP_ALPHA_COLOR );
	SDL_BlitSurface( surface, NULL, letter->surface, &DestR );
	SDL_FreeSurface( surface );
	letter->height = height;
	letter->width = width;
}


PREFIX void spFontChangeArrowButton( spFontPointer font, spLetterPointer letter, Uint32 character, int direction, Uint16 fgColor, Uint16 bgColor )
{
	letter->color = fgColor;
	SDL_Color sdlcolor = {( fgColor >> 11 ) << 3, ( ( fgColor << 5 ) >> 10 ) << 2, ( ( fgColor & 31 ) << 3 )};
	SDL_Color background =	{( bgColor >> 11 ) << 3, ( ( bgColor << 5 ) >> 10 ) << 2, ( ( bgColor & 31 ) << 3 )};
	int height = font->maxheight;
	int width = height;
	letter->surface = spCreateSurface(width, height);
	SDL_LockSurface(letter->surface);
	int x,y;
	Uint16* pixel = (Uint16*)letter->surface->pixels;
	int w = letter->surface->pitch/letter->surface->format->BytesPerPixel;
	int R = spGetRFromColor(bgColor);
	int G = spGetGFromColor(bgColor);
	int B = spGetBFromColor(bgColor);
	int circle_width = height*9/10;
	for (x = 0; x < letter->surface->w; x++)
		for (y = 0; y < letter->surface->h; y++)
		{
			int f_width = abs(x-width/2);
			f_width = f_width - width/2 + height/2;
			f_width = spMax(0,f_width);
			if (f_width*f_width+(y-height/2)*(y-height/2) <= (circle_width+1)*(circle_width+1)/4)
			{
				int X = x-letter->surface->w/2;
				int Y = y-letter->surface->h/2;
				if (direction == SP_BUTTON_ARROW_LEFT && X+letter->surface->w/4 > abs(Y) && X < letter->surface->w/8)
					pixel[x+y*w] = fgColor;
				else
				if (direction == SP_BUTTON_ARROW_RIGHT && letter->surface->w/4-X > abs(Y) && X > -letter->surface->w/8)
					pixel[x+y*w] = fgColor;
				else
				if (direction == SP_BUTTON_ARROW_UP && Y+letter->surface->h/4 > abs(X) && Y < letter->surface->h/8)
					pixel[x+y*w] = fgColor;
				else
				if (direction == SP_BUTTON_ARROW_DOWN && letter->surface->h/4-Y > abs(X) && Y > -letter->surface->h/8)
					pixel[x+y*w] = fgColor;
				else
				{
					int factor = (f_width*f_width+(y-height/2)*(y-height/2));
					factor *= 256;
					factor /= circle_width*circle_width/4;
					if (factor > 256)
						factor = 256;
					factor = 256 - factor/2;
					pixel[x+y*w] = spGetRGB(R*factor/256,G*factor/256,B*factor/256);
				}
			}
			else
				pixel[x+y*w] = SP_ALPHA_COLOR;
		}
	SDL_UnlockSurface(letter->surface);
	letter->height = height;
	letter->width = width;
}

PREFIX void spFontAddButton( spFontPointer font, Uint32 character, char* caption, Uint16 fgColor, Uint16 bgColor )
{
	spLetterPointer letter = ( spLetterPointer )malloc( sizeof( spLetter ) );

	spFontChangeButton( font, letter, character, caption, fgColor, bgColor );
	letter->character = character;

	//tree insert
	font->buttonRoot = spFontInsert( letter, font->buttonRoot );
}

PREFIX void spFontAddArrowButton( spFontPointer font, Uint32 character, int direction, Uint16 fgColor, Uint16 bgColor )
{
	spLetterPointer letter = ( spLetterPointer )malloc( sizeof( spLetter ) );

	spFontChangeArrowButton( font, letter, character, direction, fgColor, bgColor );
	letter->character = character;

	//tree insert
	font->buttonRoot = spFontInsert( letter, font->buttonRoot );
}

PREFIX void spFontSetButtonBorderSigns(Uint32 left,Uint32 right)
{
	spFontButtonLeft = left;
	spFontButtonRight = right;
}

PREFIX void spFontSetSemanticButtonBorderSigns(Uint32 left,Uint32 right)
{
	spFontMapLeft = left;
	spFontMapRight = right;
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
	letter->width+=2;
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

static spLetterPointer spLetterFindMap( spLetterPointer root, spLetterPointer buttonRoot,const char* button, int *length)
{
	if ( root == NULL )
		return NULL;
	//Getting the semantic button
	char temp[64];
	int i;
	for (i = 0; i < 64; i++)
	{
		if (button[i] == spFontMapRight)
			break;
		temp[i] = button[i];
	}
	if (i == 64)
	{
		*length = 0;
		return spLetterFind(root,spFontMapLeft);
	}

	temp[i] = 0;
	*length = i+1;
	//Finding the button:
	unsigned char* realButton = spMapButtonByName(temp);
	if (realButton == NULL || strcmp(realButton,"None") == 0)
	{
		*length = 0;
		return spLetterFind(root,spFontMapLeft);
	}
	//a button?
	if (realButton[0] == spFontButtonLeft)
	{
		spLetterPointer letter;
		if (realButton[2] == spFontButtonRight)
			letter = spLetterFind(buttonRoot,realButton[1]);
		if (letter)
			return letter;
		*length = 0;
		return spLetterFind(root,spFontButtonLeft);
	}
	return NULL;
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

PREFIX int spFontDraw( Sint32 x, Sint32 y, Sint32 z, const unsigned char* text, spFontPointer font )
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
		int l_add = 0;
		if (text[l]==spFontButtonLeft && (letter = spLetterFind( font->buttonRoot, text[l+1])) && text[l+2]==spFontButtonRight)
			l+=3;
		else
		if (text[l]==spFontMapLeft && (letter = spLetterFindMap( font->root, font->buttonRoot, &(text[l+1]), &l_add)))
			l+=l_add+1;
		else
		{
			Uint32 sign = spFontGetUnicodeFromUTF8(&(text[l]));
			letter = spFontGetLetter( font, sign );
			l+=spFontLastUTF8Length;
		}
		if ( letter )
		{
			pos += letter->width >> 1;
			spBlitSurface( pos, y + letter->height / 2, z, letter->surface );
			pos += letter->width - ( letter->width >> 1 );
		}
	}
	return pos-x;
}

/* Needed by spFontDrawRight and spFontDrawMiddle */
typedef struct spLetterIterStruct *spLetterIterPointer;
typedef struct spLetterIterStruct
{
	spLetterPointer letter;
	spLetterIterPointer next; //For the Drawing functions
} spLetterIter;

PREFIX int spFontDrawRight( Sint32 x, Sint32 y, Sint32 z, const unsigned char *text, spFontPointer font )
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
			int l_add;
			if ( text[l] == spFontButtonLeft && ( letter = spLetterFind( font->buttonRoot, text[l + 1] ) ) && text[l + 2] == spFontButtonRight )
				l += 3;
			else
			if (text[l] == spFontMapLeft && (letter = spLetterFindMap( font->root, font->buttonRoot, &(text[l+1]), &l_add)))
				l+=l_add+1;
			else
			{
				Uint32 sign = spFontGetUnicodeFromUTF8( &( text[l] ) );
				letter = spFontGetLetter( font, sign );
				l += spFontLastUTF8Length;
			}
			if ( letter )
			{
				spLetterIterPointer iter = ( spLetterIterPointer )malloc( sizeof( spLetterIter ) );
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

PREFIX int spFontDrawMiddle( Sint32 x, Sint32 y, Sint32 z, const unsigned char *text, spFontPointer font )
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
			int l_add;
			if ( text[l] == spFontButtonLeft && ( letter = spLetterFind( font->buttonRoot, text[l + 1] ) ) && text[l + 2] == spFontButtonRight )
				l += 3;
			else
			if (text[l] == spFontMapLeft && (letter = spLetterFindMap( font->root, font->buttonRoot, &(text[l+1]), &l_add)))
				l+=l_add+1;
			else
			{
				Uint32 sign = spFontGetUnicodeFromUTF8( &( text[l] ) );
				letter = spFontGetLetter( font, sign );
				l += spFontLastUTF8Length;
			}
			if ( letter )
			{
				spLetterIterPointer iter = ( spLetterIterPointer )malloc( sizeof( spLetterIter ) );
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

PREFIX int spFontWidth(const unsigned char* text, spFontPointer font )
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
			int l_add;
			if ( text[l] == spFontButtonLeft && ( letter = spLetterFind( font->buttonRoot, text[l + 1] ) ) && text[l + 2] == spFontButtonRight )
				l += 3;
			else
			if (text[l] == spFontMapLeft && (letter = spLetterFindMap( font->root, font->buttonRoot, &(text[l+1]), &l_add)))
				l+=l_add+1;
			else
			{
				Uint32 sign = spFontGetUnicodeFromUTF8( &( text[l] ) );
				letter = spFontGetLetter( font, sign );
				l += spFontLastUTF8Length;
			}
			if (letter)
				width += letter->width;
		}
	}
	if (width > max_width)
		max_width = width;
	return max_width;
}

PREFIX void spFontDelete( spFontPointer font )
{
	spLetterDelete( font->root );
	spLetterDelete( font->buttonRoot );
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

typedef struct spInternalTextLineStruct *spInternalTextLinePointer;
typedef struct spInternalTextLineStruct
{
	int count;
	int width;
	char* text;
	spInternalTextLinePointer next;
} spInternalTextLine;

PREFIX spTextBlockPointer spCreateTextBlock( const unsigned char* text, int max_width, spFontPointer font)
{
	//Cutting at spaces. If line break, do!
	if (text == NULL)
		return 0;
	spInternalTextLinePointer firstLine = NULL;
	int l = strlen(text)+1;
#ifdef _MSC_VER
	char* buffer = (char*)malloc( l * sizeof(char) );
#else
	char buffer[l];
#endif
	memcpy(buffer,text,l);
	int pos = 0;
	int last_space = 0;
	int space_count = 0;
	int start = 0;
	int count = 0;
	while (1)
	{
		char c;
		int width;
		if (buffer[start] == '\n')
			pos = start;
		if (buffer[pos] == ' ' || buffer[pos] == '\n' || buffer[pos] == 0)
		{
			c = buffer[pos];
			buffer[pos] = 0;
			width = spFontWidth(&buffer[start],font);
			buffer[pos] = c;
			if (width > max_width || buffer[pos]!=' ')
			{
				spInternalTextLinePointer newLine = (spInternalTextLinePointer)malloc(sizeof(spInternalTextLine));
				newLine->next = firstLine;
				firstLine = newLine;
				count++;
				if ( width <= max_width )
					last_space = pos;
				if (space_count > 0 || width <= max_width)
				{
					c = buffer[last_space];
					buffer[last_space] = 0;
					newLine->width = spFontWidth(&buffer[start],font);
					newLine->count = (last_space-start);
					newLine->text = (char*)malloc(newLine->count+1);
					memcpy(newLine->text,&buffer[start],newLine->count+1);
					buffer[last_space] = c;
					if (buffer[last_space] == 0)
						break;
					pos = last_space+1;
				}
				else
				{
					//Searching, when to do the "line break"
					char buffer_with_minus[65535] = "-";
					int b_pos = -1;
					pos = start-1;
					width = spFontWidth(buffer_with_minus,font);
					while (width <= max_width)
					{
						b_pos++;
						pos++;
						buffer_with_minus[b_pos] = buffer[pos];
						buffer_with_minus[b_pos+1] = '-';
						buffer_with_minus[b_pos+2] = 0;
						width = spFontWidth(buffer_with_minus,font);
					}
					buffer_with_minus[b_pos] = '-';
					buffer_with_minus[b_pos+1] = 0;
					newLine->width = spFontWidth(buffer_with_minus,font);
					newLine->count = (pos-start);
					newLine->text = (char*)malloc(newLine->count+2);//+2 because of the -
					memcpy(newLine->text,buffer_with_minus,newLine->count+2);
					if (buffer[pos] == 0)
						break;
					last_space = pos;
				}
				start=pos;
				space_count = 0;
			}
			else
			{
				last_space = pos;
				space_count++;
			}
		}
		pos++;
	}
	spTextBlockPointer block = (spTextBlockPointer)malloc(sizeof(spTextBlock));
	block->line_count = count;
	block->max_width = max_width;
	block->line = (spTextLinePointer)malloc(count*sizeof(spTextLine));
	int i = count-1;
	while (firstLine)
	{
		spInternalTextLinePointer next = firstLine->next;
		block->line[i].font = font;
		block->line[i].count = firstLine->count;
		block->line[i].width = firstLine->width;
		block->line[i].text = firstLine->text;
		free(firstLine);
		i--;
		firstLine = next;
	}
	return block;
}

PREFIX void spDeleteTextBlock(spTextBlockPointer block)
{
	if (block)
	{
		int i;
		for (i = 0; i < block->line_count; i++)
			free(block->line[i].text);
		free(block->line);
		free(block);
	}
}

PREFIX int spFontDrawTextBlock(spTextBlockAlignment alignment,Sint32 x, Sint32 y, Sint32 z, spTextBlockPointer block, Sint32 height, Sint32 position, spFontPointer font )
{
	if (block == NULL)
		return -1;
	if (block->line_count < 1)
		return -1;
	int showable_lines = height/(font?font->maxheight:block->line[0].font->maxheight);
	int unseen_lines = spMax(0,block->line_count - showable_lines);
	int start_line = spFixedRoundInt(position*unseen_lines);
	int end_line = spMin(start_line+showable_lines,block->line_count);
	int i;
	spFontPointer used_font;
	for (i = start_line; i < end_line; i++)
	{
		if (font)
			used_font = font;
		else
			used_font = block->line[i].font;
		switch (alignment)
		{
			case left:
				spFontDraw(x,y,z,block->line[i].text,used_font);
				break;
			case middle:
				spFontDraw(x+(block->max_width-block->line[i].width)/2,y,z,block->line[i].text,used_font);
				break;
			case right:
				spFontDraw(x+block->max_width-block->line[i].width,y,z,block->line[i].text,used_font);
				break;
		}
		y+=used_font->maxheight;
	}
	return end_line-start_line;
}

char* add_letter_to_string(spLetterPointer letter,char* buffer,int* length)
{
	if (*length < 5)
		return buffer;
	char buffer2[5];
	sprintf(buffer,"%s",spFontGetUTF8FromUnicode(letter->character,buffer2,5));
	*length -= spFontLastUTF8Length;
	buffer = &buffer[spFontLastUTF8Length];
	if (letter->left)
		buffer = add_letter_to_string(letter->left,buffer,length);
	if (letter->right)
		buffer = add_letter_to_string(letter->right,buffer,length);
	return buffer;
}

PREFIX char* spFontGetLetterString(spFontPointer font,unsigned char* buffer,int length)
{
	if (buffer == NULL || length==0)
		return 0;
	if (font == NULL || font->root == NULL)
	{
		buffer[0] = 0;
		return 0;
	}
	add_letter_to_string(font->root,buffer,&length);
	return buffer;
}
