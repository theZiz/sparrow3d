 /* This file is part of sparrow3d.
  * Sparrow3d is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * Sparrow3d is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with Foobar.  If not, see <http://www.gnu.org/licenses/>
  * 
  * For feedback and questions about my Files and Projects please mail me,
  * Alexander Matthes (Ziz) , zizsdl_at_googlemail.com */

#include "sparrowPrimitivesAsm.h"
#include <stdlib.h>

#ifdef __arm__
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
	asm volatile( //  saving r3-r12 to the stack
		"stmfd sp!,{r3-r12} \n\t"
		//catching "awkward" laying Halfword at the beginning
		"mov r3,%0,lsr #2 \n\t"
		"subs r4,%0,r3,asl #2 \n\t"
		"beq .all_ok \n\t"
		"strh %2,[%0],#2 \n\t"
		"subs %1,%1,#1 \n\t"
		".all_ok: \n\t"
		//if l>=22 then
		//  jmp twentytwo;
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
		//  assining r2 on itself, but shifted
		"orr %2,%2,%2,lsl #16 \n\t"
		//  setting r3-r12 to r2
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
		//  setting pos til pos+43 to r2-r12
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
		//  mirroring r2 auf sich selbst spiegeln
		"orr %2,%2,%2,lsl #16 \n\t"
		//  setting r3-r7 to r2
		"mov r3,%2 \n\t"
		"mov r4,%2 \n\t"
		"mov r5,%2 \n\t"
		"mov r6,%2 \n\t"
		"mov r7,%2 \n\t"
		//  again12:
		".again12: \n\t"
		//  setting pos..pos+23 to r2-r7
		"stmia %0!,{%2,r3-r7} \n\t"
		//  l-=12;
		"subs %1,%1,#12 \n\t"
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
		//  mirroring r2 to itself
		"orr %2,%2,%2,lsl #16 \n\t"
		//  setting r3 and r4 to r2
		"mov r3,%2 \n\t"
		"mov r4,%2 \n\t"
		//  again6:
		".again6: \n\t"
		//  pos+12 auf r2-r4 setzen
		"stmia %0!,{%2,r3-r4} \n\t"
		//  l-=6;
		"subs %1,%1,#6 \n\t"
		//thelast:
		".thelast: \n\t"
		//  if l=0 then
		//    jmp end;
		"cmp %1,#0 \n\t"
		"beq .end \n\t"
		//  again:
		".again: \n\t"
		//  Setting color at pos directly and increment
		"strh %2,[%0],#2 \n\t"
		//  l--;
		"subs %1,%1,#1 \n\t"
		//  if l!=0:
		//    jmp again;
		"bne .again \n\t"
		//end:
		".end: \n\t"
		//Poping the registers
		"ldmfd sp!,{r3-r12} \n\t"
		:
		: "r" ( pos ), "r"( l ), "r"( color )
		: /*"r3","r4", "r5", "r6", "r7","r8", "r9", "r10", "r11","r12", */"cc" );
}
#endif
