/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include "sparrow3d.h"
#include <SDL_image.h>
#include <math.h>
#include <string.h>
SDL_Surface *screen;
SDL_Surface *garfield;
SDL_Surface *check;
SDL_Surface *pepper;
SDL_Surface *scientist;
spModelPointer mesh;
spModelPointer wheel[15];
spSpritePointer sprite;
Sint32 rotation = 0;
spFontPointer font = NULL;
int quality = 1;
Uint32 fpssum = 0;
Sint32 divisor = -5000;
int test = 0;
int count;
int zStuff = 1;
Uint16 lastKey = 0;
char input[32] = "";
char no_movement = 0;
int perspective = 0;
int pause = 0;

void draw_test( void )
{
	spResetZBuffer();
	if ( test == 5 )
		spClearTarget( ( spSin( rotation * 8 ) >> SP_ACCURACY - 4 ) + 16 );
	else
		spClearTarget( 0 );
	spIdentity();

	count = 0;

	spBindTexture( garfield );
	spSetLight( quality );
	spSetCulling( 1 );
	spSetZSet( zStuff );
	spSetZTest( zStuff );
	int i;
	Sint32 matrix[16];
	Sint32 px, py, pz, w;
	spSetLightPosition(0,7 << SP_ACCURACY - 3,7 << SP_ACCURACY - 3,7 << SP_ACCURACY - 3);
	spSetLightColor(0,SP_ONE,SP_ONE,SP_ONE);
	switch ( test )
	{
	case 6:
		spSetAlphaTest( 0 );
		spTranslate( 0, 0, (-8 << SP_ACCURACY));
		spRotateX(rotation);
		spRotateY(rotation);
		spRotateZ(rotation);
		
		memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) ); //glPush()
		spTranslate(0,2 << SP_ACCURACY,0);
		spSetLightPosition(0,0,0,0);
		spSetLightColor(0,0,SP_ONE,SP_ONE);
		spProjectPoint3D( 0,0,0, &px,&py,&pz, &w, 1);
		spEllipse3D(0,0,0,1 << SP_ACCURACY-3,1 << SP_ACCURACY-3,spGetFastRGB(0,255,255));
		spSetAlphaTest(1);
		spFontDrawMiddle( px,py-font->maxheight/2,pz, "light", font );
		spSetAlphaTest(0);
		spTranslate(0,-4 << SP_ACCURACY,0);
		spEnableLight(1,1);
		spSetLightPosition(1,0,0,0);
		spSetLightColor(1,SP_ONE,SP_ONE,0);
		spProjectPoint3D( 0,0,0, &px,&py,&pz, &w, 1);
		spEllipse3D(0,0,0,1 << SP_ACCURACY-3,1 << SP_ACCURACY-3,spGetFastRGB(255,255,0));
		spSetAlphaTest(1);
		spFontDrawMiddle( px,py-font->maxheight/2,pz, "light", font );
		spSetAlphaTest(0);
		memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) ); //glPop()
		
		for (i = 0; i < 15; i++)
		{
			memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) ); //glPush()
			
			spRotateY(SP_PI*i*4/15);
			spTranslate(0,0,-35 << SP_ACCURACY-4);
			
			spRotateX(SP_PI*i/15);
			spRotateZ( (i & 1) ?rotation*4:(-rotation*4+SP_PI/11));
			count = spMesh3D( wheel[i], 1 );
			
			memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) ); //glPop()
		}
		spEnableLight(1,0);
		break;
	case 5:
		spRotozoomSurface( screen->w / 4, screen->h / 4, 0, garfield, spSin( rotation * 4 ) + ( 3 << SP_ACCURACY - 1 ) >> 2, spCos( rotation * 8 ) + ( 3 << SP_ACCURACY - 1 ) >> 2, rotation );
		spRotozoomSurfacePart( 3 * screen->w / 4, screen->h / 4, 0, garfield, garfield->w / 4, garfield->h / 4, garfield->w / 2, garfield->w / 2, spSin( rotation * 4 ) + ( 3 << SP_ACCURACY - 1 ) >> 1, spCos( rotation * 8 ) + ( 3 << SP_ACCURACY - 1 ) >> 1, rotation );
		sprite->rotation = 0;
		spDrawSprite( screen->w / 5, 5 * screen->h / 8, 0, sprite );
		sprite->zoomX = spSin( rotation * 8 ) + ( 3 << SP_ACCURACY - 1 );
		sprite->zoomY = spCos( rotation * 6 ) + ( 3 << SP_ACCURACY - 1 );
		spDrawSprite( 2 * screen->w / 5, 5 * screen->h / 8, 0, sprite );
		sprite->rotation = rotation * 4;
		spDrawSprite( 3 * screen->w / 5, 5 * screen->h / 8, 0, sprite );
		sprite->zoomX = SP_ONE;
		sprite->zoomY = SP_ONE;
		spDrawSprite( 4 * screen->w / 5, 5 * screen->h / 8, 0, sprite );
		break;
	case 4:
		srand( 0 );
		for ( i = 0; i < 5; i++ )
			spEllipseBorder( rand() % screen->w, rand() % screen->h, 0, rand() % screen->w / 4, rand() % screen->h / 4, 10, 20, rand() % 65536 );
		for ( i = 0; i < 5; i++ )
			spEllipse( rand() % screen->w, rand() % screen->h, 0, rand() % screen->w / 4, rand() % screen->h / 4, rand() % 65536 );
		for ( i = 0; i < 5; i++ )
			spRectangleBorder( rand() % screen->w, rand() % screen->h, 0,
							   rand() % screen->w / 2, rand() % screen->h / 2, 12, 6, rand() % 65536 );
		for ( i = 0; i < 5; i++ )
			spRectangle( rand() % screen->w, rand() % screen->h, 0,
						 rand() % screen->w / 2, rand() % screen->h / 2, rand() % 65536 );
		for ( i = 0; i < 100; i++ )
			spLine( rand() % screen->w, rand() % screen->h, 0,
					rand() % screen->w, rand() % screen->h, 0, rand() % 65536 );
		break;
	case 3:
		spSetAlphaTest( 0 );
		spTranslate( 0, 0, (-6 << SP_ACCURACY)+spSin(rotation)*6 );
		spRotateX( rotation );
		spRotateY( rotation );
		spRotateZ( rotation );
		count = spMesh3D( mesh, 1 );
		break;
	case 2:
		spSetAlphaTest( 1 );
		spTranslate( 0, 0, ( -11<<SP_ACCURACY ) + spSin( rotation * 4 ) * 3 );
		spRotateZ( rotation );
		spRotateX( spSin( rotation ) >> 2 );
		spRotateY( spCos( rotation * 2 ) >> 2 );
		int x, y;
		for ( x = -6; x <= 6; x++ )
			for ( y = -6; y <= 6; y++ )
				if ( x + y & 1 )
				{
					Sint32 matrix[16];
					memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) );
					spTranslate( x << SP_ACCURACY + 1, y << SP_ACCURACY + 1, 0 );
					spQuad3D( -SP_ONE, SP_ONE, 0,
							  -SP_ONE, -SP_ONE, 0,
							  SP_ONE, -SP_ONE, 0,
							  SP_ONE, SP_ONE, 0, 32767 );
					//spQuadTex3D(-SP_ONE, SP_ONE, 0,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
					//            -SP_ONE,-SP_ONE, 0,1,garfield->h-SP_FONT_EXTRASPACE-1,
					//             SP_ONE,-SP_ONE, 0,garfield->w-SP_FONT_EXTRASPACE-1,garfield->h-SP_FONT_EXTRASPACE-1,
					//             SP_ONE, SP_ONE, 0,garfield->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,65535);
					memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) );
				}
		for ( x = -6; x <= 6; x++ )
			for ( y = -6; y <= 6; y++ )
				if ( !( x + y & 1 ) )
				{
					Sint32 matrix[16];
					memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) );
					spTranslate( x << SP_ACCURACY + 1, y << SP_ACCURACY + 1, 0 );
					spQuad3D( -SP_ONE, SP_ONE, 2 << SP_ACCURACY,
							  -SP_ONE, -SP_ONE, 2 << SP_ACCURACY,
							  SP_ONE, -SP_ONE, 2 << SP_ACCURACY,
							  SP_ONE, SP_ONE, 2 << SP_ACCURACY, 65535 );
					//top
					//if (y<0)
					spQuad3D( -SP_ONE, SP_ONE, 2 << SP_ACCURACY,
							  SP_ONE, SP_ONE, 2 << SP_ACCURACY,
							  SP_ONE, SP_ONE, 0 << SP_ACCURACY,
							  -SP_ONE, SP_ONE, 0 << SP_ACCURACY, 65535 );
					//bottom
					//if (y>0)
					spQuad3D( -SP_ONE, -SP_ONE, 0 << SP_ACCURACY,
							  SP_ONE, -SP_ONE, 0 << SP_ACCURACY,
							  SP_ONE, -SP_ONE, 2 << SP_ACCURACY,
							  -SP_ONE, -SP_ONE, 2 << SP_ACCURACY, 65535 );
					//left
					//if (x>0)
					spQuad3D( -SP_ONE, -SP_ONE, 2 << SP_ACCURACY,
							  -SP_ONE, SP_ONE, 2 << SP_ACCURACY,
							  -SP_ONE, SP_ONE, 0 << SP_ACCURACY,
							  -SP_ONE, -SP_ONE, 0 << SP_ACCURACY, 65535 );
					//right
					//if (x<0)
					spQuad3D( SP_ONE, -SP_ONE, 0 << SP_ACCURACY,
							  SP_ONE, SP_ONE, 0 << SP_ACCURACY,
							  SP_ONE, SP_ONE, 2 << SP_ACCURACY,
							  SP_ONE, -SP_ONE, 2 << SP_ACCURACY, 65535 );
					memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) );
				}
		break;
	case 1:
		spSetAlphaTest( 1 );
		spTranslate( 0, 0, -15 << SP_ACCURACY );
		spRotateY( rotation );
		int a;
		for ( a = 0; a < 16; a++ )
		{
			spRotateY( SP_PI / 8 );
			Sint32 brightness = ( spCos( rotation + a * SP_PI / 8 ) >> SP_HALF_ACCURACY ) * abs( spCos( rotation + a * SP_PI / 8 ) >> SP_HALF_ACCURACY ) / 2 + ( 3 << SP_ACCURACY - 1 );
			Uint16 color = ( ( brightness >> SP_ACCURACY - 4 ) << 11 ) + ( ( brightness >> SP_ACCURACY - 5 ) << 5 ) + ( brightness >> SP_ACCURACY - 4 );
			for ( y = -21; y <= 21; y += 7 )
			{
				if ( ( y + a ) & 8 )
					spQuadTex3D( -3 << SP_ACCURACY - 2, y + 3 << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1, SP_FONT_EXTRASPACE, SP_FONT_EXTRASPACE,
								 -3 << SP_ACCURACY - 2, y - 3 << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1, 1, garfield->h - SP_FONT_EXTRASPACE - 1,
								 3 << SP_ACCURACY - 2, y - 3 << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1, garfield->w - SP_FONT_EXTRASPACE - 1, garfield->h - SP_FONT_EXTRASPACE - 1,
								 3 << SP_ACCURACY - 2, y + 3 << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1, garfield->w - SP_FONT_EXTRASPACE - 1, SP_FONT_EXTRASPACE, color );

				else if ( ( y + a + 1 ) & 8 )
					spRectangle3D( 0, y << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, SDL_GetTicks() / 128 );
				else if ( ( y + a + 2 ) & 8 )
					spEllipse3D( 0, y << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 2, 3 << SP_ACCURACY - 2, -SDL_GetTicks() / 128 );
				else if ( ( y + a + 3 ) & 8 )
					spRectangleBorder3D( 0, y << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 1<<SP_ACCURACY - 2, 1<<SP_ACCURACY - 2, SDL_GetTicks() / 64 );
				else if ( ( y + a + 4 ) & 8 )
					spEllipseBorder3D( 0, y << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 2, 3 << SP_ACCURACY - 2, 1<<SP_ACCURACY - 2, 1<<SP_ACCURACY - 2, -SDL_GetTicks() / 64 );
				else if ( ( y + a + 5 ) & 8 )
					//spBlit3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,pepper);
					spRotozoomSurface3D( 0, y << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1, pepper, spSin( rotation * 4 ) + ( 3 << SP_ACCURACY - 1 ), spCos( rotation * 8 ) + ( 3 << SP_ACCURACY - 1 ), rotation );
				else
					spQuad3D( -3 << SP_ACCURACY - 2, y + 3 << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1,
							  -3 << SP_ACCURACY - 2, y - 3 << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1,
							  3 << SP_ACCURACY - 2, y - 3 << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1,
							  3 << SP_ACCURACY - 2, y + 3 << SP_ACCURACY - 2, 9 << SP_ACCURACY - 1, color );

			}
		}
		break;
	case 0:
		spSetAlphaTest( 0 );
		/*spSetPattern8(0b10101010,
		              0b11011101,
		              0b10101010,
		              0b01010101,
		              0b10101010,
		              0b11011101,
		              0b10101010,
		              0b01010101);*/
		spSetPattern8(170,//0b10101010, //0b doesn't work with older gcc versions
		               85,//0b01010101,
		              170,//0b10101010,
		               85,//0b01010101,
		              170,//0b10101010,
		               85,//0b01010101,
		              170,//0b10101010,
		               85);//0b01010101);
		spSetAlphaPattern4x4(127,0);
		/*spSetPattern8(0b11111111,
		              0b11000111,
		              0b10111011,
		              0b01101101,
		              0b01010101,
		              0b01101101,
		              0b10111011,
		              0b11000111);*/
		spTranslate( spSin( rotation / 3 ), spSin( rotation / 5 ), ( -7 << SP_ACCURACY ) );
		spRotateX( rotation );
		spRotateY( rotation );
		spRotateZ( rotation );

		//Front / Back
		/*Uint16 color1 = 12345 | 31727;
		Uint16 color2 = 23456 | 31727;
		Uint16 color3 = 34567 | 31727;
		Uint16 color4 = 45678 | 31727;
		Uint16 color5 = 56789 | 31727;
		Uint16 color6 = 61234 | 31727;*/
		Uint16 color1 = 0xFFFF;
		Uint16 color2 = 0xFFFF;
		Uint16 color3 = 0xFFFF;
		Uint16 color4 = 0xFFFF;
		Uint16 color5 = 0xFFFF;
		Uint16 color6 = 0xFFFF;
		spBindTexture( garfield );
		spQuadTex3D( -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, garfield->h - 1,
					 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, 0,
					 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, garfield->w - 1, 0,
					 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, garfield->w - 1, garfield->h - 1, color1 );
		spQuadTex3D( 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, garfield->h - 1,
					 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
					 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, garfield->w - 1, 0,
					 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, garfield->w - 1, garfield->h - 1, color2 );
		//Left / Right
		spQuadTex3D( -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, garfield->h - 1,
					 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
					 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, garfield->w - 1, 0,
					 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, garfield->w - 1, garfield->h - 1, color3 );
		spBindTexture( check );
		spQuadTex3D( 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, garfield->h - 1,
					 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
					 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, garfield->w - 1, 0,
					 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, garfield->w - 1, garfield->h - 1, color4 );
		//Up / Down
		spQuadTex3D( 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, garfield->h - 1,
					 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
					 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, garfield->w - 1, 0,
					 -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, garfield->w - 1, garfield->h - 1, color5 );
		spQuadTex3D( -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, 0, garfield->h - 1,
					 -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 0, 0,
					 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, garfield->w - 1, 0,
					 3 << SP_ACCURACY - 1, -3 << SP_ACCURACY - 1, 3 << SP_ACCURACY - 1, garfield->w - 1, garfield->h - 1, color6 );
					 
		spSetAlphaPattern4x4((spSin(rotation) + SP_ONE) / 512 % 512,8);
		//Front / Back
		spTranslate( -3 << SP_ACCURACY, 0, 0 );
		spQuad3D( -SP_ONE, SP_ONE, SP_ONE,
				  -SP_ONE, -SP_ONE, SP_ONE,
				  SP_ONE, -SP_ONE, SP_ONE,
				  SP_ONE, SP_ONE, SP_ONE, spGetRGB((spSin(rotation*4)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation*2)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation)+SP_ONE)>>SP_ACCURACY-7) );
		spQuad3D( SP_ONE, SP_ONE, -SP_ONE,
				  SP_ONE, -SP_ONE, -SP_ONE,
				  -SP_ONE, -SP_ONE, -SP_ONE,
				  -SP_ONE, SP_ONE, -SP_ONE, spGetRGB((spSin(rotation*4)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation*2)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation)+SP_ONE)>>SP_ACCURACY-7) );
		//Left / Right
		spQuad3D( -SP_ONE, SP_ONE, SP_ONE,
				  -SP_ONE, SP_ONE, -SP_ONE,
				  -SP_ONE, -SP_ONE, -SP_ONE,
				  -SP_ONE, -SP_ONE, SP_ONE, spGetRGB((spSin(rotation*4)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation*2)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation)+SP_ONE)>>SP_ACCURACY-7) );
		spQuad3D( SP_ONE, -SP_ONE, SP_ONE,
				  SP_ONE, -SP_ONE, -SP_ONE,
				  SP_ONE, SP_ONE, -SP_ONE,
				  SP_ONE, SP_ONE, SP_ONE, spGetRGB((spSin(rotation*4)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation*2)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation)+SP_ONE)>>SP_ACCURACY-7) );
		//Up / Down
		spQuad3D( SP_ONE, SP_ONE, SP_ONE,
				  SP_ONE, SP_ONE, -SP_ONE,
				  -SP_ONE, SP_ONE, -SP_ONE,
				  -SP_ONE, SP_ONE, SP_ONE, spGetRGB((spSin(rotation*4)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation*2)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation)+SP_ONE)>>SP_ACCURACY-7) );
		spQuad3D( -SP_ONE, -SP_ONE, SP_ONE,
				  -SP_ONE, -SP_ONE, -SP_ONE,
				  SP_ONE, -SP_ONE, -SP_ONE,
				  SP_ONE, -SP_ONE, SP_ONE, spGetRGB((spSin(rotation*4)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation*2)+SP_ONE)>>SP_ACCURACY-7,(spSin(rotation)+SP_ONE)>>SP_ACCURACY-7) );
		spDeactivatePattern();
		break;
	}
	spSetZSet( 0 );
	spSetZTest( 0 );
	spSetAlphaTest( 1 );
	//testing touchscreen
	if (spGetInput()->touchscreen.pressed)
	{
		spBlitSurface(spGetInput()->touchscreen.x,spGetInput()->touchscreen.y,0,pepper);
	}

	spFontDraw( 0, 2, 0, "Previous [L]", font );
	spFontDrawRight( screen->w - 2, 2, 0, "[R] next", font );
	switch (perspective)
	{
		case 0: spFontDrawRight( screen->w - 2, screen->h - 3*font-> maxheight, 0, "[X] perspective off", font ); break;
		case 1: spFontDrawRight( screen->w - 2, screen->h - 3*font-> maxheight, 0, "[X] perspective work around", font ); break;
		case 2: spFontDrawRight( screen->w - 2, screen->h - 3*font-> maxheight, 0, "[X] perspective on", font ); break;
	}
	if (pause)
		spFontDraw( 2, screen->h - 3*font-> maxheight, 0, "[B] play", font );
	else
		spFontDraw( 2, screen->h - 3*font-> maxheight, 0, "[B] pause", font );
	switch ( test )
	{
	case 0:
		spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, "Test 1:\nRotating Cube", font );
		break;
	case 1:
		spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, "Test 2:\n3D Tube", font );
		break;
	case 2:
		spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, "Test 3:\nFulfilling", font );
		break;
	case 3:
		spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, "Test 4:\nMesh Loading", font );
		break;
	case 4:
		spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, "Test 5:\nPrimitives", font );
		break;
	case 5:
		spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, "Test 6:\nSprites & Rotozoom", font );
		break;
	case 6:
		spFontDrawMiddle( screen->w / 2, font->maxheight + 2, 0, "Test 7:\nMAKE IT STOP!", font );
		break;
	}
	if ( quality )
		spFontDraw( 0, screen->h - font->maxheight, 0, "Light On [A]", font );
	else
		spFontDraw( 0, screen->h - font->maxheight, 0, "Light Off [A]", font );
	char buffer[256];
	if ( zStuff )
		spFontDraw( 0, screen->h - font->maxheight * 2, 0, "Z Test/Set On [Y]", font );
	else
		spFontDraw( 0, screen->h - font->maxheight * 2, 0, "Z Test/Set Off [Y]", font );

	sprintf( buffer, "%02i:%02i", divisor / 60000, ( divisor / 1000 ) % 60 );
	//spFontDrawRight( screen->w - 2, screen->h - font->maxheight * 2, 0, buffer, font );
	sprintf( buffer, "fps: %i", spGetFPS() );
	spFontDrawMiddle( screen->w/2, 1, 0, buffer, font );
	if (spIsKeyboardPolled())
		spFontDrawRight( screen->w - 2, screen->h - font->maxheight * 2, 0, "[S] Finish Text", font );
	else
		spFontDrawRight( screen->w - 2, screen->h - font->maxheight * 2, 0, "[S] Enter Text", font );
	spFontDrawRight( screen->w - 2, screen->h - font->maxheight, 0, "[E] Exit", font );
	char utf8buffer[5];
	sprintf(buffer,"Pressing \"%s\"",spFontGetUTF8FromUnicode(lastKey,utf8buffer,5));
	if (lastKey)
		spFontDrawMiddle( screen->w / 2, screen->h /2 - font->maxheight/2, 0, buffer, font );
	if (input[0])
		spFontDrawMiddle( screen->w / 2, screen->h /2 + font->maxheight/2, 0, input, font );

	if (spGetInput()->analog_axis[0] < 0)
	{
		sprintf(buffer,"<");
		int i;
		for (i = 0; i < -spGetInput()->analog_axis[0]; i+=-SP_ANALOG_AXIS_MIN/5)
			sprintf(buffer,"%s-",buffer);
	}	
	else
		sprintf(buffer," ");
	sprintf(buffer,"%s\"%i\" ",buffer,spGetInput()->analog_axis[0]);
	if (spGetInput()->analog_axis[0] > 0)
	{
		int i;
		for (i = 0; i < spGetInput()->analog_axis[0]; i+=SP_ANALOG_AXIS_MAX/5)
			sprintf(buffer,"%s-",buffer);
		sprintf(buffer,"%s>",buffer);
	}	
	spFontDraw( 2, font->maxheight, 0, buffer, font );

	if (spGetInput()->analog_axis[1] < 0)
	{
		sprintf(buffer,"^\n");
		int i;
		for (i = 0; i < -spGetInput()->analog_axis[1]; i+=-SP_ANALOG_AXIS_MIN/5)
			sprintf(buffer,"%s|\n",buffer);
	}	
	else
		sprintf(buffer," ");
	sprintf(buffer,"%s\"%i\"\n",buffer,spGetInput()->analog_axis[1]);
	if (spGetInput()->analog_axis[1] > 0)
	{
		int i;
		for (i = 0; i < spGetInput()->analog_axis[1]; i+=SP_ANALOG_AXIS_MAX/5)
			sprintf(buffer,"%s|\n",buffer);
		sprintf(buffer,"%sv",buffer);
	}	
	spFontDraw( 2, font->maxheight*2, 0, buffer, font );
	if (spIsKeyboardPolled())
		spBlitSurface(screen->w/2,screen->h-spGetVirtualKeyboard()->h/2,0,spGetVirtualKeyboard());
	spFlip();
}


