/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#ifndef __TEST_MESH__
#define __TEST_MESH__

void init_mesh(char* parameter1,char* parameter2);
char* caption_mesh(char* caption);
char* settings_mesh(char* caption,int button);
void draw_mesh(int rotation);
void calc_mesh();
void quit_mesh();

#endif
