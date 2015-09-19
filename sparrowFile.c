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

#include "sparrowFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifndef __GNUC__
	#include <windows.h>
	#include <tchar.h>
#else
	#include <dirent.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

#define replaceLinuxWithWindowsStuff(STR) \
{\
	int i = 0;\
	while (STR[i]!=0)\
	{\
		if (STR[i]=='/')\
			STR[i]='\\';\
		i++;\
	}\
}\

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
	buffer[pos] = 0;
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

PREFIX int spWriteOneLine( spFilePointer file , char* buffer)
{
	if (buffer && buffer[0])
		if (SDL_RWwrite( file, buffer, strlen(buffer) , 1) != 1)
			return 1;
	char c = '\n';
	if (SDL_RWwrite( file, &c, 1, 1) != 1)
		return 1;
	return 0;
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
		#ifdef _WIN32

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
#ifdef _WIN32
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
#ifdef _WIN32
	char *windowsName = (char*)malloc(strlen(dirname)+1);
	sprintf(windowsName,"%s",dirname);
	replaceLinuxWithWindowsStuff(windowsName);
	if (RemoveDirectory(windowsName))
	{
		free(windowsName);
		return SP_FILE_EVERYTHING_OK;
	}
	free(windowsName);
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
#ifdef _WIN32
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

spFileError internalFileGetDirectory(spFileListPointer* pointer,spFileListPointer* last,char* directory,int recursive,int no_hidden_files)
{
#ifdef _WIN32
	char* realDirectory = directory;
	if (strcmp(directory,".") == 0)
		realDirectory = &(directory[1]);
	if (directory[0] == '.' && directory[1] == '/')
		realDirectory = &(directory[2]);
	char windowsSearchString[512];
	if (realDirectory[0]!=0)
		sprintf(windowsSearchString,"%s\\*",realDirectory);
	else
		sprintf(windowsSearchString,"*");
	//Searching '/' and replacing with '\\'
	replaceLinuxWithWindowsStuff(windowsSearchString);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(windowsSearchString,&FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return SP_FILE_UNKNOWN_ERROR;
	do
	{
		if (strcmp(FindFileData.cFileName,".") == 0 || strcmp(FindFileData.cFileName,"..") == 0)
			continue;
		if (no_hidden_files && FindFileData.cFileName[0]=='.')
			continue;
		if (no_hidden_files && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			continue;
		spFileListPointer newOne = (spFileListPointer)malloc(sizeof(spFileList));
		sprintf(newOne->name,"%s/%s",directory,FindFileData.cFileName);
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			newOne->type = SP_FILE_DIRECTORY;
		else
			newOne->type = SP_FILE_FILE;
		if (*last)
		{
			(*last)->next = newOne;
			newOne->prev = *last;
			newOne->next = NULL;
			*last = newOne;
		}
		else
		{
			newOne->prev = NULL;
			newOne->next = NULL;
			*pointer = newOne;
			*last = newOne;
			(*pointer)->count = 0;
		}
		(*pointer)->count++;
		if ((newOne->type & SP_FILE_DIRECTORY) && recursive)
		{
			spFileError error = internalFileGetDirectory(pointer,last,newOne->name,1,no_hidden_files);
			if (error != SP_FILE_EVERYTHING_OK)
			{
				FindClose(hFind);
				return error;
			}
		}
		newOne->last_acc = 0;
		newOne->last_mod = 0;
	}
	while (FindNextFile(hFind,&FindFileData));
	FindClose(hFind);
	return SP_FILE_EVERYTHING_OK;
#else
	struct stat stat_buf;
	DIR* d = opendir(directory);
	if (d)
	{
		struct dirent *dir;
		while ((dir = readdir(d)) != NULL)
		{
			if (strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0)
				continue;
			if (no_hidden_files && dir->d_name[0]=='.')
				continue;
			spFileListPointer newOne = (spFileListPointer)malloc(sizeof(spFileList));
			int l;
			if ((l = strlen(directory)) > 0 && directory[l-1] == '/')
				sprintf(newOne->name,"%s%s",directory,dir->d_name);
			else
				sprintf(newOne->name,"%s/%s",directory,dir->d_name);
			lstat(newOne->name,&stat_buf);
			if (S_ISLNK(stat_buf.st_mode))
			{
				newOne->type = SP_FILE_LINK;
				stat(newOne->name,&stat_buf);
			}
			else
				newOne->type = 0;
			if (S_ISDIR(stat_buf.st_mode))
				newOne->type |= SP_FILE_DIRECTORY;
			else
				newOne->type |= SP_FILE_FILE;
			newOne->last_mod = stat_buf.st_mtime;
			newOne->last_acc = stat_buf.st_atime;
			if (*last)
			{
				(*last)->next = newOne;
				newOne->prev = *last;
				newOne->next = NULL;
				*last = newOne;
			}
			else
			{
				newOne->prev = NULL;
				newOne->next = NULL;
				*pointer = newOne;
				*last = newOne;
				(*pointer)->count = 0;
			}
			(*pointer)->count++;
			if ((newOne->type & SP_FILE_DIRECTORY) && recursive)
			{
				spFileError error = internalFileGetDirectory(pointer,last,newOne->name,1,no_hidden_files);
				if (error != SP_FILE_EVERYTHING_OK)
				{
					closedir(d);
					return error;
				}
			}
		}
		if (errno == EBADF)
			return SP_FILE_UNKNOWN_ERROR;
	}
	else
	{
		switch (errno)
		{
			case EACCES:
				return SP_FILE_ACCESS_ERROR;
			case ENOENT:
				return SP_FILE_NOT_FOUND_ERROR;
			case ENOTDIR:
				return SP_FILE_INVALID_PARAMETER_ERROR;
			default:
				return SP_FILE_UNKNOWN_ERROR;
		}
	}
	closedir(d);
	return SP_FILE_EVERYTHING_OK;
#endif
}

PREFIX spFileError spFileGetDirectory(spFileListPointer* pointer,char* directory,int recursive,int no_hidden_files)
{
	spFileListPointer last = NULL;
	*pointer = NULL;
	return internalFileGetDirectory(pointer,&last,directory,recursive,no_hidden_files);
}

PREFIX void spFileDeleteList(spFileListPointer list)
{
	while (list)
	{
		spFileListPointer next = list->next;
		free(list);
		list = next;
	}
}

int internalCompareByName ( const void * elem1, const void * elem2 )
{
	spFileListPointer left  = *((spFileListPointer*)(elem1));
	spFileListPointer right = *((spFileListPointer*)(elem2));
	int ll = strlen(left->name)+1;
	int rl = strlen(right->name)+1;
	char left_name[ll];
	char right_name[rl];
	int i;
	for (i = 0; i < ll; i++)
		if (left->name[i] >= 'a' && left->name[i] <= 'z')
			left_name[i] = left->name[i]-'a'+'A';
		else
			left_name[i] = left->name[i];
	for (i = 0; i < rl; i++)
		if (right->name[i] >= 'a' && right->name[i] <= 'z')
			right_name[i] = right->name[i]-'a'+'A';
		else
			right_name[i] = right->name[i];
	return strcmp(left_name,right_name);
}

int internalCompareByNameBackwards ( const void * elem1, const void * elem2 )
{
	return -internalCompareByName(elem1,elem2);
}

int internalCompareByType ( const void * elem1, const void * elem2 )
{
	spFileListPointer left  = *((spFileListPointer*)(elem1));
	spFileListPointer right = *((spFileListPointer*)(elem2));
	return (left->type & 1) < (right->type & 1)?1:((left->type & 1) > (right->type & 1)?-1:0);
}

int internalCompareByTypeBackwards ( const void * elem1, const void * elem2 )
{
	spFileListPointer left  = *((spFileListPointer*)(elem1));
	spFileListPointer right = *((spFileListPointer*)(elem2));
	return (left->type & 1) < (right->type & 1)?-1:((left->type & 1) > (right->type & 1)?1:0);
}

int internalCompareByTypeName ( const void * elem1, const void * elem2 )
{
	int result = internalCompareByType(elem1,elem2);
	if (result == 0)
	{
		return internalCompareByName(elem1,elem2);
	}
	return result;
}

int internalCompareByTypeNameBackwards ( const void * elem1, const void * elem2 )
{
	int result = internalCompareByTypeBackwards(elem1,elem2);
	if (result == 0)
	{
		return internalCompareByNameBackwards(elem1,elem2);
	}
	return result;
}

PREFIX void spFileSortList(spFileListPointer* list,spFileSortType sortBy)
{
	if (!(*list))
		return;
	//Creating quick sort array
	int count = (*list)->count;
#ifdef _MSC_VER
	spFileListPointer *sarray = new spFileListPointer[count];
#else
	spFileListPointer sarray[count];
#endif
	int i;
	spFileListPointer mom = *list;
	for (i = 0; i < count; i++)
	{
		sarray[i] = mom;
		mom = mom->next;
	}
	switch (sortBy)
	{
		case SP_FILE_SORT_BY_NAME:
			qsort(sarray,count,sizeof(spFileListPointer),internalCompareByName);
			break;
		case SP_FILE_SORT_BY_NAME | SP_FILE_SORT_BACKWARDS:
			qsort(sarray,count,sizeof(spFileListPointer),internalCompareByNameBackwards);
			break;
		case SP_FILE_SORT_BY_TYPE:
			qsort(sarray,count,sizeof(spFileListPointer),internalCompareByType);
			break;
		case SP_FILE_SORT_BY_TYPE | SP_FILE_SORT_BACKWARDS:
			qsort(sarray,count,sizeof(spFileListPointer),internalCompareByTypeBackwards);
			break;
		case SP_FILE_SORT_BY_TYPE_AND_NAME:
			qsort(sarray,count,sizeof(spFileListPointer),internalCompareByTypeName);
			break;
		case SP_FILE_SORT_BY_TYPE_AND_NAME | SP_FILE_SORT_BACKWARDS:
			qsort(sarray,count,sizeof(spFileListPointer),internalCompareByTypeNameBackwards);
			break;
	}

	*list = sarray[0];
	(*list)->prev = NULL;
	(*list)->count = count;
	for (i = 1; i < count; i++)
	{
		sarray[i  ]->prev = sarray[i-1];
		sarray[i-1]->next = sarray[  i];
	}
	sarray[count-1]->next = NULL;

#ifdef _MSC_VER
	delete[] sarray;
#endif
}

PREFIX char* spConfigGetPath(char* buffer,char* subfolder,char* file)
{
	#if defined GCW || (defined DESKTOP && !defined _WIN32)
		sprintf(buffer,"%s/.config/%s",getenv("HOME"),subfolder);
		spCreateDirectoryChain(buffer);
		sprintf(buffer,"%s/.config/%s/%s",getenv("HOME"),subfolder,file);
	#else
		sprintf(buffer,"./%s",file);
	#endif
	return buffer;	
}

spConfigEntryPointer internalNewEntry(spConfigPointer config,char* key,char* value)
{
	spConfigEntryPointer entry = (spConfigEntryPointer)malloc(sizeof(spConfigEntry));
	sprintf(entry->key,"%s",key);
	sprintf(entry->value,"%s",value);
	entry->next = NULL;
	if (config->lastEntry)
		config->lastEntry->next = entry;
	else
		config->firstEntry = entry;
	config->lastEntry = entry;
	return entry;
}	

PREFIX spConfigPointer spConfigRead(char* filename,char* subfolder)
{
	spConfigPointer config = (spConfigPointer)malloc(sizeof(spConfig));
	config->firstEntry = NULL;
	config->lastEntry = NULL;
	config->filename = (char*)malloc(strlen(filename)+strlen(subfolder)+128);
	spConfigGetPath(config->filename,subfolder,filename);
	spFilePointer file = SDL_RWFromFile(config->filename,"rb");
	if (file == NULL)
		return config;
	char buffer[1024];
	char key[64];
	char value[512];
	while (spReadOneLine(file,buffer,1024) == 0)
	{
		if (buffer[0] == '#') //comment
		{
			key[0] = 0;
			sprintf(value,"%s",&(buffer[1]));
			internalNewEntry(config,key,value);
			continue;
		}
		char* middle = strchr(buffer,':');
		if (middle == NULL)
			continue;
		middle[0] = 0;
		sprintf(key,"%s",buffer);
		middle++;
		while (middle[0] == ' ')
			middle++;
		sprintf(value,"%s",middle);
		internalNewEntry(config,key,value);
	}
	SDL_RWclose(file);
	return config;
}

PREFIX void spConfigWrite(spConfigPointer config)
{
	spFilePointer file = SDL_RWFromFile(config->filename,"wb");
	spConfigEntryPointer entry = config->firstEntry;
	while (entry)
	{
		char buffer[1024];
		if (entry->key[0] == 0) //comment
			sprintf(buffer,"#%s",entry->value);
		else
			sprintf(buffer,"%s: %s",entry->key,entry->value);
		spWriteOneLine(file,buffer);
		entry = entry->next;
	}
	SDL_RWclose(file);
}

PREFIX void spConfigFree(spConfigPointer config)
{
	spConfigEntryPointer entry = config->firstEntry;
	while (entry)
	{
		spConfigEntryPointer next = entry->next;
		free(entry);
		entry = next;
	}
	free(config->filename);
	free(config);
}

spConfigEntryPointer internalGetEntry(spConfigPointer config,char* key)
{
	spConfigEntryPointer entry = config->firstEntry;
	while (entry)
	{
		if (strcmp(entry->key,key) == 0)
			return entry;
		entry = entry->next;
	}
	return NULL;
}

PREFIX char* spConfigGetString(spConfigPointer config,char* key,char* default_value)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
		entry = internalNewEntry(config,key,default_value);
	return entry->value;
}

PREFIX int spConfigGetBool(spConfigPointer config,char* key,int default_value)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		if (default_value)
			entry = internalNewEntry(config,key,"True");
		else
			entry = internalNewEntry(config,key,"False");
	}
	if (strcmp(entry->value,"True") == 0)
		return 1;
	if (strcmp(entry->value,"true") == 0)
		return 1;
	if (strcmp(entry->value,"1") == 0)
		return 1;
	return 0;
}

