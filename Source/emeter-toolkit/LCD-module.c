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
//  File: LCD-module.c
//
//  Steve Underwood <steve-underwood@ti.com>
//  Texas Instruments Hong Kong Ltd.
//
//  $Id: LCD-module.c,v 1.5 2007/08/07 05:53:40 a0754793 Exp $
//
//--------------------------------------------------------------------------
#include <stdint.h>
#include "emeter-toolkit-custom.h"
#include "emeter-toolkit.h"

#if SUPPORT_LCD_MODULE
//These routines support an external LCD module attached to a
//port on an MSP430. This is not guaranteed to work for any
//particular module. However, most LCD modules have a lot in
//common, so this code is unlikely to require major changes to
//support any specific module.

//LCD command codes, preshifted for efficiency
#define READ        (6 << 5)
#define WRITE       (5 << 5)
#define RDMODIFYWR  (5 << 5)
#define WORKMODE    (4 << 5)

#define LCD_SYS_DIS     0x00    /* Turn off system oscillator and LCD bias generator */ 
#define LCD_SYS_EN      0x01    /* Turn on system osciallator */
#define LCD_LCD_OFF     0x02    /* Turn off LCD bias generator */
#define LCD_LCD_ON      0x03    /* Turn on LCD bias generator */
#define LCD_TIMER_DIS   0x04    /* Disable time base output */
#define LCD_WDT_DIS     0x05    /* Disable WDT flag output */
#define LCD_TIMER_EN    0x06    /* Enable time base output */
#define LCD_WDT_EN      0x07    /* Enable WDT flag output */
#define LCD_TONE_OFF    0x08    /* Turn off tone */
#define LCD_TONE_ON     0x09    /* Turn on tone */
#define LCD_CLR_TIMER   0x0C    /* Clear contents of time base generator */
#define LCD_CLR_WDT     0x0E    /* Clear contents of WDT */ 
#define LCD_XTAL_32K    0x14    /* System clock from crystal */
#define LCD_RC_256K     0x18    /* System clock from internal R/C */
#define LCD_EXT_256K    0x1C    /* System clock from external source */
#define LCD_BIAS_1_2    0x20
#define LCD_BIAS_1_3    0x21    
#define LCD_TONE_4K     0x40    /* Tone frequency 4kHz */
#define LCD_TONE_2K     0x60    /* Tone frequency 2kHz */
#define LCD_IRQ_DIS     0x80    /* Disable IRQ output */

static __inline__ void delay_a(void)
{
    register int i;

#if defined(__GNUC__)  &&  defined(__MSP430__)
    i = 10;
    __asm__ __volatile__ (
        "1: \n"
        " dec   %[i] \n"
        " jge   1b \n"
        : [i] "+r"(i));
#elif defined(__AQCOMPILER__)  ||  defined(__IAR_SYSTEMS_ICC__)
    for (i = 10;  i > 0;  i--)
        _NOP();
#endif
}

static __inline__ void delay_b(void)
{
    register int i;
    
#if defined(__GNUC__)  &&  defined(__MSP430__)
    i = 90;
    __asm__ __volatile__ (
        "1: \n"
        " dec   %[i] \n"
        " jge   1b \n"
        : [i] "+r"(i));
#elif defined(__AQCOMPILER__)  ||  defined(__IAR_SYSTEMS_ICC__)
    for (i = 90;  i > 0;  i--)
        _NOP();
#endif
}

static void LCDsend(uint8_t x, int len)
{
    for (  ;  len > 0;  len--)
    {
        delay_a();
        LCD_PORT_OUT &= ~LCD_WR_BIT;
        if (x & 0x80)
            LCD_PORT_OUT |= LCD_DATA_BIT;
        else 
            LCD_PORT_OUT &= ~LCD_DATA_BIT;
        delay_b();
        x <<= 1;
        LCD_PORT_OUT |= LCD_WR_BIT;
   }
}

