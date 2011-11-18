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

PREFIX spFontPointer spFontLoad(char* fontname,Uint32 size)
{
  TTF_Font* ttf = TTF_OpenFont(fontname,size);
  if (!ttf)
  {
    printf("Failed to load Font %s, dude...\n",fontname);
    printf("  Error was: \"%s\"\n",TTF_GetError());
    return NULL;
  }
  spFontPointer font = (spFontPointer)malloc(sizeof(spFontStruct));
  font->font = ttf;
  font->root = NULL;
  font->size = size;
  return font;
}

Sint32 spLetterGetHeight(spLetterPointer letter)
{
  return letter ? letter->binary_height : -1;
}

spLetterPointer spLetterRotateLeft(spLetterPointer K2)
{
  spLetterPointer K1; 

  K1 = K2->left; 
  K2->left = K1->right; 
  K1->right = K2; 

  K2->binary_height = spMax( spLetterGetHeight( K2->left ), spLetterGetHeight( K2->right ) ) + 1; 
  K1->binary_height = spMax( spLetterGetHeight( K1->left ), spLetterGetHeight( K2        ) ) + 1; 

  return K1;
}

spLetterPointer spLetterRotateRight(spLetterPointer K1)
{
  spLetterPointer K2; 

  K2 = K1->right; 
  K1->right = K2->left; 
  K2->left = K1; 

  K1->binary_height = spMax( spLetterGetHeight( K1->left  ), spLetterGetHeight( K1->right ) ) + 1; 
  K2->binary_height = spMax( spLetterGetHeight( K2->right ), spLetterGetHeight( K1        ) ) + 1; 

  return K2;
}

spLetterPointer spLetterDoubleLeft(spLetterPointer K3)
{
  K3->left = spLetterRotateRight( K3->left ); 
  return spLetterRotateLeft( K3 );
}

spLetterPointer spLetterDoubleRight(spLetterPointer K3)
{
  K3->right = spLetterRotateLeft( K3->right ); 
  return spLetterRotateRight( K3 );
}

spLetterPointer spFontInsert(spLetterPointer letter,spLetterPointer root)
{
  if( root == NULL )
  {
    root = letter;
    letter->binary_height = 0;
    letter->left = NULL;
    letter->right = NULL; 
  }
  else
  if( letter->character < root->character )
  {
    root->left = spFontInsert( letter, root->left );
    if( spLetterGetHeight( root->left ) - spLetterGetHeight( root->right ) == 2 )
      if( letter->character < root->left->character )
        root = spLetterRotateLeft( root );
      else
        root = spLetterDoubleLeft( root );
  }
  else
  if( letter->character > root->character )
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

PREFIX void spFontAdd(spFontPointer font,Uint32 character,Uint16 color)
{
  spLetterPointer letter = (spLetterPointer)malloc(sizeof(spLetterStruct));
  letter->character = character;
  char buffer[5];
  buffer[0]=character;
  buffer[1]=0; //TODO: utf8
  SDL_Color sdlcolor = {(color>>11)<<3,((color<<5)>>10)<<2,((color & 31)<<3)};
  SDL_Surface* surface = TTF_RenderUTF8_Solid(font->font,buffer,sdlcolor);
  int width = surface->w+SP_FONT_EXTRASPACE*2;
  if (width & 1)
    width++;
  SDL_Surface* dummy = SDL_CreateRGBSurface(SDL_SWSURFACE,width,surface->h+SP_FONT_EXTRASPACE*2,16, 0 , 0 , 0 ,  0);
  SDL_FillRect(dummy,NULL,SP_ALPHA_COLOR);

  SDL_Rect DestR;
  DestR.x = SP_FONT_EXTRASPACE;
  DestR.y = SP_FONT_EXTRASPACE;
  DestR.w = surface->w;
  DestR.h = surface->h;
  SDL_BlitSurface(surface, NULL, dummy, &DestR);
  
  letter->surface = SDL_DisplayFormat(dummy);
  SDL_FreeSurface(dummy);
  SDL_FreeSurface(surface);
  
  TTF_SizeUTF8(font->font,buffer,&(letter->width),NULL);
  
  //tree insert
  font->root = spFontInsert(letter,font->root);
}

PREFIX void spFontAddRange(spFontPointer font,Uint32 from,Uint32 to,Uint16 color)
{
  Uint32 letter;
  for (letter = from; letter <= to; letter++)
    spFontAdd(font,letter,color);
}

spLetterPointer spLetterFind(spLetterPointer root,Uint32 character)
{
  if (root == NULL)
    return NULL;
  
  if (character < root->character)
    return spLetterFind(root->left,character);
  if (character > root->character)
    return spLetterFind(root->right,character);
  return root;
}

PREFIX spLetterPointer spFontGetLetter(spFontPointer font,Uint32 character)
{
  return spLetterFind(font->root,character);
}


PREFIX void spFontDelete(spFontPointer font)
{
  //TODO: Implement
}