PREFIX void spConfigSetBool(spConfigPointer config,char* key,int value)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		if (value)
			entry = internalNewEntry(config,key,"True");
		else
			entry = internalNewEntry(config,key,"False");
	}
	else
	{
		if (value)
			sprintf(entry->value,"True");
		else
			sprintf(entry->value,"False");
	}		
}

PREFIX int spConfigGetInt(spConfigPointer config,char* key,int default_value)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		char buffer[32];
		sprintf(buffer,"%i",default_value);
		entry = internalNewEntry(config,key,buffer);
	}
	return atoi(entry->value);
}

PREFIX void spConfigSetInt(spConfigPointer config,char* key,int value)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		char buffer[32];
		sprintf(buffer,"%i",value);
		entry = internalNewEntry(config,key,buffer);
	}
	else
		sprintf(entry->value,"%i",value);
}

PREFIX float spConfigGetFloat(spConfigPointer config,char* key,float default_value)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		char buffer[32];
		sprintf(buffer,"%f",default_value);
		entry = internalNewEntry(config,key,buffer);
	}
	return spAtoFloat(entry->value);
}

PREFIX void spConfigSetFloat(spConfigPointer config,char* key,float value)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		char buffer[32];
		sprintf(buffer,"%f",value);
		entry = internalNewEntry(config,key,buffer);
	}
	else
		sprintf(entry->value,"%f",value);
}

