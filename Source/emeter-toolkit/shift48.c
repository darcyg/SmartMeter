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
//  File: shift48.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: shift48.c,v 1.3 2007/08/07 05:53:48 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include <io.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

void shift48(register int16_t x[3], int how_far)
{
#if !defined(__MSP430__)  ||  defined(__IAR_SYSTEMS_ICC__)
    int64_t tmp;
#endif

    if (how_far < 0)
    {
        while (how_far++)
        {
#if defined(__GNUC__)  &&  defined(__MSP430__)
            __asm__ __volatile__ (
                " rra 4(%[x]) \n"
                " rrc 2(%[x]) \n"
                " rrc 0(%[x]) \n"
                : [x] "+r"(x)
                : );
#elif defined(__AQCOMPILER__)
            /$
                rra 4(@x)
                rrc 2(@x)
                rrc 0(@x)
            $/
#else
            tmp = x[2];
            tmp = tmp << 16;
            tmp |= x[1];
            tmp = tmp << 16;
            tmp |= x[0];
            tmp >>= 1;
            x[2] = (tmp >> 32) & 0xFFFF;
            x[1] = (tmp >> 16) & 0xFFFF;
            x[0] = tmp & 0xFFFF;
#endif
        }
    }
    else
    {
        while (how_far--)
        {
#if defined(__GNUC__)  &&  defined(__MSP430__)
            __asm__ __volatile__ (
                " rla 0(%[x]) \n"
                " rlc 2(%[x]) \n"
                " rlc 4(%[x]) \n"
                : [x] "+r"(x)
                : );
#elif defined(__AQCOMPILER__)
            /$
                rla 0(@x)
                rlc 2(@x)
                rlc 4(@x)
            $/
#else
            tmp = x[2];
            tmp = tmp << 16;
            tmp |= x[1];
            tmp = tmp << 16;
            tmp |= x[0];
            tmp <<= 1;
            x[2] = (tmp >> 32) & 0xFFFF;
            x[1] = (tmp >> 16) & 0xFFFF;
            x[0] = tmp & 0xFFFF;
#endif
        }
    }
}
