/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#ifndef __TEST_SPRITES__
#define __TEST_SPRITES__

void init_sprites();
char* caption_sprites(char* caption);
char* settings_sprites(char* caption,int button);
void draw_sprites(int rotation);
void calc_sprites(int steps);
void quit_sprites();

#endif
