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

/* File: sparrowFile
 * 
 * SparrowFile is for file handling. Most basic tasks (opening, reading a
 * amount of bytes, closing) are already done by SDL. sparrowFile extends this
 * to checking whether a file exists, reading until specific signs, searching
 * files, creating folders, etc. Use only /! not \ ... For more information
 * about file functions see http://www.libsdl.org/cgi/docwiki.cgi/SDL_API
 * and scroll down to "Files (RWops)". Keep in mind: spFilePointer and
 * SDL_RWops* are EXACTLY the same. ;-) */

#ifndef _SPARROW_FILE_H
#define _SPARROW_FILE_H
#include "sparrow3d.h"
#include <SDL.h>

/* enum: spFileError
 * 
 * Enumeration for errors while working with files
 * 
 * SP_FILE_EVERYTHING_OK - no error, everything ok
 * SP_FILE_ACCESS_ERROR - error while accessing the file
 * SP_FILE_NOT_FOUND_ERROR - the file coudln't be found
 * SP_FILE_ALREADY_EXISTS_ERROR - the file already exists
 * SP_FILE_INVALID_PARAMETER_ERROR - some invalid parameter like a file as
 * parameter if a path is needed
 * SP_FILE_UNKNOWN_ERROR - unknown error */
typedef enum
{
	SP_FILE_EVERYTHING_OK = 0,
	SP_FILE_ACCESS_ERROR = 1,
	SP_FILE_NOT_FOUND_ERROR = 2,
	SP_FILE_ALREADY_EXISTS_ERROR = 3,
	SP_FILE_INVALID_PARAMETER_ERROR = 4,
	SP_FILE_UNKNOWN_ERROR = 5
} spFileError;

/* enum: spFileType
 * 
 * Enumeration for file types
 * 
 * SP_FILE_FILE - a normal file
 * SP_FILE_DIRECTORY - a directory
 * SP_FILE_LINK - a link to a file or a directory */
typedef enum
{
	SP_FILE_FILE = 0,
	SP_FILE_DIRECTORY = 1,
	SP_FILE_LINK = 2,
} spFileType;

/* enum: spFileSortType
 * 
 * This enumeration is needed for sorting files when searching in a folder
 * 
 * SP_FILE_SORT_BY_NAME - Sorting by name
 * SP_FILE_SORT_BY_TYPE - Sorting by type
 * SP_FILE_SORT_BY_TYPE_AND_NAME - Sort by both
 * SP_FILE_SORT_BACKWARDS - Combine this with the above to sort backwards */
typedef enum
{
	SP_FILE_SORT_BY_NAME = 0,
	SP_FILE_SORT_BY_TYPE = 1,
	SP_FILE_SORT_BY_TYPE_AND_NAME = 2,
	SP_FILE_SORT_BACKWARDS = 4
} spFileSortType;
	

//type: spFilePointer
//Same lik SDL_RWops*, but looks better ;)
typedef SDL_RWops *spFilePointer;

/* type: spFileList
 * 
 * Linked list type for searching results, which contains found files.
 * 
 * Variables:
 * name - filename
 * type - filetype, see <spFileType>
 * prev - previous found
 * next - next found
 * count - only valid for the first element, describes the number of found files*/
typedef struct spFileListStruct *spFileListPointer;
typedef struct spFileListStruct {
	char name[256];
	spFileType type;
	spFileListPointer prev;
	spFileListPointer next;
	int count; //only valid for the first element!
} spFileList;

/* Function: spFileExists
 * 
 * Tests, whether the file "filename" exists ;-)
 * 
 * Parameters:
 * filename - the filename to check
 * 
 * Returns:
 * int - 1 if the file exists, 0 if not*/
PREFIX int spFileExists( const char* filename );

/* Function: spReadOneLine
 * 
 * Reads one line from a SDL_RWops file. 
 * 
 * Parameters:
 * file - the file to read
 * buffer - where the file content is written to
 * buffer_len - the length of buffer (+ zero byte!).
 * 
 * Returns:
 * int - If the end of file is reached, 1 is return, else 0*/
PREFIX int spReadOneLine( spFilePointer file , char* buffer, int buffer_len);

