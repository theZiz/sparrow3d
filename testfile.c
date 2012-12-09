/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include "../sparrow3d/sparrow3d.h"

int main( int argc, char **argv )
{
	//In this program we don't use sparrowCore functionalities, so no init functions.
	printf("This program shows the sparrowFile functionality of getting directory content.\n");
	printf("F = File\n");
	printf("D = Direcotry\n");
	printf("L = (Sym)link\n--------------------\n");
	spFileListPointer list;
	spFileError error = spFileGetDirectory(&list,".",1,1);
	printf("Files: %i\n",list->count);
	spFileSortList(&list,SP_FILE_SORT_BY_TYPE_AND_NAME | SP_FILE_SORT_BACKWARDS);
	spFileListPointer mom = list;
	while (mom)
	{
		switch (mom->type)
		{
			case SP_FILE_FILE:
				printf("F  %s\n",mom->name);
				break;
			case SP_FILE_DIRECTORY:
				printf("D  %s\n",mom->name);
				break;
			case SP_FILE_FILE | SP_FILE_LINK:
				printf("FL %s\n",mom->name);
				break;
			case SP_FILE_DIRECTORY | SP_FILE_LINK:
				printf("DL %s\n",mom->name);
				break;
    }
		mom = mom->next;
	}
	printf("Error: %i\n",error);
	spFileDeleteList(list);
	
	return 0;
}
