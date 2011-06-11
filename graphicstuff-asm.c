#include "graphicstuff.h"

int engineWindowXasm;
int engineWindowYasm;

void engineSetWindowXasm(int x)
{
  engineWindowXasm=x;
}

void engineSetWindowYasm(int y)
{
  engineWindowYasm=y;
}

#ifdef ARMCPU
void hline(SDL_Surface* screen,Sint32 x,Sint32 y,Sint32 l_,Uint16 color_,char check)
{
  //l_++;
  if (check)
  { 
    if (y<0)
      return;
    if (y>=engineWindowYasm) //Their is a FUCKING Bug if I write in the last line on the gp2x. Dont know why...
      return;
    if (x<0)
    {
      l_=x+l_;
      x=0;
    }
    if (x+l_>=engineWindowXasm)
      l_=engineWindowXasm-x;
    if (l_<=0)
      return;
  }
  Uint16* pixel=(Uint16*)screen->pixels;
  register Uint32 pos asm("r0");
  pos=((x+y*engineWindowXasm)<<1)+(Uint32)pixel;
  register Uint32 l asm("r1");
  l=l_;
  register Uint32 color asm("r2");
  color=color_;
  asm volatile(//  r3-r12 auf dem Stack retten
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
               : "r" (pos), "r"(l), "r"(color)
               : "r3","r4", "r5", "r6", "r7","r8", "r9", "r10", "r11","r12", "cc");
}    
#endif

//senquack - fast ARM ASM 16:16 fixed point divide routine:
//					Credit goes to Henry Thomas and the website is
//					http://me.henri.net/fp-div.html
//int32_t fpdiv(register int32_t numerator, register int32_t denominator)
#ifdef ARMCPU
int fpdiv(register int numerator, register int denominator)
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
#endif
