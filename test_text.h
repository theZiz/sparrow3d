/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#ifndef __TEST_TEXT__
#define __TEST_TEXT__

#include <sparrow3d.h>

void init_text(int argc, char** argv, spFontPointer font);
char* caption_text(char* caption);
char* settings_text(char* caption,int button);
void draw_text(int rotation, spFontPointer font);
void calc_text(int steps);
void quit_text();

#endif
