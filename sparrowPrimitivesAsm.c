/*
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms of the
 GNU Lesser General Public license (the  "LGPL License") version 2 or higher, in
 which case the provisions of LGPL License are applicable instead of those above
 
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include "sparrowPrimitives.h"
#include <stdlib.h>

#ifdef ARMCPU
PREFIX void spHorizentalLine( Uint16* pixel, Sint32 x, Sint32 y, Sint32 l_, Uint32 color_, Uint32 check, Sint32 engineWindowX, Sint32 engineWindowY )
{
	//l_++;
	if ( check )
	{
		if ( y < 0 )
			return;
		if ( y >= engineWindowY ) //Their is a FUCKING Bug if I write in the last line on the gp2x. Dont know why...
			return;
		if ( x < 0 )
		{
			l_ = x + l_;
			x = 0;
		}
		if ( x + l_ >= engineWindowX )
			l_ = engineWindowX - x;
		if ( l_ <= 0 )
			return;
	}
	register Uint32 pos asm( "r0" );
	pos = ( ( x + y * engineWindowX ) << 1 ) + ( Uint32 )pixel;
	register Uint32 l asm( "r1" );
	l = l_;
	register Uint32 color asm( "r2" );
	color = color_;
	asm volatile( //  r3-r12 auf dem Stack retten
		"stmfd sp!,{r3-r12} \n\t"
		//"Doof gelegenes" Halfword am Anfang abfangen
		"mov r3,%0,lsr #2 \n\t"
		"subs r4,%0,r3,asl #2 \n\t"
		"beq .all_ok \n\t"
		"strh %2,[%0],#2 \n\t"
		"subs %1,%1,#1 \n\t"
		".all_ok: \n\t"
		//if l>=22 then
		//jmp twentytwo;
		"cmp %1,#22\n\t"
		"bge .twentytwo\n\t"
		//if l>=12 then
		//  jmp twelve;
		"cmp %1,#12\n\t"
		"bge .twelve \n\t"
		//if l>=6 then
		//  jmp six;
		"cmp %1,#6\n\t"
		"bge .six \n\t"
		//else
		//  jmp thelast;
		"bne .thelast\n\t"
		//twentytwo:
		".twentytwo: \n\t"
		//  r2 auf sich selbst spiegeln
		"orr %2,%2,%2,lsl #16 \n\t"
		//  r3-r12 auf r2 setzen
		"mov r3,%2 \n\t"
		"mov r4,%2 \n\t"
		"mov r5,%2 \n\t"
		"mov r6,%2 \n\t"
		"mov r7,%2 \n\t"
		"mov r8,%2 \n\t"
		"mov r9,%2 \n\t"
		"mov r10,%2 \n\t"
		"mov r11,%2 \n\t"
		"mov r12,%2 \n\t"
		//  again22:
		".again22: \n\t"
		//  pos+44 auf r2-r12 setzen
		"stmia %0!,{%2,r3-r12} \n\t"
		//  l-=22;
		"subs %1,%1,#22 \n\t"
		//  if l>=22 then
		//    jmp again22;
		"cmp %1,#22 \n\t"
		"bge .again22\n\t"
		//  if l>=12 then
		//    jmp again12;
		"cmp %1,#12 \n\t"
		"bge .again12\n\t"
		//  if l>=6 then
		//    jmp again6;
		"cmp %1,#6 \n\t"
		"bge .again6\n\t"
		//  else
		//    jmp thelast;
		"bne .thelast\n\t"
		" \n\t"
		//twelve:
		".twelve: \n\t"
		//  r2 auf sich selbst spiegeln
		"orr %2,%2,%2,lsl #16 \n\t"
		//  r3-r7 auf r2 setzen
		"mov r3,%2 \n\t"
		"mov r4,%2 \n\t"
		"mov r5,%2 \n\t"
		"mov r6,%2 \n\t"
		"mov r7,%2 \n\t"
		//  again12:
		".again12: \n\t"
		//  pos+24 auf r2-r7 setzen
		"stmia %0!,{%2,r3-r7} \n\t"
		//  l-=12;
		"subs %1,%1,#12 \n\t"
		//  if l>=12 then
		//    jmp again12;
		"cmp %1,#12 \n\t"
		"bge .again12\n\t"
		//  if l>=6 then
		//    jmp again6;
		"cmp %1,#6 \n\t"
		"bge .again6\n\t"
		//  else
		//    jmp thelast;
		"bne .thelast\n\t"
		" \n\t"
		//six:
		".six: \n\t"
		//  r2 auf sich selbst spiegeln
		"orr %2,%2,%2,lsl #16 \n\t"
		//  r3-r4 auf r2 setzen
		"mov r3,%2 \n\t"
		"mov r4,%2 \n\t"
		//  again6:
		".again6: \n\t"
		//  pos+12 auf r2-r4 setzen
		"stmia %0!,{%2,r3-r4} \n\t"
		//  l-=6;
		"subs %1,%1,#6 \n\t"
		//  if l>=6 then
		//    jmp again6;
		"cmp %1,#6 \n\t"
		"bge .again6 \n\t"
		//thelast:
		".thelast: \n\t"
		//  if l=0 then
		//    jmp end;
		"cmp %1,#0 \n\t"
		"beq .end \n\t"
		//  again:
//               "subs %0,%0,#2 \n\t"
//               "adds %1,%1,#1 \n\t"
		".again: \n\t"
		//  Speicherstelle an Position pos setzen und direkt inkrementieren
		"strh %2,[%0],#2 \n\t"
		//  l um 1 verkleinern
		"subs %1,%1,#1 \n\t"
		//  if l!=0:
		//    jmp again;
		"bne .again \n\t"
		//end:
		".end: \n\t"
		"ldmfd sp!,{r3-r12} \n\t"
		:
		: "r" ( pos ), "r"( l ), "r"( color )
		: /*"r3","r4", "r5", "r6", "r7","r8", "r9", "r10", "r11","r12", */"cc" );
}
#endif
