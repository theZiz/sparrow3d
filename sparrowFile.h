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

/* sparrowFile is for file handling. Most basic tasks (opening, reading a
 * amount of bytes, closing) are already done by SDL. sparrowFile extends this
 * to checking, whether a file exists, Reading until specific signs, searching
 * files, creating folders, etc. */
#ifndef _SPARROW_FILE_H
#define _SPARROW_FILE_H
#include "sparrowDefines.h"
#include <SDL.h>


/* spFileExists tests, whether the file "filename" exists ;-) */
PREFIX int spFileExists( char* filename );

/* spReadOneLine reads one line from a SDL_RWops file. This line is
 * written to buffer. buffer_len is the length of buffer (with zero
 * byte!). If the end of file is reached, 1 is return, else 0.*/
PREFIX int spReadOneLine( SDL_RWops *file , char* buffer, int buffer_len);

/* spReadUntil reads signs from the file "file" until the buffer is full
 * (buffer_len) or the sign "end_sign" is reached. The sign before 
 * "end_sign" is the last sign of the string! If you read more signs
 * from the file, the sign AFTER "end_sign" is the next you will read.
 * It is usefull for parsing simple XML files. Like spReadOneLine it
 * returns 1 if eof is reached, else 0. Windows uses 2 signs for line breaks
 * (VERY smart...), so if you set ignore_windows_return to 1, \r will be
 * ignored. Necessary for Windows textfiles, but for binary files it doesn't
 * make sense maybe.*/
PREFIX int spReadUntil( SDL_RWops *file , char* buffer, int buffer_len, char end_sign,char ignore_windows_return);

#endif
