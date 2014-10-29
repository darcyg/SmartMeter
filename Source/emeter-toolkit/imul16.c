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
//  File: imul16.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: imul16.c,v 1.3 2007/08/07 05:53:46 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

//This routine implements a 16x16->32 2's complement multiplier. If a hardware
//multiplier is available it is used. If no hardware multiplier is available,
//Booth's algorithm is used to directly implement a signed multiplier in software.
//C does not normally express a 16x16->32 operation, and the speedup of achieving
//a 32-bit answer by a full 32x32 multiply is substantial.
int32_t imul16(register int16_t x, register int16_t y)
{
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
        : [z] "=r"(z)
        : [x] "r"(x), [y] "r"(y));
#else
    register int16_t x1;

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
        : [z] "+r"(z), [x1] "+r"(x1)
        : [x] "r"(x), [y] "r"(y));
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
    $/
#else
    register int16_t x1;

    /$
        clr     @z.0
        mov     @z.0,@z.1

        mov     @z.0,@x1
        tst     @x
        jge     booth_2
        mov     #-1,@x1
        jmp     booth_2

booth_6
        add     @x,@z.1
        addc    @x1,@z.0
booth_1
        rla     @x
        rlc     @x1
booth_2
        rra     @y
        jc      booth_5
        jne     booth_1
        jmp     booth_4

booth_5
        sub     @x,@z.1
        subc    @x1,@z.0
booth_3
        rla     @x
        rlc     @x1
        rra     @y
        jnc     booth_6
        cmp     #0xFFFF,@y
        jne     booth_3

booth_4
    $/
#endif
#else
    z = (int32_t) x*(int32_t) y;
#endif
    return z;
}
