/*
 The contents of this file are subject to the Mozilla Public License        
 Version 1.1 (the "License"); you may not use this file except in           
 compliance with the License. You may obtain a copy of the License at       
 http://www.mozilla.org/MPL/                                                
                                                                            
 Software distributed under the License is distributed on an "AS IS"        
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the    
 License for the specific language governing rights and limitations         
 under the License.                                                         
                                                                            
 Alternatively, the contents of this file may be used under the terms       
 of the GNU Lesser General Public license (the  "LGPL License"), in which case the  
 provisions of LGPL License are applicable instead of those                  
 above.                                                                     
                                                                            
 For feedback and questions about my Files and Projects please mail me,     
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com                         
*/
#include "sparrowMath.h"
#include <stdlib.h>
#include <math.h>

Sint32 spCosvalue[(2*SP_PI)>>SP_COS_ACCURACY];


PREFIX void spInitMath(void)
{
  int a;
  for (a=0;a<((SP_PI<<1)>>SP_COS_ACCURACY);a++)
    spCosvalue[a]=(Sint32)(cos((float)(a<<SP_COS_ACCURACY)/SP_ACCURACY_FACTOR)*SP_ACCURACY_FACTOR);
}

PREFIX Sint32 spSin(Sint32 value)
{
  return spCos(value+((3*SP_PI)>>1));
}

PREFIX Sint32 spCos(Sint32 value)
{
  return spCosvalue[(abs(value)%(SP_PI<<1))>>SP_COS_ACCURACY];
}

#ifdef ARMCPU
Sint32 spFPdiv(register Sint32 numerator, register Sint32 denominator)
{
//senquack
//    register int32_t quotient;
    register int quotient;
//    asm("num     .req %[numerator]      @ Map Register Equates\n\t"
    __asm__ volatile ("num     .req %[numerator]      @ Map Register Equates\n\t"
        "den     .req %[denominator]\n\t"
        "mod     .req r2\n\t"
        "cnt     .req r3\n\t"
        "quo     .req r4\n\t"
        "sign    .req r12\n\t"
        /* set sign and ensure numerator and denominator are positive */
        "cmp den, #0                    @ exceptioin if den == zero\n\t"
        "beq .div0\n\t"
        "eor sign, num, den             @ sign = num ^ den\n\t"
        "rsbmi den, den, #0             @ den = -den if den < 0\n\t"
        "subs mod, den, #1              @ mod = den - 1\n\t"
        "beq .div1                      @ return if den == 1\n\t"
        "movs cnt, num                  @ num = -num if num < 0\n\t"
        "rsbmi num, num, #0\n\t"
        /* skip if deniminator >= numerator */
        "movs cnt, num, lsr #16         @ return if den >= num << 16\n\t"
        "bne .cont\n\t"
        "cmp den, num, lsl #16\n\t"
        "bhs .numLeDen\n\t"
    "\n.cont:\n\t"
        /* test if denominator is a power of two */
        "tst den, mod                   @ if(den & (den - 1) == 0)\n\t"
//senquack - was missing terminating quote here:
        "beq .powerOf2                  @ den is power of 2\n\t"
        /* count leading zeros */
        "stmfd sp!, {r4}                @ push r4 (quo) onto the stack\n\t"
        "mov cnt, #28                   @ count difference in leading zeros\n\t"
        "mov mod, num, lsr #4           @ between num and den\n\t"
        "cmp den, mod, lsr #12; subls cnt, cnt, #16; movls mod, mod, lsr #16\n\t"
        "cmp den, mod, lsr #4 ; subls cnt, cnt, #8 ; movls mod, mod, lsr #8\n\t"
        "cmp den, mod         ; subls cnt, cnt, #4 ; movls mod, mod, lsr #4\n\t"
        /* shift numerator left by cnt bits */
        "mov num, num, lsl cnt          @ mod:num = num << cnt\n\t"
        "mov quo, #0\n\t"
        "rsb den, den, #0               @ negate den for divide loop\n\t"
        /* skip cnt iterations in the divide loop */
        "adds num, num, num             @ start: num = mod:num / den\n\t"
        "add pc, pc, cnt, lsl #4        @ skip cnt x 4 x 4 iterations\n\t"
        "nop                            @ nop instruction takes care of pipeline\n\t"
        /* inner loop unrolled x 48 */
        ".rept 47                       @ inner loop x 48\n\t"
        "    adcs mod, den, mod, lsl #1\n\t"
        "    subcc mod, mod, den\n\t"
        "    adc quo, quo, quo\n\t"
        "    adds num, num, num\n\t"
        ".endr\n\t"
        "adcs mod, den, mod, lsl #1\n\t"
        "subcc mod, mod, den\n\t"
        "adc quo, quo, quo\n\t"
        /* negate quotient if signed */
        "cmp sign, #0                   @ negate quotient if sign < 0\n\t"
        "mov num, quo\n\t"
        "rsbmi num, num, #0\n\t"
        "ldmfd sp!, {r4}                @ pop r4 (quo) off the stack\n\t"
        "mov pc, lr                     @return\n\t"
        /* divide by zero handler */
    "\n.div0:\n\t"
        "mov num, #0\n\t"
        "mov pc, lr                     @return\n\t"
        /* divide by one handler */
    "\n.div1:\n\t"
        "cmp sign, #0\n\t"
        "mov num, num, asl #16\n\t"
        "rsbmi num, num, #0\n\t"
        "mov pc, lr                     @return\n\t"
        /* numerator less than or equal to denominator handler */
    "\n.numLeDen:\n\t"
        "mov num, #0                    @ quotient = 0 if num < den\n\t"
        "moveq num, sign, asr #31       @ negate quotient if sign < 0\n\t"
        "orreq num, num, #1             @ quotient = 1 if num == den\n\t"
        "mov pc, lr                     @return\n\t"
        /* power of two handler */
    "\n.powerOf2:\n\t"
        "mov cnt, #0\n\t"
        "cmp den, #(1 << 16); movhs cnt, #16    ; movhs den, den, lsr #16\n\t"
        "cmp den, #(1 << 8) ; addhs cnt, cnt, #8; movhs den, den, lsr #8\n\t"
        "cmp den, #(1 << 4) ; addhs cnt, cnt, #4; movhs den, den, lsr #4\n\t"
        "cmp den, #(1 << 2) ; addhi cnt, cnt, #3; addls cnt, cnt, den, lsr #1\n\t"
        "rsb mod, cnt, #32\n\t"
        "mov den, num, lsr #16          @ den:num = num << 16\n\t"
        "mov num, num, lsl #16\n\t"
        "mov num, num, lsr cnt          @ num = num >> cnt | den << mod\n\t"
        "orr num, num, den, lsl mod\n\t"
        "cmp sign, #0\n\t"
        "rsbmi num, num, #0             @ negate quotient if sign < 0"
        /* output registers */
        : [quotient] "=r" (quotient)
        /* input registers */
        : [numerator] "0" (numerator), [denominator] "r" (denominator)
        /* clobbered registers */
        : "r2" /* mod */, "r3" /* cnt */, "r12" /* sign */);
    return quotient;
}
#else
Sint32 spFPdiv(Sint32 numerator,Sint32 denominator)
{
  return ((numerator<<SP_HALF_ACCURACY)/denominator)<<SP_HALF_ACCURACY;
}
#endif

