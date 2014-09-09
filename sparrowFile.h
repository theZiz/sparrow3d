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
 * name (char*) - filename
 * type (spFileType) - filetype, see <spFileType>
 * last_mod - time of the last modification in seconds since 1.1.1970 00:00:00
 * last_acc - time of the last access in seconds since 1.1.1970 00:00:00
 * prev (spFileList*) - previous found
 * next (spFileList*) - next found
 * count (int) - only valid for the first element, describes the number of found files*/
typedef struct spFileListStruct *spFileListPointer;
typedef struct spFileListStruct {
	char name[256];
	spFileType type;
	Sint64 last_mod;
	Sint64 last_acc;
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
 * int - If the end of file is reached, 1 is returned, else 0*/
PREFIX int spReadOneLine( spFilePointer file , char* buffer, int buffer_len);

/* Function: spWriteOneLine
 *
 * Writes one line from a SDL_RWops file.
 *
 * Parameters:
 * file - the file to read
 * buffer - content to be written
 *
 * Returns:
 * int - 1 at error, 0 else*/
PREFIX int spWriteOneLine( spFilePointer file , char* buffer);

/* Function: spReadUntil
 *
 * Reads signs from the file "file" until the buffer is full
 * (buffer_len) or the sign "end_sign" is reached. The sign before
 * "end_sign" is the last sign of the string! If you read more signs
 * from the file, the sign AFTER "end_sign" is the next you will read.
 * It is useful for parsing simple XML files.
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
 * Creates a directory chain (like /home/user/.config/pinball/settings) if
 * it not exists already. That means if you create e.g. rainbow/dash, the folder
 * rainbow will be created if it doesn't exist.
 *
 * Parameters:
 * directories - the path of the directory to create
 *
 * Returns:
 * spFileError - <SP_FILE_EVERYTHING_OK> if no error, else see
 * <spFileError>*/
PREFIX spFileError spCreateDirectoryChain( const char* directories );

/* Function: spRemoveFile
 *
 * Removes/deletes a file
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
 * Removes an EMPTY directory
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
 * Puts a double linked list of found files in directory to pointer, found files
 * are directly extracted from the system and may very well not be ordered.
 * Call <spFileSortList> on the list afterwards.
 * Finally call <spFileDeleteList> to dispose the list.
 * Returned filepaths include the passed directory string
 *
 * Parameters:
 * pointer - pointer to an existing <spFileListPointer> (not a struct), the result
 * will be placed in here
 * directory - path of the directory to be searched. Use forward slashes (even
 * on Windows). Use "." for working directory, ".." for parent dir.
 * DON'T include a trailing slash
 * recursive - if 0 only _directory_ will be searched, if 1 the subdirectories,
 * too. Be carefull with infinite directory loops!
 * no_hidden_files - if 1, hidden files like .git are ignored.
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

/* Function: spConfigGetPath
 * 
 * Returns the path to config files, which may be everywhere on different
 * targets, e.g. in appdata for the pandora, home-folder for the gcw, etc.
 * 
 * Parameters:
 * buffer - buffer to fill with the path
 * subfolder - name of the program used to create subfolders e.g. in ~/.config.
 * Not used by every target. In fact only by x86 linux and gcw.
 * file - name of the config file to give the path to
 * 
 * Returns:
 * char* - filled pointer to buffer*/
PREFIX char* spConfigGetPath(char* buffer,char* subfolder,char* file);

/* type: spConfigEntry
 *
 * List type for config entries.
 *
 * Variables:
 * key (char[64]) - name of the config setting, e.g. "resolution"
 * value (char[512]) - value of the config setting, e.g. "800x480"
 * next (pointer) - next element in list*/
typedef struct spConfigEntryStruct *spConfigEntryPointer;
typedef struct spConfigEntryStruct
{
	char key[64];
	char value[512];
	spConfigEntryPointer next;
} spConfigEntry;

/* type: spConfig
 *
 * Config type
 *
 * Variables:
 * filename (char*) - full path of the config
 * firstEntry (pointer) - pointer to the first entry in the config file*/
typedef struct spConfigStruct *spConfigPointer;
typedef struct spConfigStruct
{
	char* filename;
	spConfigEntryPointer firstEntry;
	spConfigEntryPointer lastEntry;
} spConfig;

/* Function: spConfigRead
 * 
 * Reads a config and saves it to a <spConfig> struct
 * 
 * Parameters:
 * filename - filename of the config. It will be saved to a specific folder on
 * every target
 * subfolder - needed to "calculate" this specific folder. Should just be your
 * program name
 * 
 * Returns:
 * spConfigPointer - Pointer to a <spConfig> Struct*/
PREFIX spConfigPointer spConfigRead(char* filename,char* subfolder);

/* Function: spConfigWrite
 * 
 * Writes the config to the file.
 * 
 * Parameters:
 * config - config, that shall be written */
PREFIX void spConfigWrite(spConfigPointer config);

/* Function: spConfigFree
 * 
 * Frees the struct if you don't need it anymore
 * 
 * Parameters:
 * config - config, that shall be freed */
PREFIX void spConfigFree(spConfigPointer config);

/* Function: spConfigGetString
 * 
 * Returns the string value to a specific key. If the key is not found, it will
 * be added with the default_value. There is no Setter function, because you
 * can edit the returned pointer directly.
 * 
 * Parameters:
 * config - config to read from
 * key - key of the item
 * default_value - value to add/return if the key is not available yet
 * 
 * Returns:
 * char* - the value*/
PREFIX char* spConfigGetString(spConfigPointer config,char* key,char* default_value);

/* Function: spConfigGetStringWithComment
 * 
 * Returns the string value to a specific key. If the key is not found, it will
 * be added with the default_value. There is no Setter function, because you
 * can edit the returned pointer directly.
 * If the key is not available in the config yet, the comment will be added
 * before the setting
 * 
 * Parameters:
 * config - config to read from
 * key - key of the item
 * default_value - value to add/return if the key is not available yet
 * comment - a comment, which will be set before the key-value-pair in the
 * config if it doesn't exist yet
 * 
 * Returns:
 * char* - the value*/
PREFIX char* spConfigGetStringWithComment(spConfigPointer config,char* key,char* default_value,char* comment);

/* Function: spConfigGetBool
 * 
 * Returns the bool value to a specific key. If the key is not found, it will
 * be added with the default_value.
 * 
 * Parameters:
 * config - config to read from
 * key - key of the item
 * default_value - value to add/return if the key is not available yet
 * 
 * Returns:
 * int - the value. 1 means true, 0 false*/
PREFIX int spConfigGetBool(spConfigPointer config,char* key,int default_value);

/* Function: spConfigGetBoolWithComment
 * 
 * Returns the bool value to a specific key. If the key is not found, it will
 * be added with the default_value.
 * If the key is not available in the config yet, the comment will be added
 * before the setting
 * 
 * Parameters:
 * config - config to read from
 * key - key of the item
 * default_value - value to add/return if the key is not available yet
 * comment - a comment, which will be set before the key-value-pair in the
 * config if it doesn't exist yet
 * 
 * Returns:
 * int - the value. 1 means true, 0 false*/
PREFIX int spConfigGetBoolWithComment(spConfigPointer config,char* key,int default_value,char* comment);

/* Function: spConfigSetBool
 * 
 * Sets the bool value to a specific key. If the key is not found, it will
 * be added.
 * 
 * Parameters:
 * config - config to write to
 * key - key of the item
 * value - value to set*/
PREFIX void spConfigSetBool(spConfigPointer config,char* key,int value);

/* Function: spConfigSetBoolWithComment
 * 
 * Sets the bool value to a specific key. If the key is not found, it will
 * be added.
 * If the key is not available in the config yet, the comment will be added
 * before the setting
 * 
 * Parameters:
 * config - config to write to
 * key - key of the item
 * value - value to set
 * comment - a comment, which will be set before the key-value-pair in the
 * config if it doesn't exist yet*/
PREFIX void spConfigSetBoolWithComment(spConfigPointer config,char* key,int value,char* comment);

/* Function: spConfigGetInt
 * 
 * Returns the int value to a specific key. If the key is not found, it will
 * be added with the default_value.
 * 
 * Parameters:
 * config - config to read from
 * key - key of the item
 * default_value - value to add/return if the key is not available yet
 * 
 * Returns:
 * int - the value*/
PREFIX int spConfigGetInt(spConfigPointer config,char* key,int default_value);

/* Function: spConfigGetIntWithComment
 * 
 * Returns the int value to a specific key. If the key is not found, it will
 * be added with the default_value.
 * If the key is not available in the config yet, the comment will be added
 * before the setting
 * 
 * Parameters:
 * config - config to read from
 * key - key of the item
 * default_value - value to add/return if the key is not available yet
 * comment - a comment, which will be set before the key-value-pair in the
 * config if it doesn't exist yet
 * 
 * Returns:
 * int - the value*/
PREFIX int spConfigGetIntWithComment(spConfigPointer config,char* key,int default_value,char* comment);

/* Function: spConfigSetInt
 * 
 * Sets the int value to a specific key. If the key is not found, it will
 * be added.
 * 
 * Parameters:
 * config - config to write to
 * key - key of the item
 * value - value to set*/
PREFIX void spConfigSetInt(spConfigPointer config,char* key,int value);

/* Function: spConfigSetIntWithComment
 * 
 * Sets the int value to a specific key. If the key is not found, it will
 * be added.
 * If the key is not available in the config yet, the comment will be added
 * before the setting
 * 
 * Parameters:
 * config - config to write to
 * key - key of the item
 * value - value to set
 * comment - a comment, which will be set before the key-value-pair in the
 * config if it doesn't exist yet*/
PREFIX void spConfigSetIntWithComment(spConfigPointer config,char* key,int value,char* comment);

/* Function: spConfigGetFloat
 * 
 * Returns the float value to a specific key. If the key is not found, it will
 * be added with the default_value.
 * 
 * Parameters:
 * config - config to read from
 * key - key of the item
 * default_value - value to add/return if the key is not available yet
 * 
 * Returns:
 * float - the value*/
PREFIX float spConfigGetFloat(spConfigPointer config,char* key,float default_value);

/* Function: spConfigGetFloatWithComment
 * 
 * Returns the float value to a specific key. If the key is not found, it will
 * be added with the default_value.
 * If the key is not available in the config yet, the comment will be added
 * before the setting
 * 
 * Parameters:
 * config - config to read from
 * key - key of the item
 * default_value - value to add/return if the key is not available yet
 * comment - a comment, which will be set before the key-value-pair in the
 * config if it doesn't exist yet
 * 
 * Returns:
 * float - the value*/
PREFIX float spConfigGetFloatWithComment(spConfigPointer config,char* key,float default_value,char* comment);

/* Function: spConfigSetFloat
 * 
 * Sets the float value to a specific key. If the key is not found, it will
 * be added.
 * 
 * Parameters:
 * config - config to write to
 * key - key of the item
 * value - value to set*/
PREFIX void spConfigSetFloat(spConfigPointer config,char* key,float value);

/* Function: spConfigSetFloatWithComment
 * 
 * Sets the float value to a specific key. If the key is not found, it will
 * be added.
 * If the key is not available in the config yet, the comment will be added
 * before the setting
 * 
 * Parameters:
 * config - config to write to
 * key - key of the item
 * value - value to set
 * comment - a comment, which will be set before the key-value-pair in the
 * config if it doesn't exist yet*/
PREFIX void spConfigSetFloatWithComment(spConfigPointer config,char* key,float value,char* comment);
#endif
