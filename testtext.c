/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include <sparrow3d.h>

spFontPointer font = NULL;

void draw_function( void )
{
	spClearTarget( 0 );
	spSetZTest(0);
	spSetZSet(0);
	
	char buffer[32];
	sprintf(buffer,"FPS: %i",spGetFPS());
	spFontDrawMiddle( spGetWindowSurface()->w/2, spGetWindowSurface()->h/2-font->maxheight/2, 0, "Everything interesting happens", font );
	spFontDrawMiddle( spGetWindowSurface()->w/2, spGetWindowSurface()->h/2+font->maxheight/2, 0, "in stdout. Please have a look there", font );
	spFontDrawRight( spGetWindowSurface()->w-1, spGetWindowSurface()->h-font->maxheight, 0, buffer, font );
	
	spFlip();
}


int calc_function( Uint32 steps )
{
	if ( spGetInput()->button[SP_BUTTON_START] )
		return 1;
	return 0;
}

void resize(Uint16 w,Uint16 h)
{
	spFontShadeButtons(1);
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./font/Play-Bold.ttf", 12 * spGetSizeFactor() >> SP_ACCURACY );
	spFontSetShadeColor(0);
	spFontAdd( font, SP_FONT_GROUP_ASCII, 65535 ); //whole ASCII
	spFontAdd( font, "äüöÄÜÖßẞ", 65535 ); //German stuff (same like spFontAdd( font, SP_FONT_GROUP_GERMAN, 0 ); )
	spFontAddBorder( font, 0 );
	spFontAddButton( font, 'A', SP_BUTTON_A_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'B', SP_BUTTON_B_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'X', SP_BUTTON_X_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'Y', SP_BUTTON_Y_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'L', SP_BUTTON_L_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'R', SP_BUTTON_R_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'S', SP_BUTTON_START_NAME, 65535, spGetRGB( 64, 64, 64 ) );
	spFontAddButton( font, 'E', SP_BUTTON_SELECT_NAME, 65535, spGetRGB( 64, 64, 64 ) );
}

int main( int argc, char **argv )
{
	//sparrow3D Init
	//spSetDefaultWindowSize( 640, 480 ); //Creates a 640x480 window at PC instead of 320x240
	spInitCore();

	//Setup;
	spSelectRenderTarget(spCreateDefaultWindow());
	resize(spGetWindowSurface()->w,spGetWindowSurface()->h);
	
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
				return 1;
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
				return 1;
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
	spBundlePointer bundle = spLoadBundle(filename,1);
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
	spTextBlockPointer block = spCreateTextBlock( spGetTranslationFromCaption(bundle,"example text"),400,font);
	printf("=============== %i\n",block->line_count);
	for (i = 0; i < block->line_count; i++)
	{
		printf("%i: %s\n",i,block->line[i].text);
	}

	spLoop( draw_function, calc_function, 10, resize, NULL );
	
	spDeleteTextBlock(block);

	//Winter Wrap up, Winter Wrap up
	spFontDelete( font );
	spQuitCore();
	
	spDeleteBundle(bundle,0);
	return 0;
}
