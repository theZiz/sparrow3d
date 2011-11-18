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

#define SP_FONT_EXTRASPACE 8

/*a letter in a binary tree of a font*/
typedef struct spLetterStruct_ *spLetterPointer;
typedef struct spLetterStruct_
{
  Uint32 character; //later it could be utf8 characters
  SDL_Surface* surface; //the character's surface
  Sint32 width; //the character's length
  Sint32 binary_height; //NOT the height of the letter! just for the binary tree
  spLetterPointer left,right; //smaller and bigger character
} spLetterStruct;

/*root of a binary tree of all letters in this font*/
typedef struct spFontStruct_ *spFontPointer;
typedef struct spFontStruct_
{
  TTF_Font* font;
  spLetterPointer root; //the root of the binary letter tree
  Uint32 size;
} spFontStruct;

PREFIX spFontPointer spFontLoad(char* fontname,Uint32 size);

PREFIX void spFontAdd(spFontPointer font,Uint32 character,Uint16 color);

PREFIX void spFontAddRange(spFontPointer font,Uint32 from,Uint32 to,Uint16 color);

PREFIX spLetterPointer spFontGetLetter(spFontPointer font,Uint32 character);

PREFIX void spFontDelete(spFontPointer font);

#endif