int calc_test( Uint32 steps )
{
	spUpdateSprite( sprite, steps );
	int i;
	for ( i = 0; i < steps; i++ )
	{
		divisor++;
		if ( divisor > 0 )
			fpssum += spGetFPS();
		//if (divisor == 60000)
		//return 1;
	}
	if (no_movement)
		rotation = 20000;//SP_PI;
	else
		rotation += steps << SP_ACCURACY - 11;
	
	if ( spIsKeyboardPolled())
	{
		if ( spGetInput()->button[SP_BUTTON_START] )
		{
			spGetInput()->button[SP_BUTTON_START] = 0;
			spStopKeyboardInput();
		}
		return 0;
	}
	if ( spGetInput()->button[SP_BUTTON_A] )
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		quality = 1 - quality;
	}
	if ( spGetInput()->button[SP_BUTTON_X] )
	{
		spGetInput()->button[SP_BUTTON_X] = 0;
		perspective = (perspective+1) % 3;
		switch (perspective)
		{
			case 0: spSetPerspectiveTextureMapping(0); spSetAffineTextureHack(0); break;
			case 1: spSetAffineTextureHack(1); break;
			case 2: spSetPerspectiveTextureMapping(1); break;
		}
	}
	if ( spGetInput()->button[SP_BUTTON_B] )
	{
		spGetInput()->button[SP_BUTTON_B] = 0;
		pause = 1-pause;
	}
	if (pause)
		rotation -= steps << SP_ACCURACY - 11;

	if ( spGetInput()->button[SP_BUTTON_R] )
	{
		spGetInput()->button[SP_BUTTON_R] = 0;
		test = ( test + 1 ) % 7;
	}
	if ( spGetInput()->button[SP_BUTTON_L] )
	{
		spGetInput()->button[SP_BUTTON_L] = 0;
		test = ( test + 6 ) % 7;
	}
	if ( spGetInput()->button[SP_BUTTON_Y] )
	{
		spGetInput()->button[SP_BUTTON_Y] = 0;
		zStuff = 1 - zStuff;
	}
	if ( spGetInput()->button[SP_BUTTON_SELECT] )
		return 1;
	if ( spGetInput()->button[SP_BUTTON_START] )
	{
		spGetInput()->button[SP_BUTTON_START] = 0;
		spPollKeyboardInput(input,32,SP_BUTTON_RIGHT_MASK | SP_BUTTON_DOWN_MASK);
	}

	return 0;
}

