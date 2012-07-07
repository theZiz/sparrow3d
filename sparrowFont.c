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
#include "sparrowFont.h"
#include "sparrowPrimitives.h"
#include <stdlib.h>

Uint32 spFontButtonLeft = '[';
Uint32 spFontButtonRight = ']';
int spFontStrategy = SP_FONT_INTELLIGENT;

PREFIX spFontPointer spFontLoad(const char* fontname, Uint32 size )
{
	TTF_Font* ttf = TTF_OpenFont( fontname, size );
	if ( !ttf )
	{
		printf( "Failed to load Font \"%s\", dude...\n", fontname );
		printf( "  Error was: \"%s\"\n", TTF_GetError() );
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

Sint32 spLetterGetHeight( spLetterPointer letter )
{
	return letter ? letter->binary_height : -1;
}

spLetterPointer spLetterRotateLeft( spLetterPointer K2 )
{
	spLetterPointer K1;

	K1 = K2->left;
	K2->left = K1->right;
	K1->right = K2;

	K2->binary_height = spMax( spLetterGetHeight( K2->left ), spLetterGetHeight( K2->right ) ) + 1;
	K1->binary_height = spMax( spLetterGetHeight( K1->left ), spLetterGetHeight( K2        ) ) + 1;

	return K1;
}

spLetterPointer spLetterRotateRight( spLetterPointer K1 )
{
	spLetterPointer K2;

	K2 = K1->right;
	K1->right = K2->left;
	K2->left = K1;

	K1->binary_height = spMax( spLetterGetHeight( K1->left  ), spLetterGetHeight( K1->right ) ) + 1;
	K2->binary_height = spMax( spLetterGetHeight( K2->right ), spLetterGetHeight( K1        ) ) + 1;

	return K2;
}

spLetterPointer spLetterDoubleLeft( spLetterPointer K3 )
{
	K3->left = spLetterRotateRight( K3->left );
	return spLetterRotateLeft( K3 );
}

spLetterPointer spLetterDoubleRight( spLetterPointer K3 )
{
	K3->right = spLetterRotateLeft( K3->right );
	return spLetterRotateRight( K3 );
}

spLetterPointer spFontInsert( spLetterPointer letter, spLetterPointer root )
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
	char buffer[5];
	buffer[0] = character;
	buffer[1] = 0; //TODO: utf8
	SDL_Color sdlcolor = {( color >> 11 ) << 3, ( ( color << 5 ) >> 10 ) << 2, ( ( color & 31 ) << 3 )};
	SDL_Surface* surface = TTF_RenderUTF8_Solid( font->font, buffer, sdlcolor );
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

	TTF_SizeUTF8( font->font, buffer, &( letter->width ), &( letter->height ) );
	if ( font->maxheight < letter->height )
		font->maxheight = letter->height;
}

PREFIX void spFontAdd( spFontPointer font, Uint32 character, Uint16 color )
{
	spLetterPointer letter = ( spLetterPointer )malloc( sizeof( spLetterStruct ) );

	spFontChangeLetter( font, letter, character, color );
	letter->character = character;

	//tree insert
	font->root = spFontInsert( letter, font->root );
}

PREFIX void spFontAddRange( spFontPointer font, Uint32 from, Uint32 to, Uint16 color )
{
	Uint32 letter;
	for ( letter = from; letter <= to; letter++ )
		spFontAdd( font, letter, color );
}

int spFontCorrectStrategy(char* button)
{
  if (spFontStrategy != SP_FONT_INTELLIGENT)
    return spFontStrategy;
  #if defined GP2X || defined CAANOO || defined DINGUX || defined PANDORA
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
  int width;
  if (spFontCorrectStrategy(caption) == SP_FONT_BUTTON)
  {
    char buffer[5];
    buffer[0] = character;
    buffer[1] = 0; //TODO: utf8
    SDL_Surface* surface = TTF_RenderUTF8_Solid( font->font, caption, sdlcolor );
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

    SDL_FreeSurface( surface );
  }
  else
  {
    int buttonWidth = font->maxheight + SP_FONT_EXTRASPACE * 2;
    int border = buttonWidth/14;
    SDL_Surface* surface = TTF_RenderUTF8_Solid( font->font, caption, sdlcolor );
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
        if (x < border        || y < border ||
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

    SDL_FreeSurface( surface );
  }

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

void spLetterAddBorder( spLetterPointer letter, Uint16 bordercolor )
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
}

void spLetterReplaceColor( spLetterPointer letter, Uint16 oldcolor, Uint16 newcolor )
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

spLetterPointer spLetterFind( spLetterPointer root, Uint32 character )
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
		font->cache.cache[character - font->cacheOffset] = result;
		return result;
	}
	return spLetterFind( font->root, character );
}

PREFIX void spFontDraw( Sint32 x, Sint32 y, Sint32 z, const char* text, spFontPointer font )
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
      letter = spFontGetLetter( font, text[l] ); //TODO utf8
		if ( letter )
		{
			pos += letter->width >> 1;
			spBlitSurface( pos, y + letter->height / 2, z, letter->surface );
			pos += letter->width - ( letter->width >> 1 );
		}
		l++;
	}
}

PREFIX void spFontDrawRight( Sint32 x, Sint32 y, Sint32 z,const char* text, spFontPointer font )
{
	int l = 0;
	int width;
	spLetterIterPointer first = NULL;
	spLetterIterPointer last = NULL;
	int again = 1;
  while (again)
  {
		width = 0;
		again = 0;
		while ( text[l] != 0 )
		{
			if (text[l] == '\n')
			{
				again = 1;
				l++;
				break;
			}    
      spLetterPointer letter;
      //first we look for buttons
      if (text[l]==spFontButtonLeft && (letter = spLetterFind( font->buttonRoot, text[l+1])) && text[l+2]==spFontButtonRight)
        l+=2;
      else
        letter = spFontGetLetter( font, text[l] ); //TODO utf8
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
			l++;
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
		if (again)
			y+=font->maxheight;
	}
}

PREFIX void spFontDrawMiddle( Sint32 x, Sint32 y, Sint32 z,const char* text, spFontPointer font )
{
	int l = 0;
	int width;
	spLetterIterPointer first = NULL;
	spLetterIterPointer last = NULL;
	int again = 1;
  while (again)
  {
		width = 0;
		again = 0;
		while ( text[l] != 0 )
		{
			if (text[l] == '\n')
			{
				again = 1;
				l++;
				break;
			}
      spLetterPointer letter;
      //first we look for buttons
      if (text[l]==spFontButtonLeft && (letter = spLetterFind( font->buttonRoot, text[l+1])) && text[l+2]==spFontButtonRight)
        l+=2;
      else
        letter = spFontGetLetter( font, text[l] ); //TODO utf8
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
			l++;
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
		if (again)
		  y+=font->maxheight;
	}
}

PREFIX int spFontWidth(const char* text, spFontPointer font )
{
	int width = 0;
	int l = 0;
	while ( text[l] != 0 )
	{
		spLetterPointer letter = spFontGetLetter( font, text[l] ); //TODO utf8
		if ( letter )
			width += letter->width;
		l++;
	}
	return width;
}

void spLetterDelete( spLetterPointer letter )
{
	if ( letter == NULL )
		return;
	spLetterDelete( letter->left );
	spLetterDelete( letter->right );
	SDL_FreeSurface( letter->surface );
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