/* Function: spReadUntil
 * 
 * Reads signs from the file "file" until the buffer is full
 * (buffer_len) or the sign "end_sign" is reached. The sign before 
 * "end_sign" is the last sign of the string! If you read more signs
 * from the file, the sign AFTER "end_sign" is the next you will read.
 * It is usefull for parsing simple XML files. 
 * 
 * Parameters:
 * file - the file to be read
 * buffer - the buffer for reading signs
 * buffer_len - the length of the buffer
 * end_sign - the sign to which it will be read
 * ignore_windows_return - Windows uses 2 signs for line breaks (VERY smart...),
 * so if you set ignore_windows_return to 1, \r will be ignored. Necessary for
 * Windows text files, but for binary files it doesn't make sense at all.
 * 
 * Returns:
 * int - Like spReadOneLine it returns 1 if eof is reached, else 0.*/
PREFIX int spReadUntil( spFilePointer file , char* buffer, int buffer_len, char end_sign,char ignore_windows_return);

/* Function: spCreateDirectoryChain
 * 
 * Creates a directory chain (like /home/user/.config/pinball/settings/) if
 * it not exists already. That means if you create e.g. rainbow/dash, the folder
 * rainbow will be created if it doesn't exist.
 * 
 * Parameters:
 * directories - the path of the directory to create
 * 
 * Returns:
 * spFileError - <SP_FILE_EVERYTHING_OK> if no error, else see
 * <spFileError>*/
PREFIX spFileError spCreateDirectoryChain( const char* directories);

/* Function: spRemoveFile
 * 
 * Removes a file
 * 
 * Parameters:
 * filename - the name of the file
 * 
 * Returns:
 * spFileError - <SP_FILE_EVERYTHING_OK> if no error, else see
 * <spFileError>
 * 
 * See Also:
 * <spRemoveDirectory>*/
PREFIX spFileError spRemoveFile( const char* filename );

/* Function: spRemoveDirectory
 * 
 * Removes a directory
 * 
 * Parameters:
 * dirname - the name of the directory
 * 
 * Returns:
 * spFileError - <SP_FILE_EVERYTHING_OK> if no error, else see
 * <spFileError>
 * 
 * See Also:
 * <spRemoveFile>*/
PREFIX spFileError spRemoveDirectory( const char* dirname );

/* Function: spRenameFile
 * 
 * Renames a file
 * 
 * Parameters:
 * filename - the old filename
 * newname - the new filename
 * 
 * Returns:
 * spFileError - <SP_FILE_EVERYTHING_OK> if no error, else see
 * <spFileError>
 * 
 * See Also:
 * <spRenameDirectory>*/
PREFIX spFileError spRenameFile( const char* filename , const char* newname);

/* Function: spRenameDirectory
 * 
 * Renames a directory, works exactly like spRenameFile. In fact it is just a
 * redefinement.
 * 
 * See Also:
 * <spRenameFile>*/
#define spRenameDirectory spRenameFile

/* Function: spFileGetDirectory
 * 
 * Searches for files
 * 
 * Parameters:
 * pointer - pointer of a pointer, that needs to exist. In this structure the
 * result will be saved
 * directory - path of the directory to be searched.
 * recursive - if 0 only _directory_ will be searched, if 1 the subdirectories,
 * too. Be carefull with infinite direction loops!
 * no_hidden_files - if 1, hidden files are ignored like .git.
 * 
 * Returns:
 * spFileError - <SP_FILE_EVERYTHING_OK> if no error, else see
 * <spFileError>*/
PREFIX spFileError spFileGetDirectory(spFileListPointer* pointer, char* directory,int recursive,int no_hidden_files);

/* Function: spFileDeleteList
 * 
 * Deletes a list created by spFileGetDirectory.
 * 
 * Parameters:
 * list - the list to be deleted*/
PREFIX void spFileDeleteList(spFileListPointer list);

/* Function: spFileSortList
 * 
 * Sorts the file list like you want.
 * 
 * Parameters:
 * list - the list to be sorted
 * sortBy - the sorting pattern. The possible sort pattern are
 * (self-explanatory): <SP_FILE_SORT_BY_NAME>, <SP_FILE_SORT_BY_TYPE>,
 * <SP_FILE_SORT_BY_TYPE_AND_NAME>. Add <SP_FILE_SORT_BACKWARDS> to sort
 * backwards */
PREFIX void spFileSortList(spFileListPointer* list,spFileSortType sortBy);

#endif
