//--------------------------------------------------------------------------
//
//  Software for MSP430 based e-meters.
//
//  THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
//  REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY, 
//  INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS 
//  FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR 
//  COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE. 
//  TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET 
//  POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY 
//  INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR 
//  YOUR USE OF THE PROGRAM.
//
//  IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL, 
//  CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY 
//  THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT 
//  OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM. 
//  EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF 
//  REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS 
//  OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF 
//  USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S 
//  AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF 
//  YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS 
//  (U.S.$500).
//
//  Unless otherwise stated, the Program written and copyrighted 
//  by Texas Instruments is distributed as "freeware".  You may, 
//  only under TI's copyright in the Program, use and modify the 
//  Program without any charge or restriction.  You may 
//  distribute to third parties, provided that you transfer a 
//  copy of this license to the third party and the third party 
//  agrees to these terms by its first use of the Program. You 
//  must reproduce the copyright notice and any other legend of 
//  ownership on each copy or partial copy, of the Program.
//
//  You acknowledge and agree that the Program contains 
//  copyrighted material, trade secrets and other TI proprietary 
//  information and is protected by copyright laws, 
//  international copyright treaties, and trade secret laws, as 
//  well as other intellectual property laws.  To protect TI's 
//  rights in the Program, you agree not to decompile, reverse 
//  engineer, disassemble or otherwise translate any object code 
//  versions of the Program to a human-readable form.  You agree 
//  that in no event will you alter, remove or destroy any 
//  copyright notice included in the Program.  TI reserves all 
//  rights not specifically granted under this license. Except 
//  as specifically provided herein, nothing in this agreement 
//  shall be construed as conferring by implication, estoppel, 
//  or otherwise, upon you, any license or other right under any 
//  TI patents, copyrights or trade secrets.
//
//  You may not use the Program in non-TI devices.
//
//  File: q1_15_mulq.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: q1_15_mulq.c,v 1.3 2007/08/07 05:53:47 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

uint16_t rndnum;
uint16_t tpdnum;

