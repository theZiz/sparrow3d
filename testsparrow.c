/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,		 
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com												 
*/
#include "../sparrow3d/sparrow3d.h"
#include <SDL_image.h>

SDL_Surface* screen;
SDL_Surface* garfield;
SDL_Surface* pepper;
SDL_Surface* scientist;
spModelPointer mesh;
spSpritePointer sprite;
Sint32 rotation = 0;
spFontPointer font = NULL;
int quality=1;
Uint32 fpssum = 0;
Sint32 divisor = -5000;
int test = 0;
int count;
int zStuff = 1;

void draw_test(void)
{
	spResetZBuffer();
	if (test == 5)
		spClearTarget((spSin(rotation*8)>>SP_ACCURACY-4)+16);
	else
		spClearTarget(0);
	spIdentity();
	
	count = 0;
	
	spBindTexture(garfield);
	spSetLight(quality);
	spSetCulling(1);
	spSetZSet(zStuff);
	spSetZTest(zStuff);
	int i;

	switch (test)
	{
		case 5:
				spRotozoomSurface(screen->w/4,screen->h/4,-1,garfield,spSin(rotation*4)+(3<<SP_ACCURACY-1)>>2,spCos(rotation*8)+(3<<SP_ACCURACY-1)>>2,rotation);
				spRotozoomSurfacePart(3*screen->w/4,screen->h/4,-1,garfield,garfield->w/4,garfield->h/4,garfield->w/2,garfield->w/2,spSin(rotation*4)+(3<<SP_ACCURACY-1)>>1,spCos(rotation*8)+(3<<SP_ACCURACY-1)>>1,rotation);
				sprite->rotation = 0;
				spDrawSprite(screen->w/5,5*screen->h/8,-1,sprite);
				sprite->zoomX = spSin(rotation*8)+(3<<SP_ACCURACY-1);
				sprite->zoomY = spCos(rotation*6)+(3<<SP_ACCURACY-1);
				spDrawSprite(2*screen->w/5,5*screen->h/8,-1,sprite);
				sprite->rotation = rotation*4;
				spDrawSprite(3*screen->w/5,5*screen->h/8,-1,sprite);
				sprite->zoomX = 1<<SP_ACCURACY;
				sprite->zoomY = 1<<SP_ACCURACY;
				spDrawSprite(4*screen->w/5,5*screen->h/8,-1,sprite);
			break;
		case 4:
			srand(0);
			for (i = 0;i<5;i++)
				spEllipseBorder(rand()%screen->w,rand()%screen->h,-1,rand()%screen->w/4,rand()%screen->h/4,10,20,rand()%65536);
			for (i = 0;i<5;i++)
				spEllipse(rand()%screen->w,rand()%screen->h,-1,rand()%screen->w/4,rand()%screen->h/4,rand()%65536);
			for (i = 0;i<5;i++)
				spRectangleBorder(rand()%screen->w,rand()%screen->h,-1,
													rand()%screen->w/2,rand()%screen->h/2,12,6,rand()%65536);
			for (i = 0;i<5;i++)
				spRectangle(rand()%screen->w,rand()%screen->h,-1,
										rand()%screen->w/2,rand()%screen->h/2,rand()%65536);
			for (i = 0;i<100;i++)
				spLine(rand()%screen->w,rand()%screen->h,-1,
							 rand()%screen->w,rand()%screen->h,-1,rand()%65536);
			break;
		case 3:
			spSetAlphaTest(0);
			spTranslate(0,0,-8<<SP_ACCURACY);
			spRotateX(rotation);
			spRotateY(rotation);
			spRotateZ(rotation);
			count = spMesh3D(mesh,1);
			break;
		case 2:
			spSetAlphaTest(1);
			spTranslate(0,0,(-11<<SP_ACCURACY)+spSin(rotation*4)*3);	
			spRotateZ(rotation);
			spRotateX(spSin(rotation)>>2);
			spRotateY(spCos(rotation*2)>>2);
			int x,y;
			for (x = -6; x <= 6; x++)
				for (y = -6; y<= 6; y++)
					if (x+y & 1)
					{
						Sint32 matrix[16];
						memcpy(matrix,spGetMatrix(),16*sizeof(Sint32));
						spTranslate(x<<SP_ACCURACY+1,y<<SP_ACCURACY+1,0);
						spQuad3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 0,
										 -1<<SP_ACCURACY,-1<<SP_ACCURACY, 0,
											1<<SP_ACCURACY,-1<<SP_ACCURACY, 0,
											1<<SP_ACCURACY, 1<<SP_ACCURACY, 0,32767);
						//spQuadTex3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 0,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
						//						-1<<SP_ACCURACY,-1<<SP_ACCURACY, 0,1,garfield->h-SP_FONT_EXTRASPACE-1,
						//						 1<<SP_ACCURACY,-1<<SP_ACCURACY, 0,garfield->w-SP_FONT_EXTRASPACE-1,garfield->h-SP_FONT_EXTRASPACE-1,
						//						 1<<SP_ACCURACY, 1<<SP_ACCURACY, 0,garfield->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,65535);				
						memcpy(spGetMatrix(),matrix,16*sizeof(Sint32));			
					}
			for (x = -6; x <= 6; x++)
				for (y = -6; y<= 6; y++)
					if (!(x+y & 1))
					{
						Sint32 matrix[16];
						memcpy(matrix,spGetMatrix(),16*sizeof(Sint32));
						spTranslate(x<<SP_ACCURACY+1,y<<SP_ACCURACY+1,0);
						spQuad3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,
										 -1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,
											1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,
											1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,65535);
						//top
						//if (y<0)
						spQuad3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,
											1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,
											1<<SP_ACCURACY, 1<<SP_ACCURACY, 0<<SP_ACCURACY,
										 -1<<SP_ACCURACY, 1<<SP_ACCURACY, 0<<SP_ACCURACY,65535);
						//bottom
						//if (y>0)
						spQuad3D(-1<<SP_ACCURACY,-1<<SP_ACCURACY, 0<<SP_ACCURACY,
											1<<SP_ACCURACY,-1<<SP_ACCURACY, 0<<SP_ACCURACY,
											1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,
										 -1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,65535);
						//left
						//if (x>0)
						spQuad3D(-1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,
										 -1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,
										 -1<<SP_ACCURACY, 1<<SP_ACCURACY, 0<<SP_ACCURACY,
										 -1<<SP_ACCURACY,-1<<SP_ACCURACY, 0<<SP_ACCURACY,65535);
						//right
						//if (x<0)
						spQuad3D( 1<<SP_ACCURACY,-1<<SP_ACCURACY, 0<<SP_ACCURACY,
											1<<SP_ACCURACY, 1<<SP_ACCURACY, 0<<SP_ACCURACY,
											1<<SP_ACCURACY, 1<<SP_ACCURACY, 2<<SP_ACCURACY,
											1<<SP_ACCURACY,-1<<SP_ACCURACY, 2<<SP_ACCURACY,65535);
						memcpy(spGetMatrix(),matrix,16*sizeof(Sint32));			
					}
			break;
		case 1:
			spSetAlphaTest(1);
			spTranslate(0,0,-15<<SP_ACCURACY);	
			spRotateY(rotation);
			int a;
			for (a = 0; a<16; a++)
			{
				spRotateY(SP_PI/8);
				Sint32 brightness = (spCos(rotation+a*SP_PI/8)>>SP_HALF_ACCURACY)*abs(spCos(rotation+a*SP_PI/8)>>SP_HALF_ACCURACY)/2+(3<<SP_ACCURACY-1);
				Uint16 color = ((brightness>>SP_ACCURACY-4)<<11)+((brightness>>SP_ACCURACY-5)<<5)+(brightness>>SP_ACCURACY-4);
				for (y = -21; y<=21; y+=7)
				{
					if ((y+a) & 8)
						spQuadTex3D(-3<<SP_ACCURACY-2,y+3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
												-3<<SP_ACCURACY-2,y-3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,1,garfield->h-SP_FONT_EXTRASPACE-1,
												 3<<SP_ACCURACY-2,y-3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,garfield->w-SP_FONT_EXTRASPACE-1,garfield->h-SP_FONT_EXTRASPACE-1,
												 3<<SP_ACCURACY-2,y+3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,garfield->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,color);
					
					else
					if ((y+a+1) & 8)
						spRectangle3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,3<<SP_ACCURACY-1,3<<SP_ACCURACY-1,SDL_GetTicks()/128);
					else
					if ((y+a+2) & 8)
						spEllipse3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,3<<SP_ACCURACY-2,3<<SP_ACCURACY-2,-SDL_GetTicks()/128);
					else
					if ((y+a+3) & 8)
						spRectangleBorder3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,3<<SP_ACCURACY-1,3<<SP_ACCURACY-1,1<<SP_ACCURACY-2,1<<SP_ACCURACY-2,SDL_GetTicks()/64);
					else
					if ((y+a+4) & 8)
						spEllipseBorder3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,3<<SP_ACCURACY-2,3<<SP_ACCURACY-2,1<<SP_ACCURACY-2,1<<SP_ACCURACY-2,-SDL_GetTicks()/64);
					else
					if ((y+a+5) & 8)
						//spBlit3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,pepper);
						spRotozoomSurface3D(0,y<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,pepper,spSin(rotation*4)+(3<<SP_ACCURACY-1),spCos(rotation*8)+(3<<SP_ACCURACY-1),rotation);
					else
						spQuad3D(-3<<SP_ACCURACY-2,y+3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,
										 -3<<SP_ACCURACY-2,y-3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,
											3<<SP_ACCURACY-2,y-3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,
											3<<SP_ACCURACY-2,y+3<<SP_ACCURACY-2, 9<<SP_ACCURACY-1,color);
					
				}
			}
			break;
		case 0: 
			spSetAlphaTest(0);
			spTranslate(spSin(rotation/3),spSin(rotation/5),(-7<<SP_ACCURACY));
			spRotateX(rotation);
			spRotateY(rotation);
			spRotateZ(rotation);
			
			//Front / Back
			spQuadTex3D(-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,garfield->h-1,
									-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,0,
									 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,0,
									 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,12345 | 31727);
			spQuadTex3D( 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,garfield->h-1,
									 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,0,
									-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,0,
									-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,23456 | 31727);
			//Left / Right
			spQuadTex3D(-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,garfield->h-1,
									-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,0,
									-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,0,
									-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,34567 | 31727);
			spQuadTex3D( 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,garfield->h-1,
									 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,0,
									 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,0,
									 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,45678 | 31727);	
			//Up / Down
			spQuadTex3D( 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,garfield->h-1,
									 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,0,
									-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,0,
									-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,56789 | 31727);
			spQuadTex3D(-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,0,garfield->h-1,
									-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,0,0,
									 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,garfield->w-1,0,
									 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,garfield->w-1,garfield->h-1,61234 | 31727);
			//Front / Back
			spTranslate(-3<<SP_ACCURACY,0,0);
			spQuad3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,
							 -1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,
								1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,
								1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,12345 | 31727);
			spQuad3D( 1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,
								1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
							 -1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
							 -1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,23456 | 31727);
			//Left / Right
			spQuad3D(-1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,
							 -1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,
							 -1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
							 -1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,34567 | 31727);
			spQuad3D( 1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,
								1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
								1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,
								1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,45678 | 31727);	
			//Up / Down
			spQuad3D( 1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,
								1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,
							 -1<<SP_ACCURACY, 1<<SP_ACCURACY,-1<<SP_ACCURACY,
							 -1<<SP_ACCURACY, 1<<SP_ACCURACY, 1<<SP_ACCURACY,56789 | 31727);
			spQuad3D(-1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,
							 -1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
								1<<SP_ACCURACY,-1<<SP_ACCURACY,-1<<SP_ACCURACY,
								1<<SP_ACCURACY,-1<<SP_ACCURACY, 1<<SP_ACCURACY,61234 | 31727);
			break;
	}
	spSetZSet(0);
	spSetZTest(0);
	spSetAlphaTest(1);
	spFontDraw(0,2,-1,"Previous ("SP_BUTTON_L_NAME")",font);
	spFontDrawRight(screen->w-2,2,-1,"("SP_BUTTON_R_NAME") Next",font);
	switch (test)
	{
		case 0:
			spFontDrawMiddle(screen->w/2,font->maxheight+2,-1,"Test 1: Rotating Cube",font);
			break;
		case 1:
			spFontDrawMiddle(screen->w/2,font->maxheight+2,-1,"Test 2: 3D Tube",font);
			break;
		case 2:
			spFontDrawMiddle(screen->w/2,font->maxheight+2,-1,"Test 3: Fulfilling",font);
			break;
		case 3:
			spFontDrawMiddle(screen->w/2,font->maxheight+2,-1,"Test 4: Mesh Loading",font);
			break;
		case 4:
			spFontDrawMiddle(screen->w/2,font->maxheight+2,-1,"Test 5: Primitives",font);
			break;
		case 5:
			spFontDrawMiddle(screen->w/2,font->maxheight+2,-1,"Test 6: Sprites & Rotozoom",font);
			break;
	}
	if (quality)
		spFontDraw(0,screen->h-font->maxheight,-1,"Light On ("SP_BUTTON_A_NAME")",font);
	else
		spFontDraw(0,screen->h-font->maxheight,-1,"Light Off ("SP_BUTTON_A_NAME")",font);
	char buffer[256];
	if (zStuff)
		spFontDraw(0,screen->h-font->maxheight*2,-1,"Z Test/Set On ("SP_BUTTON_Y_NAME")",font);
	else
		spFontDraw(0,screen->h-font->maxheight*2,-1,"Z Test/Set Off ("SP_BUTTON_Y_NAME")",font);
	sprintf(buffer,"%02i:%02i",divisor/60000,(divisor/1000)%60);
	spFontDrawRight(screen->w-2,screen->h-font->maxheight*2,-1,buffer,font);
	sprintf(buffer,"fps: %i",spGetFPS());
	spFontDrawRight(screen->w-2,screen->h-font->maxheight,-1,buffer,font);
	
	spFlip();
}