static uint8_t LCDget(int len)
{
    uint8_t x;

    x = 0;
    LCD_PORT_OUT |= LCD_DATA_BIT;
    LCD_PORT_DIR &= (~LCD_DATA_BIT);
    for (  ;  len > 0;  len--)
    {
        delay_a();/**/
        delay_a();/**/
        delay_a();/**/
        delay_a();
        LCD_PORT_OUT &= ~LCD_RD_BIT;
        delay_a();
        delay_a();/**/
        delay_a();/**/
        delay_a();/**/
        delay_a();/**/
        delay_a();/**/
        x <<= 1;
        if ((LCD_PORT_IN & LCD_DATA_BIT))
            x |= 0x10;
        delay_a();/**/
        delay_a();/**/
        LCD_PORT_OUT |= LCD_RD_BIT;
        delay_a();/**/
        delay_a();/**/
    }
    LCD_PORT_DIR |= LCD_DATA_BIT;
    return  x;
}

static void LCDsend_mode(uint8_t x)
{
    LCD_PORT_OUT |= (LCD_CS_BIT | LCD_DATA_BIT | LCD_WR_BIT | LCD_RD_BIT);
    LCD_PORT_OUT &= ~LCD_CS_BIT;
    LCDsend(x, 3);
}

static void set_lcd_working_mode(uint8_t mode)
{
    LCDsend_mode(WORKMODE);
    LCDsend(mode, 8);
    delay_a();
    LCD_PORT_OUT &= ~(LCD_DATA_BIT | LCD_WR_BIT);
    delay_b();
    LCD_PORT_OUT |= (LCD_CS_BIT | LCD_WR_BIT);
}

//Initialise the LCD display, and set it to initially display
//all segments.
void LCDinit(void)
{
    int i;

    /* Configure the LCD driver communication port */
    LCD_PORT_DIR |= (LCD_CS_BIT | LCD_WR_BIT | LCD_RD_BIT | LCD_DATA_BIT);

    /* Turn on system oscillator */
    set_lcd_working_mode(LCD_SYS_EN);
    /* Turn on LCD bias generator */
    set_lcd_working_mode(LCD_LCD_ON);
    /* System clock source, on-chip RC oscillator RC 256K */
    set_lcd_working_mode(LCD_RC_256K);
    /* LCD 1/2 bias option 4 commons */
    set_lcd_working_mode(LCD_BIAS_1_2 + 8);
    for (i = 1;  i <= lcd_cells;  i++)
        LCDchar(CHAR_ALL, i);
}

void LCDsleep(void)
{
    //TODO:
}

void LCDawaken(void)
{
    //TODO:
}

//Change a specified group of LCD cells.
void LCDchars(const uint8_t *s, int pos, int len)
{
    pos--;
    for (  ;  len > 0;  --len)
    {
        LCDsend_mode(WRITE);
        //Send the address
        LCDsend(pos++ << 3, 6);
        //Send the data
        LCDsend(*s++, 8);
        //Deselect the LCD
        LCD_PORT_OUT |= LCD_CS_BIT;
    }
}

//Change a specified LCD cell.
void LCDchar(uint16_t ch, int pos)
{
    LCDsend_mode(WRITE);
    //Send the address
    LCDsend((pos - 1) << 3, 6);
    //Send the data
    LCDsend(ch, 8);
    //Deselect the LCD
    LCD_PORT_OUT |= LCD_CS_BIT;
}

//Note: This only modifies the top nibble, but that is usually all
//you want to control SEG_h
void LCDmodify_char(uint16_t ch, int pos, int on)
{
    uint8_t x;

    LCDsend_mode(RDMODIFYWR);
    //Send the address
    LCDsend((pos - 1) << 3, 6);
    //Get the current data
    x = LCDget(4);
    //Modify it
    if (on)
        x |= ch;
    else
        x &= ~ch;
    //Send the new data back
    LCDsend(x, 4);
    //Deselect the LCD
    LCD_PORT_OUT |= LCD_CS_BIT;
}

#endif