//senquack - credit for this fast sqrt goes to Wilco Dijkstra http://www.finesse.demon.co.uk/steven/sqrt.html
#define iter1(N) \
    try_ = root + (1 << (N)); \
    if (n >= try_ << (N))   \
    {   n -= try_ << (N);   \
        root |= 2 << (N); \
    }

PREFIX Sint32 spSqrt (Sint32 n)
{
    Sint32 root = 0, try_;
    iter1 (15);    iter1 (14);    iter1 (13);    iter1 (12);
    iter1 (11);    iter1 (10);    iter1 ( 9);    iter1 ( 8);
    iter1 ( 7);    iter1 ( 6);    iter1 ( 5);    iter1 ( 4);
    iter1 ( 3);    iter1 ( 2);    iter1 ( 1);    iter1 ( 0);
    return root << (SP_HALF_ACCURACY-1);
}


PREFIX Sint32 spMin(Sint32 a,Sint32 b)
{
  if (a<b)
    return a;
  return b;
}

PREFIX Sint32 spMin3(Sint32 a,Sint32 b, Sint32 c)
{
  
}

PREFIX Sint32 spMin4(Sint32 a,Sint32 b, Sint32 c, Sint32 d)
{
  
}

PREFIX Sint32 spMax(Sint32 a,Sint32 b)
{
  
}

PREFIX Sint32 spMax3(Sint32 a,Sint32 b, Sint32 c)
{
  
}

PREFIX Sint32 spMax4(Sint32 a,Sint32 b, Sint32 c, Sint32 d)
{
  
}