void resize( Uint16 w, Uint16 h )
{
	//Settings up the onboard keyboard:
	if (spGetSizeFactor() <= SP_ONE)
		spSetVirtualKeyboard(SP_VIRTUAL_KEYBOARD_IF_NEEDED,0,h-w*48/320,w,w*48/320,spLoadSurface("./data/keyboard320.png"),spLoadSurface("./data/keyboardShift320.png"));
	else
	if (spGetSizeFactor() <= 2*SP_ONE)
		spSetVirtualKeyboard(SP_VIRTUAL_KEYBOARD_IF_NEEDED,0,h-w*48/320,w,w*48/320,spLoadSurface("./data/keyboard640.png"),spLoadSurface("./data/keyboardShift640.png"));
	else
		spSetVirtualKeyboard(SP_VIRTUAL_KEYBOARD_IF_NEEDED,0,h-w*48/320,w,w*48/320,spLoadSurface("./data/keyboard1280.png"),spLoadSurface("./data/keyboardShift1280.png"));
	
	
	//Setup of the new/resized window
	spSetPerspective( 45.0, ( float )spGetWindowSurface()->w / ( float )spGetWindowSurface()->h, 1.0f, 100.0f );

	//Font Loading
	spFontShadeButtons(1);
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./font/StayPuft.ttf", 13 * spGetSizeFactor() >> SP_ACCURACY );
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

void eventHandling(SDL_Event *event)
{
	if (event->type == SDL_KEYDOWN)
	{
		lastKey = event->key.keysym.unicode;
		char buffer[5];
		printf("keydown event 0x%x = \"%s\" keysym=%i\n",lastKey,spFontGetUTF8FromUnicode(lastKey,buffer,5),event->key.keysym.sym);
	}
	else
	if (event->type == SDL_KEYUP)
		lastKey = 0;
}


int main( int argc, char **argv )
{
	if (argc > 1)
		no_movement = 1;
	//sparrow3D Init
	//spSetDefaultWindowSize( 640, 480 ); //Creates a 640x480 window at PC instead of 320x240
	spInitCore();

	//Setup
	screen = spCreateDefaultWindow();
	spSelectRenderTarget(screen);
	spUsePrecalculatedNormals(1);
	resize( screen->w, screen->h );
	//Textures loading
	garfield = spLoadSurface( "./data/garfield.png" );
	check = spLoadSurface( "./data/check.png" );
	pepper = spLoadSurface( "./data/pepper.png" );
	scientist = spLoadSurface( "./data/science_guy_frames01.png" );
	
	spBindTexture( garfield );

	//Mesh loading
	mesh = spMeshLoadObj( "./data/testmeshuv_tri.obj", garfield, 65535 );
	int i;
	for (i = 0; i < 15; i++)
		wheel[i] = spMeshLoadObj( "./data/wheel.obj", NULL, spGetHSV(i*2*SP_PI/15,255,255));

	//Sprite Creating
	sprite = spNewSprite(NULL);
	spNewSubSpriteTilingRow( sprite, scientist, 1, 1, 22, 46, 24, 48, 9 ,100);
	//spNewSubSpriteWithTiling(sprite,scientist,0,0,32,48,100);
	
	spSetAffineTextureHack(0);

	//All glory the main loop
	spLoop( draw_test, calc_test, 10, resize, eventHandling );
	

	//Winter Wrap up, Winter Wrap up 
	spFontDelete( font );
	spMeshDelete( mesh );
	for (i = 0; i < 15; i++)
		spMeshDelete( wheel[i] );
	spDeleteSprite( sprite );
	spDeleteSurface( garfield );
	spDeleteSurface( check );
	spDeleteSurface( pepper );
	spDeleteSurface( scientist );
	spQuitCore();
	printf( "Average fps: %.1f\n", ( float )fpssum / ( float )divisor );
	return 0;
}