PREFIX char* spConfigGetStringWithCommentBefore(spConfigPointer config,char* key,char* default_value,char* comment)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		internalNewEntry(config,"",comment);
		entry = internalNewEntry(config,key,default_value);
	}
	return entry->value;
}

PREFIX int spConfigGetBoolWithCommentBefore(spConfigPointer config,char* key,int default_value,char* comment)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		internalNewEntry(config,"",comment);
		if (default_value)
			entry = internalNewEntry(config,key,"True");
		else
			entry = internalNewEntry(config,key,"False");
	}
	if (strcmp(entry->value,"True") == 0)
		return 1;
	if (strcmp(entry->value,"true") == 0)
		return 1;
	if (strcmp(entry->value,"1") == 0)
		return 1;
	return 0;
}

PREFIX void spConfigSetBoolWithCommentBefore(spConfigPointer config,char* key,int value,char* comment)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		internalNewEntry(config,"",comment);
		if (value)
			entry = internalNewEntry(config,key,"True");
		else
			entry = internalNewEntry(config,key,"False");
	}
	else
	{
		if (value)
			sprintf(entry->value,"True");
		else
			sprintf(entry->value,"False");
	}		
}

PREFIX int spConfigGetIntWithCommentBefore(spConfigPointer config,char* key,int default_value,char* comment)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		internalNewEntry(config,"",comment);
		char buffer[32];
		sprintf(buffer,"%i",default_value);
		entry = internalNewEntry(config,key,buffer);
	}
	return atoi(entry->value);
}

PREFIX void spConfigSetIntWithCommentBefore(spConfigPointer config,char* key,int value,char* comment)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		internalNewEntry(config,"",comment);
		char buffer[32];
		sprintf(buffer,"%i",value);
		entry = internalNewEntry(config,key,buffer);
	}
	else
		sprintf(entry->value,"%i",value);
}

PREFIX float spConfigGetFloatWithCommentBefore(spConfigPointer config,char* key,float default_value,char* comment)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		internalNewEntry(config,"",comment);
		char buffer[32];
		sprintf(buffer,"%f",default_value);
		entry = internalNewEntry(config,key,buffer);
	}
	return spAtoFloat(entry->value);
}

PREFIX void spConfigSetFloatWithCommentBefore(spConfigPointer config,char* key,float value,char* comment)
{
	spConfigEntryPointer entry = internalGetEntry(config,key);
	if (entry == NULL)
	{
		internalNewEntry(config,"",comment);
		char buffer[32];
		sprintf(buffer,"%f",value);
		entry = internalNewEntry(config,key,buffer);
	}
	else
		sprintf(entry->value,"%f",value);
}
