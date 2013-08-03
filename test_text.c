/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_text.h"
#include <sparrow3d.h>
#include <strings.h>

spTextBlockPointer block;
Sint32 position = 0;
spBundlePointer bundle;

void init_text(int argc, char** argv, spFontPointer font)
{
	printf("This example application reads different texts from data/texts.txt\n");
	printf("or a given file and prints them on the standard output.\n");
	printf("How to use this example:\n");
	printf("./testtext(.sh) [-l language] [-f file]\n");
	printf("  possible languages are de, en or fr. If you don't define a\n");
	printf("  lanugage, the default language (en) will be used.\n");
	printf("\n");
	char defaultname[] = "./data/texts.txt";
	char* filename = defaultname;
	Uint16 language = SP_LANGUAGE_EN;
	int i;
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i],"-l") == 0)
		{
			i++;
			if (i >= argc)
			{
				printf("No language given after \"-l\"!\n");
				break;
			}
			if (strcmp(argv[i],"en") == 0)
				language = SP_LANGUAGE_EN;
			else
			if (strcmp(argv[i],"de") == 0)
				language = SP_LANGUAGE_DE;
			else
			if (strcmp(argv[i],"fr") == 0)
				language = SP_LANGUAGE_FR;
			else
			{
				printf("Don't know the language \"%s\"\n",argv[i]);
				break;
			}
		}
	}
	switch (language)
	{
		case SP_LANGUAGE_EN:
			printf("Choose the English language!\n");
			break;
		case SP_LANGUAGE_DE:
			printf("Choose the German language!\n");
			break;
		case SP_LANGUAGE_FR:
			printf("Choose the French language!\n");
			break;
	}
	spSetDefaultLanguage(language);
	//Loading
	spReadPossibleLanguages("./data/languages.txt");
	printf("Possible Languages are (%i languages read from file):\n",spGetPossibleLanguagesCount());
	for (i = 0; i < spGetPossibleLanguagesCount(); i++)
	{
		char buffer[3];
		Uint16 language = spGetPossibleLanguage(i);
		buffer[0] = language>>8;
		buffer[1] = language;
		buffer[2] = 0;
		printf("  %s: %s\n",buffer,spGetPossibleLanguageName(i));
	}
	
	printf("Loading \"%s\"... ",filename);	
	bundle = spLoadBundle(filename,1);
	if (bundle)
		printf(" Success\n");
	else
	{
		printf("\nCouldn't load \"%s\", using some internal values\n",filename);
		bundle = spCreateTextBundle();
		spTextPointer text = spCreateText("Meow",bundle);
		spAddTranslation(text,SP_LANGUAGE_EN,"Meow");
		spAddTranslation(text,SP_LANGUAGE_DE,"Miau");
		spAddTranslation(text,SP_LANGUAGE_FR,"Miaou");
		text = spCreateText("cookie",bundle);
		spAddTranslation(text,SP_LANGUAGE_EN,"Cookies!");
		spAddTranslation(text,SP_LANGUAGE_DE,"Kekse!");
		spAddTranslation(text,SP_LANGUAGE_FR,"Baguettes!");
	}
	
	spTextPointer t = bundle->firstText;
	while (t)
	{
		printf("  \"%s\": \"%s\"\n",t->caption,spGetTranslation(t));
		t = t->next;
	}
	
	//Creating blocks:
	block = spCreateTextBlock( spGetTranslationFromCaption(bundle,"example text"),spGetWindowSurface()->w-font->maxheight*4,font);
	printf("=============== %i\n",block->line_count);
	for (i = 0; i < block->line_count; i++)
	{
		printf("%i: %s\n",i,block->line[i].text);
	}
}

char* caption_text(char* caption)
{
	sprintf(caption,"Scrolling");
	return caption;
}

char* settings_text(char* caption,int button)
{
	switch (button)
	{
		case SP_BUTTON_B:
			sprintf(caption,"Use the D-Pad");
			break;
		case SP_BUTTON_A:
			sprintf(caption,"for scrolling");
			break;
	}
	return caption;
}

void draw_text(int rotation, spFontPointer font)
{
	spSetZTest( 0 );
	spSetZSet( 0 );
	spFontDrawTextBlock(left,font->maxheight*2,font->maxheight*3,0,block,spGetWindowSurface()->h-font->maxheight*6,position,font);
	spSetZTest( 1 );
	spSetZSet( 1 );
}

void calc_text(int steps)
{
	position += spGetInput()->analog_axis[1]*(Sint32)steps >> 9;
	if (position > SP_ONE)
		position = SP_ONE;
	if (position < 0)
		position = 0;
}

void quit_text()
{
	spDeleteTextBlock(block);
	spDeleteBundle(bundle,0);
}