int calc_test(Uint32 steps)
{
	spUpdateSprite(sprite,steps);
	int i;
	for (i = 0;i<steps;i++)
	{
		divisor++;
		if (divisor>0)
			fpssum+=spGetFPS();
		//if (divisor == 60000)
			//return 1;
	}
	rotation+=steps<<SP_ACCURACY-11;
	if (spGetInput()->button[SP_BUTTON_A])
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		quality = 1-quality;
	}
	if (spGetInput()->button[SP_BUTTON_R])
	{
		spGetInput()->button[SP_BUTTON_R] = 0;
		test = (test+1)%6;
	}
	if (spGetInput()->button[SP_BUTTON_L])
	{
		spGetInput()->button[SP_BUTTON_L] = 0;
		test = (test+5)%6;
	}
	if (spGetInput()->button[SP_BUTTON_Y])
	{
		spGetInput()->button[SP_BUTTON_Y] = 0;
		zStuff = 1-zStuff;
	}
	if (spGetInput()->button[SP_BUTTON_START])
		return 1;
	return 0; 
}

void resize(Uint16 w,Uint16 h)
{
	//Setup of the new/resized window
	spSelectRenderTarget(spGetWindowSurface());
	spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,0.1,100);

	//Font Loading
	if (font)
		spFontDelete(font);
	font = spFontLoad("./font/StayPuft.ttf",20*spGetSizeFactor()>>SP_ACCURACY);
	spFontAddRange(font,' ','~',0);//whole ASCII
	spFontAddBorder(font,65535);
}


