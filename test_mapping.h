/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#ifndef __TEST_MAPPING__
#define __TEST_MAPPING__

#include <sparrow3d.h>

void init_mapping();
char* caption_mapping(char* caption);
char* settings_mapping(char* caption,int button);
void draw_mapping(int rotation, spFontPointer font);
void calc_mapping(int steps);
void quit_mapping();

#endif
