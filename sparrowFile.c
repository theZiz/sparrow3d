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

#include "sparrowFile.h"
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

PREFIX int spFileExists( const char* filename )
{
  SDL_RWops *file = SDL_RWFromFile(filename, "rb");
  if (file)
  {
    SDL_RWclose(file);
    return 1;
  }
  return 0;
}

PREFIX int spReadOneLine( spFilePointer file , char* buffer, int buffer_len)
{
	return spReadUntil(file,buffer,buffer_len,'\n',1);
}

PREFIX int spReadUntil( spFilePointer file , char* buffer, int buffer_len, char end_sign,char ignore_windows_return)
{
	int pos = 0;
	if (ignore_windows_return)
		while (pos < buffer_len)
		{
			if (SDL_RWread( file, &(buffer[pos]), 1, 1 ) <= 0)
				return 1; //EOF
			if ( buffer[pos] == end_sign )
				break;
			if (buffer[pos] != '\r') //fucking windows line break
				pos++;
		}
	else
		while (pos < buffer_len)
		{
			if (SDL_RWread( file, &(buffer[pos]), 1, 1 ) <= 0)
				return 1; //EOF
			if ( buffer[pos] == end_sign )
				break;
		}	
	buffer[pos] = 0;
	return 0; //not EOF
}

PREFIX spFileError spCreateDirectoryChain( const char* directories)
{
	//Creating copy:
	int len = strlen(directories)+1;
	#ifdef __GNUC__
		char directoriesCopy[len];
	#else
		char* directoriesCopy = (char*)malloc( len * sizeof(char) );
	#endif
	memcpy(directoriesCopy,directories,len);
	//Splitting in subdirectories
	char* subString = directoriesCopy;
	char* endOfString = strchr(subString,'/');
	if (endOfString == NULL)
		endOfString = strchr(subString,0);
	spFileError result;
	while (endOfString)
	{
		char oldChar = endOfString[0];
		endOfString[0] = 0;
		#ifdef WIN32

			if (CreateDirectory(directoriesCopy,NULL))
				result = SP_FILE_EVERYTHING_OK;
			else
			if (GetLastError() == ERROR_ALREADY_EXISTS)
				result = SP_FILE_ALREADY_EXISTS_ERROR;
			else
			{
				result = SP_FILE_ACCESS_ERROR;
				break;
			}
		#else
			int error = mkdir(directoriesCopy,0777);
			if (error == 0) //thats okay :)
					result = SP_FILE_EVERYTHING_OK;
			else
			if (errno == EEXIST || errno == ENOENT) //thats okay :)
					result = SP_FILE_ALREADY_EXISTS_ERROR;
			else //not okay
			{
				result = SP_FILE_ACCESS_ERROR;
				break;
			}
		#endif
		endOfString[0] = oldChar;
		if (oldChar == 0)
			break;
		subString = &(endOfString[1]);
		endOfString = strchr(subString,'/');
		if (endOfString == NULL)
			endOfString = strchr(subString,0);
	}
	
	#ifndef __GNUC__
		free(directoriesCopy);
	#endif
	return result;
}

PREFIX spFileError spRemoveFile( const char* filename )
{
#ifdef WIN32
	if (DeleteFile(filename))
		return SP_FILE_EVERYTHING_OK;
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
		return SP_FILE_NOT_FOUND_ERROR;
	return SP_FILE_ACCESS_ERROR;
#else
	if (remove(filename))
	{
		if (errno == ENOENT)
			return SP_FILE_NOT_FOUND_ERROR;
		return SP_FILE_ACCESS_ERROR;
	}
	return SP_FILE_EVERYTHING_OK;
#endif
}

PREFIX spFileError spRemoveDirectory( const char* dirname )
{
#ifdef WIN32
	if (RemoveDirectory(dirname))
		return SP_FILE_EVERYTHING_OK;
	if (GetLastError() == ERROR_PATH_NOT_FOUND)
		return SP_FILE_NOT_FOUND_ERROR;
	return SP_FILE_ACCESS_ERROR;
#else
	if (rmdir(dirname))
	{
		if (errno == ENOENT)
			return SP_FILE_NOT_FOUND_ERROR;
		return SP_FILE_ACCESS_ERROR;
	}
	return SP_FILE_EVERYTHING_OK;
#endif
}

PREFIX spFileError spRenameFile( const char* filename , const char* newname)
{
#ifdef WIN32
	if (MoveFile(filename,newname))
		return SP_FILE_EVERYTHING_OK;
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
		return SP_FILE_NOT_FOUND_ERROR;
	return SP_FILE_ACCESS_ERROR;
#else
	if (rename(filename,newname))
	{
		if (errno == ENOENT)
			return SP_FILE_NOT_FOUND_ERROR;
		return SP_FILE_ACCESS_ERROR;
	}
	return SP_FILE_EVERYTHING_OK;	
#endif
}
