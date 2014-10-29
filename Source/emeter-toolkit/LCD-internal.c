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
//  File: LCD-internal.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: LCD-internal.c,v 1.9 2009/02/26 08:09:59 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include <stdlib.h>
#include <io.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

//
//  This code is based on a 4-mux display, something like the following diagram.
//  By adapting variables it can be customised for various layouts.
//
//           MSP430F449
//         ---------------
//     .  |               |
//    /|\ |           XIN |---
//     |  |               |    32768Hz
//     ---| RST      XOUT |---
//        |               |
//        |               |   Vcc
//        |               |     |
//        |               |    /
//        |               |    \ 30k
//        |           R32 |____/
//        |               |    /
//        |               |    \ 30k
//        |           R31 |____/
//        |               |    /
//        |               |    \ 30k
//        |           R30 |____/
//        |               |    |
//        |               |   GND
//        |               |
//        |               |          ____________LCD__T218010___________
//        |          COM3 |---------|2 COM4      __    __    __    __   |
//        |          COM2 |---------|1 COM3     |__|  |__|  |__|  |__|  |
//        |          COM1 |---------|3 COM2     |__|  |__|  |__|  |__|  |
//        |               |    _____|20 COM1     __    __    __    __   |
//        |          COM0 |___|_____|4  COM1                            |
//        |               |         |  S0            S14                |
//        |               |         |__19____________5__________________|
//        |               |            |             |
//        |      (28)S16  |------------              |
//        |           :   |   :                      |
//        |           :   |   :                      |
//        |      (42)S30  |-------------------------- 
//        |               |
//         ---------------
//

#if SUPPORT_INTERNAL_LCD

//Initialise the LCD display, and set it to initially display
//all segments.
void LCDinit(void)
{
    int i;

    for (i = 0;  i < 21;  i++)
        LCDMEM[i] = 0;
#if defined(__MSP430_HAS_LCD4__)
#if defined(__MSP430_E423__) || defined(__MSP430_E425__) || defined(__MSP430_E427__)
    LCDCTL = (LCDSG0_3 | LCD4MUX | LCDON);
#else
    LCDCTL = (LCDSG0_5 | LCD4MUX | LCDON);
#endif
#endif
#if defined(__MSP430_HAS_LCD_A__)
    /* LCD-A controller setup */
    LCDACTL = LCDFREQ_128 | LCD4MUX | LCDSON | LCDON;
    LCDAPCTL0 = LCDS0 | LCDS4 | LCDS8 | LCDS12 | LCDS16 | LCDS20 | LCDS24 | LCDS28;
    LCDAPCTL1 = LCDS32 | LCDS36;
    LCDAVCTL0 = LCDCPEN;
    LCDAVCTL1 = 1 << 1;
#endif
    for (i = 1;  i <= lcd_cells;  i++)
        LCDchar(0xFFFF, i);
#if defined(__MSP430_HAS_LCD4__)
#if defined(__MSP430_412__) || defined(__MSP430_413__) || defined(__MSP430_435__) || defined(__MSP430_436__) || defined(__MSP430_437__) || defined(__MSP430_447__) || defined(__MSP430_448__) || defined(__MSP430_449__)
    P5SEL |= (BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
#endif
#endif
#if defined(__MSP430_HAS_LCD_A__)
#if defined(__MSP430_G4618__) || defined(__MSP430_G4619__)
    P5SEL |= (BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
#else
    P5SEL |= (BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
#endif
#endif
}

void LCDsleep(void)
{
#if defined(__MSP430_HAS_LCD4__)
    LCDCTL &= ~(LCDON);
#else
    LCDACTL &= ~(LCDON);
#endif
}

void LCDawaken(void)
{
#if defined(__MSP430_HAS_LCD4__)
    LCDCTL |= LCDON;
#else
    LCDACTL |= LCDON;
#endif
}

//Change a specified group of LCD cells.
void LCDchars(const uint8_t *s, int pos, int len)
{
    if (lcd_pos_step < 0)
        pos = -pos;
    if (abs(lcd_pos_step) > 1)
        pos <<= 1;
    pos = lcd_pos_base + pos;
    for (  ;  len > 0;  --len)
    {
        LCDMEM[pos] = *s++;
        pos += lcd_pos_step;
    }
}

//Change a specified LCD cell.
void LCDchar(uint16_t ch, int pos)
{
    if (lcd_pos_step < 0)
        pos = -pos;
    if (abs(lcd_pos_step) > 1)
    {
        pos = lcd_pos_base + (pos << 1);
        LCDMEM[pos] = ch & 0xFF;
        pos += (lcd_pos_step >> 1);
        LCDMEM[pos] = ch >> 8;
    }
    else
    {
        pos = lcd_pos_base + pos;
        LCDMEM[pos] = ch;
    }
}

void LCDmodify_char(uint16_t ch, int pos, int on)
{
    if (lcd_pos_step < 0)
        pos = -pos;
    if (abs(lcd_pos_step) > 1)
    {
        pos = lcd_pos_base + (pos << 1);
        if (on)
        {
            LCDMEM[pos] |= ch & 0xFF;
            pos += (lcd_pos_step >> 1);
            LCDMEM[pos] |= ch >> 8;
        }
        else
        {
            LCDMEM[pos] &= ~(ch & 0xFF);
            pos += (lcd_pos_step >> 1);
            LCDMEM[pos] |= ~(ch >> 8);
        }
    }
    else
    {
        pos = lcd_pos_base + pos;
        if (on)
            LCDMEM[pos] |= ch;
        else
            LCDMEM[pos] &= ~ch;
    }
}

#endif
