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
 * files, creating folders, etc. Use only /! not \ ... For more information
 * about file functions see http://www.libsdl.org/cgi/docwiki.cgi/SDL_API
 * and scroll down to "Files (RWops)". Keep in mind: spFilePointer and
 * SDL_RWops* are EXACTLY the same. ;-) */

#ifndef _SPARROW_FILE_H
#define _SPARROW_FILE_H
#include "sparrow3d.h"
#include <SDL.h>

#ifndef getLastError
#define getLastError GetLastError
#endif

typedef enum
{
	SP_FILE_EVERYTHING_OK = 0,
	SP_FILE_ACCESS_ERROR = 1,
	SP_FILE_NOT_FOUND_ERROR = 2,
	SP_FILE_ALREADY_EXISTS_ERROR = 3
} spFileError;

//Just because it looks better :)
typedef SDL_RWops *spFilePointer;

/* spFileExists tests, whether the file "filename" exists ;-) */
PREFIX int spFileExists( const char* filename );

/* spReadOneLine reads one line from a SDL_RWops file. This line is
 * written to buffer. buffer_len is the length of buffer (with zero
 * byte!). If the end of file is reached, 1 is return, else 0.*/
PREFIX int spReadOneLine( spFilePointer file , char* buffer, int buffer_len);

/* spReadUntil reads signs from the file "file" until the buffer is full
 * (buffer_len) or the sign "end_sign" is reached. The sign before 
 * "end_sign" is the last sign of the string! If you read more signs
 * from the file, the sign AFTER "end_sign" is the next you will read.
 * It is usefull for parsing simple XML files. Like spReadOneLine it
 * returns 1 if eof is reached, else 0. Windows uses 2 signs for line breaks
 * (VERY smart...), so if you set ignore_windows_return to 1, \r will be
 * ignored. Necessary for Windows textfiles, but for binary files it doesn't
 * make sense maybe.*/
PREFIX int spReadUntil( spFilePointer file , char* buffer, int buffer_len, char end_sign,char ignore_windows_return);

/* Creates a directory chain (like /home/user/.config/pinball/settings/) if
 * it not exists already. */
PREFIX spFileError spCreateDirectoryChain( const char* directories);

/* Removes the file filename. Returns 0 at success, otherwise see spFileError */
PREFIX spFileError spRemoveFile( const char* filename );

/* Removes the directory dirname. Returns 0 at success, otherwise see
 * spFileError */
PREFIX spFileError spRemoveDirectory( const char* dirname );

/* Rename the file filename the newname. Returns 0 at success, otherwise see
 * spFileError. You can use the same function for files and directories. That
 * is the reason, why I just defined another name for the same function. ;-) */
PREFIX spFileError spRenameFile( const char* filename , const char* newname);
#define spRenameDirectory spRenameFile

#endif