int main(int argc, char **argv)
{
	//sparrow3D Init
	spSetDefaultWindowSize(640,480); //Creates a 640x480 window at PC instead of 320x240
	spInitCore();
	
	//Setup
	screen = spCreateDefaultWindow();
	resize(screen->w,screen->h);
	
	//Textures loading
	garfield = spLoadSurface("./data/garfield.png");
	pepper = spLoadSurface("./data/pepper.png");
	scientist = spLoadSurface("./data/science_guy_frames01.png");
	spBindTexture(garfield);
	
	//Mesh loading
	mesh = spMeshLoadObj("./data/testmeshuv_tri.obj",garfield,65535);
	//mesh = spMeshLoadObj("./data/bamuv.obj",garfield,65535);
	//mesh = spMeshLoadObj("./data/foobar.obj",garfield,65535);
	
	//Sprite Creating
	sprite = spNewSprite();
	int i;
	for (i = 0;i<9;i++)
		spNewSubSpriteWithTiling(sprite,scientist,i*24+1,1,22,46,100);
	//spNewSubSpriteWithTiling(sprite,scientist,0,0,32,48,100);
	
	//All glory the main loop
	spLoop(draw_test,calc_test,10,resize,NULL);
	
	//Winter Wrap up, Winter Wrap up …
	spFontDelete(font);
	spMeshDelete(mesh);
	spDeleteSprite(sprite);
	SDL_FreeSurface(garfield);
	SDL_FreeSurface(pepper);
	SDL_FreeSurface(scientist);
	spQuitCore();
	printf("Average fps: %.1f\n",(float)fpssum/(float)divisor);
	return 0;
}
