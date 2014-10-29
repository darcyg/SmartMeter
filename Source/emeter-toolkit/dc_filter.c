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
//    File: dc_filter.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: dc_filter.c,v 1.4 2007/08/07 05:53:41 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

/* A routine to filter away the DC content from an AC mains waveform
   signal. It does this my using a heavily damped integrator to estimate
   the DC level. The current DC level is then subtracted from the signal. */
int16_t dc_filter(register int32_t *p, register int16_t x)
{
    //One would like to estimate DC by something like:
    //  z += ((((int32_t) x << 15) - z) >> 14);
    //  return  x - (z >> 15);
    //but this:
    //  z += ((((int32_t) x << 16) - z) >> 14);
    //  return  x - (z >> 16);
    //is a bit faster, and the shift by 16 will never
    //cause an overflow in this application. However,
    //remember this is not a generic DC filter!
#if defined(__GNUC__)  &&  defined(__MSP430__)
    register int32_t z;

    z = 0x12345678;
    __asm__ (
        " mov   %[x],%B[z] \n"
        " clr   %A[z] \n"
        " sub   0(%[p]),%A[z] \n"       // subtract filter_state from (sample << 16)
        " subc  2(%[p]),%B[z] \n"
        " rla   %A[z] \n"               // >> 14
        " rlc   %B[z] \n"
        " jc    1f \n"
        " rlc   %A[z] \n"
        " rlc   %B[z] \n"
        " rlc   %A[z] \n"
        " and   #0x0003,%A[z] \n"
        " jmp   2f \n"
        "1: \n"
        " rlc   %A[z] \n"
        " rlc   %B[z] \n"
        " rlc   %A[z] \n"
        " bis   #0xFFFC,%A[z] \n"
        "2: \n"
        " add   0(%[p]),%B[z] \n"       // + filter_state
        " addc  2(%[p]),%A[z] \n"
        " mov   %B[z],0(%[p]) \n"       // save new filter_state
        " mov   %A[z],2(%[p]) \n"
        " sub   %A[z],%[x] \n"          // sample - filter_state
        : [x] "+r"(x)
        : [p] "r"(p), [z] "r"(z));
#elif defined(__AQCOMPILER__)
    register int32_t z;

    z = 0x12345678;
    /$
        mov     @x,@z.0
        clr     @z.1
        sub     0(@p),@z.1      ; subtract filter_state from (sample << 16)
        subc    2(@p),@z.0
        rla     @z.1            ; >> 14
        rlc     @z.0
        jc      dc_filter_1
        rlc     @z.1
        rlc     @z.0
        rlc     @z.1
        and     #0x0003,@z.1
        jmp     dc_filter_2
dc_filter_1
        rlc     @z.1
        rlc     @z.0
        rlc     @z.1
        bis     #0xFFFC,@z.1
dc_filter_2
        add     0(@p),@z.0      ; + filter_state
        addc    2(@p),@z.1
        mov     @z.0,0(@p)      ; save new filter_state
        mov     @z.1,2(@p)
        sub     @z.1,@x         ; sample - filter_state
    $/
#else
    *p += ((((int32_t) x << 16) - *p) >> 14);
    x -= (*p >> 16);
#endif
    return x;
}
