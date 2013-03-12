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

typedef enum
{
	SP_FILE_EVERYTHING_OK = 0,
	SP_FILE_ACCESS_ERROR = 1,
	SP_FILE_NOT_FOUND_ERROR = 2,
	SP_FILE_ALREADY_EXISTS_ERROR = 3,
	SP_FILE_INVALID_PARAMETER_ERROR = 4,
	SP_FILE_UNKNOWN_ERROR = 5
} spFileError;

typedef enum
{
	SP_FILE_FILE = 0,
	SP_FILE_DIRECTORY = 1,
	SP_FILE_LINK = 2,
} spFileType;

typedef enum
{
	SP_FILE_SORT_BY_NAME = 0,
	SP_FILE_SORT_BY_TYPE = 1,
	SP_FILE_SORT_BY_TYPE_AND_NAME = 2,
	SP_FILE_SORT_BACKWARDS = 4
} spFileSortType;
	

//Just because it looks better :)
typedef SDL_RWops *spFilePointer;

typedef struct spFileListStruct *spFileListPointer;
typedef struct spFileListStruct {
	char name[256];
	spFileType type;
	spFileListPointer prev;
	spFileListPointer next;
	int count; //only valid for the first element!
} spFileList;

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

/* Renames the file filename to newname. Returns 0 at success, otherwise see
 * spFileError. You can use the same function for files and directories. That
 * is the reason, why I just defined another name for the same function. ;-) */
PREFIX spFileError spRenameFile( const char* filename , const char* newname);
#define spRenameDirectory spRenameFile

/* Searching in given directory (recursive = 0) or given directory and all
 * subdirectories (recursives = 1). Be carefull with infinite directory-loops!
 * Returns a double linked lists of files names with file names based on the
 * given directory. Set no_hidden_files to 1, if you don't want hidden files to
 * be in the list like .git directoriews... However: The functions sets pointer
 * to NULL, if the directory is empty. Returns spFileError.*/
PREFIX spFileError spFileGetDirectory(spFileListPointer* pointer, char* directory,int recursive,int no_hidden_files);

/* Deletes a list created by spFileGetDirectory.*/
PREFIX void spFileDeleteList(spFileListPointer list);

/* Sorts the file list like you want. The possible sort pattern are
 * (self-explanatory):
 * - SP_FILE_SORT_BY_NAME
 * - SP_FILE_SORT_BY_TYPE
 * - SP_FILE_SORT_BY_TYPE_AND_NAME
 * Add SP_FILE_SORT_BACKWARDS to sort backwards */
PREFIX void spFileSortList(spFileListPointer* list,spFileSortType sortBy);

#endif