//Q1.15 style 16x16=>16 multiply with TPD (triangular
//probability density) dithered rounding of the result.
int16_t Q1_15_mulq(register int16_t x, register int16_t y)
{
    register int16_t x1;
    register int32_t z;

#if defined(__GNUC__)  &&  defined(__MSP430__)
#if defined(__MSP430_HAS_MPY__)
    __asm__ (
"__MPY=0x130 \n"
"__MPYS=0x132 \n"
"__MAC=0x134 \n"
"__MACS=0x136 \n"
"__OP2=0x138 \n"
"__RESLO=0x13a \n"
"__RESHI=0x13c \n"
"__SUMEXT=0x13e \n"
        " push.w  r2 \n"
        " dint \n"
        " mov   %[x],&__MPYS \n"
        " mov   %[y],&__OP2 \n"
        " mov   &__RESHI,%B[z] \n"
        " mov   &__RESLO,%A[z] \n"
        " pop.w r2 \n"
        //Do TPD dithering at the Q1.15 level
        " add   %4,%A[z] \n"
        " addc  #0,%B[z] \n"
        //Shift to Q1.15 format (i.e. the top 16 bits are returned)
        " rla   %A[z] \n"
        " rlc   %B[z] \n"
        " mov   %B[z],%[x1] \n"
        : [z] "=r"(z), [x1] "=r"(x1)
        : [x] "r"(x), [y] "r"(y), [tpdnum] "r"(tpdnum));
#else
    z = 0;
    x1 = 0;
    __asm__ (
        " tst   %[x] \n"
        " jge   2f \n"
        " mov   #-1,%[x1] \n"
        " jmp   2f \n"
        "6: \n"
        " add   %[x],%A[z] \n"
        " addc  %[x1],%B[z] \n"
        "1: \n"
        " rla   %[x] \n"
        " rlc   %[x1] \n"
        "2: \n"
        " rra   %[y] \n"
        " jc    5f \n"
        " jne   1b \n"
        " jmp   4f \n"
        "5: \n"
        " sub   %[x],%A[z] \n"
        " subc  %[x1],%B[z] \n"
        "3: \n"
        " rla   %[x] \n"
        " rlc   %[x1] \n"
        " rra   %[y] \n"
        " jnc   6b \n"
        " cmp   #0xFFFF,%[y] \n"
        " jne   3b \n"
        "4: \n"
        //Do TPD dithering at the Q1.15 level
        " add   %[tpdnum],%A[z] \n"
        " addc  #0,%B[z] \n"
        //Shift to Q1.15 format (i.e. the top 16 bits are returned)
        " rla   %A[z] \n"
        " rlc   %B[z] \n"
        " mov   %B[z],%[x1] \n"
        : [z] "+r"(z), [x1] "+r"(x1)
        : [x] "r"(x), [y] "r"(y), [tpdnum] "r"(tpdnum));
#endif
#elif defined(__AQCOMPILER__)
#if defined(__MSP430_HAS_MPY__)
    /$
        push.w  SR
        dint
        mov     @x,&MPYS_
        mov     @y,&OP2_
        mov     &RESHI_,@z.0
        mov     &RESLO_,@z.1
        pop.w   SR
        //Do TPD dithering at the Q1.15 level
        add     @tpdnum,@z.1
        addc    #0,@z.0
        //Shift to Q1.15 format (i.e. the top 16 bits are returned)
        rla     @z.1
        rlc     @z.0
        mov     @z.0,@x1
    $/
#else
    /$
        clr     @z.0
        mov     @z.0,@z.1
        mov     @z.0,@x1
        tst     @x
        jge     xbooth_2
        mov     #-1,@x1
        jmp     xbooth_2

xbooth_6
        add     @x,@z.1
        addc    @x1,@z.0
xbooth_1
        rla     @x
        rlc     @x1
xbooth_2
        rra     @y
        jc      xbooth_5
        jne     xbooth_1
        jmp     xbooth_4

xbooth_5
        sub     @x,@z.1
        subc    @x1,@z.0
xbooth_3
        rla     @x
        rlc     @x1
        rra     @y
        jnc     xbooth_6
        cmp     #0xFFFF,@y
        jne     xbooth_3

xbooth_4
        //Do TPD dithering at the Q1.15 level
        add     @tpdnum,@z.1
        addc    #0,@z.0
        //Shift to Q1.15 format (i.e. the top 16 bits are returned)
        rla     @z.1
        rlc     @z.0
        mov     @z.0,@x1
    $/
#endif
#elif defined(__IAR_SYSTEMS_ICC__)
#if defined(__MSP430_HAS_MPY__)
        istate_t s;

        s = __get_interrupt_state();
        __disable_interrupt();
        MPYS = x;
        OP2 = y;
        z = ((int32_t) RESHI << 16);
        z |= RESLO;
        z += tpdnum;
        __set_interrupt_state(s);
        //Shift to Q1.15 format (i.e. the top 16 bits are returned)
        z <<= 1;
        x1 = z >> 16;
#else
    z = ((int32_t) x*(int32_t) y) + tpdnum;
    x1 = (z >> 15);
#endif
#else
    z = ((int32_t) x*(int32_t) y) + tpdnum;
    x1 = (z >> 15);
#endif
    return x1;
}

// Use a linear congrual generator function to generate
// a TPD (triangular probability density) weighted random
// number. Adding a pair of even distribution random numbers
// will create a TPD. We can reuse an old random number in the
// generation of each new TPD value, as there should be very
// weak correlation between the numbers.
void tpd_gen(void)
{
    register int16_t x1;
    register int16_t x2;

    x1 = rndnum;
#if defined(__GNUC__)  &&  defined(__MSP430__)
    x2 = 0;
    __asm__ (
        " sub   %[x1],%[x2] \n"     //0
        " rla   %[x1] \n"
        " add   %[x1],%[x2] \n"     //1     
        " rla   %[x1] \n"
        " sub   %[x1],%[x2] \n"     //2     
        " rla   %[x1] \n"
        " rla   %[x1] \n"
        " add   %[x1],%[x2] \n"     //4
        " rla   %[x1] \n"
        " rla   %[x1] \n"
        " sub   %[x1],%[x2] \n"     //6     
        " rla   %[x1] \n"
        " add   %[x1],%[x2] \n"     //7     
        " rla   %[x1] \n"
        " rla   %[x1] \n"
        " rla   %[x1] \n"
        " sub   %[x1],%[x2] \n"     //10
        : [x2] "+r"(x2), [x1] "+r"(x1));
#elif defined(__AQCOMPILER__)
    x2 = 0;
    /$
        sub     @x1,@x2     //0
        rla     @x1
        add     @x1,@x2     //1
        rla     @x1
        sub     @x1,@x2     //2
        rla     @x1
        rla     @x1
        add     @x1,@x2     //4
        rla     @x1
        rla     @x1
        sub     @x1,@x2     //6
        rla     @x1
        add     @x1,@x2     //7
        rla     @x1
        rla     @x1
        rla     @x1
        sub     @x1,@x2     //10
    $/
#else
    x2 = x1*31821U;
#endif

    x2 = (x2 + 13849) & 0x3FFF;
    tpdnum = rndnum + x2;
    rndnum = x2;
}
