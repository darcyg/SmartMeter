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
//  File: dds.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: dds.c,v 1.3 2007/08/07 05:53:42 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

const int16_t sine_table128[128] =
{
    0x00C9,0x025B,0x03ED,0x057F,0x0710,0x08A2,0x0A33,0x0BC3,
    0x0D53,0x0EE3,0x1072,0x1201,0x138E,0x151B,0x16A8,0x1833,
    0x19BD,0x1B47,0x1CCF,0x1E56,0x1FDC,0x2161,0x22E5,0x2467,
    0x25E8,0x2767,0x28E5,0x2A61,0x2BDC,0x2D55,0x2ECC,0x3041,
    0x31B5,0x3326,0x3496,0x3604,0x376F,0x38D8,0x3A40,0x3BA5,
    0x3D07,0x3E68,0x3FC5,0x4121,0x427A,0x43D0,0x4524,0x4675,
    0x47C3,0x490F,0x4A58,0x4B9E,0x4CE1,0x4E21,0x4F5E,0x5097,
    0x51CE,0x5302,0x5433,0x5560,0x568A,0x57B0,0x58D4,0x59F3,
    0x5B10,0x5C29,0x5D3E,0x5E50,0x5F5E,0x6068,0x616F,0x6271,
    0x6371,0x646C,0x6563,0x6657,0x6746,0x6832,0x6919,0x69FD,
    0x6ADC,0x6BB8,0x6C8F,0x6D62,0x6E30,0x6EFB,0x6FC1,0x7083,
    0x7141,0x71FA,0x72AF,0x735F,0x740B,0x74B2,0x7555,0x75F4,
    0x768E,0x7723,0x77B4,0x7840,0x78C7,0x794A,0x79C8,0x7A42,
    0x7AB6,0x7B26,0x7B92,0x7BF8,0x7C5A,0x7CB7,0x7D0F,0x7D62,
    0x7DB0,0x7DFA,0x7E3F,0x7E7F,0x7EBA,0x7EF0,0x7F21,0x7F4D,
    0x7F75,0x7F97,0x7FB5,0x7FCE,0x7FE1,0x7FF0,0x7FFA,0x7FFF
};

//This is a simple direct digital synthesis (DDS) function to generate sine
//waves. This version uses a 128 entry sin/cos table to cover one quadrant.
int16_t dds_offset(int phase_acc, int phase_offset)
{
    int i;
    int j;

    //Need 9 bit for a 128 element quadrant
    i = (phase_acc + phase_offset) >> 7;
    if (i >= 0)
    {
        j = FALSE;
    }
    else
    {
        j = TRUE;
        i = -i - 1;
    }
    if (i >= 128)
        i = 255 - i;         
    i = sine_table128[i];
    if (j)
        return  -i;
    return  i;
}

int16_t dds(int *phase_acc, int phase_rate)
{
    *phase_acc += phase_rate;
    return dds_offset(*phase_acc, 0);
}
